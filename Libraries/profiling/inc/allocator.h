// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_PROFILING_ALLOCATOR_H__
#define __WN_PROFILING_ALLOCATOR_H__

#include "WNMemory/inc/allocator.h"

namespace wn {
namespace profiling {

class allocator final : public memory::allocator {
public:
  allocator(memory::allocator* _allocator, const char* _allocator_name)
    : m_allocator(_allocator), m_allocator_name(_allocator_name) {}

  ~allocator() final {}

  void* allocate(size_t _size) final;
  void* reallocate(void* _ptr, size_t _new_size) final;
  void deallocate(void* _ptr) final;

  void* aligned_allocate(size_t _size, size_t _alignment) final;
  void* aligned_reallocate(
      void* _ptr, size_t _new_size, size_t _alignment) final;
  void aligned_deallocate(void* _ptr) final;

private:
  memory::allocator* m_allocator;
  const char* m_allocator_name;
};

}  // namespace profiling
}  // namespace wn

#endif  // __WN_PROFILING_ALLOCATOR_H__