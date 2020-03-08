// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MEMORY_INTERNAL_ALLOCATION_H__
#define __WN_MEMORY_INTERNAL_ALLOCATION_H__

#include <cstdlib>

#include "WNCore/inc/types.h"
#include "WNMemory/inc/internal/stack_allocation.h"
#include "WNMemory/inc/manipulation.h"

namespace wn {
namespace memory {
namespace internal {

inline void* aligned_malloc(size_t _size, size_t _alignment) {
  void* real_ptr = std::malloc((_size + _alignment - 1) + 2 * sizeof(void*));

  if (real_ptr) {
    void* adjusted_ptr = reinterpret_cast<void*>(
        ((reinterpret_cast<size_t>(real_ptr) + sizeof(void*) + _alignment - 1) &
            ~(_alignment - 1)));

    *(reinterpret_cast<void**>(adjusted_ptr) - 1) = real_ptr;
    *(reinterpret_cast<size_t*>(adjusted_ptr) - 2) = _size;

    return adjusted_ptr;
  }

  return nullptr;
}

void aligned_free(void*);

inline void* aligned_realloc(void* _ptr, size_t _new_size, size_t _alignment) {
  void* temp_ptr = aligned_malloc(_new_size, _alignment);

  if (_ptr) {
    const size_t old_size = *(reinterpret_cast<size_t*>(_ptr) - 2);
    const size_t copy_size = old_size < _new_size ? old_size : _new_size;

    temp_ptr = memcpy(temp_ptr, _ptr, copy_size);
  }

  aligned_free(_ptr);

  return temp_ptr;
}

inline void aligned_free(void* _ptr) {
  if (_ptr) {
    std::free(*(reinterpret_cast<void**>(_ptr) - 1));
  }
}

}  // namespace internal
}  // namespace memory
}  // namespace wn

#endif  // __WN_MEMORY_INTERNAL_ALLOCATION_H__
