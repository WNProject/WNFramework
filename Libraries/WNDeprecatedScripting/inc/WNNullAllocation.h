// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_NULL_ALLOCATION_H__
#define __WN_NULL_ALLOCATION_H__
#include "WNDeprecatedScripting/inc/WNExpression.h"

namespace WNScripting {
    class WNNullAllocation : public WNExpression {
    public:
        WNNullAllocation();
        virtual ~WNNullAllocation();
        eWNTypeError GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);
    private:
    };
}
#endif//__WN_NULL_ALLOCATION_H__
