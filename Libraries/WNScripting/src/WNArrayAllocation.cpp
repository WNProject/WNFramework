// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNArrayAllocation.h"
#include "WNScripting/inc/WNTypeNode.h"
#include "WNScripting/inc/WNCodeModule.h"
#include "WNScripting/inc/WNTypeManager.h"
#include "WNScripting/inc/WNNodeHelpers.h"

using namespace WNScripting;

WNArrayAllocation::WNArrayAllocation() :
    mLevels(0),
    mType(wn_nullptr),
    mCopyInitializer(wn_nullptr) {
}

WNArrayAllocation::~WNArrayAllocation() {
    if(mType) {
        wn::memory::destroy(mType);
    }
    for(WNScriptLinkedList<WNExpression>::WNScriptLinkedListNode* i = mArrayInitializers.first; i != wn_nullptr; i = i->next) {
        wn::memory::destroy(i->value);
    }
}

wn_void WNArrayAllocation::SetType(WNTypeNode* _typeNode) {
    mType = _typeNode;
}

wn_void WNArrayAllocation::AddExpression(WNExpression* _expr) {
    mArrayInitializers.PushBack(_expr);
    mLevels++;
}

wn_void WNArrayAllocation::AddLevel() {
    mLevels++;
}

eWNTypeError WNArrayAllocation::GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) {
    mNewlyCreated = wn_true;
    mForceUse = wn_true;
    eWNTypeError err = ok;

    if(mArrayInitializers.first != wn_nullptr && mCopyInitializer != wn_nullptr) {
        _compilationLog.Log(WNLogging::eError, 0, "Cannot specify both size and copy construction");
        LogLine(_compilationLog, WNLogging::eError);
    }


    if(ok != (err = mType->GetType(_module.GetTypeManager(), mScriptType, _compilationLog))){
        _compilationLog.Log(WNLogging::eError, 0, "Cannot find type ");
        LogLine(_compilationLog, WNLogging::eError);
        return(err);
    }
    for(wn_size_t i = 0; i < mLevels; ++i){
        if(ok != (err = _module.GetTypeManager().GetArrayOf(mScriptType, mScriptType))) {
            _compilationLog.Log(WNLogging::eError, 0, "Cannot create array of ", mScriptType->mName);
            LogLine(_compilationLog, WNLogging::eError);
            return(err);
        }
    }
    if(mCopyInitializer) {
        const GenerateCopyConstruction* construction = _module.GetTypeManager().GetCopyConstructionOperation(mScriptType);
        if(!construction) {
            _compilationLog.Log(WNLogging::eError, 0, "Could not find copy construction operation for ", mScriptType->mName);
            LogLine(_compilationLog, WNLogging::eError);
            return(eWNCannotCreateType);
        }

        if(ok != (err = mCopyInitializer->GenerateCode(_module, _def, _compilationLog))) {
            return(err);
        }
        llvm::Value* copyValue = mCopyInitializer->GetValue();
        if(mCopyInitializer->RequiredUse()) {
            _compilationLog.Log(WNLogging::eError, 0, "Cannot copy construct temporary value, construct instead");
            LogLine(_compilationLog, WNLogging::eError);
            return(eWNInvalidCast);
        }
        if(mCopyInitializer->GetType() != mScriptType) {
            const GenerateCastingOperation* castOp = _module.GetTypeManager().GetCastingOperation(mCopyInitializer->GetType(), mScriptType);
            if(!castOp) {
                _compilationLog.Log(WNLogging::eError, 0, "Cannot construct array of type ", mScriptType->mName, " from type ", mCopyInitializer->GetType()->mName, " different type");
                LogLine(_compilationLog, WNLogging::eError);
                return(eWNInvalidCast);
            } else {
                if(ok != (err = castOp->Execute(_module.GetBuilder(), copyValue, copyValue))) {
                    _compilationLog.Log(WNLogging::eCritical, 0, "Error casting ", mCopyInitializer->GetType()->mName, " from type ", mScriptType->mName, " different type");
                    LogLine(_compilationLog, WNLogging::eError);
                    return(eWNInvalidCast);
                }
            }
        }

        if(ok != (err = construction->Execute(_module, mValue, mScriptType, copyValue, _def, _compilationLog))) {
            LogLine(_compilationLog, WNLogging::eError);
            return(err);
        }
    } else {
        const GenerateConstruction* construction = _module.GetTypeManager().GetConstructionOperation(mScriptType);
        if(!construction) {
            _compilationLog.Log(WNLogging::eError, 0, "Could not find construction operation for ", mScriptType->mName);
            LogLine(_compilationLog, WNLogging::eError);
            return(eWNCannotCreateType);
        }
        if(ok != (err = construction->Execute(_module, mValue, mScriptType, mArrayInitializers.first, _def, _compilationLog))) {
            LogLine(_compilationLog, WNLogging::eError);
            return(err);
        }
    }
    return(ok);
}

wn_void WNArrayAllocation::SetCopyInitializer(WNExpression* _expression) {
    mCopyInitializer = _expression;
}