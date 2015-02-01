// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNFunction.h"
#include "WNScripting/inc/WNTypeElement.h"
#include "WNScripting/inc/WNDeclaration.h"
#include "WNScripting/inc/WNCodeModule.h"
#include "WNScripting/inc/WNDeclList.h"
#include "WNScripting/inc/WNTypeManager.h"
#include "WNScripting/inc/WNScopedVariableList.h"
#include "WNScripting/inc/WNInstructionList.h"
#include "WNScripting/inc/WNNodeHelpers.h"

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
#include "llvm/IR/Function.h"

#ifdef _WN_MSVC
    #pragma warning(pop)
#endif

using namespace WNScripting;

WNFunction::WNFunction(WNDeclaration* _decl, WNDeclList* _params, WNInstructionList* _body) :
    mDeclaration(_decl),
    mParameters(_params),
    mBody(_body),
    mThisType(wn_nullptr),
    mIsOverride(false),
    mIsVirtual(false){
}

WNFunction::~WNFunction() {
    WN_DELETE(mDeclaration);
    if(mParameters) {
        WN_DELETE(mParameters);
    }
    WN_DELETE(mBody);
}

eWNTypeError WNFunction::GenerateCode(WNCodeModule& _module, WNScriptType owningType, WNLogging::WNLog& _compilationLog) {
    if(mIsOverride && mIsVirtual) {
        _compilationLog.Log(WNLogging::eError,0, "Error: Function ", mDeclaration->GetName(), " can not be both virtual and override");
        LogLine(_compilationLog, WNLogging::eError);
        return(invalid_parameters);
    }
    mThisType = owningType;
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
    eWNTypeError err = ok;
    WNScriptType returnType;
    wn_char funcName[2048];
    funcName[0] = '\0';

    if(owningType && owningType->mName) {
        WNStrings::WNStrNCat(funcName, owningType->mName, 1024);
        WN_DEBUG_ASSERT(WNStrings::WNStrLen(funcName) == WNStrings::WNStrLen(owningType->mName));
        WNStrings::WNStrCat(funcName, "$");
    }
    WNStrings::WNStrNCat(funcName, mDeclaration->GetName(), 1024);


    if(ok != (err = mDeclaration->GetType(_module.GetTypeManager(), returnType, _compilationLog))) {
        _compilationLog.Log(WNLogging::eError, 0, "Error cannot resolve type");
        LogLine(_compilationLog, WNLogging::eError);
        return(err);
    }
    std::vector<WNScriptType> parameterTypes;
    if(owningType) {
        parameterTypes.insert(parameterTypes.begin(), owningType);
    }

    if(mParameters) {
        if(ok != (err = mParameters->GetTypeList(_module.GetTypeManager(), parameterTypes, _compilationLog))) {
            _compilationLog.Log(WNLogging::eError, 0, "Error invalid parameters");
            LogLine(_compilationLog, WNLogging::eError);
            return(err);
        }
    }

    WNFunctionDefinition * def = _module.GetFunctionDefinition(funcName, parameterTypes);
    if(!def) {
        _compilationLog.Log(WNLogging::eError, 0, "Function Definition does not exist: ", funcName);
        LogLine(_compilationLog, WNLogging::eError);
        return(error);
    }
    llvm::BasicBlock* bb = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", def->mFunction);
    builder->SetInsertPoint(bb);

    llvm::Function::arg_iterator fIter = def->mFunction->arg_begin();
    if(owningType) {
        fIter->setName("this");
        llvm::Value* mThis = builder->CreateAlloca(owningType->mLLVMType);
        builder->CreateStore(fIter, mThis);
        WNScriptVariable* var = WN_SCRIPTNODE_NEW(WNScriptVariable(owningType, "this", mThis, true));
        if(ok != (err = _module.GetScopedVariableList().PushVariable(var))) {
            _compilationLog.Log(WNLogging::eError, 0, "Variable already exists ", "this");
            LogLine(_compilationLog, WNLogging::eError);
            return(err);
        }

        std::vector<llvm::Value*> GepArray;
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 1));
        llvm::Value* tempValue = builder->CreateLoad(builder->CreateGEP(fIter, GepArray));


        for(size_t i = 0; i < owningType->mStructTypes.size(); ++i) {
            GepArray.clear();
            GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
            GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), i));
            llvm::Value* val = builder->CreateGEP(tempValue, GepArray);
            WNContainedStructType& st = owningType->mStructTypes[i];
            if(ok != (err = _module.GetScopedVariableList().PushVariable(new WNScriptVariable(st.mType, st.mName, val, true)))) {
                _compilationLog.Log(WNLogging::eError, 0, "Variable already exists ", st.mName);
                LogLine(_compilationLog, WNLogging::eError);
                return(err);
            }
        }
        ++fIter;
    }

    if(mParameters) {
        const WNScriptLinkedList<WNDeclaration>* declarations = NULL;
        if(ok != (err = mParameters->GetDeclarations(declarations))) {
            _compilationLog.Log(WNLogging::eError, 0, "Cannot find declarations for parameters ");
            LogLine(_compilationLog, WNLogging::eError);
            return(err);
        }

        WNScriptLinkedList<WNDeclaration>::WNScriptLinkedListNode* iter = declarations->first;
        for(;iter != wn_nullptr && fIter != def->mFunction->arg_end();
                                            iter = iter->next, ++fIter) {
            fIter->setName(iter->value->GetName());
            WNScriptType t = wn_nullptr;
            if(ok != (err = iter->value->GetType(_module.GetTypeManager(), t, _compilationLog))) {
                _compilationLog.Log(WNLogging::eError, 0, "Cannot find type for declaration: ", iter->value->GetName());
                LogLine(_compilationLog, WNLogging::eError);
                return(err);
            }
            const GenerateAllocation* alloc = _module.GetTypeManager().GetAllocationOperation(t);
            if(!alloc) {
                _compilationLog.Log(WNLogging::eError, 0, "Cannot allocate variable of type: ", t->mName);
                LogLine(_compilationLog, WNLogging::eError);
                return(eWNCannotCreateType);
            }
            llvm::Value* allocLoc = wn_nullptr;
            if(ok != (err = alloc->Execute(_module, t, iter->value->GetName(), false, allocLoc))) {
                _compilationLog.Log(WNLogging::eCritical, 0, "Error allocating a variable of type: ", t->mName);
                LogLine(_compilationLog, WNLogging::eCritical);
                return(err);
            }
            const GenerateAssignment* assign = _module.GetTypeManager().GetAssignmentOperation(t, AT_EQ);
            if(!assign)
            {
                _compilationLog.Log(WNLogging::eError, 0, "Cannot assign a variable of type: ", t->mName);
                LogLine(_compilationLog, WNLogging::eError);
            }
            if(ok != (err = assign->Execute(_module, true, t, fIter, allocLoc, def, _compilationLog))) {
                _compilationLog.Log(WNLogging::eCritical, 0, "Error assigning a variable of type: ", t->mName);
                LogLine(_compilationLog, WNLogging::eCritical);
                return(err);
            }
            WNScriptVariable* var = WN_SCRIPTNODE_NEW(WNScriptVariable(t, iter->value->GetName(), allocLoc));
            if(ok != (err = _module.GetScopedVariableList().PushVariable(var))) {
                _compilationLog.Log(WNLogging::eError, 0, "Variable already exists ", iter->value->GetName());
                LogLine(_compilationLog, WNLogging::eError);
                return(err);
            }
        }
        if(fIter != def->mFunction->arg_end() && iter != wn_nullptr) {
            _compilationLog.Log(WNLogging::eError, 0, "Invalid number of parameters to function");
            LogLine(_compilationLog, WNLogging::eError);
            return(error);
        }
    }

    if(ok != (err = mBody->GenerateCode(_module, def, _compilationLog))) {
        return(err);
    }
    if(!mBody->Returns()) {
        if(ok != (err = _module.GetScopedVariableList().GenerateReturn(_module, def, _compilationLog))) {
            _compilationLog.Log(WNLogging::eCritical, 0, "Cannot create proper return");
            LogLine(_compilationLog, WNLogging::eCritical);
            return(err);
        }
        if(ok != (err = GenerateVOIDReturn(_module, def))) {
            _compilationLog.Log(WNLogging::eError, 0, "Trying to create void return for non-void function");
            LogLine(_compilationLog, WNLogging::eError);
            return(err);
        }
    }
    _module.GetScopedVariableList().ClearScope();
    builder->ClearInsertionPoint();

    return(ok);
}

