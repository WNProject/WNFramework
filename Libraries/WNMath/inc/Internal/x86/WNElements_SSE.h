// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_ELEMENTS_SSE_H__
#define __WN_MATH_INTERNAL_X86_ELEMENTS_SSE_H__

#ifndef __WN_SSE_AVAILABLE
    #error "This header requires a minimum of SSE to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        struct WN_ALIGN(16) __WNElements<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 1 && Dimension <= 4)>::Value> {
            union {
                __m128 mXMMValues[1];
                WN_FLOAT32 mValues[Dimension];
            };
        };
    }
}

#ifndef __WN_AVX_AVAILABLE
    #include "WNMath/inc/Internal/x86/WNElements_SSE-NoAVX.h"
#endif

#endif // __WN_MATH_INTERNAL_X86_ELEMENTS_SSE_H__