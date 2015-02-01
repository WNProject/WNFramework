// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_MULTIPLY_ADD_NUMBERS_SSE_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_MULTIPLY_ADD_NUMBERS_SSE_H__

#ifndef __WN_SSE_AVAILABLE
    #error "This header requires a minimum of SSE to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpMultiplyAddNumbers<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const WN_FLOAT32& _number1, const WN_FLOAT32& _number2) {
                _elements.mXMMValues[0] = _mm_add_ss(_mm_mul_ss(_elements.mXMMValues[0], _mm_load_ss(&_number1)), _mm_load_ss(&_number2));

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpMultiplyAddNumbers() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpMultiplyAddNumbers<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 2)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const WN_FLOAT32& _number1, const WN_FLOAT32& _number2) {
                const __m128 number1 = _mm_setr_ps(_number1, _number1, 0.0f, 0.0f);
                const __m128 number2 = _mm_setr_ps(_number2, _number2, 0.0f, 0.0f);

                _elements.mXMMValues[0] = _mm_add_ps(_mm_mul_ps(_elements.mXMMValues[0], number1), number2);

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpMultiplyAddNumbers() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpMultiplyAddNumbers<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const WN_FLOAT32& _number1, const WN_FLOAT32& _number2) {
                const __m128 number1 = _mm_setr_ps(_number1, _number1, _number1, 0.0f);
                const __m128 number2 = _mm_setr_ps(_number2, _number2, _number2, 0.0f);

                _elements.mXMMValues[0] = _mm_add_ps(_mm_mul_ps(_elements.mXMMValues[0], number1), number2);

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpMultiplyAddNumbers() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpMultiplyAddNumbers<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const WN_FLOAT32& _number1, const WN_FLOAT32& _number2) {
                _elements.mXMMValues[0] = _mm_add_ps(_mm_mul_ps(_elements.mXMMValues[0], _mm_set1_ps(_number1)), _mm_set1_ps(_number2));

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpMultiplyAddNumbers() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_MULTIPLY_ADD_NUMBERS_SSE_H__