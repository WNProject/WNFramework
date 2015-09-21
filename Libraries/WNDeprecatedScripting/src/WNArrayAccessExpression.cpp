// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNDeprecatedScripting/inc/WNIncludeLLVM.h"
#include "llvm/IR/IRBuilder.h"
#include "WNDeprecatedScripting/inc/WNEndIncludeLLVM.h"

#include "WNDeprecatedScripting/inc/WNArrayAccessExpression.h"
#include "WNDeprecatedScripting/inc/WNCodeModule.h"
#include "WNDeprecatedScripting/inc/WNTypeManager.h"
#include "WNDeprecatedScripting/inc/WNExpression.h"

using namespace WNScripting;

WNArrayAccessExpr::WNArrayAccessExpr(WNExpression* _expr) {
    mArrayAccess = _expr;
}

WNArrayAccessExpr::~WNArrayAccessExpr() {
    wn::memory::destroy(mArrayAccess);
}

eWNTypeError WNArrayAccessExpr::GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
    eWNTypeError err;
    if(ok != (err = mBaseExpression->GenerateCode(_module, _def, _compilationLog))) {
        return(err);
    }
    if(ok != (err = mArrayAccess->GenerateCode(_module, _def, _compilationLog))) {
        return(err);
    }

    WNScriptType intType = NULL;
    if(ok != (err = _module.GetTypeManager().get_type_by_name("Int", intType))) {
         _compilationLog.Log(WNLogging::eCritical, 0, "Cannot find Int type");
        LogLine(_compilationLog, WNLogging::eCritical);
        return(eWNInvalidCast);
    }


    llvm::Value* rhs = mArrayAccess->GetValue();
    if(intType != mArrayAccess->GetType()) {
        const GenerateCastingOperation* castOp = _module.GetTypeManager().GetCastingOperation(mArrayAccess->GetType(), intType);
        if(!castOp) {
             _compilationLog.Log(WNLogging::eError, 0, "Cannot cast from ", mArrayAccess->GetType()->mName, " to ", intType->mName);
            LogLine(_compilationLog, WNLogging::eError);
            return(eWNInvalidCast);
        }
        if(ok != (err = castOp->Execute(builder, mArrayAccess->GetValue(), rhs))) {
            return(err);
        }
    }

    const GenerateArrayAccessOperation* arrayOp = _module.GetTypeManager().GetArrayAccessOperation(mBaseExpression->GetType(), intType);
    if(!arrayOp){
        _compilationLog.Log(WNLogging::eError, 0, "No valid array access operator exists for ", mBaseExpression->GetType()->mName);
        LogLine(_compilationLog, WNLogging::eError);
        return(eWNInvalidOperation);
    }

    if(ok != (err = arrayOp->Execute(_module, mBaseExpression->GetValue(), rhs, mValue, mValueLocation, _def))) {
        _compilationLog.Log(WNLogging::eCritical, 0, "Error generating array access operation ");
        LogLine(_compilationLog, WNLogging::eCritical);
        return(err);
    }
    mScriptType = mBaseExpression->GetType()->mArrayType;
    return(ok);
}

