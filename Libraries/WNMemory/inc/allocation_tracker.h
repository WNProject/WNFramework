// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MEMORY_ALLOCATION_TRACKER_H__
#define __WN_MEMORY_ALLOCATION_TRACKER_H__

#include "WNMemory/inc/allocator.h"
#include "core/inc/assert.h"

#include <mutex>
#include <unordered_map>

namespace wn {
namespace memory {

class allocation_tracker : public allocator {
public:
  allocation_tracker(allocator* _allocator)
    : allocator(),
      m_allocator(_allocator),
      m_total_allocated(0),
      m_total_deallocated(0) {}

  virtual ~allocation_tracker() override {
    WN_DEBUG_ASSERT(
        empty(), "memory leak detected, did not deallocate all allocations");
  }

  virtual void* allocate(size_t _size) override {
    void* ptr = m_allocator->allocate(_size);

    notify_allocated(ptr, _size);

    return ptr;
  }

  virtual void* reallocate(void* _ptr, size_t _new_size) override {
    void* ptr = m_allocator->reallocate(_ptr, _new_size);

    notify_reallocated(_ptr, ptr, _new_size);

    return ptr;
  }

  virtual void deallocate(void* _ptr) override {
    notify_deallocated(_ptr);

    m_allocator->deallocate(_ptr);
  }

  virtual void* aligned_allocate(size_t _size, size_t _alignment) override {
    void* ptr = m_allocator->aligned_allocate(_size, _alignment);

    notify_allocated(ptr, _size);

    return ptr;
  }

  virtual void* aligned_reallocate(
      void* _ptr, size_t _new_size, size_t _alignment) override {
    void* ptr = m_allocator->aligned_reallocate(_ptr, _new_size, _alignment);

    notify_reallocated(_ptr, ptr, _new_size);

    return ptr;
  }

  virtual void aligned_deallocate(void* _ptr) override {
    notify_deallocated(_ptr);

    m_allocator->aligned_deallocate(_ptr);
  }

  size_t allocated() const {
    return m_total_allocated;
  }

  size_t deallocated() const {
    return m_total_deallocated;
  }

  bool empty() const {
    return allocated() == deallocated();
  }

  std::unordered_map<void*, size_t> current_allocations() const {
    return m_elements;
  }

private:
  void notify_allocated(void* _ptr, size_t _size) {
    const std::lock_guard<std::mutex> guard(m_lock);

    m_elements[_ptr] = _size;
    m_total_allocated += _size;
  }

  void notify_reallocated(void* _old_ptr, void* _new_ptr, size_t _size) {
    const std::lock_guard<std::mutex> guard(m_lock);

    if (_old_ptr) {
      WN_RELEASE_ASSERT(m_elements.find(_old_ptr) != m_elements.end(),
          "you are trying to free a pointer from an incorrect allocator");

      m_total_deallocated += m_elements[_old_ptr];
      m_elements.erase(_old_ptr);
    }

    m_elements[_new_ptr] = _size;
    m_total_allocated += _size;
  }

  void notify_deallocated(void* _ptr) {
    if (_ptr) {
      const std::lock_guard<std::mutex> guard(m_lock);

      WN_RELEASE_ASSERT(m_elements.find(_ptr) != m_elements.end(),
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

}  // namespace memory
}  // namespace wn

#endif  // __WN_MEMORY_ALLOCATION_TRACKER_H__
