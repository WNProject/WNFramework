// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_FILE_SYSTEM_MAPPING_H__
#define __WN_FILE_SYSTEM_MAPPING_H__

#include "WNContainers/inc/WNStringView.h"
#include "WNCore/inc/WNPair.h"
#include "WNCore/inc/WNUtility.h"
#include "WNFileSystem/inc/WNFile.h"
#include "WNFileSystem/inc/WNResult.h"
#include "WNMemory/inc/WNUniquePtr.h"

namespace wn {
namespace file_system {

class mapping : core::non_copyable {
public:
  virtual ~mapping() = default;

  virtual bool exists_file(const containers::string_view _path) const = 0;

  virtual bool exists_directory(const containers::string_view _path) const = 0;

  virtual file_ptr create_file(
      const containers::string_view _path, result& _result) = 0;

  virtual result create_directory(const containers::string_view _path) = 0;

  result recursive_create_directory(containers::string_view _directory);

  virtual file_ptr open_file(
      const containers::string_view _path, result& _result) = 0;

  virtual result delete_directory(const containers::string_view _path) = 0;

  virtual result delete_file(const containers::string_view _path) = 0;

  result initialize_files(std::initializer_list<
      core::pair<containers::string_view, containers::string_view>>
          _files);

protected:
  WN_FORCE_INLINE mapping(memory::allocator* _allocator,
      containers::string&& _path, const bool _cleanup = false)
    : m_root_path(core::move(_path)),
      m_allocator(_allocator),
      m_cleanup(_cleanup) {}

  bool sanitize_and_validate_path(const containers::string_view _path,
      containers::string& _full_path) const;

  const containers::string m_root_path;
  memory::allocator* m_allocator;
  const bool m_cleanup;
};

using mapping_ptr = memory::unique_ptr<mapping>;

}  // namespace file_system
}  // namespace wn

#endif  // __WN_FILE_SYSTEM_MAPPING_H__
