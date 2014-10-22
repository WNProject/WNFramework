// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNScriptFile.h"
#include "WNScripting/inc/WNFunction.h"
#include "WNScripting/inc/WNStruct.h"
#include "WNScripting/inc/WNCodeModule.h"
#include "WNContainers/inc/WNAlgorithm.h"
#include <algorithm>
using namespace WNScripting;

WNScriptFile::WNScriptFile() {
}

WNScriptFile::~WNScriptFile() {
    for(WNScriptLinkedList<WN_CHAR>::WNScriptLinkedListNode* i = mIncludes.first; i != WN_NULL; i = i->next) {
        WN_DELETE(i->value);
    }
    for(WNScriptLinkedList<WNFunction>::WNScriptLinkedListNode* i = mFunctions.first; i != WN_NULL; i = i->next) {
        WN_DELETE(i->value);
    }
    for(WNScriptLinkedList<WNStruct>::WNScriptLinkedListNode* i = mStructs.first; i != WN_NULL; i = i->next) {
        WN_DELETE(i->value);
    }
}

void WNScriptFile::AddFunction(WNFunction* _node) {
    mFunctions.PushBack(_node);
}
    
void WNScriptFile::AddStruct(WNStruct* _node) {
    mStructs.PushBack(_node);
}

void WNScriptFile::AddInclude(WN_CHAR* _node) {
    mIncludes.PushBack(_node);
}

const WNScriptLinkedList<WN_CHAR>::WNScriptLinkedListNode* WNScriptFile::GetFirstInclude() const {
    return(mIncludes.first);
}

eWNTypeError WNScriptFile::PreProcess(WNCodeModule& _module, std::vector<WNScriptType>& _containedTypes, std::vector<WNFunctionDefinition*>& _containedFunctions, WNLogging::WNLog& _compilationLog) {
    eWNTypeError err = eWNOK;
    for(WNScriptLinkedList<WNStruct>::WNScriptLinkedListNode* i = mStructs.first; i != WN_NULL; i = i->next) {
        if(eWNOK != (err = i->value->ExportNewTypes(_module, _containedTypes, _containedFunctions, _compilationLog))) {
            return(err);
        }
    }
    return(eWNOK);
}

eWNTypeError WNScriptFile::FinalizePreProcess(WNCodeModule& _module, std::vector<WNFunctionDefinition*>& _containedFunctions, std::list<WNStruct*>& _toBeLinkedStructs, WNLogging::WNLog& _compilationLog) {
    eWNTypeError err = eWNOK;
    for(WNScriptLinkedList<WNStruct>::WNScriptLinkedListNode* i = mStructs.first; i != WN_NULL; i = i->next) {
        if(eWNOK != (err = i->value->FinalizeNewTypes(_module, _containedFunctions, _compilationLog))) {
            if(eWNUnknownResolution == err) {
                _toBeLinkedStructs.push_back(i->value);
            } else {
                return(err);
            }
        }
    }

    for(WNScriptLinkedList<WNFunction>::WNScriptLinkedListNode* i = mFunctions.first; i != WN_NULL; i = i->next) {
        if(eWNOK != (err = i->value->ExportFunctions(_module, WN_NULL, _containedFunctions, _compilationLog))) {
            return(err);
        }
    }
    return(eWNOK);
}

eWNTypeError WNScriptFile::DumpHeaders(WNCodeModule& _module, WNLogging::WNLog& _compilationLog) {
    eWNTypeError err = eWNOK;
    for(WNScriptLinkedList<WNStruct>::WNScriptLinkedListNode* i = mStructs.first; i != WN_NULL; i = i->next) {
        if(eWNOK != (err = i->value->GenerateHeader(_module, _compilationLog))) {
            return(err);
        }
    }
    for(WNScriptLinkedList<WNFunction>::WNScriptLinkedListNode* i = mFunctions.first; i != WN_NULL; i = i->next) {
        if(eWNOK != (err = i->value->GenerateHeader(_module, WN_NULL, _compilationLog))) {
            return(err);
        }
    }
    return(eWNOK);
}

eWNTypeError WNScriptFile::GenerateCode(WNCodeModule& _module, WNLogging::WNLog& _compilationLog) {
    eWNTypeError err = eWNOK;
    for(WNScriptLinkedList<WNStruct>::WNScriptLinkedListNode* i = mStructs.first; i != WN_NULL; i = i->next) {
        if(eWNOK != (err = i->value->GenerateCode(_module, _compilationLog))) {
            return(err);
        }
    }
    for(WNScriptLinkedList<WNFunction>::WNScriptLinkedListNode* i = mFunctions.first; i != WN_NULL; i = i->next) {
        if(eWNOK != (err = i->value->GenerateCode(_module, WN_NULL, _compilationLog))) {
            return(err);
        }
    }
    return(eWNOK);
}

