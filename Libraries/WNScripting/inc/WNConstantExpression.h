// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_CONSTANT_EXPRESSION_H__
#define __WN_CONSTANT_EXPRESSION_H__

#include "WNScripting/inc/WNExpression.h"
#include "WNScripting/inc/WNScriptingEnums.h"
namespace WNScripting {
    class WNConstantExpression : public WNExpression {
    public:
        WNConstantExpression(WNScriptTypeName _type, const char* _text);
        virtual ~WNConstantExpression();
        virtual eWNTypeError GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog); 
    private:
        WNScriptTypeName mTypeName;
        char* mText;
    };
}
#endif//__WN_CONSTANT_EXPRESSION_H__

