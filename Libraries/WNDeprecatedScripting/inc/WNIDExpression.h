// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_ID_EXPRESSION_H__
#define __WN_ID_EXPRESSION_H__
#include "WNDeprecatedScripting/inc/WNExpression.h"
namespace WNScripting {
    class WNIDExpression : public WNExpression {
    public:
        WNIDExpression(const char* _name);
        virtual ~WNIDExpression();
        virtual eWNTypeError GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);
    private:
        char* mName;
    };
}
#endif//__WN_ID_EXPRESSION_H__
