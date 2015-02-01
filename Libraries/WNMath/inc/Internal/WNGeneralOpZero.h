// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_GENERAL_OP_ZERO_H__
#define __WN_MATH_INTERNAL_GENERAL_OP_ZERO_H__

#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNTypeTraits.h"
#include "WNMath/inc/Internal/WNElements.h"

namespace WNMath {
    namespace __WNInternal {
        template <typename Type, WN_UINT32 Dimension>
        class __WNGeneralHelperZero {
            WN_STATIC_ASSERT_DESC(Dimension > 0, "Must have a dimension of at least 1");

        public:
            static WN_FORCE_INLINE __WNElements<Type, Dimension>& Execute(__WNElements<Type, Dimension>& _elements) {
                for (WN_UINT32 i = 0; i < Dimension; ++i) {
                    _elements.mValues[i] = static_cast<Type>(0);
                }

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperZero() {}
        };

        template <typename Type>
        class __WNGeneralHelperZero<Type, 1> {
        public:
            static WN_FORCE_INLINE __WNElements<Type, 1>& Execute(__WNElements<Type, 1>& _elements) {
                _elements.mValues[0] = static_cast<Type>(0);

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperZero() {}
        };

        template <typename Type>
        class __WNGeneralHelperZero<Type, 2> {
        public:
            static WN_FORCE_INLINE __WNElements<Type, 2>& Execute(__WNElements<Type, 2>& _elements) {
                _elements.mValues[0] = static_cast<Type>(0);
                _elements.mValues[1] = static_cast<Type>(0);

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperZero() {}
        };

        template <typename Type>
        class __WNGeneralHelperZero<Type, 3> {
        public:
            static WN_FORCE_INLINE __WNElements<Type, 3>& Execute(__WNElements<Type, 3>& _elements) {
                _elements.mValues[0] = static_cast<Type>(0);
                _elements.mValues[1] = static_cast<Type>(0);
                _elements.mValues[2] = static_cast<Type>(0);

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperZero() {}
        };

        template <typename Type>
        class __WNGeneralHelperZero<Type, 4> {
        public:
            static WN_FORCE_INLINE __WNElements<Type, 4>& Execute(__WNElements<Type, 4>& _elements) {
                _elements.mValues[0] = static_cast<Type>(0);
                _elements.mValues[1] = static_cast<Type>(0);
                _elements.mValues[2] = static_cast<Type>(0);
                _elements.mValues[3] = static_cast<Type>(0);

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralHelperZero() {}
        };

        template <typename Type, WN_UINT32 Dimension, typename = WNCore::WNEnableWhen<WN_TRUE>::Value>
        class __WNGeneralOpZero {
        public:
            static WN_FORCE_INLINE __WNElements<Type, Dimension>& Execute(__WNElements<Type, Dimension>& _elements) {
                return(__WNGeneralHelperZero<Type, Dimension>::Execute(_elements));
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpZero() {}
        };
    }
}

#include "WNMath/inc/WNConfig.h"

#ifdef __WN_MATH_EXTENSIONS_ENABLED
    #ifdef _WN_X86
        #include "WNMath/inc/Internal/x86/WNGeneralOpZero.h"
    #endif
#endif

#endif // __WN_MATH_INTERNAL_GENERAL_OP_ZERO_H__
