// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_SCRIPTING_ENGINE_H__
#define __WN_SCRIPTING_ENGINE_H__

#include "WNContainers/inc/WNHashMap.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNScripting/inc/WNErrors.h"

namespace wn {
namespace scripting {
class type_validator;

class engine {
public:
  engine(type_validator* _validator, memory::allocator* _allocator)
    : m_num_warnings(0),
      m_num_errors(0),
      m_allocator(_allocator),
      m_validator(_validator),
      m_registered_types(_allocator) {
    register_builtin_types();
  }
  typedef void (*void_func)();
  virtual ~engine() {}
  virtual parse_error parse_file(const char* file) = 0;

  wn_size_t errors() const {
    return m_num_errors;
  }
  wn_size_t warnings() const {
    return m_num_warnings;
  }

  template <typename T, typename... Args>
  bool get_function_pointer(
      containers::string_view _name, T (*&function)(Args...)) const;

protected:
  wn_size_t m_num_warnings;
  wn_size_t m_num_errors;
  memory::allocator* m_allocator;
  type_validator* m_validator;
  virtual void_func get_function(containers::string_view _name) const = 0;

private:
  void register_builtin_types();
  containers::hash_map<void*, uint32_t> m_registered_types;
};

}  // namespace scripting
}  // namespace wn

#include "WNScripting/inc/WNEngine.inl"
#endif  //__WN_SCRIPTING_ENGINE_H__