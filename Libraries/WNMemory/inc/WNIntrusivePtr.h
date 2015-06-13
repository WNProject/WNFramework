// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MEMORY_INTRUSIVE_PTR_H__
#define __WN_MEMORY_INTRUSIVE_PTR_H__

#include "WNMemory/inc/WNBasic.h"

namespace wn {
namespace memory {

template <typename T>
class intrusive_ptr final {
public:
  typedef T element_type;

  WN_FORCE_INLINE intrusive_ptr() :
    m_pointer(wn_nullptr) {
  }

  WN_FORCE_INLINE intrusive_ptr(const wn_nullptr_t) :
    intrusive_ptr() {
  }

  template <typename Y>
  WN_FORCE_INLINE explicit intrusive_ptr(Y* ptr, const wn_bool add_ref = wn_true) :
    intrusive_ptr() {
    reset(ptr, add_ref);
  }

  WN_FORCE_INLINE intrusive_ptr(intrusive_ptr&& ptr) :
    intrusive_ptr() {
    reset(ptr.m_pointer);

    ptr.reset();
  }

  WN_FORCE_INLINE intrusive_ptr(const intrusive_ptr& ptr) :
    intrusive_ptr() {
    reset(ptr.m_pointer);
  }

  template <typename Y>
  WN_FORCE_INLINE intrusive_ptr(intrusive_ptr<Y>&& ptr) :
    intrusive_ptr() {
    reset(ptr.m_pointer);

    ptr.reset();
  }

  template <typename Y>
  WN_FORCE_INLINE intrusive_ptr(const intrusive_ptr<Y>& ptr) :
    intrusive_ptr() {
    reset(ptr.m_pointer);
  }

  WN_FORCE_INLINE ~intrusive_ptr() {
    remove_reference();
  }

  WN_FORCE_INLINE T* operator -> () const {
    return(m_pointer);
  }

  WN_FORCE_INLINE T& operator * () const {
    return(*m_pointer);
  }

