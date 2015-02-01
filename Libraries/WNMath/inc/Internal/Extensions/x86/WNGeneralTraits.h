// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_TRAITS_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_TRAITS_H__

#ifdef __WN_SSE_AVAILABLE
    #include "WNMath/inc/Internal/Extensions/x86/SSE/WNGeneralTraits.h"
#endif

namespace wn {
    namespace internal {
        namespace math {
            #ifdef __WN_SSE_AVAILABLE
                typedef general_traits_sse general_traits;
            #endif
        }
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_TRAITS_H__