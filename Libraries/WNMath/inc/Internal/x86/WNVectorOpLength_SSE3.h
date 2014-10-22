// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_VECTOR_OP_LENGTH_SSE3_H__
#define __WN_MATH_INTERNAL_X86_VECTOR_OP_LENGTH_SSE3_H__

#ifndef __WN_SSE3_AVAILABLE
    #error "This header requires a minimum of SSE3 to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNVectorOpLength<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 2)>::Value> {
        public:
            static WN_FORCE_INLINE WN_FLOAT32 Execute(const __WNElements<WN_FLOAT32, Dimension>& _elements) {
                const __m128 squared = _mm_mul_ps(_elements.mXMMValues[0], _elements.mXMMValues[0]);
                const __m128 lengthSquared = _mm_hadd_ps(squared, squared);

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
        class __WNVectorOpLength<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3 || Dimension == 4)>::Value> {
        public:
            static WN_FORCE_INLINE WN_FLOAT32 Execute(const __WNElements<WN_FLOAT32, Dimension>& _elements) {
                const __m128 squared = _mm_mul_ps(_elements.mXMMValues[0], _elements.mXMMValues[0]);
                const __m128 accumulation = _mm_hadd_ps(squared, squared);
                const __m128 lengthSquared = _mm_hadd_ps(accumulation, accumulation);

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
        class __WNVectorOpLength<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 2)>::Value> {
        public:
            static WN_FORCE_INLINE WN_FLOAT64 Execute(const __WNElements<WN_FLOAT64, Dimension>& _elements) {
                const __m128d squared = _mm_mul_pd(_elements.mXMMValues[0], _elements.mXMMValues[0]);

                return(_mm_cvtsd_f64(_mm_sqrt_sd(_mm_setzero_pd(), _mm_hadd_pd(squared, squared))));
            }

        private:
            WN_FORCE_INLINE __WNVectorOpLength() {}
        };

        template <WN_UINT32 Dimension>
        class __WNVectorOpLength<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3 || Dimension == 4)>::Value> {
        public:
            static WN_FORCE_INLINE WN_FLOAT64 Execute(const __WNElements<WN_FLOAT64, Dimension>& _elements) {
                const __m128d squared1 = _mm_mul_pd(_elements.mXMMValues[0], _elements.mXMMValues[0]);
                const __m128d squared2 = _mm_mul_pd(_elements.mXMMValues[1], _elements.mXMMValues[1]);
                const __m128d accumulation1 = _mm_hadd_pd(squared1, squared1);
                const __m128d accumulation2 = _mm_hadd_pd(squared2, squared2);

                return(_mm_cvtsd_f64(_mm_sqrt_sd(_mm_setzero_pd(), _mm_add_sd(accumulation1, accumulation2))));
            }

        private:
            WN_FORCE_INLINE __WNVectorOpLength() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_VECTOR_OP_LENGTH_SSE3_H__