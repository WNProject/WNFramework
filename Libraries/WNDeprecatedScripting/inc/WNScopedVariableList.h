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
        WNScriptVariable(WNScriptType _type, const wn_char* _name, llvm::Value* _alloca, wn_bool _dontClean = false):
            mType(_type),
            mLlvmAlloca(_alloca),
            mDontClean(_dontClean) {
                mName = wn::memory::strndup(_name, 256);
        }
        virtual ~WNScriptVariable() {
            if(mName != wn_nullptr) {
                wn::memory::destroy(mName);
            }
        }
        const WNScriptType GetType() const { return(mType); }
        const wn_char* GetName() const { return(mName); }
        llvm::Value* GetLocation() const { return(mLlvmAlloca); }
        wn_bool Clean() const { return(!mDontClean); }
    private:
        WNScriptType mType;
        wn_char* mName;
        llvm::Value* mLlvmAlloca;
        wn_bool mDontClean;
    };

    class WNScopedVariableList {
    public:
        virtual ~WNScopedVariableList(){}
        virtual eWNTypeError PushVariable(WNScriptVariable* _variable) = 0;
        virtual wn_void PushScopeBlock(WNCodeModule& _module) = 0;
        virtual eWNTypeError PopScopeBlock(WNCodeModule& _module, wn_bool _cleanStack, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) = 0;
        virtual eWNTypeError GenerateReturn(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) = 0;
        virtual const WNScriptVariable* GetVariable(const wn_char* _functionName) = 0;
        virtual wn_void ClearScope() = 0;
    };
}
#endif // WN_SCOPED_VARIABLE_LIST_H__
