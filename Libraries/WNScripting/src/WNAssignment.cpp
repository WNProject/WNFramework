// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNAssignment.h"
#include "WNScripting/inc/WNLValue.h"
#include "WNScripting/inc/WNExpression.h"
#include "WNScripting/inc/WNCodeModule.h"
#include "WNScripting/inc/WNTypeManager.h"

using namespace WNScripting;

WNAssignment::WNAssignment(WNLValue* _lValue):
    mLValue(_lValue),
    mAssignType(AT_MAX),
    mAssignExpression(wn_nullptr) {
}

WNAssignment::~WNAssignment() {
    WN_DELETE(mLValue);
    if(mAssignExpression) {
        WN_DELETE(mAssignExpression);
    }
}

void WNAssignment::AddValue(WNAssignType _type, WNExpression* value) {
    mAssignType = _type;
    mAssignExpression = value;
}

eWNTypeError WNAssignment::GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) {
    eWNTypeError err = ok;
    if(ok != (err = mLValue->GenerateCode(_module, _def, _compilationLog))) {
        return(err);
    }

    if(!mAssignExpression) {
        if(mLValue->RequiredUse()) {
            _compilationLog.Log(WNLogging::eError, 0, mLValue->GetValueType()->mName, "must be assigned to");
            LogLine(_compilationLog, WNLogging::eError);
            return(eWNInvalidOperation);
        } else {
            return(ok);
        }
    }
    if(!mLValue->GetValueLocation()) {
        _compilationLog.Log(WNLogging::eError, 0, mLValue->GetValueType()->mName, "is not a valid assignee");
        LogLine(_compilationLog, WNLogging::eError);
        return(eWNBadType);
    }
    const GenerateAssignment* assign = _module.GetTypeManager().GetAssignmentOperation(mLValue->GetValueType(), mAssignType);
    if(!assign) {
        _compilationLog.Log(WNLogging::eError, 0, "Cannot generate assignment for ", mLValue->GetValueType()->mName);
        LogLine(_compilationLog, WNLogging::eError);
        return(eWNBadType);
    }

    WNScriptType type = mLValue->GetValueType();
    if(ok != (err = mAssignExpression->GenerateCode(_module, _def, _compilationLog))) {
        return(err);
    }

    llvm::Value* val = mAssignExpression->GetValue();
    if(mAssignExpression->GetType() != type) {
        const GenerateCastingOperation* op = _module.GetTypeManager().GetCastingOperation(mAssignExpression->GetType(), mLValue->GetValueType());
        if(op == wn_nullptr) {
            _compilationLog.Log(WNLogging::eError, 0, "Cannot cast from ", mAssignExpression->GetType()->mName, " to ", mLValue->GetValueType()->mName);
            LogLine(_compilationLog, WNLogging::eError);
            return(eWNInvalidCast);
        }

        if(ok != (err = op->Execute(_module.GetBuilder(), mAssignExpression->GetValue(), val))) {
            _compilationLog.Log(WNLogging::eCritical, 0, "Error generating cast from ", mAssignExpression->GetType()->mName, " to ", mLValue->GetValueType()->mName);
            LogLine(_compilationLog, WNLogging::eCritical);
            return(ok);
        }
    }
    if(ok != (err = assign->Execute(_module, false, type, val, mLValue->GetValueLocation(), _def, _compilationLog))) {
        _compilationLog.Log(WNLogging::eCritical, 0, "Error generating assignment");
        LogLine(_compilationLog, WNLogging::eCritical);
        return(err);
    }

    return(ok);
}
