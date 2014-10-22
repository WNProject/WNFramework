// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_ADD_NUMBER_SSE2_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_ADD_NUMBER_SSE2_H__

#ifndef __WN_SSE2_AVAILABLE
    #error "This header requires a minimum of SSE2 to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpAddNumber<WN_INT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 2)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT32, Dimension>& Execute(__WNElements<WN_INT32, Dimension>& _elements, const WN_INT32& _number) {
                _elements.mXMMValues[0] = _mm_add_epi32(_elements.mXMMValues[0], _mm_setr_epi32(_number, _number, 0, 0));

                return(_elements);
            }

        private:
            __WNGeneralOpAddNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpAddNumber<WN_INT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT32, Dimension>& Execute(__WNElements<WN_INT32, Dimension>& _elements, const WN_INT32& _number) {
                _elements.mXMMValues[0] = _mm_add_epi32(_elements.mXMMValues[0], _mm_setr_epi32(_number, _number, _number, 0));

                return(_elements);
            }

        private:
            __WNGeneralOpAddNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpAddNumber<WN_INT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 4)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT32, Dimension>& Execute(__WNElements<WN_INT32, Dimension>& _elements, const WN_INT32& _number) {
                _elements.mXMMValues[0] = _mm_add_epi32(_elements.mXMMValues[0], _mm_set1_epi32(_number));

                return(_elements);
            }

        private:
            __WNGeneralOpAddNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpAddNumber<WN_INT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 2)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_INT64, Dimension>& Execute(__WNElements<WN_INT64, Dimension>& _elements, const WN_INT64& _number) {
                _elements.mXMMValues[0] = _mm_add_epi64(_elements.mXMMValues[0], _mm_set1_epi64x(_number));

                return(_elements);
            }

        private:
            __WNGeneralOpAddNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpAddNumber<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 1)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements, const WN_FLOAT64& _number) {
                _elements.mXMMValues[0] = _mm_add_sd(_elements.mXMMValues[0], _mm_load_sd(&_number));

                return(_elements);
            }

        private:
            __WNGeneralOpAddNumber() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpAddNumber<WN_FLOAT64, Dimension, typename WNCore::WNEnableWhen<(Dimension == 2)>::Value> {
        public:
            static WN_FORCE_INLINE __WNElements<WN_FLOAT64, Dimension>& Execute(__WNElements<WN_FLOAT64, Dimension>& _elements, const WN_FLOAT64& _number) {
                _elements.mXMMValues[0] = _mm_add_pd(_elements.mXMMValues[0], _mm_set1_pd(_number));

                return(_elements);
            }

        private:
            __WNGeneralOpAddNumber() {}
        };
    }
}

#ifndef __WN_AVX_AVAILABLE
    #include "WNMath/inc/Internal/x86/WNGeneralOpAddNumber_SSE2-NoAVX.h"
#endif

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_ADD_NUMBER_SSE2_H__