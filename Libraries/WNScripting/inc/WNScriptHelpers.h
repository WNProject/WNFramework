// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_SCRIPTING_SCRIPT_HELPERS_H__
#define __WN_SCRIPTING_SCRIPT_HELPERS_H__

#include "WNContainers/inc/WNArray.h"
#include "WNContainers/inc/WNHashMap.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNCore/inc/type_traits.h"
#include "WNCore/inc/utilities.h"
#include "WNFunctional/inc/WNFunction.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/allocator.h"
#include "WNMemory/inc/unique_ptr.h"
#include "WNScripting/inc/WNEnums.h"
#include "WNScripting/inc/WNScriptTLS.h"
#include "WNScripting/inc/forward.h"

#include <atomic>

namespace wn {
namespace scripting {

class script_file;
struct ast_script_file;
class type_manager;
struct ast_type;
class engine;

struct struct_info final {
  size_t vtable_offset;
};

struct external_function final {
  containers::string_view name;
  containers::dynamic_array<const ast_type*> params;
  bool is_virtual;
  bool is_override;
  uint32_t virtual_index;
};

void do_engine_free(const engine* _engine, void* v);

template <typename T>
struct pass_by_reference : core::false_type {};

template <typename T>
struct needs_thunk : pass_by_reference<T> {};

// Use this type instead of normal size_t in order for
// size_t to automatically work between scripting and C++.
// This is because size_t is not a "proper" type in most
// compilers.
struct wn_size_t final {
  size_t val;
};

template <typename T, size_t S = 0>
struct wn_array_ptr;

template <typename T, size_t S = 0>
struct wn_array final {
private:
  uint32_t _size = S;
  T _values[S == 0 ? 1 : S];

public:
  wn_array_ptr<T, 0> operator&();

  T& operator[](size_t i) {
    return _values[i];
  }

  const T& operator[](size_t i) const {
    return _values[i];
  }

  size_t size() const {
    return _size;
  }
};

template <typename T, size_t S>
struct wn_array_ptr final {
  wn_array<T, S>* ptr;

  wn_array<T, S>& operator*() {
    return *ptr;
  }

  wn_array<T, S>* operator->() {
    return ptr;
  }

  operator wn_array_ptr<T, 0>() {
    return wn_array_ptr<T, 0>{reinterpret_cast<wn_array<T, 0>*>(ptr)};
  }
};

template <typename T, size_t S>
wn_array_ptr<T, 0> wn_array<T, S>::operator&() {
  static_assert(sizeof(this) == sizeof(wn_array_ptr<T, 0>),
      "Invalid conversion to array_ptr");

  auto* t = this;
  return *reinterpret_cast<wn_array_ptr<T, 0>*>(&t);
}

// Default slice
template <typename T, size_t S = 1, typename = core::enable_if_t<true>>
struct slice {
private:
  static const size_t N_Vals = 1 + ((S - 1) * 2);

  T* _value;
  size_t size_values[N_Vals] = {0};

public:
  slice() : _value(nullptr) {}

  inline slice(T* _val, containers::array<size_t, N_Vals> _sizes_strides)
    : _value(_val) {
    memory::memory_copy(&size_values[0], &_sizes_strides[0], N_Vals);
  }

  T& operator[](size_t i) {
    return _value[i];
  }

  size_t size() {
    return size_values[0];
  }
};

template <typename T, size_t S>
struct slice<T, S, typename core::enable_if<(S == 0)>::type> {};

template <typename T, size_t S>
struct slice<T, S, typename core::enable_if<(S > 1)>::type> {
private:
  static const size_t N_Vals = 1 + ((S - 1) * 2);

  T* _value;
  size_t size_values[N_Vals] = {0};

public:
  slice() : _value(nullptr) {}

  inline slice(T* _val, containers::array<size_t, N_Vals> _sizes_strides)
    : _value(_val) {
    memory::memory_copy(&size_values[0], &_sizes_strides[0], N_Vals);
  }
  size_t size() {
    return size_values[0];
  }

