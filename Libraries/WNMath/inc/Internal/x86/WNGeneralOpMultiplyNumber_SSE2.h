// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_MULTIPLY_NUMBER_SSE2_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_MULTIPLY_NUMBER_SSE2_H__

#ifndef __WN_SSE2_AVAILABLE
    #error "This header requires a minimum of SSE2 to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpMultiplyNumber<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements, const WN_FLOAT64& _number) {
                _elements.mXMMValues[0] = _mm_mul_sd(_elements.mXMMValues[0], _mm_load_sd(&_number));

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpMultiplyNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpMultiplyNumber<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 2)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements, const WN_FLOAT64& _number) {
                _elements.mXMMValues[0] = _mm_mul_pd(_elements.mXMMValues[0], _mm_set1_pd(_number));

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpMultiplyNumber() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_MULTIPLY_NUMBER_SSE2_H__