eWNTypeError WNFunction::ExportFunctions(WNCodeModule& _module, WNScriptType owningType, std::vector<WNFunctionDefinition*>& _definitions, WNLogging::WNLog& _compilationLog) {
    eWNTypeError err = ok;
    WNScriptType returnType;

    wn_char funcName[2048];
    funcName[0] = '\0';
    if(mIsOverride && !owningType) {
        _compilationLog.Log(WNLogging::eError, 0, "Error it is invalid to have an override function that is not in a class");
        LogLine(_compilationLog, WNLogging::eError);
    }


    if(owningType && owningType->mName) {
        WNStrings::WNStrNCat(funcName, owningType->mName, 1024);
        WN_DEBUG_ASSERT(WNStrings::WNStrLen(funcName) == WNStrings::WNStrLen(owningType->mName));
        WNStrings::WNStrCat(funcName, "$");
    }
    WNStrings::WNStrNCat(funcName, mDeclaration->GetName(), 1024);

    if(ok != (err = mDeclaration->GetType(_module.GetTypeManager(), returnType, _compilationLog))) {
        return(err);
    }
    std::vector<WNScriptType> parameterTypes;
    if(owningType) {
        parameterTypes.push_back(owningType);
    }
    if(mParameters) {
        if(ok != (err = mParameters->GetTypeList(_module.GetTypeManager(), parameterTypes, _compilationLog))) {
            LogLine(_compilationLog, WNLogging::eError);
            return(err);
        }
    }
    WNFunctionDefinition* def = wn_nullptr;
    if(ok != (err = _module.GenerateFunctionDefinition(funcName, parameterTypes, returnType, def, owningType, mIsVirtual || mIsOverride))) {
        parameterTypes.clear();
        return(err);
    }

    std::vector<llvm::Type*> llvmTypes;
    for(std::vector<WNScriptType>::iterator i = parameterTypes.begin(); i != parameterTypes.end(); ++i) {
        llvmTypes.push_back((*i)->mLLVMType);
    }

    llvm::FunctionType* fType = llvm::FunctionType::get(returnType->mLLVMType, llvmTypes, wn_false);
    def->mFunctionType = fType;

    _definitions.push_back(def);
    if(owningType && mIsVirtual) {
        owningType->mVTable.push_back(def);
    }
    if(owningType && mIsOverride) {
        owningType->mOverridenFunctions.push_back(def);
    }
    return(ok);
}

