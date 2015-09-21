// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MEMORY_ALLOCATOR_H__
#define __WN_MEMORY_ALLOCATOR_H__

#include "WNCore/inc/WNAssert.h"
#include "WNCore/inc/WNTypes.h"
#include "WNConcurrency/inc/WNMutex.h"
#include "WNMemory/inc/WNBasic.h"
#include "WNMemory/inc/WNUniquePtr.h"
#include <functional>
#include <mutex>
#include <unordered_map>

namespace wn {
namespace memory {

struct allocation_pair final {
  wn_void *m_location;
  wn_size_t m_count;
};

class allocation_tracker {
 public:
  allocation_tracker()
      : m_total_requested(0), m_total_returned(0), m_total_freed(0) {}
  void notify_returned(void *_ptr, wn_size_t _requested, wn_size_t _returned) {
    std::lock_guard<wn::concurrency::mutex> guard(free_lock);
    m_elements[_ptr] = _returned;
    m_total_requested += _requested;
    m_total_returned += _returned;
  }

  void notify_freed(void *_ptr) {
    if (!_ptr) { return; }
    std::lock_guard<wn::concurrency::mutex> guard(free_lock);
    WN_RELEASE_ASSERT_DESC(m_elements.find(_ptr) != m_elements.end(),
      "You are trying to free a pointer from an incorrect allocator");
    m_total_freed += m_elements[_ptr];
    m_elements.erase(_ptr);
  }

  void notify_resize(void *_ptr, void *_ret_ptr, wn_size_t _requested,
                     wn_size_t _returned) {
    std::lock_guard<wn::concurrency::mutex> guard(free_lock);
    if (_ptr) {
      m_total_freed += m_elements[_ptr];
      WN_RELEASE_ASSERT_DESC(m_elements.find(_ptr) != m_elements.end(),
        "You are trying to free a pointer from an incorrect allocator");
      m_elements.erase(_ptr);
    }
    m_elements[_ret_ptr] = _returned;
    m_total_requested += _requested;
    m_total_returned += _returned;
  }

  wn_size_t m_total_requested;
  wn_size_t m_total_returned;
  wn_size_t m_total_freed;
  wn::concurrency::mutex free_lock;
  std::unordered_map<void *, size_t> m_elements;
};

class allocator {
public:
  virtual allocation_pair allocate(const wn_size_t _size,
                                   const wn_size_t _count) = 0;
  virtual allocation_pair allocate_for_resize(const wn_size_t _size,
                                              const wn_size_t _count,
                                              const wn_size_t _old_size) = 0;
  virtual allocation_pair reallocate(wn_void *_ptr, const wn_size_t _number,
                                     const wn_size_t _count,
                                     const wn_size_t _old_size = 0) = 0;
  virtual wn_void deallocate(wn_void *_ptr) = 0;
 public:
#ifdef WN_TRACK_ALLOCATIONS
  virtual ~allocator() {
    WN_RELEASE_ASSERT(m_tracker.m_total_returned == m_tracker.m_total_freed);
  }
#else
  virtual ~allocator() = default;
#endif
  template <typename T, typename... Args>
  T *make_allocated(Args &&... _args) {
    allocation_pair p = allocate(sizeof(T), 1);
    return construct_at<T, Args...>(p.m_location, std::forward<Args>(_args)...);
  }

  template <typename T>
  allocation_pair allocate_element(const wn_size_t _count = 1) {
    return allocate(sizeof(T), _count);
  }
#ifdef WN_TRACK_ALLOCATIONS
  void notify_returned(void *_ptr, wn_size_t _requested, wn_size_t _returned) {
    m_tracker.notify_returned(_ptr, _requested, _returned);
  }

  void notify_freed(void *_ptr) { m_tracker.notify_freed(_ptr); }

  void notify_resize(void *_ptr, void *_ret_ptr, wn_size_t _requested,
                     wn_size_t _returned) {
    m_tracker.notify_resize(_ptr, _ret_ptr, _requested, _returned);
  }
  allocation_tracker m_tracker;
#else
  void WN_FORCE_INLINE notify_returned(void *, wn_size_t, wn_size_t) {}
  void WN_FORCE_INLINE notify_freed(void *) {}
  void WN_FORCE_INLINE notify_resize(void *, void *, wn_size_t, wn_size_t) {}
#endif
};

struct allocated_destroyer {
  allocated_destroyer() : m_allocator(wn_nullptr), m_deleter(wn_nullptr) {}
  allocated_destroyer(allocator *allocator, void (*deleter)(void *))
      : m_allocator(allocator), m_deleter(deleter) {}
  WN_FORCE_INLINE wn_void operator()(void *_ptr) const {
    (*m_deleter)(_ptr);
    m_allocator->deallocate(_ptr);
  }

  allocated_destroyer(allocated_destroyer &&_other) {
    m_deleter = std::move(_other.m_deleter);
    m_allocator = _other.m_allocator;
    _other.m_allocator = wn_nullptr;
    _other.m_deleter = wn_nullptr;
  }

