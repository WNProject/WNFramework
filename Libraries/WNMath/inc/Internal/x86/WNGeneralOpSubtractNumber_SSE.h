////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_SUBTRACT_NUMBER_SSE_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_SUBTRACT_NUMBER_SSE_H__

#ifndef __WN_SSE_AVAILABLE
    #error "This header requires a minimum of SSE to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpSubtractNumber<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const WN_FLOAT32& _number) {
                _elements.mXMMValues[0] = _mm_sub_ss(_elements.mXMMValues[0], _mm_load_ss(&_number));

                return(_elements);
            }

        private:
            __WNGeneralOpSubtractNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpSubtractNumber<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 2)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const WN_FLOAT32& _number) {
                _elements.mXMMValues[0] = _mm_sub_ps(_elements.mXMMValues[0], _mm_setr_ps(_number, _number, 0.0f, 0.0f));

                return(_elements);
            }

        private:
            __WNGeneralOpSubtractNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpSubtractNumber<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const WN_FLOAT32& _number) {
                _elements.mXMMValues[0] = _mm_sub_ps(_elements.mXMMValues[0], _mm_setr_ps(_number, _number, _number, 0.0f));

                return(_elements);
            }

        private:
            __WNGeneralOpSubtractNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpSubtractNumber<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const WN_FLOAT32& _number) {
                _elements.mXMMValues[0] = _mm_sub_ps(_elements.mXMMValues[0], _mm_set1_ps(_number));

                return(_elements);
            }

        private:
            __WNGeneralOpSubtractNumber() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_SUBTRACT_NUMBER_SSE_H__