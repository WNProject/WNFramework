// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_SCRIPTING_SCOPED_VARIABLE_LIST_IMPL_H__
#define __WN_SCRIPTING_SCOPED_VARIABLE_LIST_IMPL_H__
#include "WNDeprecatedScripting/inc/WNScopedVariableList.h"
#include <vector>
namespace WNLogging{
    class WNLog;
};
namespace WNScripting {
    class WNScopedVariableListImpl : public WNScopedVariableList {
    public:
        virtual ~WNScopedVariableListImpl();
        virtual eWNTypeError PushVariable(WNScriptVariable* _variable);
        virtual wn_void PushScopeBlock(WNCodeModule& _module);
        virtual eWNTypeError PopScopeBlock(WNCodeModule& _module, wn_bool _cleanStack, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);
        virtual eWNTypeError GenerateReturn(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);
        virtual const WNScriptVariable* GetVariable(const wn_char* _functionName);
        virtual wn_void ClearScope();
    private:
        std::vector<WNScriptVariable*> mScriptVariables;
        std::vector<llvm::Value*> mScopeBlocks;
    };
}

#endif//__WN_SCRIPTING_SCOPED_VARIABLE_LIST_IMPL_H__
