// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

// clang-format on
#include "WNDeprecatedScripting/inc/WNIncludeLLVM.h"
#include "llvm/IR/IRBuilder.h"
#include "WNDeprecatedScripting/inc/WNEndIncludeLLVM.h"
// clang-format off

#include "WNDeprecatedScripting/inc/WNCodeModule.h"
#include "WNDeprecatedScripting/inc/WNDeclaration.h"
#include "WNDeprecatedScripting/inc/WNExpression.h"
#include "WNDeprecatedScripting/inc/WNScopedVariableList.h"
#include "WNDeprecatedScripting/inc/WNTypeManager.h"
#include "WNDeprecatedScripting/inc/WNTypeNode.h"

using namespace WNScripting;
WNDeclaration::WNDeclaration(WNTypeNode* _type, const char* _name)
  : mType(_type),
    mName(nullptr),
    mExpr(nullptr),
    mScalarType(nullptr),
    mUnsizedArrayinitializers(0),
    mInitAssign(false) {
  COPY_STRING(_name, mName);
}

WNDeclaration::~WNDeclaration() {
  wn::memory::destroy(mType);
  wn::memory::destroy(mName);
  if (mExpr) {
    wn::memory::destroy(mExpr);
  }
  if (mScalarType) {
    wn::memory::destroy(mScalarType);
  }
}

const char* WNDeclaration::GetName() {
  return (mName);
}

eWNTypeError WNDeclaration::GetType(WNTypeManager& _manager,
    WNScriptType& _scriptType, WNLogging::WNLog& _compilationLog) {
  return (mType->GetType(_manager, _scriptType, _compilationLog));
}

void WNDeclaration::AddExpressionInitializer(
    WNExpression* _expr, bool _assign) {
  mExpr = _expr;
  mInitAssign = _assign;
}

void WNDeclaration::AddSizedArrayinitializer(WNExpression* _expr) {
  mSizedArrayinitializers.PushBack(_expr);
}

void WNDeclaration::AddUnsizedArrayinitializer() {
  mUnsizedArrayinitializers += 1;
}

eWNTypeError WNDeclaration::GenerateCode(WNCodeModule& _module,
    const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) {
  llvm::IRBuilder<>* builder =
      reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
  WNScriptType t;
  llvm::Value* allocLocation = nullptr;
  eWNTypeError err = ok;

  if (_module.GetScopedVariableList().GetVariable(mName)) {
    _compilationLog.Log(
        WNLogging::eError, 0, "Error ", mName, " already exists");
    LogLine(_compilationLog, WNLogging::eError);
    return (eWNAlreadyExists);
  }

  if (ok !=
      (err = mType->GetType(_module.GetTypeManager(), t, _compilationLog))) {
    _compilationLog.Log(WNLogging::eError, 0, "Error, cannot find type");
    LogLine(_compilationLog, WNLogging::eError);
    return (err);
  }
  const GenerateAllocation* alloc =
      _module.GetTypeManager().GetAllocationOperation(t);
  if (!alloc) {
    _compilationLog.Log(WNLogging::eError, 0,
        "Error, cannot create allocation for type", t->mName);
    LogLine(_compilationLog, WNLogging::eError);
    return (eWNCannotCreateType);
  }
  if (ok != (err = alloc->Execute(_module, t, mName, false, allocLocation))) {
    _compilationLog.Log(WNLogging::eCritical, 0,
        "Error, creating allocation for type", t->mName);
    LogLine(_compilationLog, WNLogging::eCritical);
    return (err);
  }

  if (mExpr) {
    if (ok != (err = mExpr->GenerateCode(_module, _def, _compilationLog))) {
      return (err);
    }
    if (!mExpr->GetValue()) {
      _compilationLog.Log(WNLogging::eError, 0,
          "Error, something wrong with value for expression");
      mExpr->LogLine(_compilationLog, WNLogging::eError);
      return (eWNInvalidCast);
    }
    const GenerateAssignment* assign =
        _module.GetTypeManager().GetAssignmentOperation(
            t, (mInitAssign || mExpr->IsNewlyCreated()) ? AT_CHOWN : AT_EQ);
    if (!assign) {
      _compilationLog.Log(WNLogging::eError, 0,
          "Error, cannot create assignment for type", t->mName);
      LogLine(_compilationLog, WNLogging::eError);
      return (eWNCannotCreateType);
    }
    llvm::Value* val = mExpr->GetValue();
    if (mExpr->GetType() != t) {
      const GenerateCastingOperation* cast =
          _module.GetTypeManager().GetCastingOperation(mExpr->GetType(), t);
      if (!cast) {
        _compilationLog.Log(WNLogging::eError, 0, "Error, cannot cast from ",
            mExpr->GetType()->mName, " to ", t->mName);
        mExpr->LogLine(_compilationLog, WNLogging::eError);
        return (eWNInvalidCast);
      }
      if (ok != (err = cast->Execute(builder, val, val))) {
        _compilationLog.Log(WNLogging::eCritical, 0,
            "Error, generating cast from ", mExpr->GetType()->mName, " to ",
            t->mName);
        mExpr->LogLine(_compilationLog, WNLogging::eCritical);
        return (err);
      }
    }
    if (ok != (err = assign->Execute(_module, true, t, val, allocLocation, _def,
                   _compilationLog))) {
      return (err);
    }
  }
  WNScriptVariable* var =
      WN_SCRIPTNODE_NEW(WNScriptVariable(t, mName, allocLocation));
  if (ok != (err = _module.GetScopedVariableList().PushVariable(var))) {
    return (err);
  }
  return (ok);
}

WNExpression* WNDeclaration::GetExpression() {
  return (mExpr);
}
