// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNDeclaration.h"
#include "WNScripting/inc/WNTypeNode.h"
#include "WNScripting/inc/WNCodeModule.h"
#include "WNScripting/inc/WNTypeManager.h"
#include "WNScripting/inc/WNScopedVariableList.h"
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
WNDeclaration::WNDeclaration(WNTypeNode* _type, const char* _name) :
    mType(_type),
    mName(WN_NULL),
    mExpr(WN_NULL),
    mScalarType(WN_NULL),
    mUnsizedArrayInitializers(0),
    mInitAssign(WN_FALSE) {
    COPY_STRING(_name, mName);
}

WNDeclaration::~WNDeclaration() {
    WN_DELETE(mType);
    WN_DELETE(mName);
    if(mExpr) {
        WN_DELETE(mExpr);
    }
    if(mScalarType) {
        WN_DELETE(mScalarType);
    }
}

const WN_CHAR* WNDeclaration::GetName() {
    return(mName);
}

eWNTypeError WNDeclaration::GetType(WNTypeManager& _manager, WNScriptType& _scriptType, WNLogging::WNLog& _compilationLog) {
    return(mType->GetType(_manager, _scriptType, _compilationLog));
}

void WNDeclaration::AddExpressionInitializer(WNExpression* _expr, bool _assign) {
    mExpr = _expr;
    mInitAssign = _assign;
}

void WNDeclaration::AddSizedArrayInitializer(WNExpression* _expr) {
    mSizedArrayInitializers.PushBack(_expr);
}

void WNDeclaration::AddUnsizedArrayInitializer() {
    mUnsizedArrayInitializers += 1;
}

eWNTypeError WNDeclaration::GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
    WNScriptType t;
    llvm::Value* allocLocation = WN_NULL;
    eWNTypeError err = eWNOK;

    if(_module.GetScopedVariableList().GetVariable(mName)){
        _compilationLog.Log(WNLogging::eError, 0, "Error ", mName, " already exists");
        LogLine(_compilationLog, WNLogging::eError);
        return(eWNAlreadyExists);
    }

    if(eWNOK != (err = mType->GetType(_module.GetTypeManager(), t, _compilationLog))) {
        _compilationLog.Log(WNLogging::eError, 0, "Error, cannot find type");
        LogLine(_compilationLog, WNLogging::eError);
        return(err);
    }
    const GenerateAllocation* alloc = _module.GetTypeManager().GetAllocationOperation(t);
    if(!alloc) {
        _compilationLog.Log(WNLogging::eError, 0, "Error, cannot create allocation for type", t->mName);
        LogLine(_compilationLog, WNLogging::eError);
        return(eWNCannotCreateType);
    }
    if(eWNOK != (err = alloc->Execute(_module, t, mName, false, allocLocation))) {
        _compilationLog.Log(WNLogging::eCritical, 0, "Error, creating allocation for type", t->mName);
        LogLine(_compilationLog, WNLogging::eCritical);
        return(err);
    }

    if(mExpr) {
        
        if(eWNOK != (err = mExpr->GenerateCode(_module, _def, _compilationLog))) {
            return(err);
        }
        if(!mExpr->GetValue()){
            _compilationLog.Log(WNLogging::eError, 0, "Error, something wrong with value for expression");
            mExpr->LogLine(_compilationLog, WNLogging::eError);
            return(eWNInvalidCast);
        }
        const GenerateAssignment* assign = _module.GetTypeManager().GetAssignmentOperation(t, (mInitAssign || mExpr->IsNewlyCreated())? AT_CHOWN : AT_EQ);
        if(!assign) {
            _compilationLog.Log(WNLogging::eError, 0, "Error, cannot create assignment for type", t->mName);
            LogLine(_compilationLog, WNLogging::eError);
            return(eWNCannotCreateType);
        }
        llvm::Value* val = mExpr->GetValue();
        if(mExpr->GetType() != t) {
            const GenerateCastingOperation* cast = _module.GetTypeManager().GetCastingOperation(mExpr->GetType(), t);
            if(!cast) {
                _compilationLog.Log(WNLogging::eError, 0, "Error, cannot cast from ", mExpr->GetType()->mName, " to ", t->mName);
                mExpr->LogLine(_compilationLog, WNLogging::eError);
                return(eWNInvalidCast);
            }
            if(eWNOK != (err = cast->Execute(builder, val, val))) {
                _compilationLog.Log(WNLogging::eCritical, 0, "Error, generating cast from ", mExpr->GetType()->mName, " to ", t->mName);
                mExpr->LogLine(_compilationLog, WNLogging::eCritical);
                return(err);
            }
        }
        if(eWNOK != (err = assign->Execute(_module, true, t, val, allocLocation, _def, _compilationLog))) {
            return(err);
        }
    }
    WNScriptVariable* var = WN_SCRIPTNODE_NEW(WNScriptVariable(t, mName, allocLocation));
    if(eWNOK != (err = _module.GetScopedVariableList().PushVariable(var))) {
        return(err);
    }
    return(eWNOK);
}

WNExpression* WNDeclaration::GetExpression() {
    return(mExpr);
}

