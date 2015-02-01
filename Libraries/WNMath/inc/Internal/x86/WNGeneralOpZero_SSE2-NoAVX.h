// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_ZERO_SSE2_NO_AVX_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_ZERO_SSE2_NO_AVX_H__

#ifndef __WN_SSE_AVAILABLE
    #error "This header requires a minimum of SSE to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpZero<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 3 && Dimension <= 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements) {
                _elements.mXMMValues[0] = _mm_setzero_pd();
                _elements.mXMMValues[1] = _mm_setzero_pd();

                return(_elements);
            }

        private:
            __WNGeneralOpZero() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpZero<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements) {
                for (WN_UINT32 i = 0; i < __WNNumberRegisters_XMM_PD<Dimension>::Value; ++i) {
                    _elements.mXMMValues[i] = _mm_setzero_pd();
                }

                return(_elements);
            }

        private:
            __WNGeneralOpZero() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_ZERO_SSE2_NO_AVX_H__