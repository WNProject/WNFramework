// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

// clang-format off
#include "WNDeprecatedScripting/inc/WNIncludeLLVM.h"
#include "llvm/IR/IRBuilder.h"
#include "WNDeprecatedScripting/inc/WNEndIncludeLLVM.h"
// clang-format on

#include "WNDeprecatedScripting/inc/WNCodeModule.h"
#include "WNDeprecatedScripting/inc/WNIDExpression.h"
#include "WNDeprecatedScripting/inc/WNScopedVariableList.h"
#include "WNDeprecatedScripting/inc/WNScriptingAllocations.h"
#include "WNDeprecatedScripting/inc/WNTypeManager.h"

using namespace WNScripting;

WNIDExpression::WNIDExpression(const char* _name) : mName(nullptr) {
  COPY_STRING(_name, mName);
}

WNIDExpression::~WNIDExpression() {
  wn::memory::heap_free(mName);
}

eWNTypeError WNIDExpression::GenerateCode(
    WNCodeModule& _module, const WNFunctionDefinition*, WNLogging::WNLog&) {
  llvm::IRBuilder<>* builder =
      reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
  const WNScriptVariable* var =
      _module.GetScopedVariableList().GetVariable(mName);
  eWNTypeError err = ok;
  if (var) {
    llvm::Value* loc = var->GetLocation();
    mValue = builder->CreateLoad(var->GetLocation(), false, "");
    mScriptType = var->GetType();
    mValueLocation = loc;
  } else {
    mValue = reinterpret_cast<llvm::Value*>(mName);
    if (ok != (err = _module.GetTypeManager().get_type_by_name(
                   "-Function", mScriptType))) {
      return (err);
    }
    return (ok);
  }
  return (ok);
}
