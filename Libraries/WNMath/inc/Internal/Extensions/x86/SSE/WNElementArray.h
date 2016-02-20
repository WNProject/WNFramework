// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_SSE_ELEMENT_ARRAY_H__
#define __WN_MATH_INTERNAL_X86_SSE_ELEMENT_ARRAY_H__

#include "WNMath/inc/Internal/Extensions/x86/WNUtilities.h"
#include "WNMath/inc/Internal/Generic/WNElementArray.h"

#include <xmmintrin.h>

namespace wn {
namespace internal {
namespace math {
template <typename type, const size_t dimension>
struct element_array_sse : element_array_generic<type, dimension> {};

template <const size_t dimension>
struct WN_ALIGN(16) element_array_sse<float, dimension> {
  static_assert(dimension > 1, "Must have a dimension of at least 2");

  union {
    __m128 m_xmm_values[xmm_count_ps<dimension>::value];
    float m_values[dimension];
  };
};
}
}
}

#endif  // __WN_MATH_INTERNAL_X86_SSE_ELEMENT_ARRAY_H__