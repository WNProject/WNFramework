////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_VECTOR_OP_LENGTH_SSE2_NO_AVX_H__
#define __WN_MATH_INTERNAL_X86_VECTOR_OP_LENGTH_SSE2_NO_AVX_H__

#ifndef __WN_SSE2_AVAILABLE
    #error "This header requires a minimum of SSE2 to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNVectorOpLength<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3)>::Value> {
        public:
            static WN_FORCE_INLINE WN_FLOAT64 Execute(const __WNElements<WN_FLOAT64, Dimension>& _elements) {
                const __m128d squared1 = _mm_mul_pd(_elements.mXMMValues[0], _elements.mXMMValues[0]);
                const __m128d squared2 = _mm_mul_pd(_elements.mXMMValues[1], _elements.mXMMValues[1]);
                const __m128d squared1Flipped = _mm_shuffle_pd(squared1, squared1, _MM_SHUFFLE2(0, 1));
                const __m128d squared2Packed = _mm_add_sd(squared1Flipped, _mm_add_sd(squared1, squared2));

                return(_mm_cvtsd_f64(_mm_sqrt_sd(_mm_setzero_pd(), squared2Packed)));
            }

        private:
            WN_FORCE_INLINE __WNVectorOpLength() {}
        };

        template <WN_UINT32 Dimension>
        class __WNVectorOpLength<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 4)>::Value> {
        public:
            static WN_FORCE_INLINE WN_FLOAT64 Execute(const __WNElements<WN_FLOAT64, Dimension>& _elements) {
                const __m128d squared1 = _mm_mul_pd(_elements.mXMMValues[0], _elements.mXMMValues[0]);
                const __m128d squared2 = _mm_mul_pd(_elements.mXMMValues[1], _elements.mXMMValues[1]);
                const __m128d accumulation1 = _mm_add_sd(squared1, _mm_shuffle_pd(squared1, squared1, _MM_SHUFFLE2(0, 1)));
                const __m128d accumulation2 = _mm_add_sd(squared2, _mm_shuffle_pd(squared2, squared2, _MM_SHUFFLE2(0, 1)));

                return(_mm_cvtsd_f64(_mm_sqrt_sd(_mm_setzero_pd(), _mm_add_sd(accumulation1, accumulation2))));
            }

        private:
            WN_FORCE_INLINE __WNVectorOpLength() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_VECTOR_OP_LENGTH_SSE2_NO_AVX_H__