// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNExpression.h"

using namespace WNScripting;

WNExpression::WNExpression() :
    mValue(wn_nullptr),
    mValueLocation(wn_nullptr),
    mScriptType(wn_nullptr),
    mSubValue(wn_nullptr),
    mSubValueType(wn_nullptr),
    mForceUse(wn_false),
    mNewlyCreated(wn_false) {
}

WNExpression::~WNExpression() {

}

llvm::Value* WNExpression::GetValue() const {
    return(mValue);
}

llvm::Value* WNExpression::GetValueLocation() const {
    return(mValueLocation);
}

WNScripting::WNScriptType WNExpression::GetType() const {
    return(mScriptType);
}

wn_bool WNExpression::RequiredUse() {
    return(mForceUse);
}

wn_bool WNExpression::IsNewlyCreated() {
    return(mNewlyCreated);
}

WNScripting::WNScriptType WNExpression::GetSubType() const {
    return(mSubValueType);
}

llvm::Value* WNExpression::GetSubValue() const {
    return(mSubValue);
}
