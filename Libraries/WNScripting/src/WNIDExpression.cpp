// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNIDExpression.h"
#include "WNScripting/inc/WNScriptingAllocations.h"
#include "WNScripting/inc/WNCodeModule.h"
#include "WNScripting/inc/WNScopedVariableList.h"
#include "WNScripting/inc/WNTypeManager.h"

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

WNIDExpression::WNIDExpression(const char* _name) :
    mName(wn_nullptr){
    COPY_STRING(_name, mName);
}

WNIDExpression::~WNIDExpression() {
    wn::memory::free(mName);
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

