// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNDeprecatedScripting/inc/WNLValue.h"
#include "WNDeprecatedScripting/inc/WNExpression.h"

using namespace WNScripting;

WNLValue::WNLValue(WNExpression* _expr) : mExpression(_expr) {}

WNLValue::~WNLValue() {
  wn::memory::destroy(mExpression);
}

eWNTypeError WNLValue::GenerateCode(WNCodeModule& _module,
    const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) {
  return (mExpression->GenerateCode(_module, _def, _compilationLog));
}

llvm::Value* WNLValue::GetValueLocation() {
  return (mExpression->GetValueLocation());
}

WNScriptType WNLValue::GetValueType() {
  return (mExpression->GetType());
}

bool WNLValue::RequiredUse() {
  return (mExpression->RequiredUse());
}
