// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_SUBTRACT_SSE2_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_SUBTRACT_SSE2_H__

#ifndef __WN_SSE2_AVAILABLE
    #error "This header requires a minimum of SSE2 to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpSubtract<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements1, const __WNElements<WN_FLOAT64, Dimension>& _elements2) {
                _elements1.mXMMValues[0] = _mm_sub_sd(_elements1.mXMMValues[0], _elements2.mXMMValues[0]);

                return(_elements1);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpSubtract() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpSubtract<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 2)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements1, const __WNElements<WN_FLOAT64, Dimension>& _elements2) {
                _elements1.mXMMValues[0] = _mm_sub_pd(_elements1.mXMMValues[0], _elements2.mXMMValues[0]);

                return(_elements1);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpSubtract() {}
        };
    }
}

#ifndef __WN_AVX_AVAILABLE
    #include "WNMath/inc/Internal/x86/WNGeneralOpSubtract_SSE2-NoAVX.h"
#endif

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_SUBTRACT_SSE2_H__