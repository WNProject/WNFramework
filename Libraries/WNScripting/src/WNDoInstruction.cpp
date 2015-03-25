// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNDoInstruction.h"
#include "WNScripting/inc/WNCodeModule.h"
#include "WNScripting/inc/WNInstructionList.h"
#include "WNScripting/inc/WNTypeManager.h"
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
    if(ok != (err = _module.GetTypeManager().GetTypeByName("Bool", boolType))) {
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


