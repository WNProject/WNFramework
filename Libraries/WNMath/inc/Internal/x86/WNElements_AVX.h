// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_ELEMENTS_AVX_H__
#define __WN_MATH_INTERNAL_X86_ELEMENTS_AVX_H__

#ifndef __WN_AVX_AVAILABLE
    #error "This header requires a minimum of AVX to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        struct WN_ALIGN(32) __WNElements<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5)>::Value> {
            union {
                __m256 mYMMValues[__WNNumberRegisters_YMM_PS<Dimension>::Value];
                __m128 mXMMValues[__WNNumberRegisters_XMM_PS<Dimension>::Value];
                WN_FLOAT32 mValues[Dimension];
            };
        };

        template <WN_UINT32 Dimension>
        struct WN_ALIGN(32) __WNElements<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 3)>::Value> {
            union {
                __m256d mYMMValues[__WNNumberRegisters_YMM_PD<Dimension>::Value];
                __m128d mXMMValues[__WNNumberRegisters_XMM_PD<Dimension>::Value];
                WN_FLOAT64 mValues[Dimension];
            };
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_ELEMENTS_AVX_H__