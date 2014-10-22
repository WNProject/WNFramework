// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_SCRIPTING_SCOPED_VARIABLE_LIST_IMPL_H__
#define __WN_SCRIPTING_SCOPED_VARIABLE_LIST_IMPL_H__
#include "WNScripting/inc/WNScopedVariableList.h"
#include <vector>
namespace WNLogging{
    class WNLog;
};
namespace WNScripting {
    class WNScopedVariableListImpl : public WNScopedVariableList {
    public:
        virtual ~WNScopedVariableListImpl();
        virtual eWNTypeError PushVariable(WNScriptVariable* _variable);
        virtual WN_VOID PushScopeBlock(WNCodeModule& _module);
        virtual eWNTypeError PopScopeBlock(WNCodeModule& _module, WN_BOOL _cleanStack, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);
        virtual eWNTypeError GenerateReturn(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);
        virtual const WNScriptVariable* GetVariable(const WN_CHAR* _functionName);
        virtual WN_VOID ClearScope();
    private:
        std::vector<WNScriptVariable*> mScriptVariables; 
        std::vector<llvm::Value*> mScopeBlocks;
    };
}

#endif//__WN_SCRIPTING_SCOPED_VARIABLE_LIST_IMPL_H__
