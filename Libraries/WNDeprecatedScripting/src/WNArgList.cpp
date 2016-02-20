// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNDeprecatedScripting/inc/WNArgList.h"
#include "WNDeprecatedScripting/inc/WNExpression.h"
#include "WNMemory/inc/WNBasic.h"

using namespace WNScripting;

WNArgList::WNArgList() {}

WNArgList::~WNArgList() {
  for (WNScriptLinkedList<WNFunctionExpression>::WNScriptLinkedListNode* i =
           mExpressionList.first;
       i != nullptr; i = i->next) {
    wn::memory::destroy(i->value->expr);
    wn::memory::destroy(i->value);
  }
}

void WNArgList::AddExpression(WNExpression* _expr, bool _handOwnership) {
  mExpressionList.PushBack(new WNFunctionExpression(_expr, _handOwnership));
}

WNScriptLinkedList<WNFunctionExpression>& WNArgList::GetExpressions() {
  return (mExpressionList);
}
