// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.


#include "WNDeprecatedScripting/inc/WNIncludeLLVM.h"
#include "llvm/IR/IRBuilder.h"
#include "WNDeprecatedScripting/inc/WNEndIncludeLLVM.h"

#include "WNDeprecatedScripting/inc/WNNodeHelpers.h" // This is intentionally out of order.
// If this is included lower, it will fail because windows.h conflitcs wiht llvm.

#include "WNDeprecatedScripting/inc/WNStructAllocation.h"
#include "WNDeprecatedScripting/inc/WNCodeModule.h"
#include "WNDeprecatedScripting/inc/WNTypeNode.h"

using namespace WNScripting;

WNStructAllocation::WNStructAllocation() :
    mType(nullptr),
    mCopyinitializer(nullptr) {
}

WNStructAllocation::~WNStructAllocation() {
    if(mType) {
        wn::memory::destroy(mType);
    }
}

void WNStructAllocation::SetType(WNTypeNode* _type) {
    mType = _type;
}

void WNStructAllocation::SetCopyInitializer(WNExpression* _expr) {
    mCopyinitializer = _expr;
}

eWNTypeError WNStructAllocation::GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) {
    mNewlyCreated = true;
    mForceUse = true;
    WNScriptType structType = nullptr;
    eWNTypeError err = ok;
    if(ok != (err = mType->GetType(_module.GetTypeManager(), structType, _compilationLog))) {
        return(err);
    }
    if(!structType->mLLVMStructType) {
        _compilationLog.Log(WNLogging::eError, 0, structType->mName, " is not a valid Struct type");
        LogLine(_compilationLog, WNLogging::eError);
        return(eWNBadType);
    }
    llvm::Value* copyValue = nullptr;
    if(mCopyinitializer) {
        if(ok != (err = mCopyinitializer->GenerateCode(_module, _def, _compilationLog))) {
            return(err);
        }
        if(mCopyinitializer->GetType() != structType) {
            const GenerateCastingOperation* op = _module.GetTypeManager().GetCastingOperation(mCopyinitializer->GetType(), structType);
            if(!op) {
                _compilationLog.Log(WNLogging::eError, 0, "Could not copy construct object of type ", structType->mName, " with object of type ", mCopyinitializer->GetType()->mName);
                LogLine(_compilationLog, WNLogging::eError);
                return(eWNInvalidCast);
            }
            if(ok != (err = op->Execute(_module.GetBuilder(), mCopyinitializer->GetValue(), copyValue))) {
                _compilationLog.Log(WNLogging::eCritical, 0, "Error copy constructing ", structType->mName );
                LogLine(_compilationLog, WNLogging::eCritical);
                return(eWNBadType);
            }
        } else {
            copyValue = mCopyinitializer->GetValue();
        }
    }

    llvm::Value* outValue = NULL;
    if(!mCopyinitializer) {
        const GenerateConstruction* construction = _module.GetTypeManager().GetConstructionOperation(structType);
        if(!construction) {
            _compilationLog.Log(WNLogging::eCritical, 0, "No Constructor for ", structType->mName );
            LogLine(_compilationLog, WNLogging::eCritical);
        }
        if(ok != (err = construction->Execute(_module, outValue, structType, nullptr, _def, _compilationLog))) {
            _compilationLog.Log(WNLogging::eError, 0, "Cannot allocate Struct: ", structType->mName);
            LogLine(_compilationLog, WNLogging::eError);
            return(err);
        }
    } else {
        const GenerateCopyConstruction* construction = _module.GetTypeManager().GetCopyConstructionOperation(structType);
        if(!construction) {
            _compilationLog.Log(WNLogging::eCritical, 0, "No Constructor for ", structType->mName );
            LogLine(_compilationLog, WNLogging::eCritical);
        }
        if(ok != (err = construction->Execute(_module, outValue, structType, copyValue, _def, _compilationLog))) {
            _compilationLog.Log(WNLogging::eError, 0, "Cannot allocate Struct: ", structType->mName);
            LogLine(_compilationLog, WNLogging::eError);
            return(err);
        }
    }

    mValue = outValue;
    mScriptType = structType;
    return(ok);
}

