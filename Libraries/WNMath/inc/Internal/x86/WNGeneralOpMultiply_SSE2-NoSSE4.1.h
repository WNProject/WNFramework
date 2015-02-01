// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_MULTIPLY_SSE2_NO_SSE4_1_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_MULTIPLY_SSE2_NO_SSE4_1_H__

#ifndef __WN_SSE2_AVAILABLE
    #error "This header requires a minimum of SSE2 to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpMultiply<WN_INT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 2 && Dimension <= 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT32, Dimension>& Execute(__WNElements<WN_INT32, Dimension>& _elements1, const __WNElements<WN_INT32, Dimension>& _elements2) {
                const __m128i temp1 = _mm_mul_epu32(_elements1.mXMMValues[0], _elements2.mXMMValues[0]);
                const __m128i temp2 = _mm_mul_epu32(_mm_srli_si128(_elements1.mXMMValues[0], 4), _mm_srli_si128(_elements2.mXMMValues[0], 4));

                _elements1.mXMMValues[0] = _mm_unpacklo_epi32(_mm_shuffle_epi32(temp1, _MM_SHUFFLE(0, 0, 2, 0)), _mm_shuffle_epi32(temp2, _MM_SHUFFLE(0, 0, 2, 0)));

                return(_elements1);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpMultiply() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpMultiply<WN_INT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 4) != 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT32, Dimension>& Execute(__WNElements<WN_INT32, Dimension>& _elements1, const __WNElements<WN_INT32, Dimension>& _elements2) {
                for (WN_UINT32 i = 0; i < __WNNumberRegisters_XMM_PI32<Dimension>::Value; ++i) {
                    const __m128i temp1 = _mm_mul_epu32(_elements1.mXMMValues[i], _elements2.mXMMValues[i]);
                    const __m128i temp2 = _mm_mul_epu32(_mm_srli_si128(_elements1.mXMMValues[i], 4), _mm_srli_si128(_elements2.mXMMValues[i], 4));

                    _elements1.mXMMValues[i] = _mm_unpacklo_epi32(_mm_shuffle_epi32(temp1, _MM_SHUFFLE(0, 0, 2, 0)), _mm_shuffle_epi32(temp2, _MM_SHUFFLE(0, 0, 2, 0)));
                }

                return(_elements1);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpMultiply() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpMultiply<WN_INT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 4) == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT32, Dimension>& Execute(__WNElements<WN_INT32, Dimension>& _elements1, const __WNElements<WN_INT32, Dimension>& _elements2) {
                for (WN_UINT32 i = 0; i < __WNNumberRegisters_XMM_PI32<Dimension>::Value; ++i) {
                    const __m128i temp1 = _mm_mul_epu32(_elements1.mXMMValues[i], _elements2.mXMMValues[i]);
                    const __m128i temp2 = _mm_mul_epu32(_mm_srli_si128(_elements1.mXMMValues[i], 4), _mm_srli_si128(_elements2.mXMMValues[i], 4));

                    _elements1.mXMMValues[i] = _mm_unpacklo_epi32(_mm_shuffle_epi32(temp1, _MM_SHUFFLE(0, 0, 2, 0)), _mm_shuffle_epi32(temp2, _MM_SHUFFLE(0, 0, 2, 0)));
                }

                const WN_UINT32 lastElement = Dimension - 1;

                _elements1.mValues[lastElement] *= _elements2.mValues[lastElement];

                return(_elements1);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpMultiply() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_MULTIPLY_SSE2_NO_SSE4_1_H__