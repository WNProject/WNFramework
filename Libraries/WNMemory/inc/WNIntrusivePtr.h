// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MEMORY_INTRUSIVE_PTR_H__
#define __WN_MEMORY_INTRUSIVE_PTR_H__

#include "WNMemory/inc/WNBasic.h"

#include <atomic>

namespace wn {
namespace memory {

class intrusive_ptr_base {
public:
  WN_FORCE_INLINE wn_void add_reference() {
    for (;;) {
      wn_size_t count = m_reference_count;
      const wn_size_t target_count = count + 1;

      if (m_reference_count.compare_exchange_strong(count, target_count)) {
        break;
      }
    }
  }

  WN_FORCE_INLINE wn_bool remove_reference() {
    for (;;) {
      wn_size_t count = m_reference_count;
      const wn_size_t target_count = count - 1;

      if (m_reference_count.compare_exchange_strong(count, target_count)) {
        return(target_count == 0);
      }
    }
  }

  WN_FORCE_INLINE wn_size_t reference_count() const {
    return(m_reference_count);
  }

protected:
  WN_FORCE_INLINE intrusive_ptr_base() :
    m_reference_count(0) {
  }

private:
  std::atomic<wn_size_t> m_reference_count;
};

template <typename T, typename D = default_destroyer<T>>
class intrusive_ptr final {
public:
  static_assert(std::is_base_of<intrusive_ptr_base, T>::value,
    "type must be child of 'intrusive_ptr_base'");

  typedef T* pointer;
  typedef typename std::add_lvalue_reference<T>::type reference;
  typedef typename std::add_lvalue_reference<D>::type destroyer_reference;
  typedef typename std::add_lvalue_reference<
    typename std::add_const<D>::type>::type const_destroyer_reference;
  typedef T element_type;
  typedef D destroyer_type;

  static_assert(core::is_callable<D, wn_void(pointer)>::value,
    "destroyer must have callable signature 'wn_void(intrusive_ptr<T, D>::pointer)'");

  WN_FORCE_INLINE intrusive_ptr() :
    m_pointer(wn_nullptr) {
  }

  WN_FORCE_INLINE intrusive_ptr(const wn_nullptr_t) :
    intrusive_ptr() {
  }

  template <typename U,
            typename = core::enable_if_t<
              std::is_convertible<U*, pointer>::value>>
  WN_FORCE_INLINE explicit intrusive_ptr(U* ptr,
                                         const wn_bool add_ref = wn_true) :
    intrusive_ptr(ptr, D(), add_ref) {
    static_assert(!std::is_pointer<D>::value,
      "constructed with null function pointer destroyer");
  }

  template <typename U,
            typename = core::enable_if_t<
              std::is_convertible<U*, pointer>::value>>
  WN_FORCE_INLINE intrusive_ptr(U* ptr, typename std::conditional<
                                std::is_reference<D>::value, D, const D&>::type d,
                                const wn_bool add_ref = wn_true) :
    intrusive_ptr(ptr, D(d), add_ref) {
  }

  template <typename U,
            typename = core::enable_if_t<
              std::is_convertible<U*, pointer>::value>>
  WN_FORCE_INLINE intrusive_ptr(U* ptr,
                                typename std::remove_reference<D>::type&& d,
                                const wn_bool add_ref = wn_true) :
    m_pointer(ptr),
    m_destroyer(std::move(d)) {
    static_assert(!std::is_reference<D>::value,
      "rvalue destroyer bound to reference");

    if (add_ref && m_pointer) {
      m_pointer->add_reference();
    }
  }

  WN_FORCE_INLINE intrusive_ptr(intrusive_ptr&& ptr) :
    intrusive_ptr(ptr.release(), std::forward<D>(ptr.get_destroyer()), wn_false) {
  }

  WN_FORCE_INLINE intrusive_ptr(const intrusive_ptr& ptr) :
    intrusive_ptr(ptr.get(), ptr.get_destroyer()) {
  }

  template <typename U, typename E>
  WN_FORCE_INLINE intrusive_ptr(intrusive_ptr<U, E>&& ptr) :
    intrusive_ptr(ptr.release(), std::forward<D>(ptr.get_destroyer()), wn_false) {
  }

  template <typename U, typename E>
  WN_FORCE_INLINE intrusive_ptr(const intrusive_ptr<U, E>& ptr) :
    intrusive_ptr(ptr.get(), ptr.get_destroyer()) {
  }

