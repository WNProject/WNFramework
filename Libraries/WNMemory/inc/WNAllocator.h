// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MEMORY_ALLOCATOR_H__
#define __WN_MEMORY_ALLOCATOR_H__

#include "WNMemory/inc/WNBasic.h"
#include <functional>
#ifdef WN_TRACK_ALLOCATIONS
#include <unordered_map>
#include <mutex>
#include "WNConcurrency/inc/WNMutex.h"
#endif
namespace wn {
namespace memory {

struct allocation_pair final {
  wn_void *m_location;
  wn_size_t m_count;
};

class allocator {
 public:
#ifdef WN_TRACK_ALLOCATIONS
  virtual ~allocator() { 
    WN_RELEASE_ASSERT(m_total_returned == m_total_freed); 
  }
  allocator() : m_total_requested(0), m_total_returned(0), m_total_freed(0) {}
#else
  virtual ~allocator() = default;
#endif

  virtual allocation_pair allocate(const wn_size_t, const wn_size_t) = 0;
  virtual allocation_pair allocate_for_resize(const wn_size_t, const wn_size_t,
                                              const wn_size_t) = 0;
  virtual allocation_pair reallocate(wn_void *, const wn_size_t,
                                     const wn_size_t, const wn_size_t = 0) = 0;
  virtual wn_void deallocate(wn_void *) = 0;

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
    m_elements[ret_ptr] = returned;
    m_elements.erase(ptr);
    m_total_requested += requested;
    m_total_returned += returned;
  }

  wn_size_t m_total_requested;
  wn_size_t m_total_returned;
  wn_size_t m_total_freed;
  wn::concurrency::mutex free_lock;
  std::unordered_map<void *, size_t> m_elements;
#else
  void WN_FORCE_INLINE notify_returned(void *ptr, wn_size_t requested,
                                       wn_size_t returned) {}
  void WN_FORCE_INLINE notify_freed(void *ptr) {}
  void WN_FORCE_INLINE notify_resize(void *ptr, void *ret_ptr,
                                     wn_size_t requested,
                                     wn_size_t returned) {}
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

  allocated_destroyer& operator=(allocated_destroyer&& other) {
    _deleter = std::move(other._deleter);
    _allocator = other._allocator;
    other._allocator = wn_nullptr;
    other._deleter = wn_nullptr;
    return *this;
  }

  template<typename T>
  static void typed_deleter(void* v) {
    T* t = static_cast<T*>(v);
    (*t).~T();
  }
  allocator *_allocator;
  void(*_deleter)(void*);
};

template<typename T>
struct typed_destroyer: public allocated_destroyer {
public:
  void destroy(void* v) {
    delete (reinterpret_cast<T*>(v));
  }
};

template <typename T, typename Deleter = allocated_destroyer>
using allocated_ptr = std::unique_ptr<T, Deleter>;

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
                                   const wn_size_t count) {
    const wn_size_t count_max =
        count > _MinimumAllocationSize ? count : _MinimumAllocationSize;
    allocation_pair pair{malloc(size * count_max), count_max};
    notify_returned(pair.m_location, size * count, size * count_max);
    return (pair);
  }

  virtual allocation_pair allocate_for_resize(const wn_size_t size,
                                              const wn_size_t count,
                                              const wn_size_t old_size) {
    const wn_size_t count_max =
        count > _MinimumAllocationSize ? count : _MinimumAllocationSize;
    const wn_size_t new_size =
        static_cast<wn_size_t>(old_size * (1 + (_ExpandPercent / 100.0f)));
    const wn_size_t allocated_number =
        new_size > count_max ? new_size : count_max;
    allocation_pair pair{malloc(size * allocated_number), allocated_number};
    notify_returned(pair.m_location, old_size, size * allocated_number);
    return (pair);
  }

  virtual allocation_pair reallocate(wn_void *ptr, const wn_size_t size,
                                     const wn_size_t count,
                                     const wn_size_t old_size = 0) {
    const wn_size_t count_max =
        count > _MinimumAllocationSize ? count : _MinimumAllocationSize;
    const wn_size_t new_size =
        static_cast<wn_size_t>(old_size * (1 + (_ExpandPercent / 100.0f)));
    const wn_size_t allocated_number =
        new_size > count_max ? new_size : count_max;
    allocation_pair pair{realloc(ptr, size * allocated_number), count_max};
    notify_resize(ptr, pair.m_location, size * count,
      size * allocated_number);
    return (pair);
  }

  virtual wn_void deallocate(wn_void *ptr) {
    notify_freed(ptr);
    free(ptr);
  }
};

typedef default_expanding_allocator<50> default_allocator;

}  // namespace memory
}  // namespace wn

#endif  // __WN_MEMORY_ALLOCATOR_H__