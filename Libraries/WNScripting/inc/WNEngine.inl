// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_SCRIPTING_ENGINE_INL__
#define __WN_SCRIPTING_ENGINE_INL__

#include <array>
#include "WNContainers/inc/WNDynamicArray.h"
#include "WNScripting/inc/WNEnums.h"
#include "WNScripting/inc/type_mangler.h"

namespace wn {
namespace scripting {

namespace {

template <size_t size>
struct expand_parameters {
  template <typename T, typename... Args>
  static bool run(containers::dynamic_array<containers::string_view>* _params) {
    _params->insert(_params->end(), {mangled_name<Args>::get()...});
    return false;
  }
};

template <>
struct expand_parameters<1> {
  template <typename T, typename P>
  static bool run(containers::dynamic_array<containers::string_view>* _params) {
    _params->push_back(mangled_name<T>::get());
    return false;
  }
};

template <>
struct expand_parameters<0> {
  template <typename T>
  static bool run(containers::dynamic_array<containers::string_view>*) {
    return false;
  }
};

template <typename U>
static typename core::enable_if<pass_by_reference<U>::value, U*>::type
pass_by_ref_if_needed(U& val) {
  return &val;
}

template <typename U>
static typename core::enable_if<!pass_by_reference<U>::value, U>::type
pass_by_ref_if_needed(U& val) {
  return val;
}

template <typename T, typename... Args>
T thunk(T (*func)(typename get_ref_passed_type<Args>::type...), Args... args) {
  return func(pass_by_ref_if_needed(args)...);
}

template <typename T, typename... Args>
T return_thunk(void (*func)(typename get_ref_passed_type<Args>::type..., T*),
    Args... args) {
  T t;
  return func(pass_by_ref_if_needed(args)..., &t);
}

}  // namespace

template <typename T, typename... Args>
bool inline engine::get_function(containers::string_view _name,
    script_function<T, Args...>* _function) const {
  containers::dynamic_array<containers::string_view> signature_types =
      m_type_manager.get_mangled_names<T, Args...>();

  containers::string s = get_mangled_name(m_allocator, _name);
  for (auto& param : signature_types) {
    s += param;
  }

  // TODO: move this to a compile-time check.
  bool use_thunk = false;
  auto types = m_type_manager.get_types<T, Args...>();
  for (auto& t : types) {
    if (m_type_manager.is_pass_by_reference(t)) {
      use_thunk = true;
    }
  }

  _function->m_function =
      reinterpret_cast<T (*)(typename get_ref_passed_type<Args>::type...)>(
          get_function_pointer(s));

  if (use_thunk && _function->m_function) {
    _function->m_thunk = &thunk<T, Args...>;
  } else {
    _function->m_thunk = nullptr;
  }

  return _function->m_function != nullptr;
}

template <typename R, typename... Args>
bool inline engine::register_function(
    containers::string_view _name, R (*_function)(Args...)) {
  containers::dynamic_array<const ast_type*> params =
      m_type_manager.get_types<R, Args...>();
  external_function f{_name, core::move(params)};
  m_type_manager.add_external(f, true, false);
  return register_c_function(
      _name, f.params, reinterpret_cast<void_f>(_function));
}

template <typename T>
void inline engine::register_cpp_type() {
  m_type_manager.register_cpp_type<T>(
      functional::function<void(containers::string_view, void*, bool)>(
          m_allocator,
          [this](containers::string_view _name, void* _ptr,
              bool _is_virtual) -> void {
            register_mangled_c_function(
                _name, reinterpret_cast<void_f>(_ptr), _is_virtual);
          }));
}

template <typename T>
void inline engine::register_child_cpp_type() {
  m_type_manager.register_child_cpp_type<T>(
      functional::function<void(containers::string_view, void*, bool)>(
          m_allocator,
          [this](containers::string_view _name, void* _ptr,
              bool _is_virtual) -> void {
            register_mangled_c_function(
                _name, reinterpret_cast<void_f>(_ptr), _is_virtual);
          }));
}

template <typename R, typename... Args>
R inline engine::invoke(
    const script_function<R, Args...>& _function, const Args&... _args) const {
  if (_function.m_thunk) {
    return _function.m_thunk(
        _function.m_function, core::forward<const Args>(_args)...);
  } else {
    // We should never end up in here, and have the reinterpret_cast do
    // anything. It is literally just here, because we don't compile-time
    // specialize this.
    // TODO: compile-time specialize this.
    using fntype = R (*)(Args...);
    auto fn = reinterpret_cast<fntype>(_function.m_function);
    return fn(core::forward<const Args>(_args)...);
  }
}

}  // namespace scripting
}  // namespace wn
#endif  // __WN_SCRIPTING_ENGINE_INL__
