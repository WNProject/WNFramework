// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_ARM_ELEMENTS_NEON_H__
#define __WN_MATH_INTERNAL_ARM_ELEMENTS_NEON_H__

#ifndef __WN_NEON_AVAILABLE
    #error "This header requires a minimum of NEON to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        struct WN_ALIGN(16) __WNElements<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 1 || Dimension == 2)>::Value> {
            union {
                float32x2_t mDValues[__WNNumberRegisters_NEON_32_D<Dimension>::Value];
                WN_FLOAT32 mValues[Dimension];
            };
        };

        template <WN_UINT32 Dimension>
        struct WN_ALIGN(16) __WNElements<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 3)>::Value> {
            union {
                float32x2_t mDValues[__WNNumberRegisters_NEON_32_D<Dimension>::Value];
                float32x4_t mQValues[__WNNumberRegisters_NEON_32_Q<Dimension>::Value];
                WN_FLOAT32 mValues[Dimension];
            };
        };

        template <WN_UINT32 Dimension>
        struct WN_ALIGN(16) __WNElements<WN_INT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 1 || Dimension == 2)>::Value> {
            union {
                int32x2_t mDValues[__WNNumberRegisters_NEON_32_D<Dimension>::Value];
                WN_INT32 mValues[Dimension];
            };
        };

        template <WN_UINT32 Dimension>
        struct WN_ALIGN(16) __WNElements<WN_INT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 3)>::Value> {
            union {
                int32x2_t mDValues[__WNNumberRegisters_NEON_32_D<Dimension>::Value];
                int32x4_t mQValues[__WNNumberRegisters_NEON_32_Q<Dimension>::Value];
                WN_INT32 mValues[Dimension];
            };
        };
    }
}

#endif // __WN_MATH_INTERNAL_ARM_ELEMENTS_NEON_H__