// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_NEGATE_SSE_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_NEGATE_SSE_H__

#ifndef __WN_SSE_AVAILABLE
    #error "This header requires a minimum of SSE to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpNegate<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 1 && Dimension <= 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements) {
                _elements.mXMMValues[0] = _mm_xor_ps(_elements.mXMMValues[0], __WN_XMM_GET_PS_CONST(SignMask1111x32));

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpNegate() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_NEGATE_SSE_H__