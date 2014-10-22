// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNCondExpression.h"
#include "WNScripting/inc/WNCodeModule.h"
#include "WNScripting/inc/WNTypeManager.h"
#include "WNMemory/inc/WNAllocation.h"

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
#include "llvm/IR/BasicBlock.h"

#ifdef _WN_MSVC
    #pragma warning(pop)
#endif

using namespace WNScripting; 

WNCondExpression::WNCondExpression(WNExpression* _cond, WNExpression* _lhs, WNExpression* _rhs):
    mCondition(_cond),
    mLHS(_lhs),
    mRHS(_rhs){
}

WNCondExpression::~WNCondExpression() {
    WN_DELETE(mLHS);
    WN_DELETE(mRHS);
}

eWNTypeError WNCondExpression::GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
    eWNTypeError err = eWNOK;
    llvm::BasicBlock* endBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
    llvm::BasicBlock* lBBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
    llvm::BasicBlock* rBBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);

    
    if(eWNOK != (err = mCondition->GenerateCode(_module, _def, _compilationLog))) {
        return(err);
    }
    WNScriptType boolType = WN_NULL;
    if(eWNOK != (err = _module.GetTypeManager().GetTypeByName("Bool", boolType))) {
        _compilationLog.Log(WNLogging::eCritical, 0, "Cannot find Bool Type");
        LogLine(_compilationLog, WNLogging::eCritical);
        return(err);
    }

    if(mCondition->GetType() != boolType) {
        _compilationLog.Log(WNLogging::eError, 0, "Conditional expression must be a boolean");
        LogLine(_compilationLog, WNLogging::eError);
        return(eWNBadType);
    }

    llvm::Value* v = mCondition->GetValue();
    llvm::Value* cv = llvm::ConstantInt::get(boolType->mLLVMType, 0);
    v = builder->CreateICmpNE(cv, v, "");
    builder->CreateCondBr(v, lBBlock, rBBlock);
    builder->SetInsertPoint(lBBlock);
    if(eWNOK != (err = mLHS->GenerateCode(_module, _def, _compilationLog))) {
        return(err);
    }
    lBBlock = builder->GetInsertBlock();
    llvm::Value* lhsValue = mLHS->GetValue();
    builder->CreateBr(endBlock);
    builder->SetInsertPoint(rBBlock);
    if(eWNOK != (err = mRHS->GenerateCode(_module, _def, _compilationLog))) {
        return(err);
    }
    llvm::Value* rhsValue = mRHS->GetValue();
    if(mLHS->GetType() != mRHS->GetType()) {
        const GenerateCastingOperation* castingOp = _module.GetTypeManager().GetCastingOperation(mLHS->GetType(), mRHS->GetType());
        if(!castingOp){
            _compilationLog.Log(WNLogging::eError, 0, "Cannot cast from ", mLHS->GetType()->mName, " to ", mRHS->GetType()->mName);
            LogLine(_compilationLog, WNLogging::eError);
            return(eWNInvalidCast);
        }
        if(eWNOK != (err = castingOp->Execute(builder, mRHS->GetValue(), rhsValue))) {
            _compilationLog.Log(WNLogging::eCritical, 0, "Error generating cast from ", mLHS->GetType()->mName, " to ", mRHS->GetType()->mName);
            LogLine(_compilationLog, WNLogging::eCritical);
            return(eWNInvalidCast);
        }
    }
    rBBlock = builder->GetInsertBlock();
    builder->CreateBr(endBlock);
    builder->SetInsertPoint(endBlock);
    
    llvm::PHINode* phi = builder->CreatePHI(mLHS->GetType()->mLLVMType, 2, "");
    phi->addIncoming(lhsValue, lBBlock);
    phi->addIncoming(rhsValue, rBBlock);
    mValue = phi;
    mScriptType = mLHS->GetType();
    return(eWNOK);
}
