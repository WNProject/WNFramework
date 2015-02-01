// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNScriptFile.h"
#include "WNScripting/inc/WNFunction.h"
#include "WNScripting/inc/WNStruct.h"
#include "WNScripting/inc/WNCodeModule.h"

using namespace WNScripting;

WNScriptFile::WNScriptFile() {
}

WNScriptFile::~WNScriptFile() {
    for(WNScriptLinkedList<wn_char>::WNScriptLinkedListNode* i = mIncludes.first; i != wn_nullptr; i = i->next) {
        WN_DELETE(i->value);
    }
    for(WNScriptLinkedList<WNFunction>::WNScriptLinkedListNode* i = mFunctions.first; i != wn_nullptr; i = i->next) {
        WN_DELETE(i->value);
    }
    for(WNScriptLinkedList<WNStruct>::WNScriptLinkedListNode* i = mStructs.first; i != wn_nullptr; i = i->next) {
        WN_DELETE(i->value);
    }
}

void WNScriptFile::AddFunction(WNFunction* _node) {
    mFunctions.PushBack(_node);
}

void WNScriptFile::AddStruct(WNStruct* _node) {
    mStructs.PushBack(_node);
}

void WNScriptFile::AddInclude(wn_char* _node) {
    mIncludes.PushBack(_node);
}

const WNScriptLinkedList<wn_char>::WNScriptLinkedListNode* WNScriptFile::GetFirstInclude() const {
    return(mIncludes.first);
}

eWNTypeError WNScriptFile::PreProcess(WNCodeModule& _module, std::vector<WNScriptType>& _containedTypes, std::vector<WNFunctionDefinition*>& _containedFunctions, WNLogging::WNLog& _compilationLog) {
    eWNTypeError err = ok;
    for(WNScriptLinkedList<WNStruct>::WNScriptLinkedListNode* i = mStructs.first; i != wn_nullptr; i = i->next) {
        if(ok != (err = i->value->ExportNewTypes(_module, _containedTypes, _containedFunctions, _compilationLog))) {
            return(err);
        }
    }
    return(ok);
}

eWNTypeError WNScriptFile::FinalizePreProcess(WNCodeModule& _module, std::vector<WNFunctionDefinition*>& _containedFunctions, std::list<WNStruct*>& _toBeLinkedStructs, WNLogging::WNLog& _compilationLog) {
    eWNTypeError err = ok;
    for(WNScriptLinkedList<WNStruct>::WNScriptLinkedListNode* i = mStructs.first; i != wn_nullptr; i = i->next) {
        if(ok != (err = i->value->FinalizeNewTypes(_module, _containedFunctions, _compilationLog))) {
            if(eWNUnknownResolution == err) {
                _toBeLinkedStructs.push_back(i->value);
            } else {
                return(err);
            }
        }
    }

    for(WNScriptLinkedList<WNFunction>::WNScriptLinkedListNode* i = mFunctions.first; i != wn_nullptr; i = i->next) {
        if(ok != (err = i->value->ExportFunctions(_module, wn_nullptr, _containedFunctions, _compilationLog))) {
            return(err);
        }
    }
    return(ok);
}

eWNTypeError WNScriptFile::DumpHeaders(WNCodeModule& _module, WNLogging::WNLog& _compilationLog) {
    eWNTypeError err = ok;
    for(WNScriptLinkedList<WNStruct>::WNScriptLinkedListNode* i = mStructs.first; i != wn_nullptr; i = i->next) {
        if(ok != (err = i->value->GenerateHeader(_module, _compilationLog))) {
            return(err);
        }
    }
    for(WNScriptLinkedList<WNFunction>::WNScriptLinkedListNode* i = mFunctions.first; i != wn_nullptr; i = i->next) {
        if(ok != (err = i->value->GenerateHeader(_module, wn_nullptr, _compilationLog))) {
            return(err);
        }
    }
    return(ok);
}

eWNTypeError WNScriptFile::GenerateCode(WNCodeModule& _module, WNLogging::WNLog& _compilationLog) {
    eWNTypeError err = ok;
    for(WNScriptLinkedList<WNStruct>::WNScriptLinkedListNode* i = mStructs.first; i != wn_nullptr; i = i->next) {
        if(ok != (err = i->value->GenerateCode(_module, _compilationLog))) {
            return(err);
        }
    }
    for(WNScriptLinkedList<WNFunction>::WNScriptLinkedListNode* i = mFunctions.first; i != wn_nullptr; i = i->next) {
        if(ok != (err = i->value->GenerateCode(_module, wn_nullptr, _compilationLog))) {
            return(err);
        }
    }
    return(ok);
}

