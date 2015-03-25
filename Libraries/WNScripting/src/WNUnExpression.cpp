// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNUnExpression.h"
#include "WNScripting/inc/WNCodeModule.h"
#include "WNScripting/inc/WNTypeManager.h"
#include "WNScripting/inc/WNExpression.h"

using namespace WNScripting;

WNUNExpression::WNUNExpression(WNUnaryType _type, WNExpression* _expr) :
    mType(_type), mExpression(_expr){
}

WNUNExpression::~WNUNExpression() {
    wn::memory::destroy(mExpression);
}

eWNTypeError WNUNExpression::GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) {
    if(mType >= UN_MAX) {
        _compilationLog.Log(WNLogging::eError, 0, "Invalid Unary Operation ", mType);
        LogLine(_compilationLog, WNLogging::eError);
        return(eWNInvalidOperation);
    }
    eWNTypeError err = ok;
    if(ok != (err = mExpression->GenerateCode(_module, _def, _compilationLog))) {
        return(err);
    }
    const GeneratePreUnaryOperation* preOp = _module.GetTypeManager().GetPreUnaryOperation(mType, mExpression->GetType());
    if(!preOp) {
        _compilationLog.Log(WNLogging::eError, 0, "Operation does not exist for type ", mExpression->GetType()->mName);
        LogLine(_compilationLog, WNLogging::eError);
        return(eWNInvalidOperation);
    }
    if(ok != (err = preOp->Execute(_module.GetBuilder(), *mExpression, mValue))) {
        return(err);
    }
    if(mType != UN_NEG) {
        mValueLocation = mExpression->GetValueLocation();
    }
    mScriptType = mExpression->GetType();
    return(ok);
}

