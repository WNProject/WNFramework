// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_TRAITS_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_TRAITS_H__

#ifdef _WN_X86_SSE
    #include "WNMath/inc/Internal/Extensions/x86/SSE/WNGeneralTraits.h"
#endif

namespace wn {
    namespace internal {
        namespace math {
            #ifdef _WN_X86_SSE
                typedef general_traits_sse general_traits;
            #endif
        }
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_TRAITS_H__