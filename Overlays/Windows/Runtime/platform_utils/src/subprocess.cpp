// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "platform_utils/inc/subprocess.h"

#include "WNContainers/inc/WNStringView.h"

#include <inttypes.h>

namespace wn {
namespace runtime {
namespace platform_utils {

bool CreatePipe2(HANDLE* _out_read, HANDLE* _out_write,
    LPSECURITY_ATTRIBUTES _attributes, bool read_overlapped,
    bool write_overlapped, DWORD buffer_size) {
  char pipe_name[MAX_PATH];
  if (!buffer_size) {
    buffer_size = 4096;
  }
  static int64_t pipe_serial = 0;
  sprintf(pipe_name, "\\\\.\\Pipe\\RemoteExeAnon.%08x.%016" PRIx64,
      GetCurrentProcessId(), InterlockedIncrement64(&pipe_serial));

  *_out_read = CreateNamedPipeA(pipe_name,
      PIPE_ACCESS_INBOUND | (read_overlapped * FILE_FLAG_OVERLAPPED),
      PIPE_TYPE_BYTE | PIPE_WAIT, 1, buffer_size, buffer_size, 0, _attributes);
  if (INVALID_HANDLE_VALUE == *_out_read) {
    return false;
  }

  *_out_write =
      CreateFileA(pipe_name, GENERIC_WRITE, 0, _attributes, OPEN_EXISTING,
          FILE_ATTRIBUTE_NORMAL | (write_overlapped * FILE_FLAG_OVERLAPPED),
          NULL  // Template file
      );
  if (INVALID_HANDLE_VALUE == *_out_write) {
    CloseHandle(*_out_read);
    return false;
  }
  return true;
}

subprocess_return call_subprocess(memory::allocator* _allocator,
    const containers::string_view _application,
    const containers::string_view _input,
    const containers::contiguous_range<const containers::string_view> _args) {
  SECURITY_ATTRIBUTES saAttr;

  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = TRUE;
  saAttr.lpSecurityDescriptor = NULL;

  HANDLE stdout_read = NULL;
  HANDLE stderr_read = NULL;
  HANDLE stdout_write = NULL;
  HANDLE stderr_write = NULL;

  HANDLE stdin_read = NULL;
  HANDLE stdin_write = NULL;

  if (!CreatePipe2(&stdout_read, &stdout_write, &saAttr, true, false, 0)) {
    return subprocess_return(containers::string(), containers::string(),
        subprocess_error::cannot_create_pipe, 0);
  }

  if (!CreatePipe2(&stderr_read, &stderr_write, &saAttr, true, false, 0)) {
    return subprocess_return(containers::string(), containers::string(),
        subprocess_error::cannot_create_pipe, 0);
  }

  if (!_input.empty()) {
    if (!CreatePipe2(&stdin_read, &stdin_write, &saAttr, false, false, 0)) {
      return subprocess_return(containers::string(), containers::string(),
          subprocess_error::cannot_create_pipe, 0);
    }

    if (!SetHandleInformation(stdin_write, HANDLE_FLAG_INHERIT, 0)) {
      return subprocess_return(containers::string(), containers::string(),
          subprocess_error::eWNPlatformError, 0);
    }
  }

  if (!SetHandleInformation(stdout_read, HANDLE_FLAG_INHERIT, 0)) {
    return subprocess_return(containers::string(), containers::string(),
        subprocess_error::eWNPlatformError, 0);
  }

  if (!SetHandleInformation(stderr_read, HANDLE_FLAG_INHERIT, 0)) {
    return subprocess_return(containers::string(), containers::string(),
        subprocess_error::eWNPlatformError, 0);
  }

  PROCESS_INFORMATION proc_info;
  memory::memory_zero(&proc_info);
  STARTUPINFOW start_info;
  memory::memory_zero(&start_info);

  start_info.cb = sizeof(STARTUPINFOW);
  start_info.hStdError = stderr_write;
  start_info.hStdOutput = stdout_write;
  if (!_input.empty()) {
    start_info.hStdInput = stdin_read;
  }

  start_info.dwFlags |= STARTF_USESTDHANDLES | CREATE_NO_WINDOW;

  const int application_data_size =
      ::MultiByteToWideChar(CP_UTF8, 0, _application.data(), -1, NULL, 0);

  containers::dynamic_array<wchar_t> application(
      _allocator, application_data_size);
  const int new_application_data_size =
      ::MultiByteToWideChar(CP_UTF8, 0, _application.data(), -1,
          application.data(), static_cast<int>(application.size()));

  int args_data_size = application_data_size;
  for (auto a : _args) {
    args_data_size += ::MultiByteToWideChar(CP_UTF8, 0, a.data(), -1, NULL, 0);
  }

  containers::dynamic_array<wchar_t> args(_allocator, args_data_size);
  memcpy(args.data(), application.data(),
      (application.size() - 1) * sizeof(wchar_t));
  size_t offs = application.size();
  for (auto a : _args) {
    WN_DEBUG_ASSERT(offs <= args.size(), "Bad args length calculation");
    args.data()[offs - 1] = L' ';
    offs += ::MultiByteToWideChar(CP_UTF8, 0, a.data(), -1, args.data() + offs,
        static_cast<int>(args.size() - offs));
  }

  BOOL success = CreateProcessW(application.data(), args.data(), nullptr,
      nullptr, TRUE, CREATE_NO_WINDOW, NULL, NULL, &start_info, &proc_info);

  CloseHandle(stderr_write);
  CloseHandle(stdout_write);
  if (!_input.empty()) {
    CloseHandle(stdin_read);
  }

  if (!success) {
    CloseHandle(stderr_read);
    CloseHandle(stdout_read);
    CloseHandle(stdin_write);
    return subprocess_return(containers::string(), containers::string(),
        subprocess_error::cannot_create_process, 0);
  }

  HANDLE read_events[2];
  read_events[0] = CreateEvent(NULL, TRUE, TRUE, NULL);
  read_events[1] = CreateEvent(NULL, TRUE, TRUE, NULL);
  const uint32_t num_buffered_bytes = 4096;
  struct stream_data {
    HANDLE handle;
    OVERLAPPED overlapped;
    char buffer[num_buffered_bytes];
    containers::string handle_data;
  };

  stream_data streams[2];
  streams[0].handle = stdout_read;
  streams[1].handle = stderr_read;
  for (size_t i = 0; i < 2; ++i) {
    memory::memory_zero(&streams[i].overlapped);
    streams[i].overlapped.hEvent = read_events[i];
    streams[i].handle_data = containers::string(_allocator);
  }

  if (!_input.empty()) {
    DWORD bytes_written = 0;
    DWORD offset = 0;
    while (success && (offset != _input.size())) {
      size_t s = _input.size() - offset;
      DWORD to_write = 0;
      if (s > 4096) {
        to_write = 4096;
      } else {
        to_write = static_cast<DWORD>(s);
      }
      success = WriteFile(stdin_write, _input.data() + offset, to_write,
          &bytes_written, nullptr);
      auto err = GetLastError();
      (void)err;
      offset += bytes_written;
    }
    CloseHandle(stdin_write);
  }

  success = ReadFile(stdout_read, streams[0].buffer, num_buffered_bytes, NULL,
      &streams[0].overlapped);
  success = success || (GetLastError() == ERROR_IO_PENDING);

  success &= ReadFile(stderr_read, streams[1].buffer, num_buffered_bytes, NULL,
      &streams[1].overlapped);
  success = success || (GetLastError() == ERROR_IO_PENDING);

  while (success) {
    DWORD dwWait = WaitForMultipleObjects(2, read_events, FALSE, INFINITE);
    DWORD i = dwWait - WAIT_OBJECT_0;
    if (i > 1) {
      break;
    }

    DWORD num_read = 0;
    success = GetOverlappedResult(
        streams[i].handle, &streams[i].overlapped, &num_read, TRUE);
    if (!success) {
      break;
    }
    streams[i].handle_data.append(streams[i].buffer, num_read);
    memory::memory_zero(&streams[i].overlapped);
    streams[i].overlapped.hEvent = read_events[i];
    success = ReadFile(streams[i].handle, streams[i].buffer, num_buffered_bytes,
        NULL, &streams[i].overlapped);
    success = success || (GetLastError() == ERROR_IO_PENDING);
  }

  CloseHandle(stderr_read);
  CloseHandle(stdout_read);
  WaitForSingleObject(proc_info.hProcess, INFINITE);
  DWORD exit;
  if (!GetExitCodeProcess(proc_info.hProcess, &exit)) {
    subprocess_return{core::move(streams[0].handle_data),
        core::move(streams[1].handle_data), subprocess_error::eWNPlatformError,
        0};
  }
  return subprocess_return(core::move(streams[0].handle_data),
      core::move(streams[1].handle_data),
      exit == 0 ? subprocess_error::ok
                : subprocess_error::process_exited_uncleanly,
      exit);
}

}  // namespace platform_utils
}  // namespace runtime
}  // namespace wn
