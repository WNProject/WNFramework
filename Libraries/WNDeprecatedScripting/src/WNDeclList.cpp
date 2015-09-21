// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNDeprecatedScripting/inc/WNDeclList.h"
#include "WNDeprecatedScripting/inc/WNDeclaration.h"
using namespace WNScripting;

WNDeclList::WNDeclList(WNDeclaration* _decl) {
    mDeclarations.PushBack(_decl);
}

WNDeclList::~WNDeclList() {
    for(WNScriptLinkedList<WNDeclaration>::WNScriptLinkedListNode* i = mDeclarations.first; i != wn_nullptr; i=i->next) {
        wn::memory::destroy(i->value);
    }
}

void WNDeclList::AddDeclaration(WNDeclaration* _decl) {
    mDeclarations.PushBack(_decl);
}

eWNTypeError WNDeclList::GetTypeList(WNTypeManager& _manager, std::vector<WNScriptType>& _vector, WNLogging::WNLog& _compilationLog) {
    eWNTypeError err = ok;
    for(WNScriptLinkedList<WNDeclaration>::WNScriptLinkedListNode *i = mDeclarations.first; i != wn_nullptr; i = i->next) {
        WNScriptType t;
        if(ok != (err = i->value->GetType(_manager, t, _compilationLog))) {
            _compilationLog.Log(WNLogging::eError, 0, "Cannot find type for variable: ", i->value->GetName());
            return err;
        }
        _vector.push_back(t);
    }
    return(ok);
}

eWNTypeError WNDeclList::GetDeclarations(const WNScriptLinkedList<WNDeclaration>*& _outDeclaration) const {
    _outDeclaration = &mDeclarations;
    return(ok);
}

