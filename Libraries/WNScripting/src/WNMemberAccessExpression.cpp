// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNMemberAccessExpression.h"
#include "WNScripting/inc/WNScriptingAllocations.h"
#include "WNScripting/inc/WNCodeModule.h"
#include "WNScripting/inc/WNTypeManager.h"

using namespace WNScripting;

WNMemberAccessExpr::WNMemberAccessExpr(const char* _member):
    mMember(0) {
    COPY_STRING(_member, mMember);
}

WNMemberAccessExpr::~WNMemberAccessExpr() {
    WN_DELETE(mMember);
}

eWNTypeError WNMemberAccessExpr::GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) {
    eWNTypeError err;
    if(ok != (err = mBaseExpression->GenerateCode(_module, _def, _compilationLog))) {
        return(err);
    }

    const GenerateIDAccessOperation* idOp = _module.GetTypeManager().GetIDAccessOperation(mBaseExpression->GetType());
    if(!idOp) {
        _compilationLog.Log(WNLogging::eError, 0, "No . operator defined for ", mBaseExpression->GetType()->mName);
        LogLine(_compilationLog, WNLogging::eError);
        return(eWNInvalidOperation);
    }
    if(!mBaseExpression->GetValueLocation()){
        //This means we are trying to access a struct as an RValue
        //Int x = MyStruct().y; //this would lead to an orphaned struct... we do not want this or to have to
        // manage orphans
        _compilationLog.Log(WNLogging::eError, 0, "You rvalue struct, MyStruct().y, you must assign the struct first: ", mBaseExpression->GetType()->mName);
        LogLine(_compilationLog, WNLogging::eError);
        return(eWNInvalidCast);
    }
    if(ok != (err = idOp->Execute(_module.GetBuilder(), mBaseExpression->GetValue(), mBaseExpression->GetValueLocation(), mMember, mScriptType, mValue, mValueLocation))) {
        _compilationLog.Log(WNLogging::eCritical, 0, "Error generating id access operation");
        LogLine(_compilationLog, WNLogging::eCritical);
        return(err);
    }
    mSubValue = mBaseExpression->GetValue();
    mSubValueType = mBaseExpression->GetType();
    return(ok);
}

