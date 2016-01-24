// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_FILE_SYSTEM_WINDOWS_MAPPING_WINDOWS_H__
#define __WN_FILE_SYSTEM_WINDOWS_MAPPING_WINDOWS_H__

#include "WNFileSystem/inc/WNMapping.h"

#include <Windows.h>

namespace wn {
namespace file_system {
namespace internal {

class mapping_windows final : public mapping {
public:
  WN_FORCE_INLINE mapping_windows(containers::string&& _path,
      memory::allocator* _allocator, const bool _cleanup = false)
    : mapping(std::move(_path), _allocator, _cleanup) {}

  virtual ~mapping_windows() override;

  virtual bool exists_file(const containers::string_view _path) const override;

  virtual bool exists_directory(
      const containers::string_view _path) const override;

  virtual file_ptr create_file(
      const containers::string_view _path, result& _result) override;

  virtual result create_directory(const containers::string_view _path) override;

  virtual file_ptr open_file(
      const containers::string_view _path, result& _result) override;

  virtual result delete_file(const containers::string_view _path) override;

  virtual result delete_directory(const containers::string_view _path) override;

private:
  WN_FORCE_INLINE DWORD get_attributes(
      const containers::string_view _path) const {
    containers::string path(m_allocator);

    if (!sanitize_and_validate_path(_path, path)) {
      return INVALID_FILE_ATTRIBUTES;
    }

    containers::array<WCHAR, MAX_PATH> unicode_buffer = {0};
    DWORD unicode_buffer_size = MAX_PATH;

    if (!convert_to_unicode(path, unicode_buffer.data(), unicode_buffer_size)) {
      return INVALID_FILE_ATTRIBUTES;
    }

    return ::GetFileAttributesW(unicode_buffer.data());
  }

  static bool convert_to_unicode(
      const containers::string& _path, WCHAR* _buffer, DWORD& _buffer_size);

  static bool recursive_remove_directory(
      const WCHAR* _path, const size_t _size);
};

}  // namespace internal
}  // namespace file_system
}  // namespace wn

#endif  // __WN_FILE_SYSTEM_WINDOWS_MAPPING_WINDOWS_H__