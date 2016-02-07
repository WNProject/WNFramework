// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNDeprecatedScripting/inc/WNNodeHelpers.h"  // This is intentionally out of order.
// If this is included lower, it will fail because windows.h conflicts with
// llvm.

#include "WNDeprecatedScripting/inc/WNArrayAllocation.h"
#include "WNDeprecatedScripting/inc/WNCodeModule.h"
#include "WNDeprecatedScripting/inc/WNTypeManager.h"
#include "WNDeprecatedScripting/inc/WNTypeNode.h"

using namespace WNScripting;

WNArrayAllocation::WNArrayAllocation()
  : mLevels(0), mType(nullptr), mCopyinitializer(nullptr) {}

WNArrayAllocation::~WNArrayAllocation() {
  if (mType) {
    wn::memory::destroy(mType);
  }
  for (WNScriptLinkedList<WNExpression>::WNScriptLinkedListNode* i =
           mArrayinitializers.first;
       i != nullptr; i = i->next) {
    wn::memory::destroy(i->value);
  }
}

void WNArrayAllocation::SetType(WNTypeNode* _typeNode) {
  mType = _typeNode;
}

void WNArrayAllocation::AddExpression(WNExpression* _expr) {
  mArrayinitializers.PushBack(_expr);
  mLevels++;
}

void WNArrayAllocation::AddLevel() {
  mLevels++;
}

eWNTypeError WNArrayAllocation::GenerateCode(WNCodeModule& _module,
    const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) {
  mNewlyCreated = true;
  mForceUse = true;
  eWNTypeError err = ok;

  if (mArrayinitializers.first != nullptr && mCopyinitializer != nullptr) {
    _compilationLog.Log(
        WNLogging::eError, 0, "Cannot specify both size and copy construction");
    LogLine(_compilationLog, WNLogging::eError);
  }

  if (ok != (err = mType->GetType(
                 _module.GetTypeManager(), mScriptType, _compilationLog))) {
    _compilationLog.Log(WNLogging::eError, 0, "Cannot find type ");
    LogLine(_compilationLog, WNLogging::eError);
    return (err);
  }
  for (size_t i = 0; i < mLevels; ++i) {
    if (ok != (err = _module.GetTypeManager().get_array_of(
                   mScriptType, mScriptType))) {
      _compilationLog.Log(
          WNLogging::eError, 0, "Cannot create array of ", mScriptType->mName);
      LogLine(_compilationLog, WNLogging::eError);
      return (err);
    }
  }
  if (mCopyinitializer) {
    const GenerateCopyConstruction* construction =
        _module.GetTypeManager().GetCopyConstructionOperation(mScriptType);
    if (!construction) {
      _compilationLog.Log(WNLogging::eError, 0,
          "Could not find copy construction operation for ",
          mScriptType->mName);
      LogLine(_compilationLog, WNLogging::eError);
      return (eWNCannotCreateType);
    }

    if (ok != (err = mCopyinitializer->GenerateCode(
                   _module, _def, _compilationLog))) {
      return (err);
    }
    llvm::Value* copyValue = mCopyinitializer->GetValue();
    if (mCopyinitializer->RequiredUse()) {
      _compilationLog.Log(WNLogging::eError, 0,
          "Cannot copy construct temporary value, construct instead");
      LogLine(_compilationLog, WNLogging::eError);
      return (eWNInvalidCast);
    }
    if (mCopyinitializer->GetType() != mScriptType) {
      const GenerateCastingOperation* castOp =
          _module.GetTypeManager().GetCastingOperation(
              mCopyinitializer->GetType(), mScriptType);
      if (!castOp) {
        _compilationLog.Log(WNLogging::eError, 0,
            "Cannot construct array of type ", mScriptType->mName,
            " from type ", mCopyinitializer->GetType()->mName,
            " different type");
        LogLine(_compilationLog, WNLogging::eError);
        return (eWNInvalidCast);
      } else {
        if (ok != (err = castOp->Execute(
                       _module.GetBuilder(), copyValue, copyValue))) {
          _compilationLog.Log(WNLogging::eCritical, 0, "Error casting ",
              mCopyinitializer->GetType()->mName, " from type ",
              mScriptType->mName, " different type");
          LogLine(_compilationLog, WNLogging::eError);
          return (eWNInvalidCast);
        }
      }
    }

    if (ok != (err = construction->Execute(_module, mValue, mScriptType,
                   copyValue, _def, _compilationLog))) {
      LogLine(_compilationLog, WNLogging::eError);
      return (err);
    }
  } else {
    const GenerateConstruction* construction =
        _module.GetTypeManager().GetConstructionOperation(mScriptType);
    if (!construction) {
      _compilationLog.Log(WNLogging::eError, 0,
          "Could not find construction operation for ", mScriptType->mName);
      LogLine(_compilationLog, WNLogging::eError);
      return (eWNCannotCreateType);
    }
    if (ok != (err = construction->Execute(_module, mValue, mScriptType,
                   mArrayinitializers.first, _def, _compilationLog))) {
      LogLine(_compilationLog, WNLogging::eError);
      return (err);
    }
  }
  return (ok);
}

void WNArrayAllocation::SetCopyInitializer(WNExpression* _expression) {
  mCopyinitializer = _expression;
}