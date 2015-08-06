// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_COND_EXPRESSION_H__
#define __WN_COND_EXPRESSION_H__

#include "WNDeprecatedScripting/inc/WNExpression.h"
namespace WNScripting {
    class WNCondExpression : public WNExpression {
    public:
        WNCondExpression(WNExpression* _cond, WNExpression* _lhs, WNExpression* _rhs);
        virtual ~WNCondExpression();
        virtual eWNTypeError GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);

    private:
        WNExpression* mCondition;
        WNExpression* mLHS;
        WNExpression* mRHS;
    };
}
#endif//__WN_COND_EXPRESSION_H__
