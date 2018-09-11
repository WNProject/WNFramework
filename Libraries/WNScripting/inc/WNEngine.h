// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_SCRIPTING_ENGINE_H__
#define __WN_SCRIPTING_ENGINE_H__

#include "WNContainers/inc/WNHashMap.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNMemory/inc/allocator.h"
#include "WNScripting/inc/WNEnums.h"
#include "WNScripting/inc/WNErrors.h"
#include "WNScripting/inc/type_manager.h"

namespace wn {
namespace scripting {
struct ast_type;

template <typename U, typename enable = void>
struct get_ref_passed_type {
  using type = U;
};

template <typename U>
struct get_ref_passed_type<U,
    typename core::enable_if<pass_by_reference<U>::value>::type> {
  using type = U*;
};

template <typename R, typename... Args>
class script_function final {
public:
  script_function() : m_function(nullptr) {}

  operator bool() const {
    return (m_function != nullptr);
  }

private:
  friend class engine;

  using fnType = R (*)(typename get_ref_passed_type<Args>::type...);
  fnType m_function = nullptr;

  using retFnType = void (*)(typename get_ref_passed_type<Args>::type..., R*);
  retFnType m_return_func = nullptr;

  using thunkType = R (*)(fnType, Args...);
  thunkType m_thunk = nullptr;

  using retThunkType = R (*)(retFnType, Args...);
  retThunkType m_return_thunk = nullptr;
};

using script_tag = char[];

template <char const* Type, typename Parent = void>
struct script_object_type final {};

template <typename ObjectType, script_tag Name, typename R, typename... Args>
struct script_member_function final {
  R (*m_function)(Args...);
};

template <typename ObjectType, script_tag Name, typename R, typename... Args>
struct script_virtual_member_function final {
  uint32_t m_vtable_offset;
};

// Implementors of this class are expected to take
// in files, and give access to function pointer callable from
// C++.
class engine {
public:
  engine(memory::allocator* _allocator, logging::log* _log)
    : m_num_warnings(0),
      m_num_errors(0),
      m_allocator(_allocator),
      m_type_manager(_allocator, _log) {
    // These exists to stop the compiler from assuming these are unused.
    (void)assign_type_names;
    (void)short_circuit_type_names;
    (void)type_classification_names;
    (void)arithmetic_type_names;
    (void)type_array_depth;
    (void)unary_type_names;
    (void)post_un_names;
  }

  virtual ~engine() {}

  // Implemented in subclasses. Given a file, determine and prepare
  // and function pointers that may exist.
  virtual parse_error parse_file(const containers::string_view file) = 0;

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
  template <typename R, typename... Args>
  bool get_function(containers::string_view _name,
      script_function<R, Args...>* _function) const;

  template <typename R, typename... Args>
  R invoke(
      const script_function<R, Args...>& _function, const Args&... _args) const;

  template <typename R, typename... Args>
  bool inline register_function(
      containers::string_view _name, R (*_function)(Args...));

  template <typename T>
  void register_cpp_type();

  template <typename T>
  void register_child_cpp_type();

  using void_func = script_function<void>;

protected:
  size_t m_num_warnings;
  size_t m_num_errors;
  memory::allocator* m_allocator;
  virtual void_f get_function_pointer(containers::string_view _name) const = 0;
  virtual bool register_c_function(containers::string_view name,
      containers::contiguous_range<const ast_type*> _types,
      void_f function) = 0;
  virtual bool register_mangled_c_function(
      containers::string_view _name, void_f _function, bool _is_virtual) = 0;

  type_manager m_type_manager;
};

}  // namespace scripting
}  // namespace wn

#include "WNScripting/inc/WNEngine.inl"
#endif  //__WN_SCRIPTING_ENGINE_H__
