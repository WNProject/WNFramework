// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNExpression.h"

using namespace WNScripting; 

WNExpression::WNExpression() :
    mValue(WN_NULL),
    mValueLocation(WN_NULL),
    mScriptType(WN_NULL),
    mSubValue(WN_NULL),
    mSubValueType(WN_NULL),
    mForceUse(WN_FALSE),
    mNewlyCreated(WN_FALSE) {
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

WN_BOOL WNExpression::RequiredUse() {
    return(mForceUse);
}

WN_BOOL WNExpression::IsNewlyCreated() {
    return(mNewlyCreated);
}

WNScripting::WNScriptType WNExpression::GetSubType() const {
    return(mSubValueType);
}

llvm::Value* WNExpression::GetSubValue() const {
    return(mSubValue);
}
