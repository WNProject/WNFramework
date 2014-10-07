////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_ADD_NUMBER_SSE_NO_AVX_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_ADD_NUMBER_SSE_NO_AVX_H__

#ifndef __WN_SSE_AVAILABLE
    #error "This header requires a minimum of SSE to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpAddNumber<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 4) == 0)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const WN_FLOAT32& _number) {
                const __m128 number = _mm_set1_ps(_number);

                for (WN_UINT32 i = 0; i < __WNNumberRegisters_XMM_PS<Dimension>::Value; ++i) {
                    _elements.mXMMValues[i] = _mm_add_ps(_elements.mXMMValues[i], number);
                }

                return(_elements);
            }

        private:
            __WNGeneralOpAddNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpAddNumber<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 4) == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const WN_FLOAT32& _number) {
                const __m128 number = _mm_set1_ps(_number);
                const WN_UINT32 lastElement = __WNNumberRegisters_XMM_PS<Dimension>::Value - 1;

                for (WN_UINT32 i = 0; i < lastElement; ++i) {
                    _elements.mXMMValues[i] = _mm_add_ps(_elements.mXMMValues[i], number);
                }

                _elements.mXMMValues[lastElement] = _mm_add_ss(_elements.mXMMValues[lastElement], number);

                return(_elements);
            }

        private:
            __WNGeneralOpAddNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpAddNumber<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 4) == 2)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const WN_FLOAT32& _number) {
                const __m128 number = _mm_set1_ps(_number);
                const WN_UINT32 count = __WNNumberRegisters_XMM_PS<Dimension>::Value;

                for (WN_UINT32 i = 0; i < count; ++i) {
                    _elements.mXMMValues[i] = _mm_add_ps(_elements.mXMMValues[i], number);
                }

                const WN_UINT32 lastElement = count - 1;

                _elements.mXMMValues[lastElement] = _mm_and_ps(_elements.mXMMValues[lastElement], __WN_XMM_GET_PS_CONST(ElementMask1100x32));

                return(_elements);
            }

        private:
            __WNGeneralOpAddNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpAddNumber<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 4) == 3)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const WN_FLOAT32& _number) {
                const __m128 number = _mm_set1_ps(_number);
                const WN_UINT32 count = __WNNumberRegisters_XMM_PS<Dimension>::Value;

                for (WN_UINT32 i = 0; i < count; ++i) {
                    _elements.mXMMValues[i] = _mm_add_ps(_elements.mXMMValues[i], number);
                }

                const WN_UINT32 lastElement = count - 1;

                _elements.mXMMValues[lastElement] = _mm_and_ps(_elements.mXMMValues[lastElement], __WN_XMM_GET_PS_CONST(ElementMask1110x32));

                return(_elements);
            }

        private:
            __WNGeneralOpAddNumber() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_ADD_NUMBER_SSE_NO_AVX_H__