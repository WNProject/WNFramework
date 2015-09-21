// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_LVALUE_H__
#define __WN_LVALUE_H__

#include "WNDeprecatedScripting/inc/WNNode.h"
#include "WNDeprecatedScripting/inc/WNTypeElement.h"
#include "WNDeprecatedScripting/inc/WNScriptingErrors.h"
#include "WNLogging/inc/WNLog.h"

namespace llvm {
    class Value;
}

namespace WNScripting {
    class WNExpression;
    class WNCodeModule;
    struct WNFunctionDefinition;
    class WNLValue : public WNNode {
    public:

        WNLValue(WNExpression* _expr);
        virtual ~WNLValue();
        virtual eWNTypeError GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);

        llvm::Value* GetValueLocation();

        WNScriptType GetValueType();

        wn_bool RequiredUse();

    private:
        WNExpression* mExpression;
    };
}
#endif//__WN_LVALUE_H__
