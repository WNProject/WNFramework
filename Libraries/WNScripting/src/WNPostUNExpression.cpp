// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNPostUNExpression.h"
#include "WNScripting/inc/WNCodeModule.h"
#include "WNScripting/inc/WNTypeManager.h"
using namespace WNScripting;

WNPostUNExpression::WNPostUNExpression(WNPostUNType _type) :
    mType(_type) {
}

WNPostUNExpression::~WNPostUNExpression() {
    
}

eWNTypeError WNPostUNExpression::GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) {
    if(mType >= UN_POSTMAX) {
        _compilationLog.Log(WNLogging::eCritical, 0, "Unknown unary type");
        LogLine(_compilationLog, WNLogging::eCritical);
        return(eWNInvalidOperation);
    }
    eWNTypeError err = eWNOK;
    if(eWNOK != (err = mBaseExpression->GenerateCode(_module, _def, _compilationLog))) {
        return(err);
    }
    if(!mBaseExpression->GetValueLocation()) {
        _compilationLog.Log(WNLogging::eError, 0, "Cannot perform post-operations on a non-lvalue");
        LogLine(_compilationLog, WNLogging::eError);
        return(eWNInvalidOperation);
    }
    const GeneratePostUnaryOperation* postOp = _module.GetTypeManager().GetPostUnaryOperation(mType, mBaseExpression->GetType());
    if(!postOp) {
        _compilationLog.Log(WNLogging::eError, 0, mBaseExpression->GetType()->mName, " does not have operation ", WNPostUNTypeNames[mType]);
        LogLine(_compilationLog, WNLogging::eError);
        return(eWNInvalidOperation);
    }
    if(eWNOK != (err = postOp->Execute(_module.GetBuilder(), *mBaseExpression, mValue))) {
        _compilationLog.Log(WNLogging::eCritical, 0, "Error generating operation ", WNPostUNTypeNames[mType], " for type: ", mBaseExpression->GetType()->mName);
        LogLine(_compilationLog, WNLogging::eCritical);
        return(err);
    }
    mValueLocation = mBaseExpression->GetValueLocation();
    mScriptType = mBaseExpression->GetType();
    return(eWNOK);
}

