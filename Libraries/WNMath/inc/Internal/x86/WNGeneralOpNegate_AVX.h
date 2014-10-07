////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_NEGATE_AVX_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_NEGATE_AVX_H__

#ifndef __WN_AVX_AVAILABLE
    #error "This header requires a minimum of AVX to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpNegate<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3 || Dimension == 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements) {
                _elements.mYMMValues[0] = _mm256_xor_pd(_elements.mYMMValues[0], __WN_YMM_GET_PD_CONST(SignMask1111x64));

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpNegate() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpNegate<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3 || Dimension == 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements) {
                _elements.mYMMValues[0] = _mm256_xor_pd(_elements.mYMMValues[0], __WN_YMM_GET_PD_CONST(SignMask1111x64));

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpNegate() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpNegate<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements) {
                const __m256d mask = __WN_YMM_GET_PD_CONST(SignMask1111x64);

                for (WN_UINT32 i = 0; i < __WNNumberRegisters_YMM_PD<Dimension>::Value; ++i) {
                    _elements.mYMMValues[i] = _mm256_xor_pd(_elements.mYMMValues[i], mask);
                }

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpNegate() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_NEGATE_AVX_H__