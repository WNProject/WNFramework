// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_DIVIDE_NUMBER_SSE2_NO_AVX_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_DIVIDE_NUMBER_SSE2_NO_AVX_H__

#ifndef __WN_SSE2_AVAILABLE
    #error "This header requires a minimum of SSE2 to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpDivideNumber<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements, const WN_FLOAT64& _number) {
                const __m128d number = _mm_set1_pd(_number);

                _elements.mXMMValues[0] = _mm_div_pd(_elements.mXMMValues[0], number);
                _elements.mXMMValues[1] = _mm_div_sd(_elements.mXMMValues[1], number);

                return(_elements);
            }

        private:
            __WNGeneralOpDivideNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpDivideNumber<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements, const WN_FLOAT64& _number) {
                const __m128d number = _mm_set1_pd(_number);

                _elements.mXMMValues[0] = _mm_div_pd(_elements.mXMMValues[0], number);
                _elements.mXMMValues[1] = _mm_div_pd(_elements.mXMMValues[1], number);

                return(_elements);
            }

        private:
            __WNGeneralOpDivideNumber() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_DIVIDE_NUMBER_SSE2_NO_AVX_H__