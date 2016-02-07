// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNFileSystem/src/Windows/WNSystemPaths.h"
#include "WNContainers/inc/WNArray.h"

namespace wn {
namespace file_system {
namespace internal {
namespace {

bool convert_to_utf8(
    LPCWSTR _buffer, const DWORD _buffer_size, containers::string& _path) {
  const int buffer_size = static_cast<const int>(_buffer_size);
  int converted_temp_path_size = ::WideCharToMultiByte(
      CP_UTF8, 0, _buffer, buffer_size, NULL, 0, NULL, NULL);

  if (converted_temp_path_size != 0) {
    containers::string path(static_cast<size_t>(converted_temp_path_size), 0);

    converted_temp_path_size = ::WideCharToMultiByte(CP_UTF8, 0, _buffer,
        buffer_size, &path[0], static_cast<const int>(path.size()), NULL, NULL);

    if (converted_temp_path_size != 0) {
      _path = std::move(path);

      return true;
    }
  }

  return false;
}

WN_INLINE DWORD get_temp_path_unicode(
    containers::array<WCHAR, MAX_PATH>& _buffer) {
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

bool get_scratch_path(containers::string& _path) {
  containers::array<WCHAR, MAX_PATH> path_buffer = {0};
  DWORD path_size = get_temp_path_unicode(path_buffer);

  if (path_size != 0) {
    wchar_t name_template[7] = {0};

    for (;;) {
      ::wcsncpy(name_template, L"XXXXXX", 6);

      if (::_wmktemp(name_template)) {
        containers::array<wchar_t, MAX_PATH> path_unique_buffer = {0};

        const int result =
            ::_snwprintf(path_unique_buffer.data(), path_unique_buffer.size(),
                L"%sscratch.%s\\", path_buffer.data(), name_template);

        if (result > 0 &&
            result < static_cast<int>(path_unique_buffer.size())) {
          path_size = static_cast<DWORD>(result);
          const WCHAR* path_unique =
              static_cast<WCHAR*>(path_unique_buffer.data());

          if (::CreateDirectoryW(path_unique, NULL) == TRUE) {
            containers::string path;

            if (convert_to_utf8(path_unique, path_size, path)) {
              _path = std::move(path);

              return true;
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

  return false;
}

}  // namespace internal
}  // namespace file_system
}  // namespace wn
