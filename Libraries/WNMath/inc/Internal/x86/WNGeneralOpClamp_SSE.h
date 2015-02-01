// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_CLAMP_SSE_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_CLAMP_SSE_H__

#ifndef __WN_SSE_AVAILABLE
    #error "This header requires a minimum of SSE to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpClamp<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const __WNElements<WN_FLOAT32, Dimension>& _minimum, const __WNElements<WN_FLOAT32, Dimension>& _maximum) {
                _elements.mXMMValues[0] = _mm_min_ss(_mm_max_ss(_elements.mXMMValues[0], _minimum.mXMMValues[0]), _maximum.mXMMValues[0]);

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpClamp() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpClamp<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 2 && Dimension <= 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT32, Dimension>& Execute(__WNElements<WN_FLOAT32, Dimension>& _elements, const __WNElements<WN_FLOAT32, Dimension>& _minimum, const __WNElements<WN_FLOAT32, Dimension>& _maximum) {
                _elements.mXMMValues[0] = _mm_min_ps(_mm_max_ps(_elements.mXMMValues[0], _minimum.mXMMValues[0]), _maximum.mXMMValues[0]);

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpClamp() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_CLAMP_SSE_H__