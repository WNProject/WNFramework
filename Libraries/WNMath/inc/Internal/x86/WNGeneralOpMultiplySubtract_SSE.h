// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_MULTIPLY_SUBTRACT_SSE_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_MULTIPLY_SUBTRACT_SSE_H__

#ifndef __WN_SSE_AVAILABLE
    #error "This header requires a minimum of SSE to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpMultiplySubtract<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements1, const __WNElements<WN_FLOAT32, Dimension>& _elements2, const __WNElements<WN_FLOAT32, Dimension>& _elements3) {
                _elements1.mXMMValues[0] = _mm_sub_ss(_elements3.mXMMValues[0], _mm_mul_ss(_elements1.mXMMValues[0], _elements2.mXMMValues[0]));

                return(_elements1);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpMultiplySubtract() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpMultiplySubtract<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >=2 && Dimension <= 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements1, const __WNElements<WN_FLOAT32, Dimension>& _elements2, const __WNElements<WN_FLOAT32, Dimension>& _elements3) {
                _elements1.mXMMValues[0] = _mm_sub_ps(_elements3.mXMMValues[0], _mm_mul_ps(_elements1.mXMMValues[0], _elements2.mXMMValues[0]));

                return(_elements1);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpMultiplySubtract() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_MULTIPLY_ADD_SSE_H__