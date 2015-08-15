// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MEMORY_ALLOCATOR_H__
#define __WN_MEMORY_ALLOCATOR_H__

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
  void notify_returned(void *ptr, wn_size_t requested, wn_size_t returned) {
    std::lock_guard<wn::concurrency::mutex> guard(free_lock);
    m_elements[ptr] = returned;
    m_total_requested += requested;
    m_total_returned += returned;
  }

  void notify_freed(void *ptr) {
    std::lock_guard<wn::concurrency::mutex> guard(free_lock);
    m_total_freed += m_elements[ptr];
    m_elements.erase(ptr);
  }

  void notify_resize(void *ptr, void *ret_ptr, wn_size_t requested,
                     wn_size_t returned) {
    std::lock_guard<wn::concurrency::mutex> guard(free_lock);
    m_total_freed += m_elements[ptr];
    m_elements.erase(ptr);
    m_elements[ret_ptr] = returned;
    m_total_requested += requested;
    m_total_returned += returned;
  }

  wn_size_t m_total_requested;
  wn_size_t m_total_returned;
  wn_size_t m_total_freed;
  wn::concurrency::mutex free_lock;
  std::unordered_map<void *, size_t> m_elements;
};

class allocator {
public:
  virtual allocation_pair allocate(const wn_size_t size,
                                   const wn_size_t count) = 0;
  virtual allocation_pair allocate_for_resize(const wn_size_t size,
                                              const wn_size_t count,
                                              const wn_size_t old_size) = 0;
  virtual allocation_pair reallocate(wn_void *ptr, const wn_size_t _number,
                                     const wn_size_t count,
                                     const wn_size_t old_size = 0) = 0;
  virtual wn_void deallocate(wn_void *ptr) = 0;
 public:
#ifdef WN_TRACK_ALLOCATIONS
  virtual ~allocator() {
    WN_RELEASE_ASSERT(m_tracker.m_total_returned == m_tracker.m_total_freed);
  }
#else
  virtual ~allocator() = default;
#endif
  template <typename T, typename... Args>
  T *make_allocated(Args &&... args) {
    allocation_pair p = allocate(sizeof(T), 1);
    return construct_at<T, Args...>(p.m_location, std::forward<Args>(args)...);
  }

  template <typename T>
  allocation_pair allocate_element(const wn_size_t count = 1) {
    return allocate(sizeof(T), count);
  }
#ifdef WN_TRACK_ALLOCATIONS
  void notify_returned(void *ptr, wn_size_t requested, wn_size_t returned) {
    m_tracker.notify_returned(ptr, requested, returned);
  }

  void notify_freed(void *ptr) { m_tracker.notify_freed(ptr); }

  void notify_resize(void *ptr, void *ret_ptr, wn_size_t requested,
                     wn_size_t returned) {
    m_tracker.notify_resize(ptr, ret_ptr, requested, returned);
  }
  allocation_tracker m_tracker;
#else
  void WN_FORCE_INLINE notify_returned(void *, wn_size_t, wn_size_t) {}
  void WN_FORCE_INLINE notify_freed(void *) {}
  void WN_FORCE_INLINE notify_resize(void *, void *, wn_size_t, wn_size_t) {}
#endif
};

struct allocated_destroyer {
  allocated_destroyer() : _allocator(wn_nullptr), _deleter(wn_nullptr) {}
  allocated_destroyer(allocator *allocator, void (*deleter)(void *))
      : _allocator(allocator), _deleter(deleter) {}
  WN_FORCE_INLINE wn_void operator()(void *ptr) const {
    (*_deleter)(ptr);
    _allocator->deallocate(ptr);
  }

  allocated_destroyer(allocated_destroyer &&other) {
    _deleter = std::move(other._deleter);
    _allocator = other._allocator;
    other._allocator = wn_nullptr;
    other._deleter = wn_nullptr;
  }

  allocated_destroyer &operator=(allocated_destroyer &&other) {
    _deleter = std::move(other._deleter);
    _allocator = other._allocator;
    other._allocator = wn_nullptr;
    other._deleter = wn_nullptr;
    return *this;
  }

  template <typename T>
  static void typed_deleter(void *v) {
    T *t = static_cast<T *>(v);
    (*t).~T();
  }
  allocator *_allocator;
  void (*_deleter)(void *);
};

template <typename T>
struct typed_destroyer : public allocated_destroyer {
 public:
  void destroy(void *v) { destroy(reinterpret_cast<T *>(v)); }
};

