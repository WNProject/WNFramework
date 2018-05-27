// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNFileSystem/inc/WNFactory.h"
#include "WNFileSystem/src/WNMemoryBackedMapping.h"
#include "WNFileSystem/src/WNSystemMapping.h"
#include "WNFileSystem/src/WNSystemPaths.h"
#include "WNFileSystem/src/WNSystemUtilities.h"
#include "WNFileSystem/src/WNUtilities.h"

namespace wn {
namespace file_system {

containers::string factory::get_executable_path() const {
  return internal::get_executable_path(m_allocator, m_system_data);
}

containers::string factory::get_current_working_path() const {
  return internal::get_current_working_path(m_allocator, m_system_data);
}

mapping_ptr factory::make_mapping(
    memory::allocator* _allocator, const mapping_type _mapping_type) const {
  if (!_allocator) {
    return nullptr;
  }

  switch (_mapping_type) {
    case mapping_type::memory_backed:
      return memory::make_unique<memory_backed_mapping>(_allocator, _allocator);
    case mapping_type::scratch_directory: {
      containers::string path(
          internal::get_scratch_path(_allocator, m_system_data));

      if (!path.empty()) {
        return memory::make_unique<internal::system_mapping>(
            _allocator, _allocator, core::move(path), true);
      }

      break;
    }
    case mapping_type::executable_directory: {
      containers::string path(
          internal::get_executable_path(_allocator, m_system_data));

      if (!path.empty()) {
        return memory::make_unique<internal::system_mapping>(
            _allocator, _allocator, core::move(path), false);
      }

      break;
    }
    case mapping_type::current_working_directory: {
      containers::string path(
          internal::get_current_working_path(_allocator, m_system_data));

      if (!path.empty()) {
        return memory::make_unique<internal::system_mapping>(
            _allocator, _allocator, core::move(path), false);
      }
    }
    case mapping_type::development_assets: {
      containers::string path(
          internal::get_dev_assets_path(_allocator, m_system_data));

      if (!path.empty()) {
        return memory::make_unique<internal::system_mapping>(
            _allocator, _allocator, core::move(path), true);
      }
      break;
    }
    default:
      break;
  }

  return nullptr;
}

mapping_ptr factory::make_mapping(
    memory::allocator* _allocator, containers::string&& _path) const {
  internal::sanitize_path(_path);

  if (!_path.empty() && internal::exists_directory(_path)) {
    return memory::make_unique<internal::system_mapping>(
        _allocator, _allocator, core::move(_path), false);
  }

  return nullptr;
}

}  // namespace file_system
}  // namespace wn
