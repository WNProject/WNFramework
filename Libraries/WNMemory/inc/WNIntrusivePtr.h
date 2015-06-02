// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MEMORY_INTRUSIVE_PTR_H__
#define __WN_MEMORY_INTRUSIVE_PTR_H__

#include "WNMemory/inc/WNAllocation.h"
#include "WNCore/inc/WNTypeTraits.h"

WN_NAMESPACE_BEGIN(memory)

template <typename _T>
class intrusive_ptr final {
public:
  typedef _T element_type;

  WN_FORCE_INLINE intrusive_ptr() :
    m_pointer(wn_nullptr) {
  }

  WN_FORCE_INLINE intrusive_ptr(const wn_nullptr_t) :
    intrusive_ptr() {
  }

  WN_FORCE_INLINE explicit intrusive_ptr(element_type* _ptr,
                                         const wn_bool _add_ref = wn_true) :
    intrusive_ptr() {
    reset(_ptr, _add_ref);
  }

  template <typename _Y>
  WN_FORCE_INLINE explicit intrusive_ptr(_Y* _ptr,
                                         const wn_bool _add_ref = wn_true) :
    intrusive_ptr() {
    reset(_ptr, _add_ref);
  }

  WN_FORCE_INLINE intrusive_ptr(intrusive_ptr&& _ptr) :
    intrusive_ptr() {
    swap(_ptr);
  }

  WN_FORCE_INLINE intrusive_ptr(const intrusive_ptr& _ptr) :
    intrusive_ptr() {
    reset(_ptr.get());
  }

  template <typename _Y>
  WN_FORCE_INLINE intrusive_ptr(intrusive_ptr<_Y>&& _ptr) :
    intrusive_ptr() {
    reset(_ptr.get());

    _ptr.reset();
  }

  template <typename _Y>
  WN_FORCE_INLINE intrusive_ptr(const intrusive_ptr<_Y>& _ptr) :
    intrusive_ptr() {
    reset(_ptr.get());
  }

  WN_FORCE_INLINE ~intrusive_ptr() {
    remove_reference();
  }

  WN_FORCE_INLINE element_type* operator -> () const {
    return(get());
  }

  WN_FORCE_INLINE element_type& operator * () const {
    return(*get());
  }

  WN_FORCE_INLINE operator wn_bool () const {
    return(get() != wn_nullptr);
  }

  WN_FORCE_INLINE intrusive_ptr& operator = (const wn_nullptr_t) {
    return(swap(intrusive_ptr()), *this);
  }

  WN_FORCE_INLINE intrusive_ptr& operator = (intrusive_ptr&& _ptr) {
    return(swap(intrusive_ptr(std::move(_ptr))), *this);
  }

  WN_FORCE_INLINE intrusive_ptr& operator = (const intrusive_ptr& _ptr) {
    return(swap(intrusive_ptr(_ptr)), *this);
  }

  template <typename _Y>
  WN_FORCE_INLINE intrusive_ptr& operator = (intrusive_ptr<_Y>&& _ptr) {
    return(swap(intrusive_ptr(std::move(_ptr))), *this);
  }

  template <typename _Y>
  WN_FORCE_INLINE intrusive_ptr& operator = (const intrusive_ptr<_Y>& _ptr) {
    return(swap(intrusive_ptr(_ptr)), *this);
  }

  WN_FORCE_INLINE element_type* get() const {
    return(m_pointer);
  }

  WN_FORCE_INLINE wn_size_t use_count() const {
    const element_type* pointer = get();

    if (pointer != wn_nullptr) {
      return(pointer->reference_count());
    }

    return(0);
  }

  WN_FORCE_INLINE wn_bool unique() const {
    return(use_count() == 1);
  }

  WN_FORCE_INLINE wn_void reset() {
    remove_reference();
  }

  WN_FORCE_INLINE wn_void reset(wn_nullptr_t) {
    reset();
  }

  WN_FORCE_INLINE wn_void reset(element_type* _ptr,
                                const wn_bool _add_ref = wn_true) {
    if (_ptr != wn_nullptr && _add_ref) {
      _ptr->add_reference();
    }

    remove_reference();

    m_pointer = _ptr;
  }

  template <typename _Y>
  WN_FORCE_INLINE wn_void reset(_Y* _ptr,
                                const wn_bool _add_ref = wn_true) {
    reset(static_cast<element_type*>(_ptr), _add_ref);
  }

  WN_FORCE_INLINE wn_void swap(intrusive_ptr& _ptr) {
    const intrusive_ptr pointer(_ptr);

    _ptr.reset(get());

    reset(pointer.get());
  }

  WN_FORCE_INLINE element_type* release() {
    element_type* pointer = get();

    m_pointer = wn_nullptr;

    return(pointer);
  }

private:
  template <typename _Y>
  friend class intrusive_ptr;

