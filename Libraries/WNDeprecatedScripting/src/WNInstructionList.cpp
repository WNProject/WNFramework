// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.


#include "WNDeprecatedScripting/inc/WNIncludeLLVM.h"
#include "llvm/IR/IRBuilder.h"
#include "WNDeprecatedScripting/inc/WNEndIncludeLLVM.h"

#include "WNDeprecatedScripting/inc/WNInstructionList.h"
#include "WNDeprecatedScripting/inc/WNCodeModule.h"
#include "WNDeprecatedScripting/inc/WNScopedVariableList.h"
#include "WNDeprecatedScripting/inc/WNInstruction.h"

using namespace WNScripting;

WNInstructionList::WNInstructionList() :
    mReturns(false) {
}

WNInstructionList::WNInstructionList(WNInstruction* inst) :
    mReturns(false) {
    mInstructions.PushBack(inst);
}

WNInstructionList::~WNInstructionList() {
    for(WNScriptLinkedList<WNInstruction>::WNScriptLinkedListNode* i = mInstructions.first; i != nullptr; i = i->next) {
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
    for(WNScriptLinkedList<WNInstruction>::WNScriptLinkedListNode* i = mInstructions.first; i != nullptr; i = i->next) {
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

bool WNInstructionList::Returns() { return(mReturns); }

