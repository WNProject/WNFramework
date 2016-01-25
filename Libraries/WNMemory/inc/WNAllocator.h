// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MEMORY_ALLOCATOR_H__
#define __WN_MEMORY_ALLOCATOR_H__

#include "WNCore/inc/WNAssert.h"
#include "WNMemory/inc/WNBasic.h"

#include <functional>
#include <mutex>
#include <unordered_map>

namespace wn {
namespace memory {

class allocator {
public:
  allocator() = default;

  virtual ~allocator() = default;

  virtual void* allocate(const size_t _size) = 0;
  virtual void* reallocate(void* _ptr, const size_t _new_size) = 0;
  virtual void deallocate(void* _ptr) = 0;

  template <typename T, typename... Args>
  WN_FORCE_INLINE T* construct(Args&&... _args) {
    void* ptr = allocate(sizeof(T));

    return construct(static_cast<T*>(ptr), core::forward<Args>(_args)...);
  }

  template <typename T, typename... Args>
  WN_FORCE_INLINE T* construct(T* _ptr, Args&&... _args) {
    if (_ptr) {
      return new (_ptr) T(core::forward<Args>(_args)...);
    }

    return nullptr;
  }

  template <typename T>
  WN_FORCE_INLINE void destroy(T* _ptr) {
    _ptr->~T();

    deallocate(_ptr);
  }
};

class allocation_tracker : public allocator {
public:
  WN_FORCE_INLINE allocation_tracker(allocator* _allocator)
    : allocator(),
      m_allocator(_allocator),
      m_total_allocated(0),
      m_total_deallocated(0) {}

  WN_FORCE_INLINE virtual ~allocation_tracker() override {
    WN_DEBUG_ASSERT_DESC(
        empty(), "memory leak detected, did not deallocate all allocations");
  }

  WN_FORCE_INLINE virtual void* allocate(const size_t _size) override {
    void* ptr = m_allocator->allocate(_size);

    notify_allocated(ptr, _size);

    return ptr;
  }

  WN_FORCE_INLINE virtual void* reallocate(
      void* _ptr, const size_t _new_size) override {
    void* ptr = m_allocator->reallocate(_ptr, _new_size);

    notify_reallocated(_ptr, ptr, _new_size);

    return ptr;
  }

  WN_FORCE_INLINE virtual void deallocate(void* _ptr) override {
    notify_deallocated(_ptr);

    m_allocator->deallocate(_ptr);
  }

  WN_FORCE_INLINE size_t allocated() const {
    return m_total_allocated;
  }

  WN_FORCE_INLINE size_t deallocated() const {
    return m_total_deallocated;
  }

  WN_FORCE_INLINE bool empty() const {
    return allocated() == deallocated();
  }

private:
  WN_FORCE_INLINE void notify_allocated(void* _ptr, const size_t _size) {
    const std::lock_guard<std::mutex> guard(m_lock);

    m_elements[_ptr] = _size;
    m_total_allocated += _size;
  }

  WN_FORCE_INLINE void notify_reallocated(
      void* _old_ptr, void* _new_ptr, const size_t _size) {
    const std::lock_guard<std::mutex> guard(m_lock);

    if (_old_ptr) {
      WN_RELEASE_ASSERT_DESC(m_elements.find(_old_ptr) != m_elements.end(),
          "you are trying to free a pointer from an incorrect allocator");

      m_total_deallocated += m_elements[_old_ptr];
      m_elements.erase(_old_ptr);
    }

    m_elements[_new_ptr] = _size;
    m_total_allocated += _size;
  }

  WN_FORCE_INLINE void notify_deallocated(void* _ptr) {
    if (_ptr) {
      const std::lock_guard<std::mutex> guard(m_lock);

      WN_RELEASE_ASSERT_DESC(m_elements.find(_ptr) != m_elements.end(),
          "you are trying to free a pointer from an incorrect allocator");

      m_total_deallocated += m_elements[_ptr];
      m_elements.erase(_ptr);
    }
  }

  std::unordered_map<void*, size_t> m_elements;
  std::mutex m_lock;
  allocator* m_allocator;
  size_t m_total_allocated;
  size_t m_total_deallocated;
};

class basic_allocator : public allocator {
public:
  WN_FORCE_INLINE basic_allocator() : allocator() {}

  WN_FORCE_INLINE virtual void* allocate(const size_t _size) override {
    return malloc(_size);
  }

  WN_FORCE_INLINE virtual void* reallocate(
      void* _ptr, const size_t _new_size) override {
    return realloc(_ptr, _new_size);
  }

  WN_FORCE_INLINE virtual void deallocate(void* _ptr) override {
    free(_ptr);
  }
};

}  // namespace memory
}  // namespace wn

#endif  // __WN_MEMORY_ALLOCATOR_H__
