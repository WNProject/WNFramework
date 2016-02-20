// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNDeprecatedScripting/inc/WNScriptFile.h"
#include "WNDeprecatedScripting/inc/WNCodeModule.h"
#include "WNDeprecatedScripting/inc/WNFunction.h"
#include "WNDeprecatedScripting/inc/WNStruct.h"

using namespace WNScripting;

WNScriptFile::WNScriptFile() {}

WNScriptFile::~WNScriptFile() {
  for (WNScriptLinkedList<char>::WNScriptLinkedListNode* i = mIncludes.first;
       i != nullptr; i = i->next) {
    wn::memory::destroy(i->value);
  }
  for (WNScriptLinkedList<WNFunction>::WNScriptLinkedListNode* i =
           mFunctions.first;
       i != nullptr; i = i->next) {
    wn::memory::destroy(i->value);
  }
  for (WNScriptLinkedList<WNStruct>::WNScriptLinkedListNode* i = mStructs.first;
       i != nullptr; i = i->next) {
    wn::memory::destroy(i->value);
  }
}

void WNScriptFile::AddFunction(WNFunction* _node) {
  mFunctions.PushBack(_node);
}

void WNScriptFile::AddStruct(WNStruct* _node) {
  mStructs.PushBack(_node);
}

void WNScriptFile::AddInclude(char* _node) {
  mIncludes.PushBack(_node);
}

const WNScriptLinkedList<char>::WNScriptLinkedListNode*
WNScriptFile::GetFirstInclude() const {
  return (mIncludes.first);
}

eWNTypeError WNScriptFile::PreProcess(WNCodeModule& _module,
    std::vector<WNScriptType>& _containedTypes,
    std::vector<WNFunctionDefinition*>& _containedFunctions,
    WNLogging::WNLog& _compilationLog) {
  eWNTypeError err = ok;
  for (WNScriptLinkedList<WNStruct>::WNScriptLinkedListNode* i = mStructs.first;
       i != nullptr; i = i->next) {
    if (ok != (err = i->value->ExportNewTypes(_module, _containedTypes,
                   _containedFunctions, _compilationLog))) {
      return (err);
    }
  }
  return (ok);
}

eWNTypeError WNScriptFile::FinalizePreProcess(WNCodeModule& _module,
    std::vector<WNFunctionDefinition*>& _containedFunctions,
    std::list<WNStruct*>& _toBeLinkedStructs,
    WNLogging::WNLog& _compilationLog) {
  eWNTypeError err = ok;
  for (WNScriptLinkedList<WNStruct>::WNScriptLinkedListNode* i = mStructs.first;
       i != nullptr; i = i->next) {
    if (ok != (err = i->value->FinalizeNewTypes(
                   _module, _containedFunctions, _compilationLog))) {
      if (eWNUnknownResolution == err) {
        _toBeLinkedStructs.push_back(i->value);
      } else {
        return (err);
      }
    }
  }

  for (WNScriptLinkedList<WNFunction>::WNScriptLinkedListNode* i =
           mFunctions.first;
       i != nullptr; i = i->next) {
    if (ok != (err = i->value->ExportFunctions(
                   _module, nullptr, _containedFunctions, _compilationLog))) {
      return (err);
    }
  }
  return (ok);
}

eWNTypeError WNScriptFile::DumpHeaders(
    WNCodeModule& _module, WNLogging::WNLog& _compilationLog) {
  eWNTypeError err = ok;
  for (WNScriptLinkedList<WNStruct>::WNScriptLinkedListNode* i = mStructs.first;
       i != nullptr; i = i->next) {
    if (ok != (err = i->value->GenerateHeader(_module, _compilationLog))) {
      return (err);
    }
  }
  for (WNScriptLinkedList<WNFunction>::WNScriptLinkedListNode* i =
           mFunctions.first;
       i != nullptr; i = i->next) {
    if (ok !=
        (err = i->value->GenerateHeader(_module, nullptr, _compilationLog))) {
      return (err);
    }
  }
  return (ok);
}

eWNTypeError WNScriptFile::GenerateCode(
    WNCodeModule& _module, WNLogging::WNLog& _compilationLog) {
  eWNTypeError err = ok;
  for (WNScriptLinkedList<WNStruct>::WNScriptLinkedListNode* i = mStructs.first;
       i != nullptr; i = i->next) {
    if (ok != (err = i->value->GenerateCode(_module, _compilationLog))) {
      return (err);
    }
  }
  for (WNScriptLinkedList<WNFunction>::WNScriptLinkedListNode* i =
           mFunctions.first;
       i != nullptr; i = i->next) {
    if (ok !=
        (err = i->value->GenerateCode(_module, nullptr, _compilationLog))) {
      return (err);
    }
  }
  return (ok);
}
