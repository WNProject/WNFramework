// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MEMORY_UNIQUE_PTR_H__
#define __WN_MEMORY_UNIQUE_PTR_H__

#include "WNMemory/inc/allocator.h"
#include "core/inc/type_traits.h"

#include <memory>

namespace wn {
namespace memory {

template <typename T>
class unique_ptr final : core::non_copyable {
public:
  static_assert(!core::is_array<T>::value, "array 'T[]' types not allowed");

  using pointer = core::add_pointer_t<T>;
  using reference = core::add_lvalue_reference_t<T>;
  using element_type = T;

  inline unique_ptr() : m_allocator(nullptr), m_pointer(nullptr) {}

  inline unique_ptr(const nullptr_t) : unique_ptr() {}

  template <typename U,
      typename = core::enable_if_t<core::is_convertible<U*, pointer>::value>>
  inline explicit unique_ptr(allocator* _allocator, U* _ptr)
    : m_allocator(_allocator), m_pointer(_ptr) {}

  inline unique_ptr(unique_ptr&& _other)
    : unique_ptr(_other.get_allocator(), _other.get()) {
    _other.clear();
  }

  template <typename U,
      typename = core::enable_if_t<core::is_convertible<U*, pointer>::value>>
  inline unique_ptr(unique_ptr<U>&& _other)
    : unique_ptr(_other.get_allocator(), _other.get()) {
    _other.clear();
  }

  inline ~unique_ptr() {
    pointer p = get();

    if (p) {
      get_allocator()->destroy(p);
    }
  }

  inline pointer operator->() const {
    return get();
  }

  inline reference operator*() const {
    WN_RELEASE_ASSERT(get() != nullptr, "cannot dereference nullptr");

    return *get();
  }

  inline operator bool() const {
    return (get() != nullptr);
  }

  inline unique_ptr& operator=(nullptr_t) {
    unique_ptr(nullptr).swap(*this);

    return *this;
  }

  inline unique_ptr& operator=(unique_ptr&& _other) {
    unique_ptr(core::move(_other)).swap(*this);

    return *this;
  }

  template <typename U,
      typename = core::enable_if_t<core::is_convertible<U*, pointer>::value>>
  inline unique_ptr& operator=(unique_ptr<U>&& _other) {
    unique_ptr(core::move(_other)).swap(*this);

    return *this;
  }

  inline pointer get() const {
    return m_pointer;
  }

  inline allocator* get_allocator() const {
    return m_allocator;
  }

  inline void reset() {
    unique_ptr().swap(*this);
  }

  inline void reset(nullptr_t) {
    unique_ptr(nullptr).swap(*this);
  }

  template <typename U,
      typename = core::enable_if_t<core::is_convertible<U*, pointer>::value>>
  inline void reset(allocator* _allocator, U* _ptr) {
    unique_ptr(_allocator, _ptr).swap(*this);
  }

  inline void swap(unique_ptr& _other) {
    core::swap(m_allocator, _other.m_allocator);
    core::swap(m_pointer, _other.m_pointer);
  }

  inline pointer release() {
    pointer p = get();

    clear();

    return p;
  }

private:
  template <typename U>
  friend class unique_ptr;

  inline void clear() {
    m_pointer = nullptr;
    m_allocator = nullptr;
  }

  allocator* m_allocator;
  pointer m_pointer;
};

template <typename T, typename U>
inline bool operator==(const unique_ptr<T>& _lhs, const unique_ptr<U>& _rhs) {
  return (_lhs.get() == _rhs.get());
}

template <typename T, typename U>
inline bool operator!=(const unique_ptr<T>& _lhs, const unique_ptr<U>& _rhs) {
  return (_lhs.get() != _rhs.get());
}

template <typename T, typename U>
inline bool operator<(const unique_ptr<T>& _lhs, const unique_ptr<U>& _rhs) {
  return (_lhs.get() < _rhs.get());
}

template <typename T, typename U>
inline bool operator>(const unique_ptr<T>& _lhs, const unique_ptr<U>& _rhs) {
  return (_lhs.get() > _rhs.get());
}

template <typename T, typename U>
inline bool operator<=(const unique_ptr<T>& _lhs, const unique_ptr<U>& _rhs) {
  return (_lhs.get() <= _rhs.get());
}

template <typename T, typename U>
inline bool operator>=(const unique_ptr<T>& _lhs, const unique_ptr<U>& _rhs) {
  return (_lhs.get() >= _rhs.get());
}

template <typename T>
inline bool operator==(const unique_ptr<T>& _ptr, nullptr_t) {
  return (_ptr.get() == nullptr);
}

template <typename T>
inline bool operator==(nullptr_t, const unique_ptr<T>& _ptr) {
  return (nullptr == _ptr.get());
}

template <typename T>
inline bool operator!=(const unique_ptr<T>& _ptr, nullptr_t) {
  return (_ptr.get() != nullptr);
}

template <typename T>
inline bool operator!=(nullptr_t, const unique_ptr<T>& _ptr) {
  return (nullptr != _ptr.get());
}

template <typename T>
inline bool operator<(const unique_ptr<T>& _ptr, nullptr_t) {
  return (_ptr.get() < nullptr);
}

template <typename T>
inline bool operator<(nullptr_t, const unique_ptr<T>& _ptr) {
  return (nullptr < _ptr.get());
}

template <typename T>
inline bool operator>(const unique_ptr<T>& _ptr, nullptr_t) {
  return (_ptr.get() > nullptr);
}

template <typename T>
inline bool operator>(nullptr_t, const unique_ptr<T>& _ptr) {
  return (nullptr > _ptr.get());
}

template <typename T>
inline bool operator<=(const unique_ptr<T>& _ptr, nullptr_t) {
  return (_ptr.get() <= nullptr);
}

template <typename T>
inline bool operator<=(nullptr_t, const unique_ptr<T>& _ptr) {
  return (nullptr <= _ptr.get());
}

template <typename T>
inline bool operator>=(const unique_ptr<T>& _ptr, nullptr_t) {
  return (_ptr.get() >= nullptr);
}

template <typename T>
inline bool operator>=(nullptr_t, const unique_ptr<T>& _ptr) {
  return (nullptr >= _ptr.get());
}

template <typename T, typename... Args>
inline unique_ptr<T> make_unique(allocator* _allocator, Args&&... _args) {
  static_assert(!core::is_array<T>::value, "array 'T[]' types not allowed");

  T* ptr = _allocator->construct<T>(core::forward<Args>(_args)...);

  if (ptr) {
    return unique_ptr<T>(_allocator, ptr);
  }

  return nullptr;
}

template <typename T, typename F>
inline unique_ptr<T> make_unique_delegated(
    allocator* _allocator, F&& _delegator) {
  static_assert(!core::is_array<T>::value, "array 'T[]' types not allowed");
  static_assert(core::is_invocable_r<T*, F, void*>::value,
      "delegate must be of the format T*(void*)");

  void* raw_ptr = _allocator->allocate(sizeof(T));
  T* ptr = _delegator(raw_ptr);

  WN_DEBUG_ASSERT(
      ptr == raw_ptr, "delegator did not construct into given memory");

  if (ptr) {
    return unique_ptr<T>(_allocator, ptr);
  }

  return nullptr;
}

template <typename T, typename... Args>
inline std::unique_ptr<T> make_std_unique(Args&&... args) {
  return std::unique_ptr<T>(new T(core::forward<Args>(args)...));
}

}  // namespace memory
}  // namespace wn

#endif  // __WN_MEMORY_UNIQUE_PTR_H__
