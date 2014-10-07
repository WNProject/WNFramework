////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_MULTIPLY_SSE4_1_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_MULTIPLY_SSE4_1_H__

#ifndef __WN_SSE4_1_AVAILABLE
    #error "This header requires a minimum of SSE4.1 to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpMultiply<WN_INT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 1 && Dimension <= 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT32, Dimension>& Execute(__WNElements<WN_INT32, Dimension>& _elements1, const __WNElements<WN_INT32, Dimension>& _elements2) {
                _elements1.mXMMValues[0] = _mm_mullo_epi32(_elements1.mXMMValues[0], _elements2.mXMMValues[0]);

                return(_elements1);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpMultiply() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpMultiply<WN_INT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT32, Dimension>& Execute(__WNElements<WN_INT32, Dimension>& _elements1, const __WNElements<WN_INT32, Dimension>& _elements2) {
                for (WN_UINT32 i = 0; i < __WNNumberRegisters_XMM_PI32<Dimension>::Value; ++i) {
                    _elements1.mXMMValues[i] = _mm_mullo_epi32(_elements1.mXMMValues[i], _elements2.mXMMValues[i]);
                }

                return(_elements1);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpMultiply() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_MULTIPLY_SSE4_1_H__