  WN_FORCE_INLINE wn_void add_reference() const {
    if (m_pointer != wn_nullptr) {
      m_pointer->add_reference();
    }
  }

  WN_FORCE_INLINE wn_void remove_reference() {
    if (m_pointer != wn_nullptr) {
      if (m_pointer->remove_reference()) {
        destroy(m_pointer);
      }

      m_pointer = wn_nullptr;
    }
  }

  element_type* m_pointer;
};

template <typename _T, typename _U>
WN_FORCE_INLINE wn_bool operator == (const intrusive_ptr<_T>& _lhs,
                                     const intrusive_ptr<_U>& _rhs) {
  return(_lhs.get() == _rhs.get());
}

template <typename _T, typename _U>
WN_FORCE_INLINE wn_bool operator != (const intrusive_ptr<_T>& _lhs,
                                     const intrusive_ptr<_U>& _rhs) {
  return(_lhs.get() != _rhs.get());
}

template <typename _T, typename _U>
WN_FORCE_INLINE wn_bool operator < (const intrusive_ptr<_T>& _lhs,
                                    const intrusive_ptr<_U>& _rhs) {
  return(_lhs.get() < _rhs.get());
}

template <typename _T, typename _U>
WN_FORCE_INLINE wn_bool operator > (const intrusive_ptr<_T>& _lhs,
                                    const intrusive_ptr<_U>& _rhs) {
  return(_lhs.get() > _rhs.get());
}

template <typename _T, typename _U>
WN_FORCE_INLINE wn_bool operator <= (const intrusive_ptr<_T>& _lhs,
                                     const intrusive_ptr<_U>& _rhs) {
  return(_lhs.get() <= _rhs.get());
}

template <typename _T, typename _U>
WN_FORCE_INLINE wn_bool operator >= (const intrusive_ptr<_T>& _lhs,
                                     const intrusive_ptr<_U>& _rhs) {
  return(_lhs.get() >= _rhs.get());
}

template <typename _T>
WN_FORCE_INLINE wn_bool operator == (const intrusive_ptr<_T>& _ptr,
                                     const wn_nullptr_t) {
  return(_ptr.get() == wn_nullptr);
}

template <typename _T>
WN_FORCE_INLINE wn_bool operator == (const wn_nullptr_t,
                                     const intrusive_ptr<_T>& _ptr) {
  return(wn_nullptr == _ptr.get());
}

template <typename _T>
WN_FORCE_INLINE wn_bool operator != (const intrusive_ptr<_T>& _ptr,
                                     const wn_nullptr_t) {
  return(_ptr.get() != wn_nullptr);
}

template <typename _T>
WN_FORCE_INLINE wn_bool operator != (const wn_nullptr_t,
                                     const intrusive_ptr<_T>& _ptr) {
  return(wn_nullptr != _ptr.get());
}

template <typename _T>
WN_FORCE_INLINE wn_bool operator < (const intrusive_ptr<_T>& _ptr,
                                    const wn_nullptr_t) {
  return(_ptr.get() < wn_nullptr);
}

template <typename _T>
WN_FORCE_INLINE wn_bool operator < (const wn_nullptr_t,
                                    const intrusive_ptr<_T>& _ptr) {
  return(wn_nullptr < _ptr.get());
}

template <typename _T>
WN_FORCE_INLINE wn_bool operator > (const intrusive_ptr<_T>& _ptr,
                                    const wn_nullptr_t) {
  return(_ptr.get() > wn_nullptr);
}

template <typename _T>
WN_FORCE_INLINE wn_bool operator > (const wn_nullptr_t,
                                    const intrusive_ptr<_T>& _ptr) {
  return(wn_nullptr > _ptr.get());
}

template <typename _T>
WN_FORCE_INLINE wn_bool operator <= (const intrusive_ptr<_T>& _ptr,
                                     const wn_nullptr_t) {
  return(_ptr.get() <= wn_nullptr);
}

template <typename _T>
WN_FORCE_INLINE wn_bool operator <= (const wn_nullptr_t,
                                     const intrusive_ptr<_T>& _ptr) {
  return(wn_nullptr <= _ptr.get());
}

template <typename _T>
WN_FORCE_INLINE wn_bool operator >= (const intrusive_ptr<_T>& _ptr,
                                     const wn_nullptr_t) {
  return(_ptr.get() >= wn_nullptr);
}

template <typename _T>
WN_FORCE_INLINE wn_bool operator >= (const wn_nullptr_t,
                                     const intrusive_ptr<_T>& _ptr) {
  return(wn_nullptr >= _ptr.get());
}

template <typename _T, typename... _Args>
WN_FORCE_INLINE intrusive_ptr<_T> make_intrusive(_Args&&... _args) {
  return(intrusive_ptr<_T>(construct<_T>(std::forward<_Args>(_args)...)));
}

WN_NAMESPACE_END

#endif // __WN_MEMORY_INTRUSIVE_PTR_H__