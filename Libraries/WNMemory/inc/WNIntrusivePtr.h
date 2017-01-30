// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MEMORY_INTRUSIVE_PTR_H__
#define __WN_MEMORY_INTRUSIVE_PTR_H__

#include "WNMemory/inc/WNAllocator.h"

#include <atomic>

namespace wn {
namespace memory {

template <typename T>
class intrusive_ptr;

class intrusive_ptr_base {
public:
  WN_FORCE_INLINE void add_reference() {
    for (;;) {
      size_t count = m_reference_count;
      const size_t target_count = count + 1;

      if (m_reference_count.compare_exchange_strong(count, target_count)) {
        break;
      }
    }
  }

  WN_FORCE_INLINE bool remove_reference() {
    for (;;) {
      size_t count = m_reference_count;
      const size_t target_count = count - 1;

      if (m_reference_count.compare_exchange_strong(count, target_count)) {
        return (target_count == 0);
      }
    }
  }

  WN_FORCE_INLINE size_t reference_count() const {
    return m_reference_count;
  }

  WN_FORCE_INLINE allocator* get_allocator() const {
    return m_allocator;
  }

protected:
  WN_FORCE_INLINE intrusive_ptr_base() : intrusive_ptr_base(nullptr) {}

  WN_FORCE_INLINE explicit intrusive_ptr_base(allocator* _allocator)
    : m_allocator(_allocator), m_reference_count(0) {}

private:
  template <typename T>
  friend class intrusive_ptr;

  std::atomic_size_t m_reference_count;
  allocator* m_allocator;
};

template <typename T>
class intrusive_ptr final {
public:
  static_assert(core::is_base_of<intrusive_ptr_base, T>::value,
      "type must be child of 'intrusive_ptr_base'");

  typedef T* pointer;
  typedef typename core::add_lvalue_reference_t<T> reference;
  typedef T element_type;

  WN_FORCE_INLINE intrusive_ptr() : m_pointer(nullptr) {}

  WN_FORCE_INLINE intrusive_ptr(const nullptr_t) : intrusive_ptr() {}

  template <typename U,
      typename = core::enable_if_t<core::is_convertible<U*, pointer>::value>>
  WN_FORCE_INLINE intrusive_ptr(allocator* _allocator, U* _ptr)
    : intrusive_ptr(_allocator, _ptr, true) {}

  WN_FORCE_INLINE intrusive_ptr(intrusive_ptr&& _other)
    : intrusive_ptr(_other.get_allocator(), _other.get(), false) {
    _other.clear();
  }

  WN_FORCE_INLINE intrusive_ptr(const intrusive_ptr& _other)
    : intrusive_ptr(_other.get_allocator(), _other.get(), true) {}

  template <typename U>
  WN_FORCE_INLINE intrusive_ptr(intrusive_ptr<U>&& _other)
    : intrusive_ptr(_other.get_allocator(), _other.get(), false) {
    _other.clear();
  }

  template <typename U>
  WN_FORCE_INLINE intrusive_ptr(const intrusive_ptr<U>& _other)
    : intrusive_ptr(_other.get_allocator(), _other.get(), true) {}

  WN_FORCE_INLINE ~intrusive_ptr() {
    pointer p = get();

    if (p) {
      if (p->remove_reference()) {
        p->get_allocator()->destroy(p);
      }
    }
  }

  WN_FORCE_INLINE pointer operator->() const {
    return get();
  }

  WN_FORCE_INLINE reference operator*() const {
    WN_RELEASE_ASSERT_DESC(get() != nullptr, "cannot dereference nullptr");

    return *get();
  }

  WN_FORCE_INLINE operator bool() const {
    return (get() != nullptr);
  }

  WN_FORCE_INLINE intrusive_ptr& operator=(const nullptr_t) {
    intrusive_ptr(nullptr).swap(*this);

    return *this;
  }

  WN_FORCE_INLINE intrusive_ptr& operator=(intrusive_ptr&& _other) {
    intrusive_ptr(core::move(_other)).swap(*this);

    return *this;
  }

  WN_FORCE_INLINE intrusive_ptr& operator=(const intrusive_ptr& _other) {
    intrusive_ptr(_other).swap(*this);

    return *this;
  }

  template <typename U>
  WN_FORCE_INLINE intrusive_ptr& operator=(intrusive_ptr<U>&& _other) {
    intrusive_ptr(core::move(_other)).swap(*this);

    return *this;
  }

  template <typename U>
  WN_FORCE_INLINE intrusive_ptr& operator=(const intrusive_ptr<U>& _other) {
    intrusive_ptr(_other).swap(*this);

    return *this;
  }

  WN_FORCE_INLINE pointer get() const {
    return m_pointer;
  }

  WN_FORCE_INLINE allocator* get_allocator() const {
    if (m_pointer) {
      return m_pointer->get_allocator();
    }

    return nullptr;
  }

  WN_FORCE_INLINE size_t use_count() const {
    return (get() ? get()->reference_count() : 0);
  }

  WN_FORCE_INLINE bool unique() const {
    return (use_count() == 1);
  }

  WN_FORCE_INLINE void reset() {
    intrusive_ptr().swap(*this);
  }

  WN_FORCE_INLINE void reset(const nullptr_t) {
    intrusive_ptr(nullptr).swap(*this);
  }

