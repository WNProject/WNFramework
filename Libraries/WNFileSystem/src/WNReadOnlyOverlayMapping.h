// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#pragma once

#ifndef __WN_READONLY_OVERLAY_MAPPING_H__
#define __WN_READONLY_OVERLAY_MAPPING_H__

#include <initializer_list>
#include "WNContainers/inc/WNDynamicArray.h"
#include "WNFileSystem/inc/WNMapping.h"

namespace wn {
namespace file_system {
class readonly_overlay_mapping final : public mapping {
public:
  readonly_overlay_mapping(memory::allocator* _allocator,
      containers::contiguous_range<mapping_ptr> _range);

  bool exists_file(const containers::string_view _path) const override;
  bool exists_directory(const containers::string_view _path) const override;

  file_ptr create_file(
      const containers::string_view _path, result& _result) override;
  result create_directory(const containers::string_view _path) override;

  file_ptr open_file(
      const containers::string_view _path, result& _result) override;
  result delete_directory(const containers::string_view _path) override;
  result delete_file(const containers::string_view _path) override;

private:
  containers::dynamic_array<mapping_ptr> m_child_mappings;
};

}  // namespace file_system
}  // namespace wn
#endif  // __WN_READONLY_OVERLAY_MAPPING_H__
