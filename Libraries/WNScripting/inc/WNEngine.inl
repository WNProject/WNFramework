// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_SCRIPTING_ENGINE_INL__
#define __WN_SCRIPTING_ENGINE_INL__

#include "WNContainers/inc/WNDynamicArray.h"
#include "WNMemory/inc/allocation.h"
#include "WNScripting/inc/WNEnums.h"
#include "WNScripting/inc/type_mangler.h"

#include <array>

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

  _function->m_function = get_function_pointer(s);

  return _function->m_function != nullptr;
}

template <typename T>
size_t inline engine::get_vtable_offset() {
  const ast_type* t = m_type_manager.get_type<T>();
  return get_vtable_offset(t);
}

template <typename T, typename... Args>
size_t inline engine::get_virtual_function(
    containers::string_view _name) const {
  containers::dynamic_array<containers::string_view> signature_types =
      m_type_manager.get_mangled_names<T, Args...>();

  auto types = m_type_manager.get_types<T, Args...>();
  size_t virtual_offset = m_type_manager.get_virtual_function(_name, types);

  return virtual_offset;
}

template <typename R, typename... Args>
struct member_maker {
  static const bool is_ret_by_ref = pass_by_reference<R>::value;
  template <R (*fn)(Args...)>
  static typename get_thunk_passed_type<R>::ret_type member_thunk(
      typename get_thunk_passed_type<Args>::type... args) {
    return get_thunk_passed_type<R>::wrap(
        (*fn)(get_thunk_passed_type<Args>::unwrap(args)...));
  }

  template <R (*fn)(Args...)>
  static void return_member_thunk(
      typename get_thunk_passed_type<Args>::type... args,
      typename get_thunk_passed_type<R>::ret_type* _ret) {
    *_ret = get_thunk_passed_type<R>::wrap(
        (*fn)(get_thunk_passed_type<Args>::unwrap(args)...));
  }
};

template <typename... Args>
struct member_maker<void, Args...> {
  static const bool is_ret_by_ref = false;
  template <void (*fn)(Args...)>
  static void member_thunk(typename get_thunk_passed_type<Args>::type... args) {
    return (*fn)(get_thunk_passed_type<Args>::unwrap(args)...);
  }

  template <void (*fn)(Args...)>
  static void return_member_thunk(
      typename get_thunk_passed_type<Args>::type... args) {
    (*fn)(get_thunk_passed_type<Args>::unwrap(args)...);
  }
};

template <typename R, typename... Args>
inline member_maker<R, Args...> get_member_maker(R (*)(Args...)) {
  return member_maker<R, Args...>{};
};

template <typename F, F fptr>
void* get_thunk() {
  void* v;
  if (decltype(get_member_maker(fptr))::is_ret_by_ref) {
    auto mt =
        &decltype(get_member_maker(fptr))::template return_member_thunk<fptr>;
    memcpy(&v, &mt, sizeof(uintptr_t));
  } else {
    auto mt = &decltype(get_member_maker(fptr))::template member_thunk<fptr>;
    memcpy(&v, &mt, sizeof(uintptr_t));
  }
  return v;
}

template <typename R, typename... Args>
containers::dynamic_array<const ast_type*> get_function_params(
    R (*)(Args...), type_manager* _manager) {
  return _manager->get_types<R, Args...>();
}

template <typename F, F _function>
bool inline engine::register_function(containers::string_view _name) {
  containers::dynamic_array<const ast_type*> params =
      get_function_params(_function, &m_type_manager);
  void* fn = get_thunk<F, _function>();
  external_function f{_name, core::move(params)};
  m_type_manager.add_external(f, true, false);
  return register_c_function(_name, f.params, reinterpret_cast<void_f>(fn));
}

