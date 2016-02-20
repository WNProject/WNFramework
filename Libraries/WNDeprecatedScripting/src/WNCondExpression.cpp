// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

// clang-format off
#include "WNDeprecatedScripting/inc/WNIncludeLLVM.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"
#include "WNDeprecatedScripting/inc/WNEndIncludeLLVM.h"
// clang-format on

#include "WNDeprecatedScripting/inc/WNCodeModule.h"
#include "WNDeprecatedScripting/inc/WNCondExpression.h"
#include "WNDeprecatedScripting/inc/WNTypeManager.h"
#include "WNMemory/inc/WNBasic.h"

using namespace WNScripting;

WNCondExpression::WNCondExpression(
    WNExpression* _cond, WNExpression* _lhs, WNExpression* _rhs)
  : mCondition(_cond), mLHS(_lhs), mRHS(_rhs) {}

WNCondExpression::~WNCondExpression() {
  wn::memory::destroy(mLHS);
  wn::memory::destroy(mRHS);
}

eWNTypeError WNCondExpression::GenerateCode(WNCodeModule& _module,
    const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) {
  llvm::IRBuilder<>* builder =
      reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
  eWNTypeError err = ok;
  llvm::BasicBlock* endBlock =
      llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
  llvm::BasicBlock* lBBlock =
      llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
  llvm::BasicBlock* rBBlock =
      llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);

  if (ok != (err = mCondition->GenerateCode(_module, _def, _compilationLog))) {
    return (err);
  }
  WNScriptType boolType = nullptr;
  if (ok !=
      (err = _module.GetTypeManager().get_type_by_name("Bool", boolType))) {
    _compilationLog.Log(WNLogging::eCritical, 0, "Cannot find Bool type");
    LogLine(_compilationLog, WNLogging::eCritical);
    return (err);
  }

  if (mCondition->GetType() != boolType) {
    _compilationLog.Log(
        WNLogging::eError, 0, "Conditional expression must be a boolean");
    LogLine(_compilationLog, WNLogging::eError);
    return (eWNBadType);
  }

  llvm::Value* v = mCondition->GetValue();
  llvm::Value* cv = llvm::ConstantInt::get(boolType->mLLVMType, 0);
  v = builder->CreateICmpNE(cv, v, "");
  builder->CreateCondBr(v, lBBlock, rBBlock);
  builder->SetInsertPoint(lBBlock);
  if (ok != (err = mLHS->GenerateCode(_module, _def, _compilationLog))) {
    return (err);
  }
  lBBlock = builder->GetInsertBlock();
  llvm::Value* lhsValue = mLHS->GetValue();
  builder->CreateBr(endBlock);
  builder->SetInsertPoint(rBBlock);
  if (ok != (err = mRHS->GenerateCode(_module, _def, _compilationLog))) {
    return (err);
  }
  llvm::Value* rhsValue = mRHS->GetValue();
  if (mLHS->GetType() != mRHS->GetType()) {
    const GenerateCastingOperation* castingOp =
        _module.GetTypeManager().GetCastingOperation(
            mLHS->GetType(), mRHS->GetType());
    if (!castingOp) {
      _compilationLog.Log(WNLogging::eError, 0, "Cannot cast from ",
          mLHS->GetType()->mName, " to ", mRHS->GetType()->mName);
      LogLine(_compilationLog, WNLogging::eError);
      return (eWNInvalidCast);
    }
    if (ok != (err = castingOp->Execute(builder, mRHS->GetValue(), rhsValue))) {
      _compilationLog.Log(WNLogging::eCritical, 0,
          "Error generating cast from ", mLHS->GetType()->mName, " to ",
          mRHS->GetType()->mName);
      LogLine(_compilationLog, WNLogging::eCritical);
      return (eWNInvalidCast);
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
  return (ok);
}
