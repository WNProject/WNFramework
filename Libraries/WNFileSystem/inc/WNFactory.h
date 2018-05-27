// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_FILE_SYSTEM_FACTORY_H__
#define __WN_FILE_SYSTEM_FACTORY_H__

#include "WNContainers/inc/WNString.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNFileSystem/inc/WNMapping.h"
#include "WNMemory/inc/allocator.h"

namespace wn {
namespace entry {

struct system_data;

}  // namespace entry

namespace file_system {

enum class mapping_type {
  memory_backed,
  scratch_directory,
  executable_directory,
  current_working_directory,
  development_assets
};

class factory {
public:
  WN_FORCE_INLINE factory(
      memory::allocator* _allocator, const entry::system_data* _system_data)
    : m_system_data(_system_data), m_allocator(_allocator) {}

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
  const entry::system_data* m_system_data;
  memory::allocator* m_allocator;
};

}  // namespace file_system
}  // namespace wn

#endif  // __WN_FILE_SYSTEM_FACTORY_H__
