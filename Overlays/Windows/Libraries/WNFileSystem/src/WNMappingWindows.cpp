// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNFileSystem/src/WNMappingWindows.h"

#include "WNFileSystem/src/WNFileWindows.h"
#include "WNFileSystem/src/WNHelpers.h"
#include "WNFileSystem/src/WNSystemUtilities.h"
#include "core/inc/assert.h"

namespace wn {
namespace file_system {
namespace internal {

mapping_windows::~mapping_windows() {
  if (m_cleanup) {
    WCHAR unicode_buffer[MAX_PATH] = {0};
    DWORD unicode_buffer_size = MAX_PATH;
    bool result =
        convert_to_unicode(m_root_path, unicode_buffer, unicode_buffer_size);

    WN_RELEASE_ASSERT(
        result, "failed to convert to root path to native format");

    result = recursive_remove_directory(unicode_buffer, unicode_buffer_size);

    WN_RELEASE_ASSERT(result, "unable to cleanup mapping root directory");
  }
}

bool mapping_windows::exists_file(const containers::string_view _path) const {
  containers::string path(m_allocator);

  if (!sanitize_and_validate_path(_path, path)) {
    return false;
  }

  return internal::exists_file(path);
}

bool mapping_windows::exists_directory(
    const containers::string_view _path) const {
  containers::string path(m_allocator);

  if (!sanitize_and_validate_path(_path, path)) {
    return false;
  }

  return internal::exists_directory(path);
}

file_ptr mapping_windows::create_file(
    const containers::string_view _path, result& _result) {
  containers::string path(m_allocator);

  if (!sanitize_and_validate_path(_path, path)) {
    _result = result::fail;

    return nullptr;
  }

  WCHAR unicode_buffer[MAX_PATH] = {0};
  DWORD unicode_buffer_size = MAX_PATH;

  if (!convert_to_unicode(path, unicode_buffer, unicode_buffer_size)) {
    _result = result::fail;

    return nullptr;
  }

  utilities::internal::handle file_handle(::CreateFileW(unicode_buffer,
      GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_NEW, 0, nullptr));

  if (!file_handle.is_valid()) {
    _result = result::fail;

    return nullptr;
  }

  _result = result::ok;

  return memory::make_intrusive<file_windows>(
      m_allocator, m_allocator, core::move(file_handle));
}

result mapping_windows::create_directory(const containers::string_view _path) {
  containers::string path(m_allocator);

  if (!sanitize_and_validate_path(_path, path)) {
    return result::fail;
  }

  WCHAR unicode_buffer[MAX_PATH] = {0};
  DWORD unicode_buffer_size = MAX_PATH;

  if (!convert_to_unicode(path, unicode_buffer, unicode_buffer_size)) {
    return result::fail;
  }

  if (::CreateDirectoryW(unicode_buffer, nullptr) != TRUE) {
    return result::fail;
  }

  return result::ok;
}

file_ptr mapping_windows::open_file(
    const containers::string_view _path, result& _result) {
  containers::string path(m_allocator);

  if (!sanitize_and_validate_path(_path, path)) {
    _result = result::fail;

    return nullptr;
  }

  WCHAR unicode_buffer[MAX_PATH] = {0};
  DWORD unicode_buffer_size = MAX_PATH;

  if (!convert_to_unicode(path, unicode_buffer, unicode_buffer_size)) {
    _result = result::fail;

    return nullptr;
  }

  utilities::internal::handle file_handle(::CreateFileW(unicode_buffer,
      GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
      OPEN_EXISTING, 0, nullptr));

  if (!file_handle.is_valid()) {
    _result = result::fail;

    return nullptr;
  }

  LARGE_INTEGER file_size;

  if (!::GetFileSizeEx(file_handle.value(), &file_size)) {
    _result = result::fail;

    return nullptr;
  }

  if (static_cast<LONGLONG>(static_cast<size_t>(file_size.QuadPart)) !=
      file_size.QuadPart) {
    _result = result::fail;

    return nullptr;
  }

  if (file_size.QuadPart > 0) {
    utilities::internal::handle file_mapping_handle(
        ::CreateFileMappingW(file_handle.value(), nullptr, PAGE_READWRITE,
            file_size.HighPart, file_size.LowPart, nullptr));

    if (!file_mapping_handle.is_valid()) {
      _result = result::fail;

      return nullptr;
    }

    LPVOID memory_mapped =
        ::MapViewOfFile(file_mapping_handle.value(), FILE_MAP_WRITE, 0, 0, 0);

    if (!memory_mapped) {
      _result = result::fail;

      return nullptr;
    }

    _result = result::ok;

    return memory::make_intrusive<file_windows>(m_allocator, m_allocator,
        core::move(file_handle), core::move(file_mapping_handle), memory_mapped,
        file_size);
  } else {
    _result = result::ok;

    return memory::make_intrusive<file_windows>(
        m_allocator, m_allocator, core::move(file_handle));
  }
}

result mapping_windows::delete_file(const containers::string_view _path) {
  containers::string path(m_allocator);

  if (!sanitize_and_validate_path(_path, path)) {
    return result::fail;
  }

  WCHAR unicode_buffer[MAX_PATH] = {0};
  DWORD unicode_buffer_size = MAX_PATH;

  if (!convert_to_unicode(path, unicode_buffer, unicode_buffer_size)) {
    return result::fail;
  }

  if (::DeleteFileW(unicode_buffer) != TRUE) {
    return result::fail;
  }

  return result::ok;
}

result mapping_windows::delete_directory(const containers::string_view _path) {
  containers::string path(m_allocator);

  if (!sanitize_and_validate_path(_path, path)) {
    return result::fail;
  }

  WCHAR unicode_buffer[MAX_PATH] = {0};
  DWORD unicode_buffer_size = MAX_PATH;

  if (!convert_to_unicode(path, unicode_buffer, unicode_buffer_size)) {
    return result::fail;
  }

  if (!recursive_remove_directory(unicode_buffer, unicode_buffer_size)) {
    return result::fail;
  }

  return result::ok;
}

bool mapping_windows::recursive_remove_directory(
    const WCHAR* _path, const size_t _size) {
  if (_size + 4 > MAX_PATH) {
    return false;
  }

  containers::array<WCHAR, MAX_PATH> path = {0};

  ::wcsncpy(path.data(), _path, _size);
  ::wcsncat(path.data(), L"\\*.*", 4);

  WIN32_FIND_DATAW find_file_data;
  const HANDLE find_handle = ::FindFirstFileW(path.data(), &find_file_data);

  if (find_handle != nullptr) {
    do {
      const WCHAR* file_name = find_file_data.cFileName;

      if (::wcscmp(file_name, L".") != 0 && ::wcscmp(file_name, L"..") != 0) {
        const size_t size = ::wcslen(file_name);
        const size_t full_size = _size + 1 + size;

        if (full_size > MAX_PATH) {
          break;
        }

        containers::array<WCHAR, MAX_PATH> sub_path = {0};

        ::wcsncpy(sub_path.data(), _path, _size);
        ::wcsncat(sub_path.data(), L"\\", 1);
        ::wcsncat(sub_path.data(), file_name, size);

        const DWORD attributes = ::GetFileAttributesW(sub_path.data());

        if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
          if (!recursive_remove_directory(sub_path.data(), full_size)) {
            break;
          }
        } else if (::DeleteFileW(sub_path.data()) != TRUE) {
          break;
        }
      }
    } while (::FindNextFileW(find_handle, &find_file_data) == TRUE);

    if (::FindClose(find_handle) != TRUE) {
      WN_DEBUG_ASSERT(false, "failed to close file find resource");
    }

    return (::RemoveDirectoryW(_path) == TRUE);
  }

  return false;
}

}  // namespace internal
}  // namespace file_system
}  // namespace wn
