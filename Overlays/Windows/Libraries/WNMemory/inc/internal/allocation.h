// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MEMORY_WINDOWS_INTERNAL_ALLOCATION_H__
#define __WN_MEMORY_WINDOWS_INTERNAL_ALLOCATION_H__

#include "core/inc/types.h"

#include <malloc.h>

#define __WN_INTERNAL_STACK_ALLOC(_size) ::_alloca(_size)

namespace wn {
namespace memory {
namespace internal {

inline void* aligned_malloc(size_t _size, size_t _alignment) {
  return ::_aligned_malloc(_size, _alignment);
}

inline void* aligned_realloc(void* _ptr, size_t _new_size, size_t _alignment) {
  return ::_aligned_realloc(_ptr, _new_size, _alignment);
}

inline void aligned_free(void* _ptr) {
  ::_aligned_free(_ptr);
}

}  // namespace internal
}  // namespace memory
}  // namespace wn

#endif  // __WN_MEMORY_WINDOWS_INTERNAL_ALLOCATION_H__
