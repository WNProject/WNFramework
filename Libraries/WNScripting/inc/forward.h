// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef _WN_SCRIPTING_INC_FORWARD_H__
#define _WN_SCRIPTING_INC_FORWARD_H__

#include "WNMemory/inc/allocator.h"

#include <atomic>

namespace wn {
namespace scripting {

class engine;

struct script_object_type {
  void free(void* val);

  memory::allocator* m_allocator;
  engine* m_engine;
  containers::string_view m_name;
  script_object_type* m_parent;
};
template <typename T>
class shared_script_pointer;

template <typename T>
class script_pointer {
public:
  using value_type = T;

  template <typename X, typename... Args>
  typename core::enable_if<!core::is_same<void, typename X::ret_type>::value,
      typename X::ret_type>::type
  invoke(X T::*v, Args... args) {
    return (type->*v).do_(type->m_engine, *this, args...);
  }

  template <typename X, typename... Args>
  typename core::enable_if<core::is_same<void, typename X::ret_type>::value,
      typename X::ret_type>::type
  invoke(X T::*v, Args... args) {
    return (type->*v).do_v(type->m_engine, *this, args...);
  }

  ~script_pointer() {}

  script_pointer(const script_pointer& _other)
    : val(_other.val), type(_other.type) {}

  script_pointer(script_pointer&& _other)
    : val(_other.val), type(_other.type) {}
  script_pointer() : val(nullptr), type(nullptr) {}

  void* unsafe_ptr() {
    return val;
  }

  const void* unsafe_ptr() const {
    return val;
  }

  void* unsafe_pass() {
    return val;
  }

  void unsafe_set_type(T* _type) {
    type = _type;
  }

  script_pointer(void* t) : val(t) {}
  script_pointer(void* _v, T* _t) : val(_v), type(_t) {}
  script_pointer(const shared_script_pointer<T>& _other);

  template <typename Q = T>
  typename core::enable_if<!core::is_same<typename Q::parent_type, void>::value,
      script_pointer<typename Q::parent_type>>::type
  parent() {
    return script_pointer<typename T::parent_type>(
        val, reinterpret_cast<typename T::parent_type*>(type->m_parent));
  }

private:
  void* val;
  T* type;
  friend class engine;
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

  shared_script_pointer& operator=(const shared_script_pointer& _other) {
    val = _other.val;
    type = _other.type;
    acquire();
    return *this;
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
  const void* unsafe_ptr() const {
    return val;
  }

  void* unsafe_pass() {
    acquire();
    return val;
  }

  void unsafe_set_type(T* _type) {
    type = _type;
  }

  shared_script_pointer(void* t) : val(t) {
    acquire();
  }

  template <typename Q = T>
  typename core::enable_if<!core::is_same<typename Q::parent_type, void>::value,
      shared_script_pointer<typename Q::parent_type>>::type
  parent() {
    return shared_script_pointer<typename T::parent_type>(
        val, reinterpret_cast<typename T::parent_type*>(type->m_parent));
  }

private:
  shared_script_pointer(void* v, T* t) : val(v), type(t) {
    acquire();
  }

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
  friend class script_pointer<T>;
};

template <typename T>
script_pointer<T>::script_pointer(const shared_script_pointer<T>& _other)
  : val(_other.val), type(_other.type) {}

template <typename T>
class shared_cpp_pointer {
public:
  using value_type = T;

  ~shared_cpp_pointer() {
    release();
  }

  shared_cpp_pointer(const shared_cpp_pointer& _other)
    : val(_other.val), m_engine(_other.m_engine) {
    acquire();
  }

  shared_cpp_pointer(shared_cpp_pointer&& _other)
    : val(_other.val), m_engine(_other.m_engine), m_free(_other.m_free) {
    _other.val = nullptr;
    _other.m_engine = nullptr;
    _other.m_free = nullptr;
  }

  template <typename U>
  shared_cpp_pointer(const shared_cpp_pointer<U>& _other)
    : val(_other.val), m_engine(_other.m_engine) {
    acquire();
  }

  template <typename U>
  shared_cpp_pointer(shared_cpp_pointer<U>&& _other)
    : val(_other.val), m_engine(_other.m_engine), m_free(_other.m_free) {
    _other.val = nullptr;
    _other.m_engine = nullptr;
    _other.m_free = nullptr;
  }

  shared_cpp_pointer operator=(const shared_cpp_pointer& _other) {
    release();
    val = _other.val;
    m_engine = _other.m_engine;
    m_free = _other.m_free;
    acquire();
    return *this;
  }

