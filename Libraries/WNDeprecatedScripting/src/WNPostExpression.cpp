// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNDeprecatedScripting/inc/WNPostExpression.h"

using namespace WNScripting;

WNPostExpression::WNPostExpression():
    mBaseExpression(wn_nullptr) {
}

WNPostExpression::~WNPostExpression() {
    if(mBaseExpression) {
        wn::memory::destroy(mBaseExpression);
    }
}

void WNPostExpression::AddBaseExpr(WNExpression* _expr) {
    mBaseExpression = _expr;
}

