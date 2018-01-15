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

}  // anonymous namespace

template <typename T, typename... Args>

bool inline engine::get_function(containers::string_view _name,
    script_function<T, Args...>& _function) const {
  containers::dynamic_array<containers::string_view> parameters(m_allocator);
  bool error = false;

  containers::string_view return_type = mangled_name<T>::get();
  if (error) {
    return false;
  }

  error =
      expand_parameters<sizeof...(Args)>::template run<T, Args...>(&parameters);
  if (error) {
    return false;
  }

  containers::string s = get_mangled_name(m_allocator, _name);
  s += return_type;
  for (auto& param : parameters) {
    s += param;
  }

  _function.m_function =
      reinterpret_cast<T (*)(Args...)>(get_function_pointer(s));
  return _function.m_function != nullptr;
}

template <typename T, typename... Args>
T inline engine::invoke(
    const script_function<T, Args...>& _function, const Args&... _args) const {
  return _function.m_function(core::forward<const Args>(_args)...);
}

}  // namespace scripting
}  // namespace wn
#endif  // __WN_SCRIPTING_ENGINE_INL__