  allocated_destroyer &operator=(allocated_destroyer &&_other) {
    m_deleter = std::move(_other.m_deleter);
    m_allocator = _other.m_allocator;
    _other.m_allocator = wn_nullptr;
    _other.m_deleter = wn_nullptr;
    return *this;
  }

  template <typename T>
  static void typed_deleter(void *_v) {
    T *t = static_cast<T *>(_v);
    (*t).~T();
  }
  allocator *m_allocator;
  void (*m_deleter)(void *);
};

template <typename T>
struct typed_destroyer : public allocated_destroyer {
 public:
  void destroy(void *_ptr) { destroy(reinterpret_cast<T *>(_ptr)); }
};

template <typename T>
using allocated_ptr = wn::memory::unique_ptr<T, allocated_destroyer>;

template <typename T, typename... Args>
WN_FORCE_INLINE allocated_ptr<T> make_allocated_ptr(allocator *_allocator,
  Args &&... args) {
  allocation_pair p = _allocator->allocate_element<T>();
  T *ptr = construct_at<T, Args...>(p.m_location, std::forward<Args>(args)...);
  return (allocated_ptr<T>(
    ptr,
    allocated_destroyer(_allocator, &allocated_destroyer::typed_deleter<T>)));
}

template <typename T, typename... Args>
WN_FORCE_INLINE allocated_ptr<T> default_allocated_ptr(allocator *_allocator,
  T *ptr) {
  return (allocated_ptr<T>(
    ptr,
    allocated_destroyer(_allocator, &allocated_destroyer::typed_deleter<T>)));
}

template <const wn_size_t _ExpandPercent,
          const wn_size_t _MinimumAllocationSize = 1>
class default_expanding_allocator : public allocator {
public:
  virtual allocation_pair allocate(const wn_size_t _size,
                                   const wn_size_t _count) override {
    const wn_size_t count_max =
      _count > _MinimumAllocationSize ? _count : _MinimumAllocationSize;
    allocation_pair pair{malloc(_size * count_max), count_max};

    return(pair);
  }

  virtual allocation_pair allocate_for_resize(
      const wn_size_t _size, const wn_size_t _count,
      const wn_size_t _old_size) override {
    const wn_size_t count_max =
      _count > _MinimumAllocationSize ? _count : _MinimumAllocationSize;
    const wn_size_t new_size =
      static_cast<wn_size_t>(_old_size * (1 + (_ExpandPercent / 100.0f)));
    const wn_size_t allocated_number =
      new_size > count_max ? new_size : count_max;
    allocation_pair pair{malloc(_size * allocated_number), allocated_number};

    return(pair);
  }

  virtual allocation_pair reallocate(wn_void *_ptr, const wn_size_t _size,
                                     const wn_size_t _count,
                                     const wn_size_t _old_size = 0) override {
    const wn_size_t count_max =
      _count > _MinimumAllocationSize ? _count : _MinimumAllocationSize;
    const wn_size_t new_size =
      static_cast<wn_size_t>(_old_size * (1 + (_ExpandPercent / 100.0f)));
    const wn_size_t allocated_number =
      new_size > count_max ? new_size : count_max;
    allocation_pair pair{realloc(_ptr, _size * allocated_number), count_max};
    notify_resize(_ptr, pair.m_location, _size * _count, _size * allocated_number);
    return (pair);
  }

  virtual wn_void deallocate(wn_void *_ptr) {
    free(_ptr);
  }
};

template <typename parent>
class test_allocator : public parent {
 public:
  size_t allocated() { return m_debug_tracker.m_total_returned; }
  size_t freed() { return m_debug_tracker.m_total_freed; }
  bool empty() {
    return m_debug_tracker.m_total_returned == m_debug_tracker.m_total_freed;
  }
  virtual allocation_pair allocate(const wn_size_t _size,
                                   const wn_size_t _count) override {
    allocation_pair p = parent::allocate(_size, _count);
    m_debug_tracker.notify_returned(p.m_location, _size * _count, _size * p.m_count);
    return p;
  }
  virtual allocation_pair allocate_for_resize(
      const wn_size_t _size, const wn_size_t _count,
      const wn_size_t _old_size) override {
    allocation_pair p = parent::allocate_for_resize(_size, _count, _old_size);
    m_debug_tracker.notify_returned(p.m_location, _size * _count, _size * p.m_count);
    return p;
  }
  virtual allocation_pair reallocate(wn_void *_ptr, const wn_size_t _size,
                                     const wn_size_t _count,
                                     const wn_size_t _old_size = 0) override {
    allocation_pair p = parent::reallocate(_ptr, _size, _count, _old_size);
    m_debug_tracker.notify_resize(_ptr, p.m_location, _size * _count,
                                _size * p.m_count);
    return p;
  }
  virtual void deallocate(wn_void *_ptr) override {
    parent::deallocate(_ptr);
    m_debug_tracker.notify_freed(_ptr);
  }

  allocation_tracker m_debug_tracker;
};

typedef test_allocator<default_expanding_allocator<50>> default_test_allocator;
typedef default_expanding_allocator<50> default_allocator;

} // namespace memory
} // namespace wn

#endif  // __WN_MEMORY_ALLOCATOR_H__