  slice<T, S - 1> operator[](size_t i) {
    T* nT = _value;
    size_t new_values[N_Vals - 2];
    nT = reinterpret_cast<T*>(
        reinterpret_cast<uint8_t*>(nT) + size_values[1] * i);
    memory::memory_copy(&new_values[0], &size_values[2], N_Vals - 2);
    return slice<T, S - 1>(nT, new_values);
  }
};

template <typename U, size_t S>
struct pass_by_reference<slice<U, S>> : core::true_type {};

template <typename T>
struct needs_thunk<script_pointer<T>> : core::true_type {};

template <typename T>
struct is_script_pointer : core::false_type {};

template <typename T>
struct is_script_pointer<script_pointer<T>> : core::true_type {};

template <typename T>
struct is_shared_script_pointer : core::false_type {};

template <typename T>
struct is_shared_script_pointer<shared_script_pointer<T>> : core::true_type {};

template <typename T>
struct is_shared_cpp_pointer : core::false_type {};

template <typename T>
struct is_shared_cpp_pointer<shared_cpp_pointer<T>> : core::true_type {};

template <typename T>
struct needs_thunk<shared_script_pointer<T>> : core::true_type {};

template <typename T>
struct needs_thunk<shared_cpp_pointer<T>> : core::true_type {};

template <typename U, typename enable = core::enable_if_t<true>>
struct get_thunk_passed_type {
  using type = U;
  using ret_type = U;

  static inline U wrap(const U& u) {
    return u;
  }
  static inline U unwrap(U u) {
    return u;
  }
};

template <typename U>
struct get_thunk_passed_type<U,
    typename core::enable_if<pass_by_reference<U>::value>::type> {
  using type = U*;
  using ret_type = U;

  static inline U* wrap(U& u) {
    return &u;
  }

  static inline U unwrap(U* u) {
    return *u;
  }

  static inline U unwrap(U u) {
    return u;
  }
};

template <typename U>
struct get_thunk_passed_type<U,
    typename core::enable_if<core::disjunction<is_script_pointer<U>,
        is_shared_script_pointer<U>, is_shared_cpp_pointer<U>>::value>::type> {
  using type = typename U::value_type*;
  using ret_type = typename U::value_type*;
  static inline typename U::value_type* wrap(const U& _u) {
    U& u = const_cast<U&>(_u);
    void* v = u.unsafe_pass();

    return reinterpret_cast<typename U::value_type*>(v);
  }

  static inline U unwrap(typename U::value_type* u) {
    return U(u);
  }
};

template <typename T>
inline T& fixup_return_type(T& _i) {
  return _i;
}

template <typename T>
inline script_pointer<T>& fixup_return_type(script_pointer<T>& t) {
  t.unsafe_set_type(reinterpret_cast<T*>(
      (*g_scripting_tls->_object_types)[core::type_id<T>::value()].get()));
  return t;
}

template <typename T>
inline shared_script_pointer<T>& fixup_return_type(
    shared_script_pointer<T>& t) {
  t.unsafe_set_type(reinterpret_cast<T*>(
      (*g_scripting_tls->_object_types)[core::type_id<T>::value()].get()));
  return t;
}

template <typename T>
inline shared_cpp_pointer<T>& fixup_return_type(shared_cpp_pointer<T>& t) {
  t.unsafe_set_engine_free(g_scripting_tls->_engine, &do_engine_free);
  return t;
}

// Simple helper that parses a script and runs any
// passes that are required to make the AST valid.
// _handle_include is a callback that is called when an #include
// is encountered and needs to be handled. It recieves the string_view name
// and returns true/false based on the success of the handling.
// Furthermore, it must fill _type_manager with any necessary type it
// encounters.
memory::unique_ptr<ast_script_file> parse_script(memory::allocator* _allocator,
    const containers::string_view file_name, containers::string_view view,
    functional::function<bool(containers::string_view)> _handle_include,
    type_manager* _type_manager, bool _print_ast_on_failure, logging::log* _log,
    size_t* _num_warnings, size_t* _num_errors);

}  // namespace scripting
}  // namespace wn

#endif  // __WN_SCRIPTING_SCRIPT_HELPERS_H__