  template <typename U,
      typename = core::enable_if_t<core::is_convertible<U*, pointer>::value>>
  WN_FORCE_INLINE void reset(allocator* _allocator, U* _ptr) {
    intrusive_ptr(_allocator, _ptr).swap(*this);
  }

  WN_FORCE_INLINE void swap(intrusive_ptr& _other) {
    core::swap(m_pointer, _other.m_pointer);
  }

  WN_FORCE_INLINE pointer release() {
    pointer p = get();

    clear();

    return p;
  }

private:
  template <typename U>
  friend class intrusive_ptr;

  template <typename U,
      typename = core::enable_if_t<core::is_convertible<U*, pointer>::value>>
  WN_FORCE_INLINE intrusive_ptr(
      allocator* _allocator, U* _ptr, const bool _add_ref)
    : m_pointer(_ptr) {
    if (m_pointer) {
      allocator* a = m_pointer->get_allocator();

      if (a) {
        WN_RELEASE_ASSERT_DESC(_allocator == a, "incorrect allocator given");
      } else {
        intrusive_ptr_base* p = static_cast<intrusive_ptr_base*>(_ptr);

        p->m_allocator = _allocator;
      }

      if (_add_ref) {
        m_pointer->add_reference();
      }
    }
  }

  WN_FORCE_INLINE void clear() {
    m_pointer = nullptr;
  }

  pointer m_pointer;
};

template <typename T, typename U>
WN_FORCE_INLINE bool operator==(
    const intrusive_ptr<T>& lhs, const intrusive_ptr<U>& rhs) {
  return (lhs.get() == rhs.get());
}

template <typename T, typename U>
WN_FORCE_INLINE bool operator!=(
    const intrusive_ptr<T>& lhs, const intrusive_ptr<U>& rhs) {
  return (lhs.get() != rhs.get());
}

template <typename T, typename U>
WN_FORCE_INLINE bool operator<(
    const intrusive_ptr<T>& lhs, const intrusive_ptr<U>& rhs) {
  return (lhs.get() < rhs.get());
}

template <typename T, typename U>
WN_FORCE_INLINE bool operator>(
    const intrusive_ptr<T>& lhs, const intrusive_ptr<U>& rhs) {
  return (lhs.get() > rhs.get());
}

template <typename T, typename U>
WN_FORCE_INLINE bool operator<=(
    const intrusive_ptr<T>& lhs, const intrusive_ptr<U>& rhs) {
  return (lhs.get() <= rhs.get());
}

template <typename T, typename U>
WN_FORCE_INLINE bool operator>=(
    const intrusive_ptr<T>& lhs, const intrusive_ptr<U>& rhs) {
  return (lhs.get() >= rhs.get());
}

template <typename T>
WN_FORCE_INLINE bool operator==(const intrusive_ptr<T>& ptr, const nullptr_t) {
  return (ptr.get() == nullptr);
}

template <typename T>
WN_FORCE_INLINE bool operator==(const nullptr_t, const intrusive_ptr<T>& ptr) {
  return (nullptr == ptr.get());
}

template <typename T>
WN_FORCE_INLINE bool operator!=(const intrusive_ptr<T>& ptr, const nullptr_t) {
  return (ptr.get() != nullptr);
}

template <typename T>
WN_FORCE_INLINE bool operator!=(const nullptr_t, const intrusive_ptr<T>& ptr) {
  return (nullptr != ptr.get());
}

template <typename T>
WN_FORCE_INLINE bool operator<(const intrusive_ptr<T>& ptr, const nullptr_t) {
  return (ptr.get() < nullptr);
}

template <typename T>
WN_FORCE_INLINE bool operator<(const nullptr_t, const intrusive_ptr<T>& ptr) {
  return (nullptr < ptr.get());
}

template <typename T>
WN_FORCE_INLINE bool operator>(const intrusive_ptr<T>& ptr, const nullptr_t) {
  return (ptr.get() > nullptr);
}

template <typename T>
WN_FORCE_INLINE bool operator>(const nullptr_t, const intrusive_ptr<T>& ptr) {
  return (nullptr > ptr.get());
}

template <typename T>
WN_FORCE_INLINE bool operator<=(const intrusive_ptr<T>& ptr, const nullptr_t) {
  return (ptr.get() <= nullptr);
}

template <typename T>
WN_FORCE_INLINE bool operator<=(const nullptr_t, const intrusive_ptr<T>& ptr) {
  return (nullptr <= ptr.get());
}

template <typename T>
WN_FORCE_INLINE bool operator>=(const intrusive_ptr<T>& ptr, const nullptr_t) {
  return (ptr.get() >= nullptr);
}

template <typename T>
WN_FORCE_INLINE bool operator>=(const nullptr_t, const intrusive_ptr<T>& ptr) {
  return (nullptr >= ptr.get());
}

template <typename T, typename... Args>
WN_FORCE_INLINE intrusive_ptr<T> make_intrusive(
    allocator* _allocator, Args&&... _args) {
  T* ptr = _allocator->construct<T>(core::forward<Args>(_args)...);

  if (ptr) {
    return intrusive_ptr<T>(_allocator, ptr);
  }

  return nullptr;
}

}  // namespace memory
}  // namespace wn

#endif  // __WN_MEMORY_INTRUSIVE_PTR_H__
