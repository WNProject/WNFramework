////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_ZERO_SSE_NO_AVX_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_ZERO_SSE_NO_AVX_H__

#ifndef __WN_SSE_AVAILABLE
    #error "This header requires a minimum of SSE to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpZero<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements) {
                for (WN_UINT32 i = 0; i < __WNNumberRegisters_XMM_PS<Dimension>::Value; ++i) {
                    _elements.mXMMValues[i] = _mm_setzero_ps();
                }

                return(_elements);
            }

        private:
            __WNGeneralOpZero() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_ZERO_SSE_NO_AVX_H__