// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNFunctionCallExpression.h"
#include "WNScripting/inc/WNCodeModule.h"
#include "WNScripting/inc/WNTypeManager.h"
#include "WNScripting/inc/WNExpression.h"
#include "WNScripting/inc/WNScriptLinkedList.h"
#include "WNScripting/inc/WNNodeHelpers.h"
#include "WNScripting/inc/WNArgList.h"

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
WNFunctionCallExpr::WNFunctionCallExpr() :
    mArgs(wn_nullptr) {

}

WNFunctionCallExpr::WNFunctionCallExpr(WNArgList* _list) :
    mArgs(_list) {
}

WNFunctionCallExpr::~WNFunctionCallExpr() {
    if(mArgs) {
        WN_DELETE(mArgs);
    }
}

eWNTypeError WNFunctionCallExpr::GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
    eWNTypeError err = ok;
    WNScriptType voidType;
    if(ok != (err = _module.GetTypeManager().GetTypeByName("Void", voidType))) {
        _compilationLog.Log(WNLogging::eCritical, 0, "Cannot find void type");
        LogLine(_compilationLog, WNLogging::eCritical);
        return(err);
    }

    WNScriptType functionType;
    if(ok != (err = mBaseExpression->GenerateCode(_module, _def, _compilationLog))) {
        return(err);
    }
    if(ok != (err = _module.GetTypeManager().GetTypeByName("-Function", functionType))) {
        _compilationLog.Log(WNLogging::eCritical, 0, "Cannot find function type");
        LogLine(_compilationLog, WNLogging::eCritical);
        return(err);
    }

    if(mBaseExpression->GetType() != functionType) {
        _compilationLog.Log(WNLogging::eCritical, 0, "Function is of wrong type");
        LogLine(_compilationLog, WNLogging::eCritical);
        return(eWNInvalidOperation);
    }

    llvm::Value* testValue = mBaseExpression->GetSubValue();
    WNScriptType testType = mBaseExpression->GetSubType();
    bool forceThis = false;
    if((testValue == wn_nullptr || testType == wn_nullptr) && _def->mThisType != wn_nullptr) {
        testType = _def->mThisType;
        const WNScriptVariable* var = _module.GetScopedVariableList().GetVariable("this");
        if(!var) {
            _compilationLog.Log(WNLogging::eCritical, 0, "Error cannot find \'this\'");
            LogLine(_compilationLog, WNLogging::eCritical);
            return(eWNInvalidOperation);
        }
        testValue = builder->CreateLoad(var->GetLocation());
    } else {
        forceThis = true;
    }

    static WNScriptLinkedList<WNFunctionExpression> staticList;
    WNScriptLinkedList<WNFunctionExpression>& localExpressionList = mArgs? mArgs->GetExpressions() : staticList;
    std::vector<FunctionParam> scriptTypes;
    for(WNScriptLinkedList<WNFunctionExpression>::WNScriptLinkedListNode* i = localExpressionList.first; i != wn_nullptr; i = i->next) {
        if(ok != (err = i->value->expr->GenerateCode(_module, _def, _compilationLog))) {
            return(err);
        }
        if(i->value->expr->RequiredUse()) {
            _compilationLog.Log(WNLogging::eError, 0, "You must assign parameter before passing to subsequent function");
            LogLine(_compilationLog, WNLogging::eError);
            return(error);
        }
        scriptTypes.push_back(FunctionParam());
        scriptTypes.back().mValue = i->value->expr->GetValue();
        scriptTypes.back().mType = i->value->expr->GetType();
        if(i->value->handOwnership) { // looks like Int a = x(<== y);
             #ifdef _WN_64_BIT
                llvm::Type* ptrType = llvm::IntegerType::getInt64Ty(llvm::getGlobalContext());
            #else
                llvm::Type* ptrType = llvm::IntegerType::getInt32Ty(llvm::getGlobalContext());
            #endif
            //We want to throw away our ownership of the object
            if(!(i->value->expr->GetType()->mLLVMStructType && i->value->expr->GetValueLocation()))
            {
                //we are trying to give away ownership of a non-obect or a non-referencable object
                _compilationLog.Log(WNLogging::eError, 0, "Trying to give away ownership of invalid object");
                LogLine(_compilationLog, WNLogging::eError);
                return(error);
            }
            llvm::Value* structLoc = i->value->expr->GetValueLocation();
            std::vector<llvm::Value*> GepArray;
            GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
            llvm::Value* spLoc = builder->CreateLoad(builder->CreateGEP(structLoc, GepArray));
            GepArray.clear();
            GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
            GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
            llvm::Value* ownLocation= builder->CreateGEP(spLoc, GepArray);
            builder->CreateStore(llvm::ConstantInt::get(ptrType, 0), builder->CreatePointerCast(ownLocation, ptrType->getPointerTo(0)));
        }
    }

    if(testType) {
        FunctionParam param = {testValue, testType};
        if(ok != (err = GenerateRecursiveThisFunctionCall(_module,
                                                 _def,
                                                 reinterpret_cast<wn_char*>(mBaseExpression->GetValue()),
                                                 scriptTypes,
                                                 mScriptType,
                                                 mValue,
                                                 param,
                                                 _compilationLog))) {
            if(forceThis) { //we made an explicit function call (a.doSomething()),
                            //therefore we CANNOT remove the "this" parameter
                _compilationLog.Log(WNLogging::eError, 0, "Cannot create function call");
                LogLine(_compilationLog, WNLogging::eError);
                return(err);
            }
        } else {
            if(mScriptType != voidType) {
                mForceUse = true;
            }
            return(ok);
        }
    }
    if(ok != (err = GenerateFunctionCall(_module,
                                                 _def,
                                                 reinterpret_cast<wn_char*>(mBaseExpression->GetValue()),
                                                 scriptTypes,
                                                 true,
                                                 mScriptType,
                                                 mValue,
                                                 _compilationLog))) {
        _compilationLog.Log(WNLogging::eError, 0, "Cannot create function call");
        LogLine(_compilationLog, WNLogging::eError);
        return(err);
    }
    if(mScriptType != voidType) {
        mForceUse = true;
    }
    return(ok);
}

