// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_CONSTANT_EXPRESSION_H__
#define __WN_CONSTANT_EXPRESSION_H__

#include "WNDeprecatedScripting/inc/WNExpression.h"
#include "WNDeprecatedScripting/inc/WNScriptingEnums.h"
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

