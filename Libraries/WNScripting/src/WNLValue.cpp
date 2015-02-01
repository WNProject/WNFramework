// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNLValue.h"
#include "WNScripting/inc/WNExpression.h"

using namespace WNScripting;

WNLValue::WNLValue(WNExpression* _expr) :
    mExpression(_expr) {

}

WNLValue::~WNLValue() {
    WN_DELETE(mExpression);
}

eWNTypeError WNLValue::GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) {
    return(mExpression->GenerateCode(_module, _def, _compilationLog));
}

llvm::Value* WNLValue::GetValueLocation() {
    return(mExpression->GetValueLocation());
}

WNScriptType WNLValue::GetValueType() {
    return(mExpression->GetType());
}

wn_bool WNLValue::RequiredUse() {
    return(mExpression->RequiredUse());
}

