// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_POST_UN_EXPRESSION_H__
#define __WN_POST_UN_EXPRESSION_H__

#include "WNDeprecatedScripting/inc/WNPostExpression.h"
#include "WNDeprecatedScripting/inc/WNScriptingEnums.h"
namespace WNScripting {
    class WNPostUNExpression : public WNPostExpression {
    public:
        WNPostUNExpression(WNPostUNType _type);
        virtual ~WNPostUNExpression();
        virtual eWNTypeError GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);
    private:
        WNPostUNType mType;

    };
}
#endif//__WN_POST_UN_EXPRESSION_H__
