// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNDeprecatedScripting/inc/WNExpression.h"

using namespace WNScripting;

WNExpression::WNExpression()
  : mValue(nullptr),
    mValueLocation(nullptr),
    mScriptType(nullptr),
    mSubValue(nullptr),
    mSubValueType(nullptr),
    mForceUse(false),
    mNewlyCreated(false) {}

WNExpression::~WNExpression() {}

llvm::Value* WNExpression::GetValue() const {
  return (mValue);
}

llvm::Value* WNExpression::GetValueLocation() const {
  return (mValueLocation);
}

WNScripting::WNScriptType WNExpression::GetType() const {
  return (mScriptType);
}

bool WNExpression::RequiredUse() {
  return (mForceUse);
}

bool WNExpression::IsNewlyCreated() {
  return (mNewlyCreated);
}

WNScripting::WNScriptType WNExpression::GetSubType() const {
  return (mSubValueType);
}

llvm::Value* WNExpression::GetSubValue() const {
  return (mSubValue);
}
