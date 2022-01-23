// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_FILE_SYSTEM_POSIX_MAPPING_POSIX_H__
#define __WN_FILE_SYSTEM_POSIX_MAPPING_POSIX_H__

#include "WNFileSystem/inc/WNMapping.h"

namespace wn {
namespace file_system {
namespace internal {

class mapping_posix final : public mapping {
public:
  inline mapping_posix(memory::allocator* _allocator,
      containers::string&& _path, const bool _cleanup = false)
    : mapping(_allocator, core::move(_path), _cleanup) {}

  virtual ~mapping_posix() override;

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
  bool recursive_remove_directory(const containers::string& _path);
};

}  // namespace internal
}  // namespace file_system
}  // namespace wn

#endif  // __WN_FILE_SYSTEM_POSIX_MAPPING_POSIX_H__
