// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNDeprecatedScripting/inc/WNIncludeLLVM.h"
#include "llvm/IR/IRBuilder.h"
#include "WNDeprecatedScripting/inc/WNEndIncludeLLVM.h"

#include "WNDeprecatedScripting/inc/WNDoInstruction.h"
#include "WNDeprecatedScripting/inc/WNCodeModule.h"
#include "WNDeprecatedScripting/inc/WNInstructionList.h"
#include "WNDeprecatedScripting/inc/WNTypeManager.h"
#include "WNDeprecatedScripting/inc/WNExpression.h"

using namespace WNScripting;

WNDoInstruction::WNDoInstruction(WNExpression* _cond, WNInstructionList* _body) :
    mCondition(_cond),
    mBody(_body) {
}

WNDoInstruction::~WNDoInstruction() {
    wn::memory::destroy(mCondition);
    wn::memory::destroy(mBody);
}

eWNTypeError WNDoInstruction::GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
    llvm::BasicBlock* bodyBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
    builder->CreateBr(bodyBlock);
    builder->SetInsertPoint(bodyBlock);
    eWNTypeError err = ok;
    if(ok != (err = mBody->GenerateCode(_module, _def, _compilationLog))) {
        return(err);
    }
    if(mBody->Returns()) {
        mReturns = true;
        return(ok);
    }
    llvm::BasicBlock* endBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
    WNScriptType boolType = wn_nullptr;
    if(ok != (err = _module.GetTypeManager().get_type_by_name("Bool", boolType))) {
        _compilationLog.Log(WNLogging::eCritical, 0, "Error, cannot find Bool type");
        LogLine(_compilationLog, WNLogging::eCritical);
        return(err);
    }
    if(ok != (err = mCondition->GenerateCode(_module, _def, _compilationLog))) {
        return(err);
    }
    if(mCondition->GetType() != boolType) {
        _compilationLog.Log(WNLogging::eError, 0, "Error, conditional must be bool type");
        LogLine(_compilationLog, WNLogging::eError);
        return(eWNBadType);
    }
    llvm::Value* cv = llvm::ConstantInt::get(boolType->mLLVMType, 0);
    llvm::Value* v = mCondition->GetValue();
    v = builder->CreateICmpNE(cv, v, "");
    builder->CreateCondBr(v, bodyBlock, endBlock);
    builder->SetInsertPoint(endBlock);
    return(ok);
}


