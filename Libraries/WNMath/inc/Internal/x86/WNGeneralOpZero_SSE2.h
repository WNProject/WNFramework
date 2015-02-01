// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_ZERO_SSE2_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_ZERO_SSE2_H__

#ifndef __WN_SSE_AVAILABLE
    #error "This header requires a minimum of SSE2 to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpZero<WN_INT32, Dimension, typename WNCore::WNEnableWhen<(Dimension >= 2 && Dimension <= 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT32, Dimension>& Execute(__WNElements<WN_INT32, Dimension>& _elements) {
                _elements.mXMMValues[0] = _mm_setzero_si128();

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpZero() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpZero<WN_INT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 2)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT64, Dimension>& Execute(__WNElements<WN_INT64, Dimension>& _elements) {
                _elements.mXMMValues[0] = _mm_setzero_si128();

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpZero() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpZero<WN_INT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT64, Dimension>& Execute(__WNElements<WN_INT64, Dimension>& _elements) {
                _elements.mXMMValues[0] = _mm_setzero_si128();

                const WN_UINT32 lastElement = Dimension - 1;

                _elements.mValues[lastElement] = 0;

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpZero() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpZero<WN_INT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT64, Dimension>& Execute(__WNElements<WN_INT64, Dimension>& _elements) {
                const __m128i zero = _mm_setzero_si128();

                _elements.mXMMValues[0] = zero;
                _elements.mXMMValues[1] = zero;

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpZero() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpZero<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 1 || Dimension == 2)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements) {
                _elements.mXMMValues[0] = _mm_setzero_pd();

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpZero() {}
        };
    }
}

#ifndef __WN_AVX_AVAILABLE
    #include "WNMath/inc/Internal/x86/WNGeneralOpZero_SSE2-NoAVX.h"
#endif

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_ZERO_SSE2_H__