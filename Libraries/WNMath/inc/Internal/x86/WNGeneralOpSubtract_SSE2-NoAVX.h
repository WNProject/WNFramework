// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_SUBTRACT_SSE2_NO_AVX_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_SUBTRACT_SSE2_NO_AVX_H__

#ifndef __WN_SSE_AVAILABLE
    #error "This header requires a minimum of SSE to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpSubtract<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements1, const __WNElements<WN_FLOAT64, Dimension>& _elements2) {
                _elements1.mXMMValues[0] = _mm_sub_pd(_elements1.mXMMValues[0], _elements2.mXMMValues[0]);
                _elements1.mXMMValues[1] = _mm_sub_sd(_elements1.mXMMValues[1], _elements2.mXMMValues[1]);

                return(_elements1);
            }

        private:
            __WNGeneralOpSubtract() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpSubtract<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements1, const __WNElements<WN_FLOAT64, Dimension>& _elements2) {
                _elements1.mXMMValues[0] = _mm_sub_pd(_elements1.mXMMValues[0], _elements2.mXMMValues[0]);
                _elements1.mXMMValues[1] = _mm_sub_pd(_elements1.mXMMValues[1], _elements2.mXMMValues[1]);

                return(_elements1);
            }

        private:
            __WNGeneralOpSubtract() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpSubtract<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 2) == 0)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements1, const __WNElements<WN_FLOAT64, Dimension>& _elements2) {
                for (WN_UINT32 i = 0; i < __WNNumberRegisters_XMM_PD<Dimension>::Value; ++i) {
                    _elements1.mXMMValues[i] = _mm_sub_pd(_elements1.mXMMValues[i], _elements2.mXMMValues[i]);
                }

                return(_elements1);
            }

        private:
            __WNGeneralOpSubtract() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpSubtract<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 2) == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements1, const __WNElements<WN_FLOAT64, Dimension>& _elements2) {
                const WN_UINT32 lastElement = __WNNumberRegisters_XMM_PD<Dimension>::Value - 1;

                for (WN_UINT32 i = 0; i < lastElement; ++i) {
                    _elements1.mXMMValues[i] = _mm_sub_pd(_elements1.mXMMValues[i], _elements2.mXMMValues[i]);
                }

                _elements1.mXMMValues[lastElement] = _mm_sub_sd(_elements1.mXMMValues[lastElement], _elements2.mXMMValues[lastElement]);

                return(_elements1);
            }

        private:
            __WNGeneralOpSubtract() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_SUBTRACT_SSE2_NO_AVX_H__