// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MEMORY_INTERNAL_GENERIC_BASIC_TRAITS_H__
#define __WN_MEMORY_INTERNAL_GENERIC_BASIC_TRAITS_H__

#include "WNCore/inc/WNUtility.h"

#include <cstring>

namespace wn {
namespace memory {
namespace internal {

struct basic_traits_generic : core::non_constructable_non_copyable {
  static WN_FORCE_INLINE void* memzero(void* dest, const size_t count) {
    return (memset(dest, 0, count));
  }

  static WN_FORCE_INLINE void* memset(
      void* dest, const uint8_t value, const size_t count) {
    return (std::memset(dest, value, count));
  }

  static WN_FORCE_INLINE void* memcpy(
      void* dest, const void* src, const size_t count) {
    return (std::memcpy(dest, src, count));
  }

  static WN_FORCE_INLINE void* memmove(
      void* dest, const void* src, const size_t count) {
    return (std::memmove(dest, src, count));
  }

  static WN_FORCE_INLINE int32_t memcmp(
      const void* lhs, const void* rhs, const size_t count) {
    return (std::memcmp(lhs, rhs, count));
  }

  template <typename T>
  static WN_FORCE_INLINE void prefetch(const T* ptr) {
    WN_UNUSED_ARGUMENT(ptr);
  }
};

}  // namespace internal
}  // namespace memory
}  // namespace wn

#endif  // __WN_MEMORY_INTERNAL_GENERIC_BASIC_TRAITS_H__