// Copyright (c) 2016, WNProject Authors. All rights reserved.
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

template <typename T, typename... Args>
class script_function {
  T (*m_function)(Args...);
  friend class engine;
};

// Implementors of this class are expected to take
// in files, and give access to function pointer callable from
// C++.
// The type_validator is expected to be set up and unchangable
// before the first call to parse_file or get_function_pointer.
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

  virtual ~engine() {}

  // Implemented in subclasses. Given a file, determine and prepare
  // and function pointers that may exist.
  virtual parse_error parse_file(const char* file) = 0;

  // Returns the number of errors that were encountered in
  // parsing the file.
  size_t errors() const {
    return m_num_errors;
  }

  // Returns the number of warnings encountered in parsing the file.
  size_t warnings() const {
    return m_num_warnings;
  }

  // Given a C-style function pointer, and a function name,
  // returns a function pointer from a parsed file that matches the
  // signature. The return types and all parameter types must
  // match exactly.
  template <typename T, typename... Args>
  bool WN_INLINE get_function(containers::string_view _name,
      script_function<T, Args...>& _function) const;
  using void_func = script_function<void>;

  template <typename T, typename... Args>
  T WN_INLINE invoke(
      const script_function<T, Args...>& _function, const Args&... _args) const;

protected:
  size_t m_num_warnings;
  size_t m_num_errors;
  memory::allocator* m_allocator;
  type_validator* m_validator;
  using void_f = void(*)();
  virtual void_f get_function_pointer(containers::string_view _name) const = 0;

private:
  void register_builtin_types();
  containers::hash_map<void*, uint32_t> m_registered_types;
};

}  // namespace scripting
}  // namespace wn

#include "WNScripting/inc/WNEngine.inl"
#endif  //__WN_SCRIPTING_ENGINE_H__