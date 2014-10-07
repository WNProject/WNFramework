////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_DIVIDE_NUMBER_SSE_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_DIVIDE_NUMBER_SSE_H__

#ifndef __WN_SSE_AVAILABLE
    #error "This header requires a minimum of SSE to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpDivideNumber<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const WN_FLOAT32& _number) {
                const __m128 number = _mm_load_ss(&_number);

                #ifdef __WN_MATH_APPROXIMATIONS_ENABLED
                    _elements.mXMMValues[0] = _mm_mul_ss(_elements.mXMMValues[0], _mm_rcp_ss(number));
                #else
                    _elements.mXMMValues[0] = _mm_div_ss(_elements.mXMMValues[0], number);
                #endif

                return(_elements);
            }

        private:
            __WNGeneralOpDivideNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpDivideNumber<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 2)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const WN_FLOAT32& _number) {
                const __m128 number = _mm_setr_ps(_number, _number, 1.0f, 1.0f);

                #ifdef __WN_MATH_APPROXIMATIONS_ENABLED
                    _elements.mXMMValues[0] = _mm_mul_ps(_elements.mXMMValues[0], _mm_rcp_ps(number));
                #else
                    _elements.mXMMValues[0] = _mm_div_ps(_elements.mXMMValues[0], number);
                #endif

                return(_elements);
            }

        private:
            __WNGeneralOpDivideNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpDivideNumber<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const WN_FLOAT32& _number) {
                const __m128 number = _mm_setr_ps(_number, _number, _number, 1.0f);

                #ifdef __WN_MATH_APPROXIMATIONS_ENABLED
                    _elements.mXMMValues[0] = _mm_mul_ps(_elements.mXMMValues[0], _mm_rcp_ps(number));
                #else
                    _elements.mXMMValues[0] = _mm_div_ps(_elements.mXMMValues[0], number);
                #endif

                return(_elements);
            }

        private:
            __WNGeneralOpDivideNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpDivideNumber<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const WN_FLOAT32& _number) {
                const __m128 number = _mm_set1_ps(_number);

                #ifdef __WN_MATH_APPROXIMATIONS_ENABLED
                    _elements.mXMMValues[0] = _mm_mul_ps(_elements.mXMMValues[0], _mm_rcp_ps(number));
                #else
                    _elements.mXMMValues[0] = _mm_div_ps(_elements.mXMMValues[0], number);
                #endif

                return(_elements);
            }

        private:
            __WNGeneralOpDivideNumber() {}
        };
    }
}

#ifndef __WN_AVX_AVAILABLE
    #include "WNMath/inc/Internal/x86/WNGeneralOpDivideNumber_SSE-NoAVX.h"
#endif

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_DIVIDE_NUMBER_SSE_H__