  WN_FORCE_INLINE operator wn_bool () const {
    return(m_pointer != wn_nullptr);
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

  template <typename Y>
  WN_FORCE_INLINE intrusive_ptr& operator = (intrusive_ptr<Y>&& ptr) {
    intrusive_ptr(std::move(ptr)).swap(*this);

    return(*this);
  }

  template <typename Y>
  WN_FORCE_INLINE intrusive_ptr& operator = (const intrusive_ptr<Y>& ptr) {
    intrusive_ptr(ptr).swap(*this);

    return(*this);
  }

  WN_FORCE_INLINE T* get() const {
    return(m_pointer);
  }

  WN_FORCE_INLINE wn_size_t use_count() const {
    return(m_pointer ? m_pointer->reference_count() : 0);
  }

  WN_FORCE_INLINE wn_bool unique() const {
    return(use_count() == 1);
  }

  WN_FORCE_INLINE wn_void reset() {
    remove_reference();
  }

  WN_FORCE_INLINE wn_void reset(const wn_nullptr_t) {
    reset();
  }

  template <typename Y>
  WN_FORCE_INLINE wn_void reset(Y* ptr, const wn_bool add_ref = wn_true) {
    if (ptr && add_ref) {
      ptr->add_reference();
    }

    reset();

    m_pointer = ptr;
  }

  WN_FORCE_INLINE wn_void swap(intrusive_ptr& ptr) {
    T* pointer = ptr.release();

    ptr.reset(m_pointer);

    reset(pointer, wn_false);
  }

  WN_FORCE_INLINE T* release() {
    T* pointer = m_pointer;

    m_pointer = wn_nullptr;

    return(pointer);
  }

private:
  template <typename Y>
  friend class intrusive_ptr;

  WN_FORCE_INLINE wn_void add_reference() const {
    if (m_pointer) {
      m_pointer->add_reference();
    }
  }

  WN_FORCE_INLINE wn_void remove_reference() {
    if (m_pointer) {
      if (m_pointer->remove_reference()) {
        destroy(m_pointer);
      }

      m_pointer = wn_nullptr;
    }
  }

  T* m_pointer;
};

template <typename T, typename U>
WN_FORCE_INLINE wn_bool operator == (const intrusive_ptr<T>& lhs,
                                     const intrusive_ptr<U>& rhs) {
  return(lhs.get() == rhs.get());
}

template <typename T, typename U>
WN_FORCE_INLINE wn_bool operator != (const intrusive_ptr<T>& lhs,
                                     const intrusive_ptr<U>& rhs) {
  return(lhs.get() != rhs.get());
}

template <typename T, typename U>
WN_FORCE_INLINE wn_bool operator < (const intrusive_ptr<T>& lhs,
                                    const intrusive_ptr<U>& rhs) {
  return(lhs.get() < rhs.get());
}

template <typename T, typename U>
WN_FORCE_INLINE wn_bool operator > (const intrusive_ptr<T>& lhs,
                                    const intrusive_ptr<U>& rhs) {
  return(lhs.get() > rhs.get());
}

template <typename T, typename U>
WN_FORCE_INLINE wn_bool operator <= (const intrusive_ptr<T>& lhs,
                                     const intrusive_ptr<U>& rhs) {
  return(lhs.get() <= rhs.get());
}

template <typename T, typename U>
WN_FORCE_INLINE wn_bool operator >= (const intrusive_ptr<T>& lhs,
                                     const intrusive_ptr<U>& rhs) {
  return(lhs.get() >= rhs.get());
}

template <typename T>
WN_FORCE_INLINE wn_bool operator == (const intrusive_ptr<T>& ptr,
                                     const wn_nullptr_t) {
  return(ptr.get() == wn_nullptr);
}

template <typename T>
WN_FORCE_INLINE wn_bool operator == (const wn_nullptr_t,
                                     const intrusive_ptr<T>& ptr) {
  return(wn_nullptr == ptr.get());
}

template <typename T>
WN_FORCE_INLINE wn_bool operator != (const intrusive_ptr<T>& ptr,
                                     const wn_nullptr_t) {
  return(ptr.get() != wn_nullptr);
}

template <typename T>
WN_FORCE_INLINE wn_bool operator != (const wn_nullptr_t,
                                     const intrusive_ptr<T>& ptr) {
  return(wn_nullptr != ptr.get());
}

template <typename T>
WN_FORCE_INLINE wn_bool operator < (const intrusive_ptr<T>& ptr,
                                    const wn_nullptr_t) {
  return(ptr.get() < wn_nullptr);
}

template <typename T>
WN_FORCE_INLINE wn_bool operator < (const wn_nullptr_t,
                                    const intrusive_ptr<T>& ptr) {
  return(wn_nullptr < ptr.get());
}

template <typename T>
WN_FORCE_INLINE wn_bool operator > (const intrusive_ptr<T>& ptr,
                                    const wn_nullptr_t) {
  return(ptr.get() > wn_nullptr);
}

template <typename T>
WN_FORCE_INLINE wn_bool operator > (const wn_nullptr_t,
                                    const intrusive_ptr<T>& ptr) {
  return(wn_nullptr > ptr.get());
}

template <typename T>
WN_FORCE_INLINE wn_bool operator <= (const intrusive_ptr<T>& ptr,
                                     const wn_nullptr_t) {
  return(ptr.get() <= wn_nullptr);
}

template <typename T>
WN_FORCE_INLINE wn_bool operator <= (const wn_nullptr_t,
                                     const intrusive_ptr<T>& ptr) {
  return(wn_nullptr <= ptr.get());
}

template <typename T>
WN_FORCE_INLINE wn_bool operator >= (const intrusive_ptr<T>& ptr,
                                     const wn_nullptr_t) {
  return(ptr.get() >= wn_nullptr);
}

template <typename T>
WN_FORCE_INLINE wn_bool operator >= (const wn_nullptr_t,
                                     const intrusive_ptr<T>& ptr) {
  return(wn_nullptr >= ptr.get());
}

template <typename T, typename... Args>
WN_FORCE_INLINE intrusive_ptr<T> make_intrusive(Args&&... args) {
  return(intrusive_ptr<T>(construct<T>(std::forward<Args>(args)...)));
}

} // namespace memory
} // namespace wn

#endif // __WN_MEMORY_INTRUSIVE_PTR_H__