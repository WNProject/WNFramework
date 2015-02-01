// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_NOT_EQUAL_SSE_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_NOT_EQUAL_SSE_H__

#ifndef __WN_SSE_AVAILABLE
    #error "This header requires a minimum of SSE to be enabled in order to work."
#endif

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        class __WNGeneralOpNotEqual<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 1)>::Value> {
        public:
            static WN_FORCE_INLINE WN_BOOL Execute((const __WNElements<WN_FLOAT32, Dimension>& _elements1, const __WNElements<WN_FLOAT32, Dimension>& _elements2) {
                return((_mm_movemask_ps(_mm_cmpeq_ss(_elements1.mXMMValues[0], _elements2.mXMMValues[0])) & 0x1) == 0);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpNotEqual() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpNotEqual<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 2)>::Value> {
        public:
            static WN_FORCE_INLINE WN_BOOL Execute(const __WNElements<WN_FLOAT32, Dimension>& _elements1, const __WNElements<WN_FLOAT32, Dimension>& _elements2) {
                return((_mm_movemask_ps(_mm_cmpeq_ps(_elements1.mXMMValues[0], _elements2.mXMMValues[0])) & 0x3) == 0);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpNotEqual() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpNotEqual<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 3)>::Value> {
        public:
            static WN_FORCE_INLINE WN_BOOL Execute((const __WNElements<WN_FLOAT32, Dimension>& _elements1, const __WNElements<WN_FLOAT32, Dimension>& _elements2) {
                return((_mm_movemask_ps(_mm_cmpeq_ps(_elements1.mXMMValues[0], _elements2.mXMMValues[0])) & 0x7) == 0);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpNotEqual() {}
        };

        template <WN_UINT32 Dimension>
        class __WNGeneralOpNotEqual<WN_FLOAT32, Dimension, typename WNCore::WNEnableWhen<(Dimension == 4)>::Value> {
        public:
            static WN_FORCE_INLINE WN_BOOL Execute((const __WNElements<WN_FLOAT32, Dimension>& _elements1, const __WNElements<WN_FLOAT32, Dimension>& _elements2) {
                return(_mm_movemask_ps(_mm_cmpeq_ps(_elements1.mXMMValues[0], _elements2.mXMMValues[0])) == 0);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpNotEqual() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_NOT_EQUAL_SSE_H__