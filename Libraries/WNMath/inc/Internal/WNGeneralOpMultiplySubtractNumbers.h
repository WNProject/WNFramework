// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_GENERAL_OP_MULTIPLY_SUBTRACT_NUMBERS_H__
#define __WN_MATH_INTERNAL_GENERAL_OP_MULTIPLY_SUBTRACT_NUMBERS_H__

#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNTypeTraits.h"
#include "WNMath/inc/Internal/WNElements.h"

namespace WNMath {
    namespace __WNInternal {
        template <typename Type, WN_UINT32 Dimension, typename = WNCore::WNEnableWhen<WN_TRUE>::Value>
        class __WNGeneralOpMultiplySubtractNumbers {
            WN_STATIC_ASSERT_DESC(Dimension > 0, "Must have a dimension of at least 1");

        public:
            static WN_FORCE_INLINE __WNElements<Type, Dimension>& Execute(__WNElements<Type, Dimension>& _elements, const Type& _number1, const Type& _number2) {
                for (WN_UINT32 i = 0; i < Dimension; ++i) {
                    _elements.mValues[i] *= _number1;
                    _elements.mValues[i] = (_number2 - _elements.mValues[i]);
                }

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpMultiplySubtractNumbers() {}
        };
    }
}

#include "WNMath/inc/WNConfig.h"

#ifdef __WN_MATH_EXTENSIONS_ENABLED
    #ifdef _WN_X86
        #include "WNMath/inc/Internal/x86/WNGeneralOpMultiplySubtractNumbers.h"
    #endif
#endif

#endif // __WN_MATH_INTERNAL_GENERAL_OP_MULTIPLY_SUBTRACT_NUMBERS_H__
