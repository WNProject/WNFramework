////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_ADD_NUMBER_SSE2_NO_AVX_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_ADD_NUMBER_SSE2_NO_AVX_H__

#ifndef __WN_SSE2_AVAILABLE
    #error "This header requires a minimum of SSE2 to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpAddNumber<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements, const WN_FLOAT64& _number) {
                const __m128d number = _mm_set1_pd(_number);

                _elements.mXMMValues[0] = _mm_add_pd(_elements.mXMMValues[0], number);
                _elements.mXMMValues[1] = _mm_add_sd(_elements.mXMMValues[1], number);

                return(_elements);
            }

        private:
            __WNGeneralOpAddNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpAddNumber<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements, const WN_FLOAT64& _number) {
                const __m128d number = _mm_set1_pd(_number);

                _elements.mXMMValues[0] = _mm_add_pd(_elements.mXMMValues[0], number);
                _elements.mXMMValues[1] = _mm_add_pd(_elements.mXMMValues[1], number);

                return(_elements);
            }

        private:
            __WNGeneralOpAddNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpAddNumber<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 2) == 0)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements, const WN_FLOAT64& _number) {
                const __m128d number = _mm_set1_pd(_number);

                for (WN_UINT32 i = 0; i < __WNNumberRegisters_XMM_PD<Dimension>::Value; ++i) {
                    _elements.mXMMValues[i] = _mm_add_pd(_elements.mXMMValues[i], number);
                }

                return(_elements);
            }

        private:
            __WNGeneralOpAddNumber() {}
        };


        template <WN_UINT32 Dimension>
        class __WNGeneralOpAddNumber<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 2) == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements, const WN_FLOAT64& _number) {
                const __m128d number = _mm_set1_pd(_number);
                const WN_UINT32 lastElement = __WNNumberRegisters_XMM_PD<Dimension>::Value - 1;

                for (WN_UINT32 i = 0; i < lastElement; ++i) {
                    _elements.mXMMValues[i] = _mm_add_pd(_elements.mXMMValues[i], number);
                }

                _elements.mXMMValues[lastElement] = _mm_add_sd(_elements.mXMMValues[lastElement], number);

                return(_elements);
            }

        private:
            __WNGeneralOpAddNumber() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_ADD_NUMBER_SSE2_NO_AVX_H__