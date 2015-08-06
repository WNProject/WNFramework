// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


#include "WNDeprecatedScripting/inc/WNIncludeLLVM.h"
#include "llvm/IR/IRBuilder.h"
#include "WNDeprecatedScripting/inc/WNEndIncludeLLVM.h"

#include "WNDeprecatedScripting/inc/WNForInstruction.h"
#include "WNDeprecatedScripting/inc/WNExpression.h"
#include "WNDeprecatedScripting/inc/WNCodeModule.h"
#include "WNDeprecatedScripting/inc/WNTypeManager.h"
#include "WNDeprecatedScripting/inc/WNScopedVariableList.h"
#include "WNDeprecatedScripting/inc/WNInstructionList.h"

using namespace WNScripting;

WNForInstruction::WNForInstruction() :
    minitializer(wn_nullptr),
    mCondition(wn_nullptr),
    mPostOp(wn_nullptr),
    mBody(wn_nullptr) {
}

WNForInstruction::~WNForInstruction() {
    if(minitializer) {
        wn::memory::destroy(minitializer);
    }
    if(mCondition) {
        wn::memory::destroy(mCondition);
    }
    if(mPostOp) {
        wn::memory::destroy(mPostOp);
    }
    if(mBody) {
        wn::memory::destroy(mBody);
    }
}

void WNForInstruction::AddInitializer(WNInstruction* _init) {
    minitializer = _init;
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
    if(minitializer) {
        if(ok != (err = minitializer->GenerateCode(_module, _def, _compilationLog))) {
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
        if(ok != (err = _module.GetTypeManager().get_type_by_name("Bool", boolType))) {
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

