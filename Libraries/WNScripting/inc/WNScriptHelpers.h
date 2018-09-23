// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_SCRIPTING_SCRIPT_HELPERS_H__
#define __WN_SCRIPTING_SCRIPT_HELPERS_H__

#include "WNContainers/inc/WNArray.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNFunctional/inc/WNFunction.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/allocator.h"
#include "WNMemory/inc/unique_ptr.h"
#include "WNScripting/inc/WNEnums.h"

#include <atomic>
namespace wn {
namespace scripting {
class script_file;
struct ast_script_file;
class type_manager;
struct ast_type;
class engine;

struct external_function {
  containers::string_view name;
  containers::dynamic_array<const ast_type*> params;
  bool is_virtual;
  bool is_override;
  uint32_t virtual_index;
};

template <typename T>
struct pass_by_reference {
  static const bool value = false;
};

template <typename T>
struct needs_thunk {
  static const bool value = pass_by_reference<T>::value;
};

// Use this type instead of normal size_t in order for
// size_t to automatically work between scripting and C++.
// This is because size_t is not a "proper" type in most
// compilers.
struct wn_size_t {
  size_t val;
};

template <typename T, size_t S = 0>
struct wn_array_ptr;

template <typename T, size_t S = 0>
struct wn_array {
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
struct wn_array_ptr {
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
  auto* t = this;
  static_assert(sizeof(this) == sizeof(wn_array_ptr<T, 0>),
      "Invalid conversion to array_ptr");
  return *reinterpret_cast<wn_array_ptr<T, 0>*>(&t);
}

// Default slice
template <typename T, size_t S = 1, typename = void>
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
struct pass_by_reference<slice<U, S>> {
  static const bool value = true;
};

struct script_object_type {
  void free(void* val) {
    //TODO(awoloszyn): Use the allocator here.
    // For now we allocated with malloc, so free with malloc
    ::free(val);
  }

  memory::allocator* m_allocator;
  engine* m_engine;
  containers::string_view m_name;
};

template <typename T>
class script_pointer {
public:
  using value_type = T;
  template <typename X, typename... Args>
  typename X::ret_type invoke(X T::*v, Args... args) {
    return (type->*v).do_(type->m_engine, this, args...);
  }

  ~script_pointer() {}

  script_pointer(const script_pointer& _other)
    : val(_other.val), type(_other.type) {
  }

  script_pointer(script_pointer&& _other)
    : val(_other.val), type(_other.type) {
  }
  script_pointer() : val(nullptr), type(nullptr) {}
  
  void* unsafe_ptr() {
    return val;
  }
  
  void unsafe_set_type(T* _type) {
    type = _type;
  }

  script_pointer(void* t) : val(t) {
  }
  script_pointer(void* _v, T* _t) : val(_v), type(_t) {}

private:

  void* val;
  T* type;
  friend class engine;
};

template <typename T>
struct needs_thunk<script_pointer<T>> {
  static const bool value = true;
};

template <typename T>
class shared_script_pointer {
public:
  using value_type = T;
  template <typename X, typename... Args>
  typename X::ret_type invoke(X T::*v, Args... args) {
    auto sp = get();
    return (type->*v).do_(type->m_engine, sp, args...);
  }
  script_pointer<T> get() {
    return script_pointer<T>(val, type);
  }

  ~shared_script_pointer() {
    release();
  }

  shared_script_pointer(const shared_script_pointer& _other)
    : val(_other.val), type(_other.type) {
    acquire();
  }

  shared_script_pointer(shared_script_pointer&& _other)
    : val(_other.val), type(_other.type) {
    _other.val = nullptr;
    _other.type = nullptr;
  }
  shared_script_pointer() : val(nullptr), type(nullptr) {}
  void* unsafe_ptr() {
    return val;
  }
  void unsafe_set_type(T* _type) {
    type = _type;
  }

  shared_script_pointer(void* t) : val(t) {
    acquire();
  }

private:
  struct shared_object_header {
    std::atomic<size_t> m_ref_count;
    void (*m_destructor)(void*);
  };

  shared_object_header* header() {
    return static_cast<shared_object_header*>(val) - 1;
  }

  void acquire() {
    if (val) {
      // When this is created (by the engine), then it will
      // also fix up T* type, but not in the constructor for a variety of
      // reasons
      header()->m_ref_count++;
    }
  }

  void release() {
    if (val && --header()->m_ref_count == 0) {
      if (header()->m_destructor) {
        (*header()->m_destructor)(val);
      }
      type->free(val);
    }
  }

  void* val;
  T* type;
  friend class engine;
};

template <typename T>
struct is_script_pointer {
  static const bool value = false;
};

template <typename T>
struct is_script_pointer<script_pointer<T>> {
  static const bool value = true;
};

template <typename T>
struct is_shared_script_pointer {
  static const bool value = false;
};

template <typename T>
struct is_shared_script_pointer<shared_script_pointer<T>> {
  static const bool value = true;
};

template <typename T>
struct needs_thunk<shared_script_pointer<T>> {
  static const bool value = true;
};

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
