// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MEMORY_ALLOCATOR_H__
#define __WN_MEMORY_ALLOCATOR_H__

#include "WNMemory/inc/allocation.h"
#include "core/inc/types.h"
#include "core/inc/utilities.h"

namespace wn {
namespace memory {

class allocator {
public:
  allocator() = default;

  virtual ~allocator() = default;

  virtual void* allocate(size_t _size) = 0;
  virtual void* reallocate(void* _ptr, size_t _new_size) = 0;
  virtual void deallocate(void* _ptr) = 0;

  virtual void* aligned_allocate(size_t _size, size_t _alignment) = 0;
  virtual void* aligned_reallocate(
      void* _ptr, size_t _new_size, size_t _alignment) = 0;
  virtual void aligned_deallocate(void* _ptr) = 0;

  template <typename T, typename... Args>
  T* construct(Args&&... _args) {
    T* ptr = static_cast<T*>(allocate(sizeof(T)));

    return construct_at(ptr, core::forward<Args>(_args)...);
  }

  template <typename T, typename... Args>
  T* construct_at(T* _ptr, Args&&... _args) {
    return memory::construct_at(_ptr, core::forward<Args>(_args)...);
  }

  template <typename T>
  void destroy(T* _ptr) {
    _ptr->~T();

    deallocate(_ptr);
  }

  template <typename T, typename... Args>
  T* aligned_construct(size_t _alignment, Args&&... _args) {
    void* ptr = aligned_allocate(sizeof(T), _alignment);

    return construct_at(static_cast<T*>(ptr), core::forward<Args>(_args)...);
  }

  template <typename T>
  void aligned_destroy(T* _ptr) {
    _ptr->~T();

    aligned_deallocate(_ptr);
  }
};

}  // namespace memory
}  // namespace wn

#endif  // __WN_MEMORY_ALLOCATOR_H__
