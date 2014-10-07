////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_NEGATE_SSE2_NO_AVX_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_NEGATE_SSE2_NO_AVX_H__

#ifndef __WN_SSE2_AVAILABLE
    #error "This header requires a minimum of SSE2 to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpNegate<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3 && Dimension == 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements) {
                const __m128d mask = __WN_XMM_GET_PD_CONST(SignMask11x64);

                _elements.mXMMValues[0] = _mm_xor_pd(_elements.mXMMValues[0], mask);
                _elements.mXMMValues[1] = _mm_xor_pd(_elements.mXMMValues[1], mask);

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpNegate() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpNegate<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements) {
                const __m128 mask = __WN_XMM_GET_PD_CONST(SignMask11x64);

                for (WN_UINT32 i = 0; i < __WNNumberRegisters_XMM_PD<Dimension>::Value; ++i) {
                    _elements.mXMMValues[i] = _mm_xor_pd(_elements.mXMMValues[i], mask);
                }

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpNegate() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_NEGATE_SSE2_NO_AVX_H__