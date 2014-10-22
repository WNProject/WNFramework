// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_VECTOR_OP_LENGTH_SSE_H__
#define __WN_MATH_INTERNAL_X86_VECTOR_OP_LENGTH_SSE_H__

#ifndef __WN_SSE_AVAILABLE
    #error "This header requires a minimum of SSE to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNVectorOpLength<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 2)>::Value> {
        public:
            static WN_FORCE_INLINE WN_FLOAT32 Execute(const __WNElements<WN_FLOAT32, Dimension>& _elements) {
                const __m128 squared = _mm_mul_ps(_elements.mXMMValues[0], _elements.mXMMValues[0]);
                const __m128 lengthSquared = _mm_add_ss(squared, _mm_shuffle_ps(squared, squared, _MM_SHUFFLE(0, 3, 2, 1)));

                #ifdef __WN_MATH_APPROXIMATIONS_ENABLED
                     const __m128 length = _mm_mul_ss(lengthSquared, _mm_rsqrt_ss(lengthSquared));
                #else
                     const __m128 length = _mm_sqrt_ss(lengthSquared);
                #endif

                return(_mm_cvtss_f32(length));
            }

        private:
            WN_FORCE_INLINE __WNVectorOpLength() {}
        };

        template <WN_UINT32 Dimension>
        class __WNVectorOpLength<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3)>::Value> {
        public:
            static WN_FORCE_INLINE WN_FLOAT32 Execute(const __WNElements<WN_FLOAT32, Dimension>& _elements) {
                const __m128 squared = _mm_mul_ps(_elements.mXMMValues[0], _elements.mXMMValues[0]);
                const __m128 accumulation = _mm_add_ss(squared, _mm_shuffle_ps(squared, squared, _MM_SHUFFLE(0, 3, 2, 1)));
                const __m128 lengthSquared = _mm_add_ss(accumulation, _mm_shuffle_ps(squared, squared, _MM_SHUFFLE(1, 0, 3, 2)));

                #ifdef __WN_MATH_APPROXIMATIONS_ENABLED
                     const __m128 length = _mm_mul_ss(lengthSquared, _mm_rsqrt_ss(lengthSquared));
                #else
                     const __m128 length = _mm_sqrt_ss(lengthSquared);
                #endif

                return(_mm_cvtss_f32(length));
            }

        private:
            WN_FORCE_INLINE __WNVectorOpLength() {}
        };

        template <WN_UINT32 Dimension>
        class __WNVectorOpLength<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 4)>::Value> {
        public:
            static WN_FORCE_INLINE WN_FLOAT32 Execute(const __WNElements<WN_FLOAT32, Dimension>& _elements) {
                const __m128 squared = _mm_mul_ps(_elements.mXMMValues[0], _elements.mXMMValues[0]);
                const __m128 accumulation1 = _mm_add_ss(squared, _mm_shuffle_ps(squared, squared, _MM_SHUFFLE(0, 3, 2, 1)));
                const __m128 accumulation2 = _mm_add_ss(accumulation1, _mm_shuffle_ps(squared, squared, _MM_SHUFFLE(1, 0, 3, 2)));
                const __m128 lengthSquared = _mm_add_ss(accumulation2, _mm_shuffle_ps(squared, squared, _MM_SHUFFLE(2, 1, 0, 3)));

                #ifdef __WN_MATH_APPROXIMATIONS_ENABLED
                     const __m128 length = _mm_mul_ss(lengthSquared, _mm_rsqrt_ss(lengthSquared));
                #else
                     const __m128 length = _mm_sqrt_ss(lengthSquared);
                #endif

                return(_mm_cvtss_f32(length));
            }

        private:
            WN_FORCE_INLINE __WNVectorOpLength() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_VECTOR_OP_LENGTH_SSE_H__