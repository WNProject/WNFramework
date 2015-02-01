// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_ARRAY_ACCESS_EXPR_H__
#define __WN_ARRAY_ACCESS_EXPR_H__

#include "WNScripting/inc/WNPostExpression.h"
namespace WNScripting {
    class WNArrayAccessExpr : public WNPostExpression {
    public:
        WNArrayAccessExpr(WNExpression* _expr);
        virtual ~WNArrayAccessExpr();
        virtual eWNTypeError GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog); 
    protected:
        WNExpression* mArrayAccess;
    };
}
#endif//__WN_ARRAY_ACCESS_EXPR_H__
