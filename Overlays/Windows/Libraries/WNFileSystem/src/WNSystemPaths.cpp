// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNFileSystem/src/WNSystemPaths.h"
#include "WNContainers/inc/WNArray.h"
#include "WNFileSystem/src/WNUtilities.h"

namespace wn {
namespace file_system {
namespace internal {
namespace {

WN_INLINE containers::string convert_to_utf8(
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

WN_INLINE DWORD get_temp_path_unicode(
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

}  // anonymous namespace

containers::string get_scratch_path(memory::allocator* _allocator) {
  containers::array<WCHAR, (MAX_PATH + 1)> path_buffer = {0};
  DWORD path_size = get_temp_path_unicode(path_buffer);

  if (path_size != 0) {
    wchar_t name_template[7] = {0};

    for (;;) {
      ::wcsncpy(name_template, L"XXXXXX", 6);

      if (::_wmktemp(name_template)) {
        containers::array<wchar_t, (MAX_PATH + 1)> path_unique_buffer = {0};

        const int result =
            ::_snwprintf(path_unique_buffer.data(), path_unique_buffer.size(),
                L"%sscratch.%s\\", path_buffer.data(), name_template);

        if (result > 0 &&
            result < static_cast<int>(path_unique_buffer.size())) {
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
      }

      break;
    }
  }

  return nullptr;
}

containers::string get_executable_path(memory::allocator* _allocator) {
  containers::dynamic_array<WCHAR> buffer(_allocator);
  size_t size = static_cast<size_t>(MAX_PATH * 0.75f);
  DWORD result = 0;

  for (;;) {
    buffer.resize(size, L'\0');

    result = GetModuleFileNameW(
        NULL, buffer.data(), static_cast<DWORD>(buffer.size()));

    if (result == 0) {
      return false;
    } else if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
      size *= 2;

      continue;
    }

    break;
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

}  // namespace internal
}  // namespace file_system
}  // namespace wn
