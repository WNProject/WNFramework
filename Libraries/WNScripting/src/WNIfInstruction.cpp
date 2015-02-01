// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNIfInstruction.h"
#include "WNScripting/inc/WNCodeModule.h"
#include "WNScripting/inc/WNTypeManager.h"
#include "WNScripting/inc/WNInstructionList.h"
#include "WNScripting/inc/WNExpression.h"

#ifdef _WN_MSVC
    #pragma warning(push)
    #pragma warning(disable: 4100)
    #pragma warning(disable: 4127)
    #pragma warning(disable: 4152)
    #pragma warning(disable: 4244)
    #pragma warning(disable: 4245)
    #pragma warning(disable: 4267)
    #pragma warning(disable: 4355)
    #pragma warning(disable: 4512)
    #pragma warning(disable: 4800)
#endif

#include "llvm/IR/IRBuilder.h"

#ifdef _WN_MSVC
    #pragma warning(pop)
#endif

using namespace WNScripting;


WNElseIf::WNElseIf(WNExpression* _cond, WNInstructionList* _body):
    mCondition(_cond),
    mBody(_body) {
}

WNElseIf::~WNElseIf() {
    WN_DELETE(mCondition);
    WN_DELETE(mBody);
}

eWNTypeError WNElseIf::GenerateCode(WNCodeModule&, const WNFunctionDefinition*, WNLogging::WNLog&) {
    return(error); //The code for the Elseif is actually generated in WNIFInstruction
}

WNExpression* WNElseIf::GetCondition() {
    return(mCondition);
}

WNInstructionList* WNElseIf::GetBody() {
    return(mBody);
}

WNIFInstruction::WNIFInstruction(WNExpression* _cond, WNInstructionList* _body) :
    mCondition(_cond),
    mBody(_body),
    mElse(wn_nullptr) {
}

WNIFInstruction::~WNIFInstruction() {
    WN_DELETE(mCondition);
    WN_DELETE(mBody);
    if(mElse) {
        WN_DELETE(mElse);
    }
    for(WNScriptLinkedList<WNElseIf>::WNScriptLinkedListNode*i =  mElseIfNodes.first; i != wn_nullptr; i = i->next) {
        WN_DELETE(i->value);
    }
}

void WNIFInstruction::AddElseIf(WNElseIf* _elseif) {
    mElseIfNodes.PushBack(_elseif);
}

void WNIFInstruction::AddElse(WNInstructionList* _else) {
    mElse = _else;
}

eWNTypeError WNIFInstruction::GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
    eWNTypeError err = ok;
    llvm::BasicBlock* ifBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
    llvm::BasicBlock* elseBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
    llvm::BasicBlock* endBlock = elseBlock;
    mReturns = true;
    if(mElse) {
        endBlock = 0;
    }

    WNScriptType boolType = wn_nullptr;
    if(ok != (err = _module.GetTypeManager().GetTypeByName("Bool", boolType))) {
        return(err);
    }

    WNExpression* curCondition = mCondition;
    WNInstructionList* curBody = mBody;

    llvm::Value* cv = llvm::ConstantInt::get(boolType->mLLVMType, 0);
    for(WNScriptLinkedList<WNElseIf>::WNScriptLinkedListNode * i = mElseIfNodes.first; i != wn_nullptr; i = i->next) {
        if(ok != (err = curCondition->GenerateCode(_module, _def, _compilationLog))) {
            return(err);
        }
        if(curCondition->GetType() != boolType) {
            _compilationLog.Log(WNLogging::eError, 0, "Condition must be of boolean type");
            LogLine(_compilationLog, WNLogging::eError);
            return(eWNBadType);
        }
        llvm::BasicBlock* bb = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
        llvm::Value* v = curCondition->GetValue();
        v = builder->CreateICmpNE(cv, v, "");
        builder->CreateCondBr(v, ifBlock, bb);
        builder->SetInsertPoint(ifBlock);
        if(ok != (err = curBody->GenerateCode(_module, _def, _compilationLog))) {
            return(err);
        }
        bool returns = (curBody->Returns());
        if(!returns) {
            mReturns = false;
            if(!endBlock) {
                endBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
            }
            builder->CreateBr(endBlock);
        }
        builder->SetInsertPoint(bb);
        ifBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
        curCondition = i->value->GetCondition();
        curBody = i->value->GetBody();
    }

    if(ok != (err = curCondition->GenerateCode(_module, _def, _compilationLog))) {
        return(err);
    }
    if(curCondition->GetType() != boolType) {
        _compilationLog.Log(WNLogging::eError, 0, "Condition must be of boolean type");
        LogLine(_compilationLog, WNLogging::eError);
        return(eWNBadType);
    }
    llvm::Value* v = curCondition->GetValue();
    v = builder->CreateICmpNE(cv, v, "");
    builder->CreateCondBr(v, ifBlock, elseBlock);
    builder->SetInsertPoint(ifBlock);
    if(ok != (err = curBody->GenerateCode(_module, _def, _compilationLog))) {
        return(err);
    }
    bool returns = (curBody->Returns());
    if(!returns) {
        if(!endBlock) {
            endBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
        }
        mReturns = false;
        builder->CreateBr(endBlock);
    }
    if(mElse) {
        builder->SetInsertPoint(elseBlock);
        mElse->GenerateCode(_module, _def, _compilationLog);
        returns = mElse->Returns();
        if(!returns) {
            if(!endBlock) {
                endBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
            }
            builder->CreateBr(endBlock);
            mReturns = false;
        }
    } else {
        mReturns = false;
    }
    if(!mReturns) {
        builder->SetInsertPoint(endBlock);
    }
    return(ok);
}

