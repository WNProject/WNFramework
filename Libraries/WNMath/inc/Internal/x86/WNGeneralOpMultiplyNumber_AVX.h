// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_MULTIPLY_NUMBER_AVX_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_MULTIPLY_NUMBER_AVX_H__

#ifndef __WN_AVX_AVAILABLE
    #error "This header requires a minimum of AVX to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpMultiplyNumber<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 8) == 0)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const WN_FLOAT32& _number) {
                const __m256 number = _mm256_broadcast_ss(&_number);

                for (WN_UINT32 i = 0; i < __WNNumberRegisters_YMM_PS<Dimension>::Value; ++i) {
                    _elements.mYMMValues[i] = _mm256_mul_ps(_elements.mYMMValues[i], number);
                }

                __WN_YMM_ZEROUPPER();

                return(_elements);
            }

        private:
            __WNGeneralOpMultiplyNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpMultiplyNumber<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 8) == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const WN_FLOAT32& _number) {
                const __m256 number = _mm256_broadcast_ss(&_number);

                for (WN_UINT32 i = 0; i < __WNNumberRegisters_YMM_PS<Dimension>::Value; ++i) {
                    _elements.mYMMValues[i] = _mm256_mul_ps(_elements.mYMMValues[i], number);
                }

                __WN_YMM_ZEROUPPER();

                const WN_UINT32 lastElement = __WNNumberRegisters_XMM_PS<Dimension>::Value - 1;

                _elements.mXMMValues[lastElement] = _mm_mul_ss(_elements.mXMMValues[lastElement], _mm_load_ss(&_number));

                return(_elements);
            }

        private:
            __WNGeneralOpMultiplyNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpMultiplyNumber<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 8) == 2)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const WN_FLOAT32& _number) {
                const __m256 number = _mm256_broadcast_ss(&_number);

                for (WN_UINT32 i = 0; i < __WNNumberRegisters_YMM_PS<Dimension>::Value; ++i) {
                    _elements.mYMMValues[i] = _mm256_mul_ps(_elements.mYMMValues[i], number);
                }

                __WN_YMM_ZEROUPPER();

                const WN_UINT32 lastElement = __WNNumberRegisters_XMM_PS<Dimension>::Value - 1;

                _elements.mXMMValues[lastElement] = _mm_mul_ps(_elements.mXMMValues[lastElement], _mm_setr_ps(_number, _number, 0.0f, 0.0f));

                return(_elements);
            }

        private:
            __WNGeneralOpMultiplyNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpMultiplyNumber<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 8) == 3)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const WN_FLOAT32& _number) {
                const __m256 number = _mm256_broadcast_ss(&_number);

                for (WN_UINT32 i = 0; i < __WNNumberRegisters_YMM_PS<Dimension>::Value; ++i) {
                    _elements.mYMMValues[i] = _mm256_mul_ps(_elements.mYMMValues[i], number);
                }

                __WN_YMM_ZEROUPPER();

                const WN_UINT32 lastElement = __WNNumberRegisters_XMM_PS<Dimension>::Value - 1;

                _elements.mXMMValues[lastElement] = _mm_mul_ps(_elements.mXMMValues[lastElement], _mm_setr_ps(_number, _number, _number, 0.0f));

                return(_elements);
            }

        private:
            __WNGeneralOpMultiplyNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpMultiplyNumber<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 8) == 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const WN_FLOAT32& _number) {
                const __m128 number1 = _mm_broadcast_ss(&_number);
                const __m256 number2 = _mm256_insertf128_ps(_mm256_castps128_ps256(number1), number1, 1);

                for (WN_UINT32 i = 0; i < __WNNumberRegisters_YMM_PS<Dimension>::Value; ++i) {
                    _elements.mYMMValues[i] = _mm256_mul_ps(_elements.mYMMValues[i], number2);
                }

                __WN_YMM_ZEROUPPER();

                const WN_UINT32 lastElement = __WNNumberRegisters_XMM_PS<Dimension>::Value - 1;

                _elements.mXMMValues[lastElement] = _mm_mul_ps(_elements.mXMMValues[lastElement], number1);

                return(_elements);
            }

        private:
            __WNGeneralOpMultiplyNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpMultiplyNumber<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements, const WN_FLOAT64& _number) {
                _elements.mYMMValues[0] = _mm256_mul_pd(_elements.mYMMValues[0], _mm256_setr_pd(_number, _number, _number, 0.0));

                __WN_YMM_ZEROUPPER();

                return(_elements);
            }

        private:
            __WNGeneralOpMultiplyNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpMultiplyNumber<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements, const WN_FLOAT64& _number) {
                _elements.mYMMValues[0] = _mm256_mul_pd(_elements.mYMMValues[0], _mm256_broadcast_sd(&_number));

                __WN_YMM_ZEROUPPER();

                return(_elements);
            }

        private:
            __WNGeneralOpMultiplyNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpMultiplyNumber<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 4) == 0)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements, const WN_FLOAT64& _number) {
                const __m256d number = _mm256_broadcast_sd(&_number);

                for (WN_UINT32 i = 0; i < __WNNumberRegisters_YMM_PD<Dimension>::Value; ++i) {
                    _elements.mYMMValues[i] = _mm256_mul_pd(_elements.mYMMValues[i], number);
                }

                __WN_YMM_ZEROUPPER();

                return(_elements);
            }

        private:
            __WNGeneralOpMultiplyNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpMultiplyNumber<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 4) == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements, const WN_FLOAT64& _number) {
                const __m128d number1 = _mm_set1_pd(_number);
                const __m256d number2 = _mm256_insertf128_pd(_mm256_castpd128_pd256(number1), number1, 1);

                for (WN_UINT32 i = 0; i < __WNNumberRegisters_YMM_PD<Dimension>::Value; ++i) {
                    _elements.mYMMValues[i] = _mm256_mul_pd(_elements.mYMMValues[i], number2);
                }

                __WN_YMM_ZEROUPPER();

                const WN_UINT32 lastElement = __WNNumberRegisters_XMM_PS<Dimension>::Value - 1;

                _elements.mXMMValues[lastElement] = _mm_mul_sd(_elements.mXMMValues[lastElement], number1);

                return(_elements);
            }

        private:
            __WNGeneralOpMultiplyNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpMultiplyNumber<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 4) == 2)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements, const WN_FLOAT64& _number) {
                const __m128d number1 = _mm_set1_pd(_number);
                const __m256d number2 = _mm256_insertf128_pd(_mm256_castpd128_pd256(number1), number1, 1);

                for (WN_UINT32 i = 0; i < __WNNumberRegisters_YMM_PD<Dimension>::Value; ++i) {
                    _elements.mYMMValues[i] = _mm256_mul_pd(_elements.mYMMValues[i], number2);
                }

                __WN_YMM_ZEROUPPER();

                const WN_UINT32 lastElement = __WNNumberRegisters_XMM_PS<Dimension>::Value - 1;

                _elements.mXMMValues[lastElement] = _mm_mul_pd(_elements.mXMMValues[lastElement], number1);

                return(_elements);
            }

        private:
            __WNGeneralOpMultiplyNumber() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_MULTIPLY_NUMBER_AVX_H__