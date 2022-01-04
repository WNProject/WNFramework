// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_SCRIPTING_SCRIPT_HELPERS_H__
#define __WN_SCRIPTING_SCRIPT_HELPERS_H__

#include "WNContainers/inc/WNArray.h"
#include "WNContainers/inc/WNHashMap.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNFunctional/inc/WNFunction.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/allocator.h"
#include "WNMemory/inc/unique_ptr.h"
#include "WNScripting/inc/WNEnums.h"
#include "WNScripting/inc/WNScriptTLS.h"
#include "WNScripting/inc/forward.h"
#include "core/inc/type_traits.h"
#include "core/inc/utilities.h"

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

// Default slice
template <typename T, size_t S = 1, typename = core::enable_if_t<true>>
struct slice {
private:
  static const size_t N_Vals = 1 + ((S - 1) * 2);

  T* _value;
  size_t size_values[N_Vals] = {0};

public:
  slice() : _value(nullptr) {
    for (size_t i = 0; i < N_Vals; ++i) {
      size_values[i] = 0;
    }
  }

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

  T* begin() {
    return _value;
  }
  T* end() {
    return _value + size_values[0];
  }
  const T* begin() const {
    return _value;
  }
  const T* end() const {
    return _value + size_values[0];
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
  T* begin() {
    return _value;
  }
  T* end() {
    return _value + size_values[0] + 1;
  }
};

template <typename T>
struct is_array_pointer : core::false_type {};

template <typename T>
struct is_array_pointer<wn_array<T>> : core::true_type {};

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

template <typename T>
struct is_array : core::false_type {};

template <typename T>
struct is_array<wn_array<T>> : core::true_type {};

template <typename U,
    typename = typename core::enable_if<core::is_same<int32_t, U>::value>::type>
struct get_script_constant {
  static containers::string str_const(
      memory::allocator* _allocator, const U& _constant) {
    char buff[11];
    memory::writeint32(buff, _constant, 11);
    return containers::string(_allocator, buff);
  }
};

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
    typename core::enable_if<core::is_same<void, U>::value>::type> {
  using type = U;
  using ret_type = U;
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

  static inline U unwrap(typename U::value_type* _u) {
    U u(_u);
    fixup_return_type(u);
    return u;
  }
};

template <typename U>
struct get_thunk_passed_type<U,
    typename core::enable_if<is_array<U>::value>::type> {
  using type = void*;
  using ret_type = void*;
  static inline void* wrap(const U& _u) {
    void* v = _u.m_ptr;
    return v;
  }

  static inline U unwrap(void* _u) {
    U u(_u);
    fixup_return_type(u);
    return u;
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

template <typename T>
inline wn_array<script_pointer<T>>& fixup_return_type(
    wn_array<script_pointer<T>>& t) {
  t.unsafe_set_type(reinterpret_cast<T*>(
      (*g_scripting_tls->_object_types)[core::type_id<T>::value()].get()));
  return t;
}

template <typename T>
inline wn_array<shared_script_pointer<T>>& fixup_return_type(
    wn_array<shared_script_pointer<T>>& t) {
  t.unsafe_set_type(reinterpret_cast<T*>(
      (*g_scripting_tls->_object_types)[core::type_id<T>::value()].get()));
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
    functional::function<bool(
        containers::string_view, containers::string_view, bool instantiated)>
        _handle_resource,
    type_manager* _type_manager, bool _print_ast_on_failure, logging::log* _log,
    size_t* _num_warnings, size_t* _num_errors);

}  // namespace scripting

namespace logging {
template <typename T, typename BuffType>
struct log_type_helper<scripting::wn_array<T>, BuffType> {
  inline static bool do_log(const scripting::wn_array<T>& _0, BuffType* _buffer,
      size_t& _buffer_left) {
    size_t last_buffer_left = _buffer_left;
    if (!log_type_helper<char[2], BuffType>::do_log(
            "[", _buffer, last_buffer_left)) {
      return false;
    }

    for (size_t i = 0; i < _0.size(); ++i) {
      if (i != 0) {
        if (!log_type_helper<char[3], BuffType>::do_log(", ",
                _buffer + (_buffer_left - last_buffer_left),
                last_buffer_left)) {
          return false;
        }
      }
      if (!log_type_helper<T, BuffType>::do_log(_0[i],
              _buffer + (_buffer_left - last_buffer_left), last_buffer_left)) {
        return false;
      }
    }

    if (!log_type_helper<char[2], BuffType>::do_log("]",
            _buffer + (_buffer_left - last_buffer_left), last_buffer_left)) {
      return false;
    }

    _buffer_left = last_buffer_left;
    return (true);
  }
};

}  // namespace logging
}  // namespace wn

#endif  // __WN_SCRIPTING_SCRIPT_HELPERS_H__
