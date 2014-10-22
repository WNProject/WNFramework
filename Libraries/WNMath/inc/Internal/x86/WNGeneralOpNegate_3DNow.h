// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_NEGATE_3D_NOW_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_NEGATE_3D_NOW_H__

#ifndef __WN_3DNOW_AVAILABLE
    #error "This header requires a minimum of 3DNow to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpNegate<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 1 || Dimension == 2)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements) {
                _elements.mMMValues[0] = _mm_xor_si64(_elements.mMMValues[0], __WN_MM_GET_PS_CONST(SignMask11x32));

                _m_femms();

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpNegate() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpNegate<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3 || Dimension == 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements) {
                const __m64 mask = __WN_MM_GET_PS_CONST(SignMask11x32);

                _elements.mMMValues[0] = _mm_xor_si64(_elements.mMMValues[0], mask);
                _elements.mMMValues[1] = _mm_xor_si64(_elements.mMMValues[1], mask);

                _m_femms();

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpNegate() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_NEGATE_3D_NOW_H__