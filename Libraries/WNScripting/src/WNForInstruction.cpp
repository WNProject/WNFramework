// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNForInstruction.h"
#include "WNScripting/inc/WNExpression.h"
#include "WNScripting/inc/WNCodeModule.h"
#include "WNScripting/inc/WNTypeManager.h"
#include "WNScripting/inc/WNScopedVariableList.h"
#include "WNScripting/inc/WNInstructionList.h"

#ifdef _WN_MSVC
    #pragma warning(push)
    #pragma warning(disable: 4100)
    #pragma warning(disable: 4127)
    #pragma warning(disable: 4152)
    #pragma warning(disable: 4244)
    #pragma warning(disable: 4245)
    #pragma warning(disable: 4267)
    #pragma warning(disable: 4355)
    #pragma warning(disable: 4512)
    #pragma warning(disable: 4800)
#endif

#include "llvm/IR/IRBuilder.h"

#ifdef _WN_MSVC
    #pragma warning(pop)
#endif

using namespace WNScripting;

WNForInstruction::WNForInstruction() :
    mInitializer(wn_nullptr),
    mCondition(wn_nullptr),
    mPostOp(wn_nullptr),
    mBody(wn_nullptr) {
}

WNForInstruction::~WNForInstruction() {
    if(mInitializer) {
        WN_DELETE(mInitializer);
    }
    if(mCondition) {
        WN_DELETE(mCondition);
    }
    if(mPostOp) {
        WN_DELETE(mPostOp);
    }
    if(mBody) {
        WN_DELETE(mBody);
    }
}

void WNForInstruction::AddInitializer(WNInstruction* _init) {
    mInitializer = _init;
}

void WNForInstruction::AddCondition(WNExpression* cond) {
    mCondition = cond;
}

void WNForInstruction::AddPostOp(WNInstruction* _inst) {
    mPostOp = _inst;
}

void WNForInstruction::AddBody(WNInstructionList* _body) {
    mBody = _body;
}

eWNTypeError WNForInstruction::GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
    eWNTypeError err = ok;

    _module.GetScopedVariableList().PushScopeBlock(_module);
    if(mInitializer) {
        if(ok != (err = mInitializer->GenerateCode(_module, _def, _compilationLog))) {
            return(err);
        }
    }
    llvm::BasicBlock* condBlock = wn_nullptr;
    llvm::BasicBlock* bodyBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
    llvm::BasicBlock* endBlock = wn_nullptr;
    if(mCondition) {
        endBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
        condBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
        builder->CreateBr(condBlock);
        builder->SetInsertPoint(condBlock);
        WNScriptType boolType = wn_nullptr;
        if(ok != (err = _module.GetTypeManager().GetTypeByName("Bool", boolType))) {
            _compilationLog.Log(WNLogging::eCritical, 0, "Error cannot find bool type");
            LogLine(_compilationLog, WNLogging::eCritical);
            return(err);
        }
        if(ok != (err = mCondition->GenerateCode(_module, _def, _compilationLog))) {
            return(err);
        }
        llvm::Value* condValue = mCondition->GetValue();
        if(mCondition->GetType() != boolType) {
            const GenerateCastingOperation* castOp = _module.GetTypeManager().GetCastingOperation(mCondition->GetType(), boolType);
            if(!castOp) {
                _compilationLog.Log(WNLogging::eError, 0, "Error cannot cast to Bool from", mCondition->GetType()->mName);
                LogLine(_compilationLog, WNLogging::eError);
                return(eWNInvalidCast);
            }
            if(ok != (err = castOp->Execute(builder, condValue, condValue))) {
                _compilationLog.Log(WNLogging::eCritical, 0, "Error creating cast to Bool from", mCondition->GetType()->mName);
                LogLine(_compilationLog, WNLogging::eCritical);
                return(err);
            }
        }

        llvm::Value* cv = llvm::ConstantInt::get(boolType->mLLVMType, 0);
        condValue = builder->CreateICmpNE(condValue, cv, "");
        builder->CreateCondBr(condValue, bodyBlock, endBlock);
    } else {
        builder->CreateBr(bodyBlock);
    }
    builder->SetInsertPoint(bodyBlock);
    if(ok != (err = mBody->GenerateCode(_module, _def, _compilationLog))) {
        return(err);
    }
    if(mPostOp) {
        if(ok != (err = mPostOp->GenerateCode(_module, _def, _compilationLog))) {
            return(err);
        }
    }
    if(mCondition) {
        builder->CreateBr(condBlock);
        builder->SetInsertPoint(endBlock);
    } else {
        builder->CreateBr(bodyBlock);
    }
    mReturns = mCondition == NULL;
    _module.GetScopedVariableList().PopScopeBlock(_module, mCondition != NULL, _def, _compilationLog);
    return(ok);
}

