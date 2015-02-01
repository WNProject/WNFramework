// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_ELEMENTS_SSE_NO_AVX_H__
#define __WN_MATH_INTERNAL_X86_ELEMENTS_SSE_NO_AVX_H__

#ifndef __WN_SSE_AVAILABLE
    #error "This header requires a minimum of SSE to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        struct WN_ALIGN(16) __WNElements<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5)>::Value> {
            union {
                __m128 mXMMValues[__WNNumberRegisters_XMM_PS<Dimension>::Value];
                WN_FLOAT32 mValues[Dimension];
            };
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_ELEMENTS_SSE_NO_AVX_H__