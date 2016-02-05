// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_PARAMETER_H__
#define __WN_PARAMETER_H__

#include "WNCore/inc/WNTypes.h"
#include "WNDeprecatedScripting/inc/WNTypeElement.h"

namespace WNScripting {
    struct WNParameter {
        WNParameter(WNScriptType _type) :
            mType(_type) {
        }
        WNParameter() :
            mType(nullptr) {
        }
        WNScriptType mType;

        union {
            size_t mSizetVal;
            uint32_t mUint32Val;
            int32_t mInt32Val;
            int64_t mInt64val;
            float mFloatVal;
            double mDoubleVal;
        };
    };
}
#endif//__WN_PARAMETER_H__