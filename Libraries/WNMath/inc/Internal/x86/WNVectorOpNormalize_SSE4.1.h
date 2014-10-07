////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_VECTOR_OP_NORMALIZE_SSE4_1_H__
#define __WN_MATH_INTERNAL_X86_VECTOR_OP_NORMALIZE_SSE4_1_H__

#ifndef __WN_SSE3_AVAILABLE
    #error "This header requires a minimum of SSE3 to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNVectorOpNormalize<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 2)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements) {
                const __m128 lengthSquared = _mm_dp_ps(_elements.mXMMValues[0], _elements.mXMMValues[0], 0x33);

                #ifdef __WN_MATH_APPROXIMATIONS_ENABLED
                     const __m128 result = _mm_mul_ps(_elements.mXMMValues[0], _mm_rsqrt_ps(lengthSquared));
                #else
                     const __m128 result = _mm_div_ps(_elements.mXMMValues[0], _mm_sqrt_ps(lengthSquared));
                #endif

                _elements.mXMMValues[0] = _mm_and_ps(result, __WN_XMM_GET_PS_CONST(ElementMask1100x32));

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNVectorOpNormalize() {}
        };

        template <WN_UINT32 Dimension>
        class __WNVectorOpNormalize<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements) {
                const __m128 lengthSquared = _mm_dp_ps(_elements.mXMMValues[0], _elements.mXMMValues[0], 0x77);

                #ifdef __WN_MATH_APPROXIMATIONS_ENABLED
                     const __m128 result = _mm_mul_ps(_elements.mXMMValues[0], _mm_rsqrt_ps(lengthSquared));
                #else
                     const __m128 result = _mm_div_ps(_elements.mXMMValues[0], _mm_sqrt_ps(lengthSquared));
                #endif

                _elements.mXMMValues[0] = _mm_and_ps(result, __WN_XMM_GET_PS_CONST(ElementMask1110x32));

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNVectorOpNormalize() {}
        };

        template <WN_UINT32 Dimension>
        class __WNVectorOpNormalize<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements) {
                const __m128 lengthSquared = _mm_dp_ps(_elements.mXMMValues[0], _elements.mXMMValues[0], 0xFF);

                #ifdef __WN_MATH_APPROXIMATIONS_ENABLED
                     _elements.mXMMValues[0] = _mm_mul_ps(_elements.mXMMValues[0], _mm_rsqrt_ps(lengthSquared));
                #else
                     _elements.mXMMValues[0] = _mm_div_ps(_elements.mXMMValues[0], _mm_sqrt_ps(lengthSquared));
                #endif

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNVectorOpNormalize() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_VECTOR_OP_NORMALIZE_SSE4_1_H__