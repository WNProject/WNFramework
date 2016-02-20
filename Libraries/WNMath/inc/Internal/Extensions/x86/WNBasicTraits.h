// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_BASIC_TRAITS_H__
#define __WN_MATH_INTERNAL_X86_BASIC_TRAITS_H__

#ifdef __WN_SSE4_1_AVAILABLE
#include "WNMath/inc/Internal/Extensions/x86/SSE4.1/WNBasicTraits.h"
#elif defined __WN_SSE2_AVAILABLE
#include "WNMath/inc/Internal/Extensions/x86/SSE2/WNBasicTraits.h"
#elif defined _WN_X86_SSE
#include "WNMath/inc/Internal/Extensions/x86/SSE/WNBasicTraits.h"
#endif

namespace wn {
namespace internal {
namespace math {
#ifdef __WN_SSE4_1_AVAILABLE
typedef basic_traits_sse4_1 basic_traits;
#elif defined __WN_SSE2_AVAILABLE
typedef basic_traits_sse2 basic_traits;
#elif defined _WN_X86_SSE
typedef basic_traits_sse basic_traits;
#endif
}
}
}

#endif  // __WN_MATH_INTERNAL_X86_BASIC_TRAITS_H__