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
T thunk(typename get_thunk_passed_type<T>::ret_type (*func)(
            typename get_thunk_passed_type<Args>::type...),
    Args... args) {
  return get_thunk_passed_type<T>::unwrap(
      func(get_thunk_passed_type<Args>::wrap(args)...));
}

template <bool b, typename T, typename... Args>
struct thunk_getter {
  static T return_thunk(
      void (*func)(typename get_thunk_passed_type<Args>::type...,
          core::add_pointer_t<typename get_thunk_passed_type<T>::ret_type>),
      Args... args) {
    typename get_thunk_passed_type<T>::ret_type t;
    func(get_thunk_passed_type<Args>::wrap(args)..., &t);
    return t;
  }
};

template <typename T, typename... Args>
struct thunk_getter<true, T, Args...> {
  static void return_thunk(
      void (*func)(typename get_thunk_passed_type<Args>::type..., void*),
      Args... args) {
    return func(get_thunk_passed_type<Args>::wrap(args)...);
  }
};

}  // namespace

template <typename T, typename... Args>
bool inline engine::get_function(containers::string_view _name,
    script_function<T, Args...>* _function) const {
  return get_function_internal<T, Args...>(_name, _function, false);
}

template <typename T, typename... Args>
bool inline engine::get_member_function(containers::string_view _name,
    script_function<T, Args...>* _function) const {
  return get_function_internal<T, Args...>(_name, _function, true);
}

template <typename T, typename... Args>
bool inline engine::get_function_internal(containers::string_view _name,
    script_function<T, Args...>* _function, bool _is_member) const {
  containers::dynamic_array<containers::string_view> signature_types =
      m_type_manager.get_mangled_names<T, Args...>();

  containers::string s = get_mangled_name(m_allocator, _name);
  if (_is_member) {
    auto types = m_type_manager.get_types<Args...>();
    WN_DEBUG_ASSERT(signature_types.size() > 1,
        "Cannot get member function with no object type");
    s = get_mangled_name(
        m_allocator, _name, m_type_manager.get_base_name(types[0]));
  }

  for (auto& param : signature_types) {
    s += param;
  }

  if (m_type_manager.is_pass_by_reference(m_type_manager.get_type<T>())) {
    s += m_type_manager.get_mangled_name<T>();
  }
  // TODO: move this to a compile-time check.
  bool use_thunk = false;
  bool use_return_thunk = false;
  auto types = m_type_manager.get_types<T, Args...>();
  bool is_return = true;
  for (auto& t : types) {
    if (m_type_manager.is_pass_by_reference(t)) {
      use_thunk = true;
      if (is_return) {
        use_return_thunk = true;
      }
    }
    is_return = false;
  }

  if (use_return_thunk) {
    _function->m_return_func =
        reinterpret_cast<void (*)(typename get_thunk_passed_type<Args>::type...,
            core::add_pointer_t<typename get_thunk_passed_type<T>::ret_type>)>(
            get_function_pointer(s));
  } else {
    _function->m_function =
        reinterpret_cast<typename get_thunk_passed_type<T>::ret_type (*)(
            typename get_thunk_passed_type<Args>::type...)>(
            get_function_pointer(s));
  }

  if (use_return_thunk && _function->m_return_func) {
    _function->m_return_thunk =
        &thunk_getter<core::is_same<T, void>::value, T, Args...>::return_thunk;
  } else if (use_thunk && _function->m_function) {
    _function->m_thunk = &thunk<T, Args...>;
  }

  return _function->m_function != nullptr ||
         _function->m_return_func != nullptr;
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
void inline engine::export_script_type() {
  m_type_manager.export_script_type<T>();
  memory::unique_ptr<T> t = memory::make_unique<T>(m_allocator);
  t->m_allocator = m_allocator;
  t->m_name = T::exported_name();
  t->m_engine = this;
  m_object_types[c_type_tag<T>::get_unique_identifier()] = core::move(t);
}

template <typename T>
bool inline engine::resolve_script_type() {
  auto t = m_object_types.find(c_type_tag<T>::get_unique_identifier());
  WN_DEBUG_ASSERT(
      t != m_object_types.end(), "Cannot resolve type that as not exported");
  T* ot = static_cast<T*>(t->second.get());

  script_type_importer<T> eti(this);
  ot->export_type(&eti);
  return eti.m_success;
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

template <typename T>
void inline fixup_return_type(
    T&, const containers::hash_map<void*,
            memory::unique_ptr<script_object_type>>&) {}

template <typename T>
void inline fixup_return_type(script_pointer<T>& t,
    const containers::hash_map<void*, memory::unique_ptr<script_object_type>>&
        object_type) {
  t.unsafe_set_type(reinterpret_cast<T*>(
      object_type[c_type_tag<T>::get_unique_identifier()].get()));
}

template <typename T>
void inline fixup_return_type(shared_script_pointer<T>& t,
    const containers::hash_map<void*, memory::unique_ptr<script_object_type>>&
        object_type) {
  t.unsafe_set_type(reinterpret_cast<T*>(
      object_type[c_type_tag<T>::get_unique_identifier()].get()));
}

template <typename R, typename... Args>
R inline engine::invoke(
    const script_function<R, Args...>& _function, Args... _args) const {
  if (_function.m_return_thunk) {
    R rt = _function.m_return_thunk(
        _function.m_return_func, core::forward<const Args>(_args)...);
    // TODO(awoloszyn): This should be replaced with a more intelligent thunk.
    // This (along with most of the rest of this function) can be switched to
    // be compile time.
    fixup_return_type(rt, m_object_types);
    return rt;
  } else if (_function.m_thunk) {
    R rt = _function.m_thunk(
        _function.m_function, core::forward<const Args>(_args)...);
    fixup_return_type(rt, m_object_types);
    return rt;
  } else {
    // We should never end up in here, and have the reinterpret_cast do
    // anything. It is literally just here, because we don't compile-time
    // specialize this.
    // TODO: compile-time specialize this.
    using fntype = typename get_thunk_passed_type<R>::ret_type (*)(
        typename get_thunk_passed_type<Args>::type...);

    auto fn = reinterpret_cast<fntype>(_function.m_function);
    R rt = fn(get_thunk_passed_type<Args>::wrap(_args)...);
    fixup_return_type(rt, m_object_types);
    return rt;
  }
}

}  // namespace scripting
}  // namespace wn
#endif  // __WN_SCRIPTING_ENGINE_INL__
