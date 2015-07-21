// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MEMORY_UNIQUE_PTR_H__
#define __WN_MEMORY_UNIQUE_PTR_H__

#include "WNMemory/inc/WNBasic.h"

namespace wn {
namespace memory {

template <typename T, typename D = default_destroyer<T>>
class unique_ptr final : public core::non_copyable {
public:
  static_assert(!std::is_array<T>::value, "array 'T[]' types not allowed");

  typedef T* pointer;
  typedef typename std::add_lvalue_reference<T>::type reference;
  typedef typename std::add_lvalue_reference<D>::type destroyer_reference;
  typedef typename std::add_lvalue_reference<
    typename std::add_const<D>::type>::type const_destroyer_reference;
  typedef T element_type;
  typedef D destroyer_type;

  static_assert(core::is_callable<D, wn_void(pointer)>::value,
    "destroyer must have callable signature 'wn_void(unique_ptr<T, D>::pointer)'");

  WN_FORCE_INLINE unique_ptr() :
    m_pointer(wn_nullptr) {
  }

  WN_FORCE_INLINE unique_ptr(const wn_nullptr_t) :
    unique_ptr() {
  }

  template <typename U,
            typename = core::enable_if_t<
              std::is_convertible<U*, pointer>::value >>
  WN_FORCE_INLINE explicit unique_ptr(U* ptr) :
    unique_ptr(ptr, D()) {
    static_assert(!std::is_pointer<D>::value,
      "constructed with null function pointer destroyer");
  }

  template <typename U,
            typename = core::enable_if_t<
              std::is_convertible<U*, pointer>::value>>
  WN_FORCE_INLINE unique_ptr(U* ptr, typename std::conditional<
                             std::is_reference<D>::value, D, const D&>::type d) :
    unique_ptr(ptr, D(d)) {
  }

  template <typename U,
            typename = core::enable_if_t<
              std::is_convertible<U*, pointer>::value>>
  WN_FORCE_INLINE unique_ptr(U* ptr,
                             typename std::remove_reference<D>::type&& d) :
    m_pointer(ptr),
    m_destroyer(std::move(d)) {
    static_assert(!std::is_reference<D>::value,
      "rvalue destroyer bound to reference");
  }

  WN_FORCE_INLINE unique_ptr(unique_ptr&& ptr) :
    unique_ptr(ptr.release(), std::forward<D>(ptr.get_destroyer())) {
  }

  template <typename U, typename E>
  WN_FORCE_INLINE unique_ptr(unique_ptr<U, E>&& ptr) :
    unique_ptr(ptr.release(), std::forward<D>(ptr.get_destroyer())) {
  }

  WN_FORCE_INLINE ~unique_ptr() {
    if (m_pointer) {
      m_destroyer(m_pointer);
    }
  }

  WN_FORCE_INLINE pointer operator -> () const {
    WN_RELEASE_ASSERT_DESC(get(), "cannot dereference nullptr");

    return(get());
  }

  WN_FORCE_INLINE reference operator * () const {
    WN_RELEASE_ASSERT_DESC(get(), "cannot dereference nullptr");

    return(*get());
  }

  WN_FORCE_INLINE operator wn_bool () const {
    return(get() != wn_nullptr);
  }

  WN_FORCE_INLINE unique_ptr& operator = (const wn_nullptr_t) {
    unique_ptr(wn_nullptr).swap(*this);

    return(*this);
  }

  WN_FORCE_INLINE unique_ptr& operator = (unique_ptr&& ptr) {
    unique_ptr(std::move(ptr)).swap(*this);

    return(*this);
  }

  template <typename U, typename E>
  WN_FORCE_INLINE unique_ptr& operator = (unique_ptr<U, E>&& ptr) {
    unique_ptr(std::move(ptr)).swap(*this);

    return(*this);
  }

  WN_FORCE_INLINE pointer get() const {
    return(m_pointer);
  }

  WN_FORCE_INLINE destroyer_reference get_destroyer() {
    return(m_destroyer);
  }

  WN_FORCE_INLINE const_destroyer_reference get_destroyer() const {
    return(m_destroyer);
  }

  WN_FORCE_INLINE wn_void reset() {
    unique_ptr().swap(*this);
  }

  WN_FORCE_INLINE wn_void reset(const wn_nullptr_t) {
    unique_ptr(wn_nullptr).swap(*this);
  }

  template <typename U,
            typename = core::enable_if_t<
              std::is_convertible<U*, pointer>::value>>
  WN_FORCE_INLINE wn_void reset(U* ptr) {
    unique_ptr(ptr).swap(*this);
  }

  WN_FORCE_INLINE wn_void swap(unique_ptr& ptr) {
    std::swap(m_destroyer, ptr.m_destroyer);
    std::swap(m_pointer, ptr.m_pointer);
  }

