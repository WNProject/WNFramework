// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNIncludeLLVM.h"
#include "llvm/IR/IRBuilder.h"
#include "WNScripting/inc/WNEndIncludeLLVM.h"

#include "WNScripting/inc/WNIDExpression.h"
#include "WNScripting/inc/WNScriptingAllocations.h"
#include "WNScripting/inc/WNCodeModule.h"
#include "WNScripting/inc/WNScopedVariableList.h"
#include "WNScripting/inc/WNTypeManager.h"

using namespace WNScripting;

WNIDExpression::WNIDExpression(const char* _name) :
    mName(wn_nullptr){
    COPY_STRING(_name, mName);
}

WNIDExpression::~WNIDExpression() {
    wn::memory::heap_free(mName);
}

eWNTypeError WNIDExpression::GenerateCode(WNCodeModule& _module, const WNFunctionDefinition*, WNLogging::WNLog&) {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
    const WNScriptVariable* var = _module.GetScopedVariableList().GetVariable(mName);
    eWNTypeError err = ok;
    if(var) {
        llvm::Value* loc = var->GetLocation();
        mValue = builder->CreateLoad(var->GetLocation(), false, "");
        mScriptType = var->GetType();
        mValueLocation = loc;
    } else {
        mValue = reinterpret_cast<llvm::Value*>(mName);
        if(ok != (err = _module.GetTypeManager().GetTypeByName("-Function", mScriptType))) {
            return(err);
        }
        return(ok);
    }
    return(ok);
}

