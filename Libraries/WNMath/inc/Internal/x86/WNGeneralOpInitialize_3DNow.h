// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_INITIALIZE_3DNOW_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_INITIALIZE_3DNOW_H__

#ifndef __WN_3DNOW_AVAILABLE
    #error "This header requires a minimum of 3DNow to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpInitialize<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 1 && (Dimension % 2) == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements) {
                const WN_UINT32 lastElement = __WNNumberRegisters_MM_PS<Dimension>::Value - 1;

                _elements.mMMValues[lastElement] = _mm_and_si64(_elements.mMMValues[lastElement], __WN_MM_GET_PS_CONST(ElementMask10x32));

                _m_femms();

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpInitialize() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_INITIALIZE_3DNOW_H__