template <typename F, typename... Args>
bool inline engine::register_resource(
    memory::unique_ptr<resource_manager> res) {
  const ast_type* t = m_type_manager.get_type<F>();
  // TODO(awoloszyn): Eventually add some more testing against these "Args".
  // This will automatically get picked up by the re-writing, but
  // might be cleaner here.

  if (!m_type_manager.register_resource_type(t, res.get())) {
    return false;
  }
  auto ptr = res.get();
  m_resources.push_back(core::move(res));
  return register_resource(ptr, t);
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

template <>
void inline engine::export_script_type<void>() {}

template <typename T>
void inline engine::export_script_type() {
  export_script_type<typename T::parent_type>();
  if (m_object_types.find(core::type_id<T>::value()) != m_object_types.end()) {
    return;
  }

  m_type_manager.export_script_type<T>();
  memory::unique_ptr<T> t = memory::make_unique<T>(m_allocator);
  t->m_allocator = m_allocator;
  t->m_name = T::exported_name();
  t->m_engine = this;
  fill_parent_type<typename T::parent_type>(t.get());
  m_object_types[core::type_id<T>::value()] = core::move(t);
}

template <typename T>
bool inline engine::register_named_constant(
    const containers::string_view& _name, const T& value) {
  const ast_type* type = m_type_manager.get_type<T>();
  containers::string sc = get_script_constant<T>::str_const(m_allocator, value);
  return m_type_manager.add_named_constant(type, _name, sc);
}

template <typename T>
bool inline engine::resolve_script_type() {
  auto t = m_object_types.find(core::type_id<T>::value());
  WN_DEBUG_ASSERT(
      t != m_object_types.end(), "Cannot resolve type that is not exported");
  T* ot = static_cast<T*>(t->second.get());
  return resolve_script_type_internal(ot);
}

template <typename T>
bool inline engine::resolve_script_type_internal(T* t) {
  typename T::parent_type* parent = static_cast<typename T::parent_type*>(t);
  bool parent_success = resolve_script_type_internal(parent);
  script_type_importer<T> eti(this);
  (t->T::export_type)(&eti);
  return eti.m_success && parent_success;
}

template <>
bool inline engine::fill_parent_type<void>(script_object_type* _t) {
  _t->m_parent = nullptr;
  return true;
}

template <typename T>
bool inline engine::fill_parent_type(script_object_type* _t) {
  auto t = m_object_types.find(core::type_id<T>::value());
  WN_DEBUG_ASSERT(
      t != m_object_types.end(), "Cannot resolve type that as not exported");
  _t->m_parent = t->second.get();
  return true;
}

template <>
bool inline engine::resolve_script_type_internal<void>(void*) {
  return true;
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

template <typename T, typename... Args>
shared_cpp_pointer<T> inline engine::make_shared_cpp(Args&&... args) {
  void* v = allocate_shared(sizeof(T));

  memory::construct_at<T>(v, core::forward<Args>(args)...);

  return shared_cpp_pointer<T>(reinterpret_cast<T*>(v), this, &do_engine_free);
}

template <typename Enable, typename R, typename... Args>
struct engine::invoke_wrapper : core::non_constructable {
  static inline R invoke(
      const script_function<R, Args...>& _function, Args... _args) {
    auto adjusted_function =
        reinterpret_cast<typename get_thunk_passed_type<R>::ret_type (*)(
            typename get_thunk_passed_type<Args>::type...)>(
            _function.m_function);

    return get_thunk_passed_type<R>::unwrap(
        adjusted_function(get_thunk_passed_type<Args>::wrap(_args)...));
  }
};

template <typename R, typename... Args>
struct engine::invoke_wrapper<core::enable_if_t<pass_by_reference<R>::value>, R,
    Args...> : core::non_constructable {
  static inline R invoke(
      const script_function<R, Args...>& _function, Args... _args) {
    auto adjusted_function =
        reinterpret_cast<void (*)(typename get_thunk_passed_type<Args>::type...,
            core::add_pointer_t<typename get_thunk_passed_type<R>::ret_type>)>(
            _function.m_function);
    typename get_thunk_passed_type<R>::ret_type ret;

    adjusted_function(get_thunk_passed_type<Args>::wrap(_args)..., &ret);

    return get_thunk_passed_type<R>::unwrap(ret);
  }
};

template <typename Enable, typename... Args>
struct engine::invoke_wrapper<Enable, void, Args...> : core::non_constructable {
  static inline void invoke(
      const script_function<void, Args...>& _function, Args... _args) {
    auto adjusted_function = reinterpret_cast<void (*)(
        typename get_thunk_passed_type<Args>::type...)>(_function.m_function);

    adjusted_function(get_thunk_passed_type<Args>::wrap(_args)...);
  }
};

template <typename R, typename... Args>
R inline engine::invoke(
    const script_function<R, Args...>& _function, Args... _args) const {
  tls_resetter reset(&m_tls_data);

  return invoke_wrapper<void, R, Args...>::invoke(
      _function, core::forward<Args>(_args)...);
}
containers::string engine::get_resource_data(containers::string_view _file) {
  return m_type_manager.get_resource_data(_file);
}

}  // namespace scripting
}  // namespace wn

#endif  // __WN_SCRIPTING_ENGINE_INL__
