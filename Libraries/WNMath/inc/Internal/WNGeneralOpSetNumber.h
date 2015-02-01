// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_GENERAL_OP_SET_NUMBER_H__
#define __WN_MATH_INTERNAL_GENERAL_OP_SET_NUMBER_H__

#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNTypeTraits.h"
#include "WNMath/inc/Internal/WNElements.h"

namespace WNMath {
    namespace __WNInternal {
        template <typename Type, WN_UINT32 Dimension, typename = WNCore::WNEnableWhen<WN_TRUE>::Value>
        class __WNGeneralOpSetNumber {
        public:
            static WN_FORCE_INLINE __WNElements<Type, Dimension>& Execute(__WNElements<Type, Dimension>& _elements, const Type& _number) {
                for (WN_UINT32 i = 0; i < Dimension; ++i) {
                    _elements.mValues[i] = _number;
                }

                return(_elements);
            }

        private:
            WN_FORCE_INLINE __WNGeneralOpSetNumber() {}
        };
    }
}

#endif // __WN_MATH_INTERNAL_GENERAL_OP_SET_NUMBER_H__
