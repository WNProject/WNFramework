// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_SCRIPTING_ENGINE_INL__
#define __WN_SCRIPTING_ENGINE_INL__

#include <array>
#include "WNContainers/inc/WNDynamicArray.h"
#include "WNScripting/inc/WNEnums.h"
#include "WNScripting/inc/WNTypeValidator.h"

namespace wn {
namespace scripting {

template <typename T>
struct type_registry {
  // Note: change this implementation if we
  // ever plan on shared-library functionality
  // for scripting. The registered type indices
  // will probably not be consistent across modules.
  static void* get_registered_index() {
    static wn_size_t i = 0;
    ++i;
    return &(i);
  }
};

void WN_INLINE engine::register_builtin_types() {
  m_registered_types[type_registry<int32_t>::get_registered_index()] =
      static_cast<uint32_t>(scripting::type_classification::int_type);
  m_registered_types[type_registry<bool>::get_registered_index()] =
      static_cast<uint32_t>(scripting::type_classification::bool_type);
  m_registered_types[type_registry<char>::get_registered_index()] =
      static_cast<uint32_t>(scripting::type_classification::char_type);
  m_registered_types[type_registry<void>::get_registered_index()] =
      static_cast<uint32_t>(scripting::type_classification::void_type);
  m_registered_types[type_registry<float>::get_registered_index()] =
      static_cast<uint32_t>(scripting::type_classification::float_type);
}

namespace {

uint32_t get_registered_index(
    const containers::hash_map<void*, uint32_t>& _types, bool* _error,
    void* _idx) {
  auto type = _types.find(_idx);
  if (type == _types.end()) {
    *_error = true;
    return 0u;
  }
  return type->second;
}

template <size_t size>
struct expand_parameters {
  template <typename T, typename... Args>
  static bool run(containers::dynamic_array<uint32_t>* _params,
      const containers::hash_map<void*, uint32_t>& _registered_types) {
    bool error = false;
    _params->insert(
        _params->end(), {get_registered_index(_registered_types, &error,
                            type_registry<Args>::get_registered_index())...});
    return error;
  }
};

template <>
struct expand_parameters<1> {
  template <typename T, typename P>
  static bool run(containers::dynamic_array<uint32_t>* _params,
      const containers::hash_map<void*, uint32_t>& _registered_types) {
    bool error = false;
    _params->push_back(get_registered_index(
        _registered_types, &error, type_registry<T>::get_registered_index()));
    return error;
  }
};

template <>
struct expand_parameters<0> {
  template <typename T>
  static bool run(containers::dynamic_array<uint32_t>*,
      const containers::hash_map<void*, uint32_t>&) {
    return false;
  }
};

}  // anonymous namespace

template <typename T, typename... Args>
bool WN_INLINE engine::get_function_pointer(
    containers::string_view _name, T (*&function)(Args...)) const {
  containers::dynamic_array<uint32_t> parameters(m_allocator);
  bool error = false;

  uint32_t return_type = get_registered_index(
      m_registered_types, &error, type_registry<T>::get_registered_index());
  if (error) {
    return false;
  }

  error = expand_parameters<sizeof...(Args)>::template run<T, Args...>(
      &parameters, m_registered_types);
  if (error) {
    return false;
  }
  function = reinterpret_cast<T (*)(Args...)>(
      get_function(m_validator->get_mangled_name(_name, return_type,
          containers::contiguous_range<uint32_t>(parameters))));
  return function != nullptr;
}

}  // namespace scripting
}  // namespace wn
#endif  // __WN_SCRIPTING_ENGINE_INL__