  shared_cpp_pointer() : val(nullptr) {}
  T* get() {
    return val;
  }
  T* operator->() {
    return val;
  }

  const T* operator->() const {
    return val;
  }

  void* unsafe_pass() {
    acquire();
    return val;
  }

  void* unsafe_ptr() {
    return val;
  }

  const void* unsafe_ptr() const {
    return val;
  }

  shared_cpp_pointer unsafe_release() {
    header()->m_ref_count--;
    return *this;
  }

private:
  static void destroy(T* t) {
    (t->T::~T)();
  }

  shared_cpp_pointer(T* v) : val(v) {
    static_assert(sizeof(&shared_cpp_pointer<T>::destroy) == sizeof(void*),
        "Unexpected static method pointer size");
    header()->m_destructor =
        reinterpret_cast<void (*)(void*)>(&shared_cpp_pointer<T>::destroy);
    acquire();
  }

  shared_cpp_pointer(T* v, engine* _engine, void(_free)(const engine*, void*))
    : shared_cpp_pointer(v) {
    m_engine = _engine;
    m_free = _free;
  }

  struct shared_object_header {
    std::atomic<size_t> m_ref_count;
    void (*m_destructor)(void*);
  };

  shared_object_header* header() {
    return reinterpret_cast<shared_object_header*>(val) - 1;
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
      m_free(m_engine, val);
    }
  }

  void unsafe_set_engine_free(
      const engine* _engine, void (*_free)(const engine*, void*)) {
    m_engine = _engine;
    m_free = _free;
  }

  T* val;
  const engine* m_engine = nullptr;
  void (*m_free)(const engine*, void*) = nullptr;
  friend class engine;

  template <typename U>
  friend shared_cpp_pointer<U>& fixup_return_type(shared_cpp_pointer<U>&);

  template <typename U, typename V>
  friend struct get_thunk_passed_type;

  template <typename U>
  friend class shared_cpp_pointer;
};

template <typename T, size_t S = 1>
struct wn_array_data {
  T& operator[](size_t i) {
    return _values[i];
  }

  const T& operator[](size_t i) const {
    return _values[i];
  }

  size_t size() const {
    return _size;
  }

  T* begin() {
    return &_values[0];
  }

  T* end() {
    return &(_values[S - 1]) + 1;
  }

private:
  uint32_t _size = 0;
  T _values[S];
};

template <typename T>
struct wn_array final {
  wn_array(void* v) : ptr(reinterpret_cast<wn_array_data<T>*>(v)) {}
  wn_array_data<T>* ptr;

  T& operator[](size_t i) {
    return (*ptr)[i];
  }

  const T& operator[](size_t i) const {
    return (*ptr)[i];
  }

  size_t size() const {
    return ptr->size();
  }

  T* begin() {
    if (ptr->size() == 0) {
      return nullptr;
    }
    return &(*ptr)[0];
  }

  T* end() {
    if (ptr->size() == 0) {
      return nullptr;
    }
    return &((*ptr)[ptr->size() - 1]) + 1;
  }
};

template <typename T>
struct wn_array<script_pointer<T>> final {
public:
  wn_array(void* v) : m_ptr(reinterpret_cast<wn_array_data<void*>*>(v)) {}
  wn_array_data<void*>* m_ptr;
  T* m_type;

  void unsafe_set_type(T* _type) {
    m_type = _type;
  }
  script_pointer<T> operator[](size_t i) {
    script_pointer<T> t((*m_ptr)[i]);
    t.unsafe_set_type(m_type);
    return t;
  }

  const script_pointer<T> operator[](size_t i) const {
    script_pointer<T> t((*m_ptr)[i]);
    t.unsafe_set_type(m_type);
    return t;
  }

  size_t size() const {
    return m_ptr->size();
  }
};

template <typename T>
struct wn_array<shared_script_pointer<T>> final {
public:
  wn_array(void* v) : m_ptr(reinterpret_cast<wn_array_data<void*>*>(v)) {}
  wn_array_data<void*>* m_ptr;
  T* m_type;

  void unsafe_set_type(T* _type) {
    m_type = _type;
  }
  shared_script_pointer<T> operator[](size_t i) {
    shared_script_pointer<T> t((*m_ptr)[i]);
    t.unsafe_set_type(m_type);
    return t;
  }

  const shared_script_pointer<T> operator[](size_t i) const {
    shared_script_pointer<T> t((*m_ptr)[i]);
    t.unsafe_set_type(m_type);
    return t;
  }

  size_t size() const {
    return m_ptr->size();
  }
};

}  // namespace scripting
}  // namespace wn

#endif  // _WN_SCRIPTING_INC_FORWARD_H__
