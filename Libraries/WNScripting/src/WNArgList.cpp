// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNArgList.h"
#include "WNMemory/inc/WNAllocation.h"
#include "WNScripting/inc/WNExpression.h"

using namespace WNScripting;

WNArgList::WNArgList() {
}

WNArgList::~WNArgList() {
    for(WNScriptLinkedList<WNFunctionExpression>::WNScriptLinkedListNode * i = mExpressionList.first; i != wn_nullptr; i = i->next) {
        WN_DELETE(i->value->expr);
        WN_DELETE(i->value);
    }
}

void WNArgList::AddExpression(WNExpression* _expr, wn_bool _handOwnership) {
    mExpressionList.PushBack(new WNFunctionExpression(_expr, _handOwnership));
}

WNScriptLinkedList<WNFunctionExpression>& WNArgList::GetExpressions() {
    return(mExpressionList);
}