  WN_FORCE_INLINE ~intrusive_ptr() {
    if (m_pointer) {
      if (m_pointer->remove_reference()) {
        m_destroyer(m_pointer);
      }

      m_pointer = wn_nullptr;
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

  WN_FORCE_INLINE intrusive_ptr& operator = (const wn_nullptr_t) {
    intrusive_ptr(wn_nullptr).swap(*this);

    return(*this);
  }

  WN_FORCE_INLINE intrusive_ptr& operator = (intrusive_ptr&& ptr) {
    intrusive_ptr(std::move(ptr)).swap(*this);

    return(*this);
  }

  WN_FORCE_INLINE intrusive_ptr& operator = (const intrusive_ptr& ptr) {
    intrusive_ptr(ptr).swap(*this);

    return(*this);
  }

  template <typename U, typename E>
  WN_FORCE_INLINE intrusive_ptr& operator = (intrusive_ptr<U, E>&& ptr) {
    intrusive_ptr(std::move(ptr)).swap(*this);

    return(*this);
  }

  template <typename U, typename E>
  WN_FORCE_INLINE intrusive_ptr& operator = (const intrusive_ptr<U, E>& ptr) {
    intrusive_ptr(ptr).swap(*this);

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

  WN_FORCE_INLINE wn_size_t use_count() const {
    return(get() ? get()->reference_count() : 0);
  }

  WN_FORCE_INLINE wn_bool unique() const {
    return(use_count() == 1);
  }

  WN_FORCE_INLINE wn_void reset() {
    intrusive_ptr().swap(*this);
  }

  WN_FORCE_INLINE wn_void reset(const wn_nullptr_t) {
    intrusive_ptr(wn_nullptr).swap(*this);
  }

  template <typename U,
            typename = core::enable_if_t<
              std::is_convertible<U*, pointer>::value>>
  WN_FORCE_INLINE wn_void reset(U* ptr, const wn_bool add_ref = wn_true) {
    intrusive_ptr(ptr, add_ref).swap(*this);
  }

  WN_FORCE_INLINE wn_void swap(intrusive_ptr& ptr) {
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
using intrusive_aligned_ptr = intrusive_ptr<T, default_aligned_destroyer<T>>;

template <typename T, typename D, typename U, typename E>
WN_FORCE_INLINE wn_bool operator == (const intrusive_ptr<T, D>& lhs,
                                     const intrusive_ptr<U, E>& rhs) {
  return(lhs.get() == rhs.get());
}

template <typename T, typename D, typename U, typename E>
WN_FORCE_INLINE wn_bool operator != (const intrusive_ptr<T, D>& lhs,
                                     const intrusive_ptr<U, E>& rhs) {
  return(lhs.get() != rhs.get());
}

template <typename T, typename D, typename U, typename E>
WN_FORCE_INLINE wn_bool operator < (const intrusive_ptr<T, D>& lhs,
                                    const intrusive_ptr<U, E>& rhs) {
  return(lhs.get() < rhs.get());
}

template <typename T, typename D, typename U, typename E>
WN_FORCE_INLINE wn_bool operator > (const intrusive_ptr<T, D>& lhs,
                                    const intrusive_ptr<U, E>& rhs) {
  return(lhs.get() > rhs.get());
}

template <typename T, typename D, typename U, typename E>
WN_FORCE_INLINE wn_bool operator <= (const intrusive_ptr<T, D>& lhs,
                                     const intrusive_ptr<U, E>& rhs) {
  return(lhs.get() <= rhs.get());
}

template <typename T, typename D, typename U, typename E>
WN_FORCE_INLINE wn_bool operator >= (const intrusive_ptr<T, D>& lhs,
                                     const intrusive_ptr<U, E>& rhs) {
  return(lhs.get() >= rhs.get());
}

template <typename T, typename D>
WN_FORCE_INLINE wn_bool operator == (const intrusive_ptr<T, D>& ptr,
                                     const wn_nullptr_t) {
  return(ptr.get() == wn_nullptr);
}

template <typename T, typename D>
WN_FORCE_INLINE wn_bool operator == (const wn_nullptr_t,
                                     const intrusive_ptr<T, D>& ptr) {
  return(wn_nullptr == ptr.get());
}

template <typename T, typename D>
WN_FORCE_INLINE wn_bool operator != (const intrusive_ptr<T, D>& ptr,
                                     const wn_nullptr_t) {
  return(ptr.get() != wn_nullptr);
}

template <typename T, typename D>
WN_FORCE_INLINE wn_bool operator != (const wn_nullptr_t,
                                     const intrusive_ptr<T, D>& ptr) {
  return(wn_nullptr != ptr.get());
}

template <typename T, typename D>
WN_FORCE_INLINE wn_bool operator < (const intrusive_ptr<T, D>& ptr,
                                    const wn_nullptr_t) {
  return(ptr.get() < wn_nullptr);
}

template <typename T, typename D>
WN_FORCE_INLINE wn_bool operator < (const wn_nullptr_t,
                                    const intrusive_ptr<T, D>& ptr) {
  return(wn_nullptr < ptr.get());
}

template <typename T, typename D>
WN_FORCE_INLINE wn_bool operator > (const intrusive_ptr<T, D>& ptr,
                                    const wn_nullptr_t) {
  return(ptr.get() > wn_nullptr);
}

template <typename T, typename D>
WN_FORCE_INLINE wn_bool operator > (const wn_nullptr_t,
                                    const intrusive_ptr<T, D>& ptr) {
  return(wn_nullptr > ptr.get());
}

template <typename T, typename D>
WN_FORCE_INLINE wn_bool operator <= (const intrusive_ptr<T, D>& ptr,
                                     const wn_nullptr_t) {
  return(ptr.get() <= wn_nullptr);
}

template <typename T, typename D>
WN_FORCE_INLINE wn_bool operator <= (const wn_nullptr_t,
                                     const intrusive_ptr<T, D>& ptr) {
  return(wn_nullptr <= ptr.get());
}

template <typename T, typename D>
WN_FORCE_INLINE wn_bool operator >= (const intrusive_ptr<T, D>& ptr,
                                     const wn_nullptr_t) {
  return(ptr.get() >= wn_nullptr);
}

template <typename T, typename D>
WN_FORCE_INLINE wn_bool operator >= (const wn_nullptr_t,
                                     const intrusive_ptr<T, D>& ptr) {
  return(wn_nullptr >= ptr.get());
}

template <typename T, typename... Args>
WN_FORCE_INLINE intrusive_ptr<T> make_intrusive(Args&&... args) {
  return(intrusive_ptr<T>(construct<T>(std::forward<Args>(args)...)));
}

template <typename T, typename... Args>
WN_FORCE_INLINE intrusive_aligned_ptr<T> make_intrusive_aligned(Args&&... args) {
  return(intrusive_aligned_ptr<T>(
    construct_aligned<T>(std::forward<Args>(args)...)));
}

} // namespace memory
} // namespace wn

#endif // __WN_MEMORY_INTRUSIVE_PTR_H__