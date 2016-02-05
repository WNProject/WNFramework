// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_EXPRESSION_H__
#define __WN_EXPRESSION_H__
#include "WNDeprecatedScripting/inc/WNNode.h"
#include "WNCore/inc/WNTypes.h"
#include "WNDeprecatedScripting/inc/WNScriptingErrors.h"
#include "WNDeprecatedScripting/inc/WNTypeElement.h"
#include "WNLogging/inc/WNLog.h"

namespace llvm {
    class Value;
}
namespace WNScripting {
    class WNCodeModule;
    struct WNFunctionDefinition;
    class WNExpression : public WNNode {
    public:
        WNExpression();
        virtual ~WNExpression();
        llvm::Value* GetValue() const;

        llvm::Value* GetValueLocation() const;

        WNScripting::WNScriptType GetType() const;

        bool RequiredUse();
        bool IsNewlyCreated();

        WNScripting::WNScriptType GetSubType() const;
        llvm::Value* GetSubValue() const;

        virtual eWNTypeError GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) = 0;
    protected:
        llvm::Value* mValue;
        llvm::Value* mValueLocation;
        llvm::Value* mSubValue;
        WNScriptType mSubValueType;
        WNScriptType mScriptType;
        bool mForceUse;
        bool mNewlyCreated;
    };
}

#endif//__WN_EXPRESSION_H__

