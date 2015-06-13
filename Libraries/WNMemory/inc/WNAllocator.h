// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MEMORY_ALLOCATOR_H__
#define __WN_MEMORY_ALLOCATOR_H__

#include "WNMemory/inc/WNBasic.h"

namespace wn {
namespace memory {

struct allocation_pair final {
  wn_void *m_location;
  wn_size_t m_count;
};

class allocator {
public:
  virtual ~allocator() = default;

  virtual allocation_pair allocate(const wn_size_t size,
                                   const wn_size_t count) = 0;
  virtual allocation_pair allocate_for_resize(const wn_size_t size,
                                              const wn_size_t count,
                                              const wn_size_t old_size) = 0;
  virtual allocation_pair reallocate(wn_void *ptr, const wn_size_t _number,
                                     const wn_size_t count,
                                     const wn_size_t old_size = 0) = 0;
  virtual wn_void deallocate(wn_void *ptr) = 0;
};

template <const wn_size_t _ExpandPercent,
          const wn_size_t _MinimumAllocationSize = 1>
class default_expanding_allocator : public allocator {
public:
  virtual allocation_pair allocate(const wn_size_t size,
                                   const wn_size_t count) {
    const wn_size_t count_max =
      count > _MinimumAllocationSize ? count : _MinimumAllocationSize;
    allocation_pair pair{malloc(size * count_max), count_max};

    return(pair);
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

    return(pair);
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

    return(pair);
  }

  virtual wn_void deallocate(wn_void *ptr) {
    free(ptr);
  }
};

typedef default_expanding_allocator<50> default_allocator;

} // namespace memory
} // namespace wn

#endif // __WN_MEMORY_ALLOCATOR_H__