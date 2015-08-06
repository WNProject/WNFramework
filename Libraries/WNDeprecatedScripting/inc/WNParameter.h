// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
            mType(wn_nullptr) {
        }
        WNScriptType mType;

        union {
            wn_size_t mSizetVal;
            wn_uint32 mUint32Val;
            wn_int32 mInt32Val;
            wn_int64 mInt64val;
            wn_float32 mFloatVal;
            wn_float64 mDoubleVal;
        };
    };
}
#endif//__WN_PARAMETER_H__