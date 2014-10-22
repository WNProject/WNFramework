// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_ONE_AVX_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_ONE_AVX_H__

#ifndef __WN_AVX_AVAILABLE
    #error "This header requires a minimum of AVX to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpOne<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 8) == 0)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements) {
                const __m256 number = _mm256_set1_ps(1.0f);

                for (WN_UINT32 i = 0; i < __WNNumberRegisters_YMM_PS<Dimension>::Value; ++i) {
                    _elements.mYMMValues[i] = number;
                }

                __WN_YMM_ZEROUPPER();

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpOne() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpOne<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 8) == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements) {
                const __m256 number = _mm256_set1_ps(1.0f);

                for (WN_UINT32 i = 0; i < __WNNumberRegisters_YMM_PS<Dimension>::Value; ++i) {
                    _elements.mYMMValues[i] = number;
                }

                __WN_YMM_ZEROUPPER();

                _elements.mXMMValues[__WNNumberRegisters_XMM_PS<Dimension>::Value - 1] = _mm_set_ss(1.0f);

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpOne() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpOne<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 8) == 2)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements) {
                const __m256 number = _mm256_set1_ps(1.0f);

                for (WN_UINT32 i = 0; i < __WNNumberRegisters_YMM_PS<Dimension>::Value; ++i) {
                    _elements.mYMMValues[i] = number;
                }

                __WN_YMM_ZEROUPPER();

                _elements.mXMMValues[__WNNumberRegisters_XMM_PS<Dimension>::Value - 1] = _mm_setr_ps(1.0f, 1.0f, 0.0f, 0.0f);

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpOne() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpOne<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 8) == 3)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements) {
                const __m256 number = _mm256_set1_ps(1.0f);

                for (WN_UINT32 i = 0; i < __WNNumberRegisters_YMM_PS<Dimension>::Value; ++i) {
                    _elements.mYMMValues[i] = number;
                }

                __WN_YMM_ZEROUPPER();

                _elements.mXMMValues[__WNNumberRegisters_XMM_PS<Dimension>::Value - 1] = _mm_setr_ps(1.0f, 1.0f, 1.0f, 0.0f);

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpOne() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpOne<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 8) == 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements) {
                const __m256 number = _mm256_set1_ps(1.0f);

                for (WN_UINT32 i = 0; i < __WNNumberRegisters_YMM_PS<Dimension>::Value; ++i) {
                    _elements.mYMMValues[i] = number;
                }

                __WN_YMM_ZEROUPPER();

                _elements.mXMMValues[__WNNumberRegisters_XMM_PS<Dimension>::Value - 1] = _mm_set1_ps(1.0f);

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpOne() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpOne<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 8) == 5)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements) {
                const WN_UINT32 lastElement = __WNNumberRegisters_YMM_PS<Dimension>::Value - 1;
                const __m256 number = _mm256_set1_ps(1.0f);

                for (WN_UINT32 i = 0; i < lastElement; ++i) {
                    _elements.mYMMValues[i] = number;
                }

                _elements.mYMMValues[lastElement] = _mm256_setr_ps(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f);

                __WN_YMM_ZEROUPPER();

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpOne() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpOne<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 8) == 6)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements) {
                const WN_UINT32 lastElement = __WNNumberRegisters_YMM_PS<Dimension>::Value - 1;
                const __m256 number = _mm256_set1_ps(1.0f);

                for (WN_UINT32 i = 0; i < lastElement; ++i) {
                    _elements.mYMMValues[i] = number;
                }

                _elements.mYMMValues[lastElement] = _mm256_setr_ps(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f);

                __WN_YMM_ZEROUPPER();

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpOne() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpOne<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 8) == 7)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements) {
                const WN_UINT32 lastElement = __WNNumberRegisters_YMM_PS<Dimension>::Value - 1;
                const __m256 number = _mm256_set1_ps(1.0f);

                for (WN_UINT32 i = 0; i < lastElement; ++i) {
                    _elements.mYMMValues[i] = number;
                }

                _elements.mYMMValues[lastElement] = _mm256_setr_ps(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f);

                __WN_YMM_ZEROUPPER();

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpOne() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpOne<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements) {
                _elements.mYMMValues[0] = _mm256_setr_pd(1.0, 1.0, 1.0, 0.0);

                 __WN_YMM_ZEROUPPER();

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpOne() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpOne<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements) {
                _elements.mYMMValues[0] = _mm256_set1_pd(1.0);

                __WN_YMM_ZEROUPPER();

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpOne() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpOne<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 4) == 0)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements) {
                for (WN_UINT32 i = 0; i < __WNNumberRegisters_YMM_PD<Dimension>::Value; ++i) {
                    _elements.mYMMValues[i] = _mm256_set1_pd(1.0);
                }

                __WN_YMM_ZEROUPPER();

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpOne() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpOne<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 4) == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements) {
                for (WN_UINT32 i = 0; i < __WNNumberRegisters_YMM_PD<Dimension>::Value; ++i) {
                    _elements.mYMMValues[i] = _mm256_set1_pd(1.0);
                }

                __WN_YMM_ZEROUPPER();

                _elements.mXMMValues[__WNNumberRegisters_XMM_PD<Dimension>::Value - 1] = _mm_set_sd(1.0);

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpOne() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpOne<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 4) == 2)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements) {
                for (WN_UINT32 i = 0; i < __WNNumberRegisters_YMM_PD<Dimension>::Value; ++i) {
                    _elements.mYMMValues[i] = _mm256_set1_pd(1.0);
                }

                __WN_YMM_ZEROUPPER();

                _elements.mXMMValues[__WNNumberRegisters_XMM_PD<Dimension>::Value - 1] = _mm_set1_pd(1.0);

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpOne() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpOne<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 4) == 3)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements) {
                for (WN_UINT32 i = 0; i < __WNNumberRegisters_YMM_PD<Dimension>::Value; ++i) {
                    _elements.mYMMValues[i] = _mm256_set1_pd(1.0);
                }

                __WN_YMM_ZEROUPPER();

                _elements.mXMMValues[__WNNumberRegisters_XMM_PD<Dimension>::Value - 1] = _mm_setr_pd(1.0, 0.0);

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpOne() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_ONE_AVX_H__