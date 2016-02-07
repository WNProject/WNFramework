// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_VECTOR_TRAITS_H__
#define __WN_MATH_INTERNAL_X86_VECTOR_TRAITS_H__

#ifdef __WN_SSE4_1_AVAILABLE
#include "WNMath/inc/Internal/Extensions/x86/SSE4.1/WNVectorTraits.h"
#elif defined __WN_SSE2_AVAILABLE
#include "WNMath/inc/Internal/Extensions/x86/SSE2/WNVectorTraits.h"
#elif defined _WN_X86_SSE
#include "WNMath/inc/Internal/Extensions/x86/SSE/WNVectorTraits.h"
#endif

namespace wn {
namespace internal {
namespace math {
#ifdef __WN_SSE4_1_AVAILABLE
typedef vector_traits_sse4_1 vector_traits;
#elif defined __WN_SSE2_AVAILABLE
typedef vector_traits_sse2 vector_traits;
#elif defined _WN_X86_SSE
typedef vector_traits_sse vector_traits;
#endif
}
}
}

#endif  // __WN_MATH_INTERNAL_X86_VECTOR_TRAITS_H__