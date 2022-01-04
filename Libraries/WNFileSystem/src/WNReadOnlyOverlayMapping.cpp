// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNFileSystem/src/WNReadOnlyOverlayMapping.h"
#include <initializer_list>

namespace wn {
namespace file_system {

readonly_overlay_mapping::readonly_overlay_mapping(
    memory::allocator* _allocator,
    containers::contiguous_range<mapping_ptr> _mappings)
  : mapping(_allocator, containers::string(_allocator), false),
    m_child_mappings(_allocator) {
  for (auto& it : _mappings) {
    m_child_mappings.push_back(core::move(it));
  }
}

bool readonly_overlay_mapping::exists_file(
    const containers::string_view _path) const {
  for (auto& it : m_child_mappings) {
    if (it->exists_file(_path)) {
      return true;
    }
  }
  return false;
}

bool readonly_overlay_mapping::exists_directory(
    const containers::string_view _path) const {
  for (auto& it : m_child_mappings) {
    if (it->exists_directory(_path)) {
      return true;
    }
  }
  return false;
}

file_ptr readonly_overlay_mapping::create_file(
    const containers::string_view, result& _result) {
  _result = result::invalid_operation;
  return nullptr;
}

result readonly_overlay_mapping::create_directory(
    const containers::string_view) {
  return result::invalid_operation;
}

file_ptr readonly_overlay_mapping::open_file(
    const containers::string_view _path, result& _result) {
  const size_t sz = m_child_mappings.size();
  for (size_t i = 0; i < sz; ++i) {
    auto& m = m_child_mappings[m_child_mappings.size() - i - 1];
    if (m->exists_file(_path)) {
      return m->open_file(_path, _result);
    }
  }
  _result = result::not_found;
  return nullptr;
}

result readonly_overlay_mapping::delete_directory(
    const containers::string_view) {
  return result::invalid_operation;
}
result readonly_overlay_mapping::delete_file(const containers::string_view) {
  return result::invalid_operation;
}

}  // namespace file_system
}  // namespace wn