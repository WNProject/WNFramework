// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_ADD_SSE_NO_AVX_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_ADD_SSE_NO_AVX_H__

#ifndef __WN_SSE_AVAILABLE
    #error "This header requires a minimum of SSE to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpAdd<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 4) != 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements1, const __WNElements<WN_FLOAT32, Dimension>& _elements2) {
                for (WN_UINT32 i = 0; i < __WNNumberRegisters_XMM_PS<Dimension>::Value; ++i) {
                    _elements1.mXMMValues[i] = _mm_add_ps(_elements1.mXMMValues[i], _elements2.mXMMValues[i]);
                }

                return(_elements1);
            }

        private:
            __WNGeneralOpAdd() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpAdd<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 5 && (Dimension % 4) == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements1, const __WNElements<WN_FLOAT32, Dimension>& _elements2) {
                const WN_UINT32 lastElement = __WNNumberRegisters_XMM_PS<Dimension>::Value - 1;

                for (WN_UINT32 i = 0; i < lastElement; ++i) {
                    _elements1.mXMMValues[i] = _mm_add_ps(_elements1.mXMMValues[i], _elements2.mXMMValues[i]);
                }

                _elements1.mXMMValues[lastElement] = _mm_add_ss(_elements1.mXMMValues[lastElement], _elements2.mXMMValues[lastElement]);

                return(_elements1);
            }

        private:
            __WNGeneralOpAdd() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_ADD_SSE_NO_AVX_H__