// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_SCRIPTING_SCOPED_VARIABLE_LIST_H__
#define __WN_SCRIPTING_SCOPED_VARIABLE_LIST_H__
#include "WNCore/inc/WNBase.h"
#include "WNDeprecatedScripting/inc/WNTypeElement.h"
#include "WNDeprecatedScripting/inc/WNScriptingErrors.h"
namespace llvm {
    class Value;
};
namespace WNLogging {
    class WNLog;
};
namespace WNScripting {
    class WNCodeModule;
    struct WNFunctionDefinition;
    class WNScriptVariable {
        public:
        WNScriptVariable(WNScriptType _type, const char* _name, llvm::Value* _alloca, bool _dontClean = false):
            mType(_type),
            mLlvmAlloca(_alloca),
            mDontClean(_dontClean) {
                mName = wn::memory::strndup(_name, 256);
        }
        virtual ~WNScriptVariable() {
            if(mName != nullptr) {
                wn::memory::destroy(mName);
            }
        }
        const WNScriptType GetType() const { return(mType); }
        const char* GetName() const { return(mName); }
        llvm::Value* GetLocation() const { return(mLlvmAlloca); }
        bool Clean() const { return(!mDontClean); }
    private:
        WNScriptType mType;
        char* mName;
        llvm::Value* mLlvmAlloca;
        bool mDontClean;
    };

    class WNScopedVariableList {
    public:
        virtual ~WNScopedVariableList(){}
        virtual eWNTypeError PushVariable(WNScriptVariable* _variable) = 0;
        virtual void PushScopeBlock(WNCodeModule& _module) = 0;
        virtual eWNTypeError PopScopeBlock(WNCodeModule& _module, bool _cleanStack, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) = 0;
        virtual eWNTypeError GenerateReturn(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) = 0;
        virtual const WNScriptVariable* GetVariable(const char* _functionName) = 0;
        virtual void ClearScope() = 0;
    };
}
#endif // WN_SCOPED_VARIABLE_LIST_H__
