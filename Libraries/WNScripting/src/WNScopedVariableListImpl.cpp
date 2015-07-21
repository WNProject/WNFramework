// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


#include "WNScripting/inc/WNIncludeLLVM.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Function.h"
#include "WNScripting/inc/WNEndIncludeLLVM.h"

#include "WNScripting/inc/WNScopedVaraiableListImpl.h"
#include "WNScripting/inc/WNCodeModule.h"
#include "WNScripting/inc/WNTypeManager.h"
#include "WNCore/inc/WNAssert.h"

using namespace WNScripting;

WNScopedVariableListImpl::~WNScopedVariableListImpl() {
}

eWNTypeError WNScopedVariableListImpl::PushVariable(WNScriptVariable* _variable) {
    WN_DEBUG_ASSERT(_variable != wn_nullptr);
    if(!mScriptVariables.empty()) {
        for(wn_int64 i = mScriptVariables.size() - 1; i >= 0; --i) {
            if(!mScriptVariables[static_cast<wn_size_t>(i)]) {
                break;
            }
            if(0 == WNStrings::WNStrNCmp(_variable->GetName(), mScriptVariables[static_cast<wn_size_t>(i)]->GetName(), 256)) {
                return(eWNAlreadyExists);
            }
        }
    }


    mScriptVariables.push_back(_variable);
    return(ok);
}

wn_void WNScopedVariableListImpl::PushScopeBlock(WNCodeModule& _module) {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>* >(_module.GetBuilder());
    llvm::Function* fnc = llvm::Intrinsic::getDeclaration(_module.GetModule(), llvm::Intrinsic::stacksave);
    mScopeBlocks.push_back(builder->CreateCall(fnc, llvm::ArrayRef<llvm::Value*>(), ""));
    mScriptVariables.push_back(wn_nullptr);
}

eWNTypeError WNScopedVariableListImpl::GenerateReturn(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) {
    eWNTypeError err = ok;
    if(mScriptVariables.empty()) {
        return(ok);
    }
    for(wn_int64 i = mScriptVariables.size() - 1; i >= 0; --i) {
        if(mScriptVariables[static_cast<wn_size_t>(i)] == wn_nullptr || !mScriptVariables[static_cast<wn_size_t>(i)]->Clean()) {
            continue;
        }

        const GenerateDestruction* dest = _module.GetTypeManager().GetDestructionOperation(mScriptVariables[static_cast<wn_size_t>(i)]->GetType());
        if(dest) {
            if(ok != (err = dest->Execute(_module, mScriptVariables[static_cast<wn_size_t>(i)]->GetType(), mScriptVariables[static_cast<wn_size_t>(i)]->GetLocation(), _def, _compilationLog)))
            {
                return(err);
            }
        }
    }
    return(ok);
}

wn_void WNScopedVariableListImpl::ClearScope() {
    if(mScriptVariables.empty()) {
        return;
    }
    for(wn_int32 i = static_cast<wn_int32>(mScriptVariables.size()) - 1; i >= 0; --i) {
        if(mScriptVariables[wn_size_t(i)] == wn_nullptr) {
            mScriptVariables.pop_back();
            continue;
        }
        wn::memory::destroy(mScriptVariables[i]);
        mScriptVariables.pop_back();
    }
}

eWNTypeError WNScopedVariableListImpl::PopScopeBlock(WNCodeModule& _module, wn_bool _cleanStack, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) {
    eWNTypeError err = ok;

    WN_DEBUG_ASSERT(!mScriptVariables.empty() && !mScopeBlocks.empty());
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>* >(_module.GetBuilder());
    for(wn_int32 i = static_cast<wn_int32>(mScriptVariables.size()) - 1; i >= 0; --i) {
        if(mScriptVariables[static_cast<wn_size_t>(i)] == wn_nullptr) {
            mScriptVariables.pop_back();
            break;
        }
        if(_cleanStack) {
            const GenerateDestruction* dest = _module.GetTypeManager().GetDestructionOperation(mScriptVariables[static_cast<wn_size_t>(i)]->GetType());
            if(dest) {
                if(ok != (err = dest->Execute(_module, mScriptVariables[i]->GetType(), mScriptVariables[static_cast<wn_size_t>(i)]->GetLocation(), _def, _compilationLog))) {
                    return(err);
                }
            }
        }
        wn::memory::destroy(mScriptVariables[static_cast<wn_size_t>(i)]);
        mScriptVariables.pop_back();
    }
    if(_cleanStack) {
        llvm::Function* fnc = llvm::Intrinsic::getDeclaration(_module.GetModule(), llvm::Intrinsic::stackrestore);
        builder->CreateCall(fnc,  mScopeBlocks.back(), "");
    }
    mScopeBlocks.pop_back();
    return(ok);
}

const WNScriptVariable* WNScopedVariableListImpl::GetVariable(const wn_char* _variableName) {
    if(mScriptVariables.empty()) {
        return(wn_nullptr);
    }
    for(wn_int64 i = mScriptVariables.size() - 1; i >= 0; --i) {
        if(mScriptVariables[static_cast<wn_size_t>(i)] && (0 == WNStrings::WNStrNCmp(_variableName, mScriptVariables[static_cast<wn_size_t>(i)]->GetName(), 256))) {
            return(mScriptVariables[static_cast<wn_size_t>(i)]);
        }
    }
    return(wn_nullptr);
}
