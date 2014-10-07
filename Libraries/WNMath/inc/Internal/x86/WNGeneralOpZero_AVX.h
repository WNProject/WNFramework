////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_ZERO_AVX_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_ZERO_AVX_H__

#ifndef __WN_AVX_AVAILABLE
    #error "This header requires a minimum of AVX to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpZero<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && Dimension <= 8)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements) {
                _elements.mYMMValues[0] = _mm256_setzero_ps();

                __WN_YMM_ZEROUPPER();

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpZero() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpZero<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 9 && ((Dimension % 8) == 0 || (Dimension % 8) > 4))>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements) {
                const __m256 number = _mm256_setzero_ps();

                for (WN_UINT32 i = 0; i < __WNNumberRegisters_YMM_PS<Dimension>::Value; ++i) {
                    _elements.mYMMValues[i] = number;
                }

                __WN_YMM_ZEROUPPER();

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpZero() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpZero<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 9 && (Dimension % 8) != 0 && (Dimension % 8) <= 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements) {
                const __m256 number = _mm256_setzero_ps();

                for (WN_UINT32 i = 0; i < __WNNumberRegisters_YMM_PS<Dimension>::Value; ++i) {
                    _elements.mYMMValues[i] = number;
                }

                __WN_YMM_ZEROUPPER();

                _elements.mXMMValues[__WNNumberRegisters_XMM_PS<Dimension>::Value - 1] = _mm_setzero_ps();

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpZero() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpZero<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 3 && Dimension <= 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements) {
                _elements.mYMMValues[0] = _mm256_setzero_pd();

                __WN_YMM_ZEROUPPER();

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpZero() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpZero<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && ((Dimension % 4) == 0 || (Dimension % 4) >= 3))>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements) {
                const __m256d number = _mm256_setzero_pd();

                for (WN_UINT32 i = 0; i < __WNNumberRegisters_YMM_PD<Dimension>::Value; ++i) {
                    _elements.mYMMValues[i] = number;
                }

                __WN_YMM_ZEROUPPER();

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpZero() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpZero<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 4) != 0 && (Dimension % 4) <= 2)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements) {
                const __m256d number = _mm256_setzero_pd();

                for (WN_UINT32 i = 0; i < __WNNumberRegisters_YMM_PD<Dimension>::Value; ++i) {
                    _elements.mYMMValues[i] = number;
                }

                __WN_YMM_ZEROUPPER();

                _elements.mXMMValues[__WNNumberRegisters_XMM_PD<Dimension>::Value - 1] = _mm_setzero_pd();

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpZero() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_ZERO_AVX_H__