eWNTypeError WNFunction::GenerateHeader(WNCodeModule& _module, WNScriptType owningType, WNLogging::WNLog& _compilationLog) {

    eWNTypeError err = ok;
    WNScriptType returnType;
    wn_char funcName[2048];
    funcName[0] = '\0';

    if(owningType && owningType->mName) {
        WNStrings::WNStrNCat(funcName, owningType->mName, 1024);
        WN_DEBUG_ASSERT(WNStrings::WNStrLen(funcName) == WNStrings::WNStrLen(owningType->mName));
        WNStrings::WNStrCat(funcName, "$");
    }
    WNStrings::WNStrNCat(funcName, mDeclaration->GetName(), 1024);


    if(ok != (err = mDeclaration->GetType(_module.GetTypeManager(), returnType, _compilationLog))) {
        return(err);
    }
    std::vector<WNScriptType> parameterTypes;
    if(owningType) {
        parameterTypes.push_back(owningType);
    }

    if(mParameters) {
        if(ok != (err = mParameters->GetTypeList(_module.GetTypeManager(), parameterTypes, _compilationLog))) {
            LogLine(_compilationLog, WNLogging::eError);
            return(err);
        }
    }
    WNFunctionDefinition* def = wn_nullptr;
    WNFunctionDefinition* equivDef = wn_nullptr;
    if(ok != (err = _module.AddFunctionDefinition(funcName, parameterTypes, returnType, def, equivDef, owningType))) {
        if(err != eWNAlreadyExists || !def->mCurrentFile) {
            parameterTypes.clear();
            return(err);
        }
    }

    std::vector<llvm::Type*> llvmTypes;
    for(std::vector<WNScriptType>::iterator i = parameterTypes.begin(); i != parameterTypes.end(); ++i) {
        llvmTypes.push_back((*i)->mLLVMType);
    }

    llvm::FunctionType* fType = llvm::FunctionType::get(returnType->mLLVMType, llvmTypes, wn_false);
    llvm::Function* func = llvm::Function::Create(fType, llvm::GlobalValue::ExternalLinkage, funcName, _module.GetModule());
    def->mFunction = func;
    def->mFunctionType = fType;

    return(ok);
}

void WNFunction::SetVirtual() {
    mIsVirtual = true;
}

void WNFunction::SetOverride() {
    mIsOverride = true;
}