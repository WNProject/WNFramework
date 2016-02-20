// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MEMORY_INTERNAL_EXTENSION_X86_SSE_BASIC_TRAITS_H__
#define __WN_MEMORY_INTERNAL_EXTENSION_X86_SSE_BASIC_TRAITS_H__

#include "WNMemory/inc/Internal/Generic/WNBasicTraits.h"

#include <xmmintrin.h>

namespace wn {
namespace memory {
namespace internal {

struct basic_traits_sse : basic_traits_generic {
  template <typename T>
  static WN_FORCE_INLINE
      typename core::enable_if<core::bool_and<core::is_floating_point<T>::value,
          !core::is_extended_type<T>::value>::value>::type
      prefetch(const T* ptr) {
    _mm_prefetch(reinterpret_cast<const char*>(ptr), _MM_HINT_T1);
  }

  template <typename T>
  static WN_FORCE_INLINE typename core::enable_if<
      !core::bool_and<core::is_floating_point<T>::value,
          !core::is_extended_type<T>::value>::value>::type
  prefetch(const T* ptr) {
    _mm_prefetch(reinterpret_cast<const char*>(ptr), _MM_HINT_T0);
  }
};

}  // namespace internal
}  // namespace memory
}  // namespace wn

#endif  // __WN_MEMORY_INTERNAL_EXTENSION_X86_SSE_BASIC_TRAITS_H__