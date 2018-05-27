// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNFileSystem/src/WNSystemPaths.h"
#include "WNContainers/inc/WNArray.h"
#include "WNFileSystem/src/WNUtilities.h"

namespace wn {
namespace file_system {
namespace internal {
namespace {

inline containers::string convert_to_utf8(
    memory::allocator* _allocator, LPCWSTR _buffer, const DWORD _buffer_size) {
  const int buffer_size = static_cast<const int>(_buffer_size);
  int converted_temp_path_size = ::WideCharToMultiByte(
      CP_UTF8, 0, _buffer, buffer_size, NULL, 0, NULL, NULL);

  if (converted_temp_path_size != 0) {
    containers::string path(
        _allocator, static_cast<size_t>(converted_temp_path_size), 0);
    converted_temp_path_size = ::WideCharToMultiByte(CP_UTF8, 0, _buffer,
        buffer_size, &path[0], static_cast<const int>(path.size()), NULL, NULL);

    if (converted_temp_path_size != 0) {
      return core::move(path);
    }
  }

  return nullptr;
}

inline DWORD get_temp_path_unicode(
    containers::array<WCHAR, (MAX_PATH + 1)>& _buffer) {
  const DWORD path_size =
      ::GetTempPathW(static_cast<DWORD>(_buffer.size()), _buffer.data());

  if (path_size != 0) {
    const DWORD result = ::CreateDirectoryW(_buffer.data(), NULL);

    if (result == TRUE || ::GetLastError() == ERROR_ALREADY_EXISTS) {
      return path_size;
    }
  }

  return 0;
}

}  // namespace

containers::string get_scratch_path(
    memory::allocator* _allocator, const entry::system_data*) {
  containers::array<WCHAR, (MAX_PATH + 1)> path_buffer = {0};
  DWORD path_size = get_temp_path_unicode(path_buffer);

  if (path_size != 0) {
    static const wchar_t alphanum[] =
        L"0123456789"
        L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        L"abcdefghijklmnopqrstuvwxyz";
    static const size_t length = 16;
    wchar_t name_template[length + 1];

    name_template[length] = 0;

    for (;;) {
      for (size_t i = 0; i < length; ++i) {
        name_template[i] =
            alphanum[rand() % ((sizeof(alphanum) / sizeof(wchar_t)) - 1)];
      }

      containers::array<wchar_t, (MAX_PATH + 1)> path_unique_buffer = {0};
      const int result =
          ::_snwprintf(path_unique_buffer.data(), path_unique_buffer.size(),
              L"%sscratch.%s\\", path_buffer.data(), name_template);

      if (result > 0 && result < static_cast<int>(path_unique_buffer.size())) {
        path_size = static_cast<DWORD>(result);
        const WCHAR* path_unique =
            static_cast<WCHAR*>(path_unique_buffer.data());

        if (::CreateDirectoryW(path_unique, NULL) == TRUE) {
          containers::string path(
              convert_to_utf8(_allocator, path_unique, path_size));

          if (!path.empty()) {
            internal::sanitize_path(path);

            return core::move(path);
          }
        } else {
          const DWORD error = ::GetLastError();

          if (error == ERROR_ALREADY_EXISTS) {
            continue;
          }
        }
      }

      break;
    }
  }

  return nullptr;
}

containers::string get_executable_path(
    memory::allocator* _allocator, const entry::system_data*) {
  containers::dynamic_array<WCHAR> buffer(_allocator);
  size_t size = static_cast<size_t>(MAX_PATH);
  DWORD result = 0;

  for (;;) {
    buffer.resize(size);

    result = GetModuleFileNameW(
        NULL, buffer.data(), static_cast<DWORD>(buffer.size()));

    if (result == 0) {
      return false;
    } else if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
      size *= 2;
    } else {
      break;
    }
  }

  containers::string path(convert_to_utf8(_allocator, buffer.data(), result));

  if (path.empty()) {
    return false;
  }

  const size_t position = path.find_last_of('\\');

  path.erase(position + 1);
  internal::sanitize_path(path);

  return core::move(path);
}

containers::string get_current_working_path(
    memory::allocator* _allocator, const entry::system_data*) {
  DWORD result = ::GetCurrentDirectoryW(0, NULL);

  if (result != 0) {
    containers::dynamic_array<WCHAR> buffer(_allocator, result);

    result = ::GetCurrentDirectoryW(
        static_cast<DWORD>(buffer.size()), buffer.data());

    if (result != 0) {
      return containers::string(
          convert_to_utf8(_allocator, buffer.data(), result));
    }
  }

  return nullptr;
}

containers::string get_dev_assets_path(
    memory::allocator* _allocator, const entry::system_data* _system_data) {
  return get_current_working_path(_allocator, _system_data);
}

}  // namespace internal
}  // namespace file_system
}  // namespace wn
