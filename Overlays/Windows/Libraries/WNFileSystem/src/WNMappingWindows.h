// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_FILE_SYSTEM_WINDOWS_MAPPING_WINDOWS_H__
#define __WN_FILE_SYSTEM_WINDOWS_MAPPING_WINDOWS_H__

#include "WNFileSystem/inc/WNMapping.h"

namespace wn {
namespace file_system {
namespace internal {

class mapping_windows final : public mapping {
public:
  inline mapping_windows(memory::allocator* _allocator,
      containers::string&& _path, const bool _cleanup = false)
    : mapping(_allocator, core::move(_path), _cleanup) {}

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
  static bool recursive_remove_directory(
      const WCHAR* _path, const size_t _size);
};

}  // namespace internal
}  // namespace file_system
}  // namespace wn

#endif  // __WN_FILE_SYSTEM_WINDOWS_MAPPING_WINDOWS_H__
