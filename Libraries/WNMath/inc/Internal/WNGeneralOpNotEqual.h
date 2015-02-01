// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_GENERAL_OP_NOT_EQUAL_H__
#define __WN_MATH_INTERNAL_GENERAL_OP_NOT_EQUAL_H__

#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNTypeTraits.h"
#include "WNMath/inc/Internal/WNElements.h"

namespace WNMath {
    namespace __WNInternal {
        template <typename Type, WN_UINT32 Dimension>
        class __WNGeneralHelperNotEqual {
            WN_STATIC_ASSERT_DESC(Dimension > 0, "Must have a dimension of at least 1");

        public:
            static WN_FORCE_INLINE WN_BOOL Execute(__WNElements<Type, Dimension>& _elements1, const __WNElements<Type, Dimension>& _elements2) {
                for (WN_UINT32 i = 0; i < Dimension; ++i) {
                    if (_elements1.mValues[i] != _elements2.mValues[i]) {
                        return(WN_TRUE);
                    }
                }

                return(WN_FALSE);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperNotEqual() {}
        };

        template <typename Type>
        class __WNGeneralHelperNotEqual<Type, 1> {
        public:
            static WN_FORCE_INLINE WN_BOOL Execute(__WNElements<Type, 1>& _elements1, const __WNElements<Type, 1>& _elements2) {
                return(_elements1.mValues[0] != _elements2.mValues[0]);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperNotEqual() {}
        };

        template <typename Type>
        class __WNGeneralHelperNotEqual<Type, 2> {
        public:
            static WN_FORCE_INLINE WN_BOOL Execute(__WNElements<Type, 2>& _elements1, const __WNElements<Type, 2>& _elements2) {
                return(_elements1.mValues[0] != _elements2.mValues[0] || _elements1.mValues[1] != _elements2.mValues[1]);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperNotEqual() {}
        };

        template <typename Type>
        class __WNGeneralHelperNotEqual<Type, 3> {
        public:
            static WN_FORCE_INLINE WN_BOOL Execute(__WNElements<Type, 3>& _elements1, const __WNElements<Type, 3>& _elements2) {
                return(_elements1.mValues[0] != _elements2.mValues[0] || _elements1.mValues[1] != _elements2.mValues[1] ||
                       _elements1.mValues[2] != _elements2.mValues[2]);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperNotEqual() {}
        };

        template <typename Type>
        class __WNGeneralHelperNotEqual<Type, 4> {
        public:
            static WN_FORCE_INLINE WN_BOOL Execute(__WNElements<Type, 4>& _elements1, const __WNElements<Type, 4>& _elements2) {
                return(_elements1.mValues[0] != _elements2.mValues[0] || _elements1.mValues[1] != _elements2.mValues[1] ||
                       _elements1.mValues[2] != _elements2.mValues[2] || _elements1.mValues[3] != _elements2.mValues[3]);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperNotEqual() {}
        };

        template <typename Type, WN_UINT32 Dimension, typename = WNCore::WNEnableWhen<WN_TRUE>::Value>
        class __WNGeneralOpNotEqual {
            WN_STATIC_ASSERT_DESC(Dimension > 0, "Must have a dimension of at least 1");

        public:
            static WN_FORCE_INLINE WN_BOOL Execute(__WNElements<Type, Dimension>& _elements1, const __WNElements<Type, Dimension>& _elements2) {
                return(__WNGeneralHelperNotEqual<Type, Dimension>::Execute(_elements1, _elements2));
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpNotEqual() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_GENERAL_OP_NOT_EQUAL_H__