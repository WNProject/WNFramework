////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_ONE_SSE2_NO_AVX_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_ONE_SSE2_NO_AVX_H__

#ifndef __WN_SSE_AVAILABLE
    #error "This header requires a minimum of SSE to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpOne<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements) {
                _elements.mXMMValues[0] = _mm_set1_pd(1.0);
                _elements.mXMMValues[1] = _mm_set_sd(1.0);

                return(_elements);
            }

        private:
            __WNGeneralOpOne() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpOne<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements) {
                const __m128d one = _mm_set1_pd(1.0);

                _elements.mXMMValues[0] = one;
                _elements.mXMMValues[1] = one;

                return(_elements);
            }

        private:
            __WNGeneralOpOne() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpOne<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 2) == 0)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements) {
                const __m128d one = _mm_set1_pd(1.0);

                for (WN_UINT32 i = 0; i < __WNNumberRegisters_XMM_PD<Dimension>::Value; ++i) {
                    _elements.mXMMValues[i] = one;
                }

                return(_elements);
            }

        private:
            __WNGeneralOpOne() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpOne<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 2) == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements) {
                const __m128d one = _mm_set1_pd(1.0);
                const WN_UINT32 lastElement = __WNNumberRegisters_XMM_PD<Dimension>::Value - 1;

                for (WN_UINT32 i = 0; i < lastElement; ++i) {
                    _elements.mXMMValues[i] = one;
                }

                _elements.mXMMValues[lastElement] = _mm_set_sd(1.0);

                return(_elements);
            }

        private:
            __WNGeneralOpOne() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_ONE_SSE2_NO_AVX_H__