// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


#include "WNScripting/inc/WNIncludeLLVM.h"
#include "llvm/IR/IRBuilder.h"
#include "WNScripting/inc/WNEndIncludeLLVM.h"

#include "WNScripting/inc/WNInstructionList.h"
#include "WNScripting/inc/WNCodeModule.h"
#include "WNScripting/inc/WNScopedVariableList.h"
#include "WNScripting/inc/WNInstruction.h"

using namespace WNScripting;

WNInstructionList::WNInstructionList() :
    mReturns(false) {
}

WNInstructionList::WNInstructionList(WNInstruction* inst) :
    mReturns(false) {
    mInstructions.PushBack(inst);
}

WNInstructionList::~WNInstructionList() {
    for(WNScriptLinkedList<WNInstruction>::WNScriptLinkedListNode* i = mInstructions.first; i != wn_nullptr; i = i->next) {
        wn::memory::destroy(i->value);
    }
}

void WNInstructionList::AddInstruction(WNInstruction* inst) {
    mInstructions.PushBack(inst);
}

eWNTypeError WNInstructionList::GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
    eWNTypeError err;
    llvm::BasicBlock* bb = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
    if(builder->GetInsertBlock() != NULL) {
        builder->CreateBr(bb);
    }
    builder->SetInsertPoint(bb);
    _module.GetScopedVariableList().PushScopeBlock(_module);
    for(WNScriptLinkedList<WNInstruction>::WNScriptLinkedListNode* i = mInstructions.first; i != wn_nullptr; i = i->next) {
        if(ok != (err = i->value->GenerateCode(_module, _def, _compilationLog))) {
            return(err);
        }
        mReturns = i->value->Returns();
        if(mReturns) {
            break;
        }
    }
    _module.GetScopedVariableList().PopScopeBlock(_module, !mReturns, _def, _compilationLog);
    return(ok);
}

wn_bool WNInstructionList::Returns() { return(mReturns); }