template <typename T>
using allocated_ptr = wn::memory::unique_ptr<T, allocated_destroyer>;

template <typename T, typename... Args>
WN_FORCE_INLINE allocated_ptr<T> make_allocated_ptr(allocator *allocator,
  Args &&... args) {
  allocation_pair p = allocator->allocate_element<T>();
  T *ptr = construct_at<T, Args...>(p.m_location, std::forward<Args>(args)...);
  return (allocated_ptr<T>(
    ptr,
    allocated_destroyer(allocator, &allocated_destroyer::typed_deleter<T>)));
}

template <typename T, typename... Args>
WN_FORCE_INLINE allocated_ptr<T> default_allocated_ptr(allocator *allocator,
  T *ptr) {
  return (allocated_ptr<T>(
    ptr,
    allocated_destroyer(allocator, &allocated_destroyer::typed_deleter<T>)));
}

template <const wn_size_t _ExpandPercent,
          const wn_size_t _MinimumAllocationSize = 1>
class default_expanding_allocator : public allocator {
public:
  virtual allocation_pair allocate(const wn_size_t size,
                                   const wn_size_t count) override {
    const wn_size_t count_max =
      count > _MinimumAllocationSize ? count : _MinimumAllocationSize;
    allocation_pair pair{malloc(size * count_max), count_max};

    return(pair);
  }

  virtual allocation_pair allocate_for_resize(
      const wn_size_t size, const wn_size_t count,
      const wn_size_t old_size) override {
    const wn_size_t count_max =
      count > _MinimumAllocationSize ? count : _MinimumAllocationSize;
    const wn_size_t new_size =
      static_cast<wn_size_t>(old_size * (1 + (_ExpandPercent / 100.0f)));
    const wn_size_t allocated_number =
      new_size > count_max ? new_size : count_max;
    allocation_pair pair{malloc(size * allocated_number), allocated_number};

    return(pair);
  }

  virtual allocation_pair reallocate(wn_void *ptr, const wn_size_t size,
                                     const wn_size_t count,
                                     const wn_size_t old_size = 0) override {
    const wn_size_t count_max =
      count > _MinimumAllocationSize ? count : _MinimumAllocationSize;
    const wn_size_t new_size =
      static_cast<wn_size_t>(old_size * (1 + (_ExpandPercent / 100.0f)));
    const wn_size_t allocated_number =
      new_size > count_max ? new_size : count_max;
    allocation_pair pair{realloc(ptr, size * allocated_number), count_max};
    notify_resize(ptr, pair.m_location, size * count, size * allocated_number);
    return (pair);
  }

  virtual wn_void deallocate(wn_void *ptr) {
    free(ptr);
  }
};

template <typename parent>
class test_allocator : public parent {
 public:
  size_t allocated() { return debug_tracker.m_total_returned; }
  size_t freed() { return debug_tracker.m_total_freed; }
  bool empty() {
    return debug_tracker.m_total_returned == debug_tracker.m_total_freed;
  }
  virtual allocation_pair allocate(const wn_size_t size,
                                   const wn_size_t count) override {
    allocation_pair p = parent::allocate(size, count);
    debug_tracker.notify_returned(p.m_location, size * count, size * p.m_count);
    return p;
  }
  virtual allocation_pair allocate_for_resize(
      const wn_size_t size, const wn_size_t count,
      const wn_size_t old_size) override {
    allocation_pair p = parent::allocate_for_resize(size, count, old_size);
    debug_tracker.notify_returned(p.m_location, size * count, size * p.m_count);
    return p;
  }
  virtual allocation_pair reallocate(wn_void *ptr, const wn_size_t size,
                                     const wn_size_t count,
                                     const wn_size_t old_size = 0) override {
    allocation_pair p = parent::reallocate(ptr, size, count, old_size);
    debug_tracker.notify_resize(ptr, p.m_location, size * count,
                                size * p.m_count);
    return p;
  }
  virtual void deallocate(wn_void *ptr) override {
    parent::deallocate(ptr);
    debug_tracker.notify_freed(ptr);
  }

  allocation_tracker debug_tracker;
};

typedef test_allocator<default_expanding_allocator<50>> default_test_allocator;
typedef default_expanding_allocator<50> default_allocator;

} // namespace memory
} // namespace wn

#endif  // __WN_MEMORY_ALLOCATOR_H__