  WN_FORCE_INLINE pointer release() {
    pointer p = get();

    m_pointer = wn_nullptr;

    return(p);
  }

private:
  D m_destroyer;
  pointer m_pointer;
};

template <typename T>
using unique_aligned_ptr = unique_ptr<T, default_aligned_destroyer<T>>;

template <typename T, typename D, typename U, typename E>
WN_FORCE_INLINE wn_bool operator == (const unique_ptr<T, D>& lhs,
                                     const unique_ptr<U, E>& rhs) {
  return(lhs.get() == rhs.get());
}

template <typename T, typename D, typename U, typename E>
WN_FORCE_INLINE wn_bool operator != (const unique_ptr<T, D>& lhs,
                                     const unique_ptr<U, E>& rhs) {
  return(lhs.get() != rhs.get());
}

template <typename T, typename D, typename U, typename E>
WN_FORCE_INLINE wn_bool operator < (const unique_ptr<T, D>& lhs,
                                    const unique_ptr<U, E>& rhs) {
  return(lhs.get() < rhs.get());
}

template <typename T, typename D, typename U, typename E>
WN_FORCE_INLINE wn_bool operator > (const unique_ptr<T, D>& lhs,
                                    const unique_ptr<U, E>& rhs) {
  return(lhs.get() > rhs.get());
}

template <typename T, typename D, typename U, typename E>
WN_FORCE_INLINE wn_bool operator <= (const unique_ptr<T, D>& lhs,
                                     const unique_ptr<U, E>& rhs) {
  return(lhs.get() <= rhs.get());
}

template <typename T, typename D, typename U, typename E>
WN_FORCE_INLINE wn_bool operator >= (const unique_ptr<T, D>& lhs,
                                     const unique_ptr<U, E>& rhs) {
  return(lhs.get() >= rhs.get());
}

template <typename T, typename D>
WN_FORCE_INLINE wn_bool operator == (const unique_ptr<T, D>& ptr,
                                     const wn_nullptr_t) {
  return(ptr.get() == wn_nullptr);
}

template <typename T, typename D>
WN_FORCE_INLINE wn_bool operator == (const wn_nullptr_t,
                                     const unique_ptr<T, D>& ptr) {
  return(wn_nullptr == ptr.get());
}

template <typename T, typename D>
WN_FORCE_INLINE wn_bool operator != (const unique_ptr<T, D>& ptr,
                                     const wn_nullptr_t) {
  return(ptr.get() != wn_nullptr);
}

template <typename T, typename D>
WN_FORCE_INLINE wn_bool operator != (const wn_nullptr_t,
                                     const unique_ptr<T, D>& ptr) {
  return(wn_nullptr != ptr.get());
}

template <typename T, typename D>
WN_FORCE_INLINE wn_bool operator < (const unique_ptr<T, D>& ptr,
                                    const wn_nullptr_t) {
  return(ptr.get() < wn_nullptr);
}

template <typename T, typename D>
WN_FORCE_INLINE wn_bool operator < (const wn_nullptr_t,
                                    const unique_ptr<T, D>& ptr) {
  return(wn_nullptr < ptr.get());
}

template <typename T, typename D>
WN_FORCE_INLINE wn_bool operator > (const unique_ptr<T, D>& ptr,
                                    const wn_nullptr_t) {
  return(ptr.get() > wn_nullptr);
}

template <typename T, typename D>
WN_FORCE_INLINE wn_bool operator > (const wn_nullptr_t,
                                    const unique_ptr<T, D>& ptr) {
  return(wn_nullptr > ptr.get());
}

template <typename T, typename D>
WN_FORCE_INLINE wn_bool operator <= (const unique_ptr<T, D>& ptr,
                                     const wn_nullptr_t) {
  return(ptr.get() <= wn_nullptr);
}

template <typename T, typename D>
WN_FORCE_INLINE wn_bool operator <= (const wn_nullptr_t,
                                     const unique_ptr<T, D>& ptr) {
  return(wn_nullptr <= ptr.get());
}

template <typename T, typename D>
WN_FORCE_INLINE wn_bool operator >= (const unique_ptr<T, D>& ptr,
                                     const wn_nullptr_t) {
  return(ptr.get() >= wn_nullptr);
}

template <typename T, typename D>
WN_FORCE_INLINE wn_bool operator >= (const wn_nullptr_t,
                                     const unique_ptr<T, D>& ptr) {
  return(wn_nullptr >= ptr.get());
}

template <typename T, typename... Args>
WN_FORCE_INLINE unique_ptr<T> make_unique(Args&&... args) {
  static_assert(!std::is_array<T>::value, "array 'T[]' types not allowed");

  return(unique_ptr<T>(construct<T>(std::forward<Args>(args)...)));
}

template <typename T, typename... Args>
WN_FORCE_INLINE unique_aligned_ptr<T> make_unique_aligned(Args&&... args) {
  static_assert(!std::is_array<T>::value, "array 'T[]' types not allowed");

  return(unique_aligned_ptr<T>(
    construct_aligned<T>(std::forward<Args>(args)...)));
}

} // namespace memory
} // namespace wn

#endif // __WN_MEMORY_UNIQUE_PTR_H__