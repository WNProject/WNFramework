////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_VECTOR_OP_LENGTH_SQUARED_SSE_NO_SSE3_H__
#define __WN_MATH_INTERNAL_X86_VECTOR_OP_LENGTH_SQUARED_SSE_NO_SSE3_H__

#ifndef __WN_SSE_AVAILABLE
    #error "This header requires a minimum of SSE to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNVectorOpLengthSquared<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 2)>::Value> {
        public:
            static WN_FORCE_INLINE WN_FLOAT32 Execute(const __WNElements<WN_FLOAT32, Dimension>& _elements) {
                const __m128 squared = _mm_mul_ps(_elements.mXMMValues[0], _elements.mXMMValues[0]);

                return(_mm_cvtss_f32(_mm_add_ss(squared, _mm_shuffle_ps(squared, squared, _MM_SHUFFLE(0, 3, 2, 1)))));
            }

        private:
            WN_FORCE_INLINE __WNVectorOpLengthSquared() {}
        };

        template <WN_UINT32 Dimension>
        class __WNVectorOpLengthSquared<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3)>::Value> {
        public:
            static WN_FORCE_INLINE WN_FLOAT32 Execute(const __WNElements<WN_FLOAT32, Dimension>& _elements) {
                const __m128 squared = _mm_mul_ps(_elements.mXMMValues[0], _elements.mXMMValues[0]);
                const __m128 accumulation = _mm_add_ss(squared, _mm_shuffle_ps(squared, squared, _MM_SHUFFLE(0, 3, 2, 1)));

                return(_mm_cvtss_f32(_mm_add_ss(accumulation, _mm_shuffle_ps(squared, squared, _MM_SHUFFLE(1, 0, 3, 2)))));
            }

        private:
            WN_FORCE_INLINE __WNVectorOpLengthSquared() {}
        };

        template <WN_UINT32 Dimension>
        class __WNVectorOpLengthSquared<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 4)>::Value> {
        public:
            static WN_FORCE_INLINE WN_FLOAT32 Execute(const __WNElements<WN_FLOAT32, Dimension>& _elements) {
                const __m128 squared = _mm_mul_ps(_elements.mXMMValues[0], _elements.mXMMValues[0]);
                const __m128 accumulation1 = _mm_add_ss(squared, _mm_shuffle_ps(squared, squared, _MM_SHUFFLE(0, 3, 2, 1)));
                const __m128 accumulation2 = _mm_add_ss(accumulation1, _mm_shuffle_ps(squared, squared, _MM_SHUFFLE(1, 0, 3, 2)));

                return(_mm_cvtss_f32(_mm_add_ss(accumulation2, _mm_shuffle_ps(squared, squared, _MM_SHUFFLE(2, 1, 0, 3)))));
            }

        private:
            WN_FORCE_INLINE __WNVectorOpLengthSquared() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_VECTOR_OP_LENGTH_SQUARED_SSE_NO_SSE3_H__