// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_NEGATE_SSE2_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_NEGATE_SSE2_H__

#ifndef __WN_SSE2_AVAILABLE
    #error "This header requires a minimum of SSE2 to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpNegate<WN_INT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 2 && Dimension <= 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT32, Dimension>& Execute(__WNElements<WN_INT32, Dimension>& _elements) {
                _elements.mXMMValues[0] = _mm_xor_si128(_elements.mXMMValues[0], __WN_XMM_GET_SI128_CONST(SignMask1111x32));

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpNegate() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpNegate<WN_INT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 4) != 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT32, Dimension>& Execute(__WNElements<WN_INT32, Dimension>& _elements) {
                const __m128i mask = __WN_XMM_GET_SI128_CONST(SignMask1111x32);

                for (WN_UINT32 i = 0; i < __WNNumberRegisters_XMM_PI32<Dimension>::Value; ++i) {
                    _elements.mXMMValues[i] = _mm_xor_si128(_elements.mXMMValues[i], mask);
                }

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpNegate() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpNegate<WN_INT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 4) == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT32, Dimension>& Execute(__WNElements<WN_INT32, Dimension>& _elements) {
                const __m128i mask = __WN_XMM_GET_SI128_CONST(SignMask1111x32);

                for (WN_UINT32 i = 0; i < __WNNumberRegisters_XMM_PI32<Dimension>::Value; ++i) {
                    _elements.mXMMValues[i] = _mm_xor_si128(_elements.mXMMValues[i], mask);
                }

                const WN_UINT32 lastElement = Dimension - 1;

                _elements.mValues[lastElement] = -_elements.mValues[lastElement];

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpNegate() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpNegate<WN_INT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 2)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT64, Dimension>& Execute(__WNElements<WN_INT64, Dimension>& _elements) {
                _elements.mXMMValues[0] = _mm_xor_si128(_elements.mXMMValues[0], __WN_XMM_GET_SI128_CONST(SignMask11x64));

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpNegate() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpNegate<WN_INT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT64, Dimension>& Execute(__WNElements<WN_INT64, Dimension>& _elements) {
                _elements.mXMMValues[0] = _mm_xor_si128(_elements.mXMMValues[0], __WN_XMM_GET_SI128_CONST(SignMask11x64));

                const WN_UINT32 lastElement = Dimension - 1;

                _elements.mValues[lastElement] = -_elements.mValues[lastElement];

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpNegate() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpNegate<WN_INT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT64, Dimension>& Execute(__WNElements<WN_INT64, Dimension>& _elements) {
                const __m128i mask = __WN_XMM_GET_SI128_CONST(SignMask11x64);

                _elements.mXMMValues[0] = _mm_xor_si128(_elements.mXMMValues[0], mask);
                _elements.mXMMValues[1] = _mm_xor_si128(_elements.mXMMValues[1], mask);

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpNegate() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpNegate<WN_INT64, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 2) == 0)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT64, Dimension>& Execute(__WNElements<WN_INT64, Dimension>& _elements) {
                const __m128i mask = __WN_XMM_GET_SI128_CONST(SignMask11x64);

                for (WN_UINT32 i = 0; i < __WNNumberRegisters_XMM_PI64<Dimension>::Value; ++i) {
                    _elements.mXMMValues[i] = _mm_xor_si128(_elements.mXMMValues[i], mask);
                }

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpNegate() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpNegate<WN_INT64, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 2) == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT64, Dimension>& Execute(__WNElements<WN_INT64, Dimension>& _elements) {
                const __m128i mask = __WN_XMM_GET_SI128_CONST(SignMask11x64);

                for (WN_UINT32 i = 0; i < __WNNumberRegisters_XMM_PI64<Dimension>::Value; ++i) {
                    _elements.mXMMValues[i] = _mm_xor_si128(_elements.mXMMValues[i], mask);
                }

                const WN_UINT32 lastElement = Dimension - 1;

                _elements.mValues[lastElement] = -_elements.mValues[lastElement];

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpNegate() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpNegate<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 1 || Dimension == 2)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements) {
                _elements.mXMMValues[0] = _mm_xor_pd(_elements.mXMMValues[0], __WN_XMM_GET_PD_CONST(SignMask11x64));

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpNegate() {}
        };
    }
}

#ifndef __WN_AVX_AVAILABLE
    #include "WNMath/inc/Internal/x86/WNGeneralOpNegate_SSE2-NoAVX.h"
#endif

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_NEGATE_SSE2_H__