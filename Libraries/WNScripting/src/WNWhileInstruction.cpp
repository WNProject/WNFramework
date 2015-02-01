// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNWhileInstruction.h"
#include "WNScripting/inc/WNCodeModule.h"
#include "WNScripting/inc/WNTypeManager.h"
#include "WNScripting/inc/WNInstructionList.h"
#include "WNScripting/inc/WNExpression.h"

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

WNWhileInstruction::WNWhileInstruction(WNExpression* _cond, WNInstructionList* _body):
    mCondition(_cond),
    mBody(_body) {
}

WNWhileInstruction::~WNWhileInstruction() {
    WN_DELETE(mCondition);
    WN_DELETE(mBody);
}

eWNTypeError WNWhileInstruction::GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
    eWNTypeError err = ok;

    llvm::BasicBlock* conditionBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
    llvm::BasicBlock* bodyBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
    llvm::BasicBlock* endBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
    
    builder->CreateBr(conditionBlock);
    builder->SetInsertPoint(conditionBlock);
    if(ok != (err = mCondition->GenerateCode(_module, _def, _compilationLog))) {
        return(err);
    }
    WNScriptType boolType = wn_nullptr;
    if(ok != (err = _module.GetTypeManager().GetTypeByName("Bool", boolType))) {
        _compilationLog.Log(WNLogging::eCritical, 0, "Error cannot find Bool type");
        LogLine(_compilationLog, WNLogging::eCritical);
        return(err);
    }

    if(mCondition->GetType() != boolType) {
        _compilationLog.Log(WNLogging::eError, 0, "Conditional type must be boolean ");
        LogLine(_compilationLog, WNLogging::eError);
        return(eWNBadType);
    }

    llvm::Value* v = mCondition->GetValue();
    llvm::Value* cv = llvm::ConstantInt::get(boolType->mLLVMType, 0);
    v = builder->CreateICmpNE(cv, v, "");
    builder->CreateCondBr(v, bodyBlock, endBlock);
    builder->SetInsertPoint(bodyBlock);
    if(ok != (err = mBody->GenerateCode(_module, _def, _compilationLog))) {
        return(err);
    }
    if(!mBody->Returns()) {
        builder->CreateBr(conditionBlock);
    } else {
        mReturns = false;
    }
    builder->SetInsertPoint(endBlock);

    return(ok);
}

