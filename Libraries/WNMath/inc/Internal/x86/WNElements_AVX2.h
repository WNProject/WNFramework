// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_ELEMENTS_AVX2_H__
#define __WN_MATH_INTERNAL_X86_ELEMENTS_AVX2_H__

#ifndef __WN_AVX2_AVAILABLE
    #error "This header requires a minimum of AVX2 to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        struct WN_ALIGN(32) __WNElements<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5)>::Value> {
            union {
                __m256i mYMMValues[__WNNumberRegisters_YMM_PI32<Dimension>::Value];
                __m128i mXMMValues[__WNNumberRegisters_XMM_PI32<Dimension>::Value];
                WN_INT32 mValues[Dimension];
            };
        };

        template <WN_UINT32 Dimension>
        struct WN_ALIGN(32) __WNElements<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 3)>::Value> {
            union {
                __m256i mYMMValues[__WNNumberRegisters_YMM_PI64<Dimension>::Value];
                __m128i mXMMValues[__WNNumberRegisters_XMM_PI64<Dimension>::Value];
                WN_INT64 mValues[Dimension];
            };
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_ELEMENTS_AVX_H__