////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_SUBTRACT_AVX_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_SUBTRACT_AVX_H__

#ifndef __WN_AVX_AVAILABLE
    #error "This header requires a minimum of AVX to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpSubtract<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 8) == 0)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements1, const __WNElements<WN_FLOAT32, Dimension>& _elements2) {
                for (WN_UINT32 i = 0; i < __WNNumberRegisters_YMM_PS<Dimension>::Value; ++i) {
                    _elements1.mYMMValues[i] = _mm256_sub_ps(_elements1.mYMMValues[i], _elements2.mYMMValues[i]);
                }

                __WN_YMM_ZEROUPPER();

                return(_elements1);
            }

        private:
            __WNGeneralOpSubtract() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpSubtract<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 8) != 0 && (Dimension % 4) == 0)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements1, const __WNElements<WN_FLOAT32, Dimension>& _elements2) {
                for (WN_UINT32 i = 0; i < __WNNumberRegisters_YMM_PS<Dimension>::Value; ++i) {
                    _elements1.mYMMValues[i] = _mm256_sub_ps(_elements1.mYMMValues[i], _elements2.mYMMValues[i]);
                }

                __WN_YMM_ZEROUPPER();

                const WN_UINT32 lastElement = __WNNumberRegisters_XMM_PS<Dimension>::Value - 1;

                _elements1.mXMMValues[lastElement] = _mm_sub_ps(_elements1.mXMMValues[lastElement], _elements2.mXMMValues[lastElement]);

                return(_elements1);
            }

        private:
            __WNGeneralOpSubtract() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpSubtract<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 3 && Dimension <= 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements1, const __WNElements<WN_FLOAT64, Dimension>& _elements2) {
                _elements1.mYMMValues[0] = _mm256_sub_pd(_elements1.mYMMValues[0], _elements2.mYMMValues[0]);

                __WN_YMM_ZEROUPPER();

                return(_elements1);
            }

        private:
            __WNGeneralOpSubtract() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpSubtract<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 4) == 0)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements1, const __WNElements<WN_FLOAT64, Dimension>& _elements2) {
                for (WN_UINT32 i = 0; i < __WNNumberRegisters_YMM_PD<Dimension>::Value; ++i) {
                    _elements1.mYMMValues[i] = _mm256_sub_pd(_elements1.mYMMValues[i], _elements2.mYMMValues[i]);
                }

                __WN_YMM_ZEROUPPER();

                return(_elements1);
            }

        private:
            __WNGeneralOpSubtract() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpSubtract<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 4) != 0 && (Dimension % 2) == 0)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements1, const __WNElements<WN_FLOAT64, Dimension>& _elements2) {
                for (WN_UINT32 i = 0; i < __WNNumberRegisters_YMM_PD<Dimension>::Value; ++i) {
                    _elements1.mYMMValues[i] = _mm256_sub_pd(_elements1.mYMMValues[i], _elements2.mYMMValues[i]);
                }

                __WN_YMM_ZEROUPPER();

                const WN_UINT32 lastElement = __WNNumberRegisters_XMM_PD<Dimension>::Value - 1;

                _elements1.mXMMValues[lastElement] = _mm_sub_pd(_elements1.mXMMValues[lastElement], _elements2.mXMMValues[lastElement]);

                return(_elements1);
            }

        private:
            __WNGeneralOpSubtract() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_SUBTRACT_AVX_H__