// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MEMORY_BASIC_ALLOCATOR_H__
#define __WN_MEMORY_BASIC_ALLOCATOR_H__

#include "WNMemory/inc/allocation.h"
#include "WNMemory/inc/allocator.h"

namespace wn {
namespace memory {

class basic_allocator : public allocator {
public:
  basic_allocator() : allocator() {}

  virtual void* allocate(size_t _size) override {
    return malloc(_size);
  }

  virtual void* reallocate(void* _ptr, size_t _new_size) override {
    return realloc(_ptr, _new_size);
  }

  virtual void deallocate(void* _ptr) override {
    free(_ptr);
  }

  virtual void* aligned_allocate(size_t _size, size_t _alignment) override {
    return aligned_malloc(_size, _alignment);
  }

  virtual void* aligned_reallocate(
      void* _ptr, size_t _new_size, size_t _alignment) override {
    return aligned_realloc(_ptr, _new_size, _alignment);
  }

  virtual void aligned_deallocate(void* _ptr) override {
    aligned_free(_ptr);
  }
};

}  // namespace memory
}  // namespace wn

#endif  // __WN_MEMORY_BASIC_ALLOCATOR_H__
