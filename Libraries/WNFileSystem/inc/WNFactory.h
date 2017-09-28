// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_FILE_SYSTEM_FACTORY_H__
#define __WN_FILE_SYSTEM_FACTORY_H__

#include "WNContainers/inc/WNString.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNFileSystem/inc/WNMapping.h"
#include "WNMemory/inc/WNAllocator.h"

namespace wn {
namespace file_system {

enum class mapping_type {
  memory_backed,
  scratch_directory,
  executable_directory,
  current_working_directory
};

class factory {
public:
  WN_FORCE_INLINE factory(memory::allocator* _allocator)
    : m_allocator(_allocator) {}

  virtual containers::string get_executable_path() const;
  virtual containers::string get_current_working_path() const;

  virtual mapping_ptr make_mapping(
      memory::allocator* _allocator, const mapping_type _mapping_type) const;

  virtual mapping_ptr make_mapping(
      memory::allocator* _allocator, containers::string&& _path) const;

  WN_FORCE_INLINE mapping_ptr make_mapping(
      memory::allocator* _allocator, const containers::string& _path) const {
    return make_mapping(_allocator, containers::string(_path));
  }

  WN_FORCE_INLINE mapping_ptr make_mapping(memory::allocator* _allocator,
      const containers::string_view _path) const {
    return make_mapping(_allocator, _path.to_string(_allocator));
  }

private:
  memory::allocator* m_allocator;
};

}  // namespace file_system
}  // namespace wn

#endif  // __WN_FILE_SYSTEM_FACTORY_H__
