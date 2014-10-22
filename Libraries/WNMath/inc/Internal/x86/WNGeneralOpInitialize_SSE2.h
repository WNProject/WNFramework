// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_INITIALIZE_SSE2_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_INITIALIZE_SSE2_H__

#ifndef __WN_SSE2_AVAILABLE
    #error "This header requires a minimum of SSE2 to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpInitialize<WN_INT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 2 && (Dimension % 4) == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT32, Dimension>& Execute(__WNElements<WN_INT32, Dimension>& _elements) {
                const WN_UINT32 lastElement = __WNNumberRegisters_XMM_PI32<Dimension>::Value - 1;

                _elements.mXMMValues[lastElement] = _mm_and_si128(_elements.mXMMValues[lastElement], __WN_XMM_GET_SI128_CONST(ElementMask1000x32));

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpInitialize() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpInitialize<WN_INT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 2 && (Dimension % 4) == 2)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT32, Dimension>& Execute(__WNElements<WN_INT32, Dimension>& _elements) {
                const WN_UINT32 lastElement = __WNNumberRegisters_XMM_PI32<Dimension>::Value - 1;

                _elements.mXMMValues[lastElement] = _mm_and_si128(_elements.mXMMValues[lastElement], __WN_XMM_GET_SI128_CONST(ElementMask1100x32));

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpInitialize() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpInitialize<WN_INT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 2 && (Dimension % 4) == 3)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT32, Dimension>& Execute(__WNElements<WN_INT32, Dimension>& _elements) {
                const WN_UINT32 lastElement = __WNNumberRegisters_XMM_PI32<Dimension>::Value - 1;

                _elements.mXMMValues[lastElement] = _mm_and_si128(_elements.mXMMValues[lastElement], __WN_XMM_GET_SI128_CONST(ElementMask1110x32));

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpInitialize() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpInitialize<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 2 && (Dimension % 2) != 0)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements) {
                const WN_UINT32 lastElement = __WNNumberRegisters_XMM_PD<Dimension>::Value - 1;

                _elements.mXMMValues[lastElement] = _mm_and_pd(_elements.mXMMValues[lastElement], __WN_XMM_GET_PD_CONST(ElementMask10x64));

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpInitialize() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_INITIALIZE_SSE2_H__