// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNStruct.h"
#include "WNScripting/inc/WNScriptingAllocations.h"
#include "WNScripting/inc/WNCodeModule.h"
#include "WNScripting/inc/WNTypeManager.h"
#include "WNScripting/inc/WNScopedVariableList.h"
#include "WNScripting/inc/WNNodeHelpers.h"
#include "WNScripting/inc/WNDeclaration.h"
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

WNStruct::WNStruct(const char* _name, bool _isClass, const char* _parentName): mParentType(wn_nullptr), mIsClass(_isClass), mType(wn_nullptr) {
    COPY_STRING(_name, mName);
    COPY_STRING(_parentName, mParentName);
}

WNStruct::~WNStruct() {
    WN_DELETE(mName);
    for(WNScriptLinkedList<WNDeclaration>::WNScriptLinkedListNode* i = mStructMembers.first; i != wn_nullptr; i=i->next) {
        WN_DELETE(i->value);
    }
    if(mParentName) {
        WN_DELETE(mParentName);
    }
}

void WNStruct::AddStructElem(WNDeclaration* _decl) {
    mStructMembers.PushBack(_decl);
}

void WNStruct::AddFunction(WNFunction* _func) {
    mStructFunctions.PushBack(_func);
}

eWNTypeError WNStruct::ExportNewTypes(WNCodeModule& _module, std::vector<WNScriptType>& _types, std::vector<WNFunctionDefinition*>& _functionDefs, WNLogging::WNLog& _compilationLog) {
    eWNTypeError err = ok;

    WNScriptType scriptType = NULL;
    if(ok != (err = _module.GetTypeManager().RegisterStructType(mName, _module.GetScriptingEngine(), scriptType))) {
        if(err != eWNAlreadyExists) {
            _compilationLog.Log(WNLogging::eError, 0, "type cannot be created");
            LogLine(_compilationLog, WNLogging::eError);
            return(err);
        } else {
            if(!scriptType->mLLVMStructType) {
                _compilationLog.Log(WNLogging::eError, 0, "Cannot create type ", mName, " since it already as an external type");
                LogLine(_compilationLog, WNLogging::eError);
                return(err);
            }
        }
    }

    WNScriptType voidReturnType;
    if(ok != (err = _module.GetTypeManager().GetTypeByName("Void", voidReturnType))) {
        _compilationLog.Log(WNLogging::eCritical, 0, "Cannot find Void type");
        LogLine(_compilationLog, WNLogging::eCritical);
        return(err);
    }

    std::vector<WNScriptType> parameterTypes;
    parameterTypes.push_back(scriptType);
    wn_size_t nameLen = WNStrings::WNStrLen(mName);
    wn_char *functionName = static_cast<wn_char*>(WN_STACK_ALLOC(sizeof(wn_char) * (nameLen + 1 + 9)));
    WNMemory::WNMemCpy(functionName, mName, nameLen);
    WNMemory::WNMemCpy(functionName + nameLen, "Const", 6);

    WNFunctionDefinition* constDef = wn_nullptr;
    if(ok != (err = _module.GenerateFunctionDefinition(functionName, parameterTypes, voidReturnType, constDef))) {
        parameterTypes.clear();
        _compilationLog.Log(WNLogging::eCritical, 0, "Cannnot create constructor definition");
        LogLine(_compilationLog, WNLogging::eCritical);
        return(err);
    }

    std::vector<llvm::Type*> llvmTypes;
    for(std::vector<WNScriptType>::iterator i = parameterTypes.begin(); i != parameterTypes.end(); ++i) {
        llvmTypes.push_back((*i)->mLLVMType);
    }

    llvm::FunctionType* fType = llvm::FunctionType::get(voidReturnType->mLLVMType, llvmTypes, wn_false);

    constDef->mFunctionType = fType;
    _functionDefs.push_back(constDef);

    WNMemory::WNMemCpy(functionName + nameLen, "Destr", 6);
    WNFunctionDefinition* destrDef = wn_nullptr;
    if(ok != (err = _module.GenerateFunctionDefinition(functionName, parameterTypes, voidReturnType, destrDef))) {
        parameterTypes.clear();
        _compilationLog.Log(WNLogging::eCritical, 0, "Cannnot create destructor definition");
        LogLine(_compilationLog, WNLogging::eCritical);
        return(err);
    }

    llvmTypes.clear();
    for(std::vector<WNScriptType>::iterator i = parameterTypes.begin(); i != parameterTypes.end(); ++i) {
        llvmTypes.push_back((*i)->mLLVMType);
    }

    fType = llvm::FunctionType::get(voidReturnType->mLLVMType, llvmTypes, wn_false);
    destrDef->mFunctionType = fType;
    _functionDefs.push_back(destrDef);

    WNMemory::WNMemCpy(functionName + nameLen, "CopyConst", 10);
    WNFunctionDefinition* copyDef = wn_nullptr;
    parameterTypes.push_back(parameterTypes.front());
    if(ok != (err = _module.GenerateFunctionDefinition(functionName, parameterTypes, voidReturnType, copyDef))) {
        parameterTypes.clear();
        _compilationLog.Log(WNLogging::eCritical, 0, "Cannnot create destructor definition");
        LogLine(_compilationLog, WNLogging::eCritical);
        return(err);
    }

    llvmTypes.clear();
    for(std::vector<WNScriptType>::iterator i = parameterTypes.begin(); i != parameterTypes.end(); ++i) {
        llvmTypes.push_back((*i)->mLLVMType);
    }

    fType = llvm::FunctionType::get(voidReturnType->mLLVMType, llvmTypes, wn_false);
    copyDef->mFunctionType = fType;
    _functionDefs.push_back(copyDef);

    _types.push_back(scriptType);
    return(ok);
}

eWNTypeError WNStruct::FinalizeNewTypes(WNCodeModule& _module, std::vector<WNFunctionDefinition*>& _definitions, WNLogging::WNLog& _compilationLog) {
    eWNTypeError err = ok;
    WNScriptType scriptType = NULL;

    if(ok != (err = _module.GetTypeManager().GetTypeByName(mName, scriptType))) {
        _compilationLog.Log(WNLogging::eError, 0, "Cannot find type ", mName );
        LogLine(_compilationLog, WNLogging::eError);
        return(err);
    }
    if(!scriptType->mLLVMStructType) {
        _compilationLog.Log(WNLogging::eError, 0, "Not a valid struct type ", scriptType->mName );
        LogLine(_compilationLog, WNLogging::eError);
        return(eWNBadType);
    }

    if(scriptType->mFinalizedStruct) {
        return(ok);
    }
    if(mParentName) {
        if(ok != (err = _module.GetTypeManager().GetTypeByName(mParentName, mParentType))) {
            _compilationLog.Log(WNLogging::eError, 0, "Cannot resolve parent type ", mParentName);
            LogLine(_compilationLog, WNLogging::eError);
            return(err);
         }
    }
    eWNTypeError retVal = ok;
    if(!mParentType || mParentType->mFinalizedStruct) {
        if(ok != (err = FinalizeStruct(_module.GetTypeManager(), _compilationLog))) {
            return(err);
        }
    } else {
        retVal = eWNUnknownResolution;
    }

    for(WNScriptLinkedList<WNFunction>::WNScriptLinkedListNode* i = mStructFunctions.first; i != wn_nullptr; i = i->next) {
        if(ok != (err = i->value->ExportFunctions(_module, scriptType, _definitions, _compilationLog))) {
            return(err);
        }
    }

    return(retVal);
}

eWNTypeError WNStruct::FinalizeStruct(WNTypeManager& _manager, WNLogging::WNLog& _compilationLog) {
    eWNTypeError err = ok;

    WNScriptType scriptType = NULL;

    if(ok != (err = _manager.GetTypeByName(mName, scriptType))) {
        _compilationLog.Log(WNLogging::eError, 0, "Cannot find type ", mName );
        LogLine(_compilationLog, WNLogging::eError);
        return(err);
    }

    if(scriptType->mFinalizedStruct) {
        return(ok);
    }
    if(mParentType && !mParentType->mFinalizedStruct) {
        return(eWNUnknownResolution);
    }
    std::vector<llvm::Type*> llvmTypes;

    for(WNScriptLinkedList<WNDeclaration>::WNScriptLinkedListNode* i = mStructMembers.first; i != wn_nullptr; i = i->next) {
        for(WNScriptLinkedList<WNDeclaration>::WNScriptLinkedListNode* j = i->next; j != wn_nullptr; j = j->next) {
            if(WNStrings::WNStrCmp(i->value->GetName(), j->value->GetName()) == 0){
                _compilationLog.Log(WNLogging::eError, 0, " Member variable already exists ", i->value->GetName());
                j->value->LogLine(_compilationLog, WNLogging::eError);
                return(eWNAlreadyExists);
            }
        }
    }

    if(mParentType) //eat all of our parent contained types
    {
        for(std::vector<WNContainedStructType>::iterator i = mParentType->mStructTypes.begin(); i != mParentType->mStructTypes.end(); ++i) {
            scriptType->mStructTypes.push_back(*i);
            llvmTypes.push_back((*i).mType->mLLVMType);
        }
    } else {
        WNScriptType voidStar;
        if(ok != (err = _manager.GetTypeByName("-Ptr", voidStar))) {
            _compilationLog.Log(WNLogging::eCritical, 0, "Cannot find Ptr type");
            return(err);
        }
        scriptType->mStructTypes.push_back(WNContainedStructType());
        scriptType->mStructTypes.back().mName = WNStrings::WNStrNDup("--vtable", 256);
        scriptType->mStructTypes.back().mType = voidStar;
        llvmTypes.push_back(voidStar->mLLVMType);
    }


    for(WNScriptLinkedList<WNDeclaration>::WNScriptLinkedListNode* i = mStructMembers.first; i != wn_nullptr; i = i->next) {
        WNScriptType type = wn_nullptr;
        if(ok != (err = i->value->GetType(_manager, type, _compilationLog))) {
            _compilationLog.Log(WNLogging::eError, 0, "Cannot find type" );
            i->value->LogLine(_compilationLog, WNLogging::eError);
            return(err);
        }
        llvmTypes.push_back(type->mLLVMType);
        WNContainedStructType containedType;
        containedType.mName = WNStrings::WNStrNDup(i->value->GetName(), 256);
        containedType.mType = type;
        scriptType->mStructTypes.push_back(containedType);
    }

    WNScriptType nullType = NULL;
    if(ok != (err = _manager.GetTypeByName("-Null", nullType))) {
        return(err);
    }

    WNScriptType functionType = NULL;
    if(ok != (err = _manager.GetTypeByName("-Function", functionType))) {
        return(err);
    }

    WNScriptType boolType = NULL;
    if(ok != (err = _manager.GetTypeByName("Bool", boolType))) {
        return(err);
    }

    WNScriptType objectType = NULL;
    if(ok != (err = _manager.GetTypeByName("-Object", objectType))) {
        return(err);
    }


    scriptType->mLLVMStructType->setBody(llvmTypes);

    _manager.RegisterAllocationOperator(scriptType, WN_NEW GenerateDefaultAllocation());
    _manager.RegisterIDAccessOperator(scriptType, WN_NEW GenerateDefaultStructAccessor(scriptType, functionType));
    _manager.RegisterAssignmentOperator(scriptType, AT_EQ, WN_NEW GenerateStructAssignment());
    _manager.RegisterAssignmentOperator(scriptType, AT_CHOWN, WN_NEW GenerateStructTransfer());
    _manager.RegisterDestructionOperator(scriptType, WN_NEW GenerateStructDestruction());
    _manager.RegisterCastingOperator(nullType, scriptType, WN_NEW GenerateReinterpretCastOperation(scriptType));
    _manager.RegisterCastingOperator(scriptType, objectType, WN_NEW GenerateReinterpretCastOperation(objectType));
    _manager.RegisterArithmeticOperator(AR_EQ, scriptType, scriptType, WN_NEW GenerateStructCompare(boolType, wn_true));
    _manager.RegisterArithmeticOperator(AR_NEQ, scriptType, scriptType, WN_NEW GenerateStructCompare(boolType, wn_false));
    _manager.RegisterConstructionOperator(scriptType, WN_NEW GenerateStructConstruction());
    _manager.RegisterCopyConstructionOperator(scriptType, WN_NEW GenerateStructCopyConstruction());

    scriptType->mParentClass = mParentType;
    mType = scriptType;
    WNScriptType ancestorType = mParentType;
    while(ancestorType) {
        _manager.RegisterCastingOperator(scriptType, ancestorType, WN_NEW GenerateReinterpretCastOperation(ancestorType));
        ancestorType = ancestorType->mParentClass;
    }

    //We have to do this down here, because the casting operations must already exist
    if(mParentType)
    {
        for(std::vector<WNScripting::WNFunctionDefinition*>::iterator thisIt =  scriptType->mVTable.begin(); thisIt != scriptType->mVTable.end(); ++thisIt) {
            int parentIndex = -1;
            if(-1 != (parentIndex = GetVirtualFunctionIndex(_manager, (*thisIt)->mName, (*thisIt)->mTypes, mParentType))) {
                _compilationLog.Log(WNLogging::eError, 0, "Cannot create function ", (*thisIt)->mName, " because it would hide ", mParentType->mVTable[parentIndex]->mName);
                return(eWNAlreadyExists);
            }
        }
        scriptType->mVTable.insert(scriptType->mVTable.begin(), mParentType->mVTable.begin(), mParentType->mVTable.end());
    }

    if(mParentType) {
        for(std::vector<WNScripting::WNFunctionDefinition*>::iterator thisIt =  scriptType->mOverridenFunctions.begin(); thisIt != scriptType->mOverridenFunctions.end(); ++thisIt) {
            int parentIndex = -1; //we dont want override to allow casts, that would be strange (and terribly incorrect)
            if(-1 == (parentIndex = GetVirtualFunctionIndex(_manager, (*thisIt)->mName, (*thisIt)->mTypes, mParentType, false))) {
                _compilationLog.Log(WNLogging::eError, 0, "Cannot create function ", (*thisIt)->mName, " because it is trying to override a non-existent base function");
                return(eWNAlreadyExists);
            }
            scriptType->mVTable[parentIndex] = *thisIt;
        }
        scriptType->mOverridenFunctions.clear();
    }
    scriptType->mFinalizedStruct = true;
    return(ok);
}

eWNTypeError WNStruct::GenerateHeader(WNCodeModule& _module, WNLogging::WNLog& _compilationLog) {
    WNScriptType scriptType = NULL;
    eWNTypeError err = ok;
    if(ok != (err = _module.GetTypeManager().RegisterStructType(mName, _module.GetScriptingEngine(), scriptType))) {
        if(err != eWNAlreadyExists) {
            _compilationLog.Log(WNLogging::eError, 0, "type already Exists: ", scriptType->mName);
            LogLine(_compilationLog, WNLogging::eError);
            return(err);
        }
    }

    WNScriptType voidReturnType;
    if(ok != (err = _module.GetTypeManager().GetTypeByName("Void", voidReturnType))) {
        _compilationLog.Log(WNLogging::eCritical, 0, "Cannot find void type: ", scriptType->mName);
        LogLine(_compilationLog, WNLogging::eCritical);
        return(err);
    }

    std::vector<WNScriptType> parameterTypes;
    parameterTypes.push_back(scriptType);
    wn_size_t nameLen = WNStrings::WNStrLen(mName);
    wn_char *functionName = static_cast<wn_char*>(WN_STACK_ALLOC(sizeof(wn_char) * (nameLen + 1 + 9)));
    WNMemory::WNMemCpy(functionName, mName, nameLen);
    WNMemory::WNMemCpy(functionName + nameLen, "Const", 6);

    WNFunctionDefinition* constDef = wn_nullptr;
    WNFunctionDefinition* equivDef = wn_nullptr;
    if(ok != (err = _module.AddFunctionDefinition(functionName, parameterTypes, voidReturnType, constDef, equivDef))) {
        if(eWNAlreadyExists != err)
        {
            parameterTypes.clear();
            _compilationLog.Log(WNLogging::eError, 0, "Could not create constructor header: ", scriptType->mName);
            LogLine(_compilationLog, WNLogging::eError);
            return(err);
        }
    }

    std::vector<llvm::Type*> llvmTypes;
    for(std::vector<WNScriptType>::iterator i = parameterTypes.begin(); i != parameterTypes.end(); ++i) {
        llvmTypes.push_back((*i)->mLLVMType);
    }

    llvm::FunctionType* fType = llvm::FunctionType::get(voidReturnType->mLLVMType, llvmTypes, wn_false);
    llvm::Function* func = llvm::Function::Create(fType, llvm::GlobalValue::ExternalLinkage, functionName, _module.GetModule());
    //func->addFnAttr(llvm::Attribute::AlwaysInline);
    constDef->mFunction = func;
    constDef->mFunctionType = fType;
    parameterTypes.push_back(scriptType);
    WNMemory::WNMemCpy(functionName + nameLen, "CopyConst", 10);

    WNFunctionDefinition* copyConstDef = wn_nullptr;
    if(ok != (err = _module.AddFunctionDefinition(functionName, parameterTypes, voidReturnType, copyConstDef, equivDef))) {
        if(eWNAlreadyExists != err)
        {
            parameterTypes.clear();
            _compilationLog.Log(WNLogging::eError, 0, "Could not create copy construction header: ", scriptType->mName);
            LogLine(_compilationLog, WNLogging::eError);
            return(err);
        }
    }

    llvmTypes.clear();
    for(std::vector<WNScriptType>::iterator i = parameterTypes.begin(); i != parameterTypes.end(); ++i) {
        llvmTypes.push_back((*i)->mLLVMType);
    }

    fType = llvm::FunctionType::get(voidReturnType->mLLVMType, llvmTypes, wn_false);
    func = llvm::Function::Create(fType, llvm::GlobalValue::ExternalLinkage, functionName, _module.GetModule());
    //func->addFnAttr(llvm::Attribute::AlwaysInline);
    copyConstDef->mFunction = func;
    copyConstDef->mFunctionType = fType;
    parameterTypes.pop_back(); //Remove the second scriptType that we put on

    WNMemory::WNMemCpy(functionName + nameLen, "Destr", 6);
    WNFunctionDefinition* destrDef = wn_nullptr;
    if(ok != (err = _module.AddFunctionDefinition(functionName, parameterTypes, voidReturnType, destrDef, equivDef))) {
        if(eWNAlreadyExists != err)
        {
            parameterTypes.clear();
            _compilationLog.Log(WNLogging::eError, 0, "Could not create destructor header: ", scriptType->mName);
            LogLine(_compilationLog, WNLogging::eError);
            return(err);
        }
    }

    llvmTypes.clear();
    for(std::vector<WNScriptType>::iterator i = parameterTypes.begin(); i != parameterTypes.end(); ++i) {
        llvmTypes.push_back((*i)->mLLVMType);
    }

    fType = llvm::FunctionType::get(voidReturnType->mLLVMType, llvmTypes, wn_false);
    func = llvm::Function::Create(fType, llvm::GlobalValue::ExternalLinkage, functionName, _module.GetModule());
    //func->addFnAttr(llvm::Attribute::AlwaysInline);
    destrDef->mFunction = func;
    destrDef->mFunctionType = fType;
    for(WNScriptLinkedList<WNFunction>::WNScriptLinkedListNode* i = mStructFunctions.first; i != wn_nullptr; i = i->next) {
        if(ok != (err = i->value->GenerateHeader(_module, scriptType, _compilationLog))) {
            return(err);
        }
    }

    return(ok);
}

eWNTypeError WNStruct::GenerateCode(WNCodeModule& _module, WNLogging::WNLog& _compilationLog) {
    WNScriptType scriptType = NULL;
    eWNTypeError err = ok;

    if(ok != (err = _module.GetTypeManager().GetTypeByName(mName, scriptType))) {
        _compilationLog.Log(WNLogging::eError, 0, "Cannot find type: ", scriptType->mName);
        LogLine(_compilationLog, WNLogging::eError);
        return(err);
    }
    if(!mParentType) {
        mParentType = scriptType->mParentClass;
    }

    if(!scriptType->mLLVMStructType) {
        _compilationLog.Log(WNLogging::eError, 0, scriptType->mName, " is not a valid Struct type");
        LogLine(_compilationLog, WNLogging::eError);
        return(eWNBadType);
    }

    if(ok != (err = FinalizeStruct(_module.GetTypeManager(), _compilationLog))) {
        return(err);
    }

    if(ok != (err = GenerateConstructor(_module, _compilationLog))) {
        return(err);
    }
    if(ok != (err = GenerateCopyConstructor(_module, _compilationLog))) {
        return(err);
    }
    if(ok != (err = GenerateDestructor(_module, _compilationLog))) {
        return(err);
    }
    for(WNScriptLinkedList<WNFunction>::WNScriptLinkedListNode* i = mStructFunctions.first; i != wn_nullptr; i = i->next) {
        if(ok != (err = i->value->GenerateCode(_module, scriptType, _compilationLog))) {
            return(err);
        }
    }

    return(ok);
}

wn_char* WNStruct::GetName() const {
    return(mName);
}

eWNTypeError WNStruct::GenerateConstructor(WNCodeModule& _module, WNLogging::WNLog& _compilationLog) {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());

    eWNTypeError err = ok;
    WNScriptType voidReturnType;
    if(ok != (err = _module.GetTypeManager().GetTypeByName("Void", voidReturnType))) {
        return(err);
    }
    WNScriptType scriptType;
    if(ok != (err = _module.GetTypeManager().GetTypeByName(mName, scriptType))) {
        return(err);
    }

    std::vector<WNScriptType> parameterTypes;
    parameterTypes.push_back(scriptType);
    wn_size_t nameLen = WNStrings::WNStrLen(mName);
    wn_char *functionName = static_cast<wn_char*>(WN_STACK_ALLOC(sizeof(wn_char) * (nameLen + 1 + 5)));
    WNMemory::WNMemCpy(functionName, mName, nameLen);
    WNMemory::WNMemCpy(functionName + nameLen, "Const", 6);

    WNFunctionDefinition * def = _module.GetFunctionDefinition(functionName, parameterTypes);
    if(!def) {
        _compilationLog.Log(WNLogging::eError, 0, "No valid constructor for operator for ", scriptType->mName);
        LogLine(_compilationLog, WNLogging::eError);
        return(error);
    }

    llvm::BasicBlock* bb = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", def->mFunction);
    builder->SetInsertPoint(bb);
    llvm::Function::arg_iterator fIter = def->mFunction->arg_begin();
    fIter->setName("this");

    //call parent's constructor if we have a parent
    if(scriptType->mParentClass) {
        wn_size_t nameLen = WNStrings::WNStrLen(scriptType->mParentClass->mName);
        wn_char *functionName = static_cast<wn_char*>(WN_STACK_ALLOC(sizeof(wn_char)* (nameLen + 1 + 5)));
        WNMemory::WNMemCpy(functionName, scriptType->mParentClass->mName, nameLen);
        WNMemory::WNMemCpy(functionName + nameLen, "Const", 6);
        std::vector<FunctionParam> fParams;
        fParams.push_back(FunctionParam());
        fParams.back().mType = scriptType->mParentClass;
        const GenerateCastingOperation* op = _module.GetTypeManager().GetCastingOperation(scriptType, scriptType->mParentClass);
        if(!op) {
            return(eWNInvalidCast);
        }
        llvm::Value* outValue = wn_nullptr;
        if(ok != (err = op->Execute(_module.GetBuilder(), fIter, outValue))) {
            return(err);
        }

        fParams.back().mValue = outValue;

        WNScriptType returnType;
        llvm::Value* returnValue;
        if(ok != (err = GenerateFunctionCall(_module, def, functionName, fParams, false, returnType, returnValue, _compilationLog))) {
            return(err);
        }
    }

    std::vector<llvm::Value*> GepArray;
    GepArray.clear();
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 1));
    llvm::Value* tempValue = builder->CreateLoad(builder->CreateGEP(fIter, GepArray));


    wn_size_t cnt = 0;
    if(mParentType) {
        cnt += mParentType->mStructTypes.size();
    } else {
        ++cnt;
    }
    //if(mParentType || mIsClass) {
        WNScriptType t;
        if(ok != _module.GetTypeManager().GetTypeByName("-SizeT", t)) {
            _compilationLog.Log(WNLogging::eCritical, 0, "Cannot find sizeT type");
            return(error);
        }
        scriptType->mStoredVTable = malloc(sizeof(wn_void*) * 2 * (scriptType->mVTable.size() + 2));
        _compilationLog.Log(WNLogging::eDebug, 0, "Creating vtable for ", scriptType->mName, " at location ", scriptType->mStoredVTable);
        memset(scriptType->mStoredVTable, 0x00, sizeof(wn_void*) * (2 * scriptType->mVTable.size() + 2));
        GepArray.clear();
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
        llvm::Value* vTableLoc = builder->CreateGEP(tempValue, GepArray);
        vTableLoc = builder->CreatePointerCast(vTableLoc, t->mLLVMType->getPointerTo(0));
        builder->CreateStore(llvm::ConstantInt::get(t->mLLVMType, reinterpret_cast<size_t>(scriptType->mStoredVTable)), vTableLoc);
        for(wn_size_t i = 0; i < scriptType->mVTable.size(); ++i) {
            reinterpret_cast<wn_void**>(scriptType->mStoredVTable)[2 * i + 3] = scriptType->mVTable[i];
        }
    //}

    for(WNScriptLinkedList<WNDeclaration>::WNScriptLinkedListNode* i = mStructMembers.first; i != wn_nullptr; ++cnt, i = i->next) {
        WNScriptType type = wn_nullptr;
        if(ok != (err = i->value->GetType(_module.GetTypeManager(), type, _compilationLog))) {
            return(err);
        }
        const GenerateAssignment * destOp = _module.GetTypeManager().GetAssignmentOperation(type, AT_EQ);
        if(!destOp) {
            _compilationLog.Log(WNLogging::eError, 0, "No assignment operator for ", type->mName);
            i->value->LogLine(_compilationLog, WNLogging::eError);
            return(eWNInvalidOperation);
        }
        WNExpression* expr = i->value->GetExpression();
        if(!expr) {
            _compilationLog.Log(WNLogging::eError, 0, "Struct members must have a valid initializer");
            i->value->LogLine(_compilationLog, WNLogging::eError);
            return(eWNInvalidOperation);
        }
        if(ok != (err = expr->GenerateCode(_module, def, _compilationLog))) {
            return(err);
        }
        llvm::Value* storeVal = expr->GetValue();
        if(expr->GetType() != type) {
            const GenerateCastingOperation* castOp = _module.GetTypeManager().GetCastingOperation(expr->GetType(), type);
            if(!castOp) {
                _compilationLog.Log(WNLogging::eError, 0, "Cannot cast from: ", expr->GetType()->mName, " to ", type->mName);
                i->value->LogLine(_compilationLog, WNLogging::eError);
                return(eWNInvalidCast);
            }
            if(ok != (err = castOp->Execute(builder, storeVal, storeVal))) {
                return(err);
            }
        }

        GepArray.clear();
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), cnt));
        llvm::Value* storeLocation = builder->CreateGEP(tempValue, GepArray);

        if(ok != (err = destOp->Execute(_module, true, type, storeVal, storeLocation, def, _compilationLog))) {
            return(err);
        }
        WNScriptVariable* var = WN_SCRIPTNODE_NEW(WNScriptVariable(type, i->value->GetName(), storeLocation));
        if(ok != (err = _module.GetScopedVariableList().PushVariable(var))) {
            return(err);
        }
    }

    _module.GetScopedVariableList().ClearScope(); //Intentionally clear the scope before we return
                                                  //we do not want to clear up anything
    GenerateVOIDReturn(_module, def);
    builder->ClearInsertionPoint();
    return(ok);
}

eWNTypeError WNStruct::GenerateCopyConstructor(WNCodeModule& _module, WNLogging::WNLog& _compilationLog) {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());

    eWNTypeError err = ok;
    WNScriptType voidReturnType;
    if(ok != (err = _module.GetTypeManager().GetTypeByName("Void", voidReturnType))) {
        return(err);
    }
    WNScriptType scriptType;
    if(ok != (err = _module.GetTypeManager().GetTypeByName(mName, scriptType))) {
        return(err);
    }

    std::vector<WNScriptType> parameterTypes;
    parameterTypes.push_back(scriptType);
    parameterTypes.push_back(scriptType);
    wn_size_t nameLen = WNStrings::WNStrLen(mName);
    wn_char *functionName = static_cast<wn_char*>(WN_STACK_ALLOC(sizeof(wn_char) * (nameLen + 1 + 9)));
    WNMemory::WNMemCpy(functionName, mName, nameLen);
    WNMemory::WNMemCpy(functionName + nameLen, "CopyConst", 10);

    WNFunctionDefinition * def = _module.GetFunctionDefinition(functionName, parameterTypes);
    if(!def) {
        _compilationLog.Log(WNLogging::eError, 0, "No valid constructor for operator for ", scriptType->mName);
        LogLine(_compilationLog, WNLogging::eError);
        return(error);
    }

    llvm::BasicBlock* bb = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", def->mFunction);
    builder->SetInsertPoint(bb);
    llvm::Function::arg_iterator fThis = def->mFunction->arg_begin();
    fThis->setName("this");
    llvm::Function::arg_iterator fOther = def->mFunction->arg_begin();
    fOther++;
    fOther->setName("other");

    //call parent's constructor if we have a parent
    if(scriptType->mParentClass) {
        wn_size_t nameLen = WNStrings::WNStrLen(scriptType->mParentClass->mName);
        wn_char *functionName = static_cast<wn_char*>(WN_STACK_ALLOC(sizeof(wn_char)* (nameLen + 1 + 9)));
        WNMemory::WNMemCpy(functionName, scriptType->mParentClass->mName, nameLen);
        WNMemory::WNMemCpy(functionName + nameLen, "CopyConst", 10);
        std::vector<FunctionParam> fParams;
        fParams.push_back(FunctionParam());
        fParams.back().mType = scriptType->mParentClass;
        const GenerateCastingOperation* op = _module.GetTypeManager().GetCastingOperation(scriptType, scriptType->mParentClass);
        if(!op) {
            return(eWNInvalidCast);
        }
        llvm::Value* outValue = wn_nullptr;
        if(ok != (err = op->Execute(_module.GetBuilder(), fThis, outValue))) {
            return(err);
        }
        fParams.back().mValue = outValue;

        //and again for our "other" parameter

        fParams.push_back(FunctionParam());
        fParams.back().mType = scriptType->mParentClass;
        if(ok != (err = op->Execute(_module.GetBuilder(), fOther, outValue))) {
            return(err);
        }
        fParams.back().mValue = outValue;


        WNScriptType returnType;
        llvm::Value* returnValue;
        if(ok != (err = GenerateFunctionCall(_module, def, functionName, fParams, false, returnType, returnValue, _compilationLog))) {
            return(err);
        }
    }

    std::vector<llvm::Value*> GepArray;
    GepArray.clear();
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 1));
    llvm::Value* tempValue = builder->CreateLoad(builder->CreateGEP(fThis, GepArray));
    llvm::Value* otherTempValue = builder->CreateLoad(builder->CreateGEP(fOther, GepArray));

    wn_size_t cnt = 0;
    if(mParentType) {
        cnt += mParentType->mStructTypes.size();
    } else {
        ++cnt;
    }
    //if(mParentType || mIsClass) {
        WNScriptType t;
        if(ok != _module.GetTypeManager().GetTypeByName("-SizeT", t)) {
            _compilationLog.Log(WNLogging::eCritical, 0, "Cannot find sizeT type");
            return(error);
        }
        GepArray.clear();
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
        llvm::Value* vTableLoc = builder->CreateGEP(tempValue, GepArray);
        vTableLoc = builder->CreatePointerCast(vTableLoc, t->mLLVMType->getPointerTo(0));
        builder->CreateStore(llvm::ConstantInt::get(t->mLLVMType, reinterpret_cast<size_t>(scriptType->mStoredVTable)), vTableLoc);
        for(wn_size_t i = 0; i < scriptType->mVTable.size(); ++i) {
            reinterpret_cast<wn_void**>(scriptType->mStoredVTable)[2 * i + 3] = scriptType->mVTable[i];
        }
    //}

    for(WNScriptLinkedList<WNDeclaration>::WNScriptLinkedListNode* i = mStructMembers.first; i != wn_nullptr; ++cnt, i = i->next) {
        WNScriptType type = wn_nullptr;
        if(ok != (err = i->value->GetType(_module.GetTypeManager(), type, _compilationLog))) {
            return(err);
        }
        const GenerateAssignment * destOp = _module.GetTypeManager().GetAssignmentOperation(type, AT_EQ);
        if(!destOp) {
            _compilationLog.Log(WNLogging::eError, 0, "No assignment operator for ", type->mName);
            i->value->LogLine(_compilationLog, WNLogging::eError);
            return(eWNInvalidOperation);
        }

        GepArray.clear();
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), cnt));
        llvm::Value* storeLocation = builder->CreateGEP(tempValue, GepArray);
        llvm::Value* loadLocation = builder->CreateGEP(otherTempValue, GepArray);
        llvm::Value* storeValue = builder->CreateLoad(loadLocation);
        if(ok != (err = destOp->Execute(_module, true, type, storeValue, storeLocation, def, _compilationLog))) {
            return(err);
        }
    }

    _module.GetScopedVariableList().ClearScope(); //Intentionally clear the scope before we return
                                                  //we do not want to clear up anything
    GenerateVOIDReturn(_module, def);
    builder->ClearInsertionPoint();
    return(ok);
}

eWNTypeError WNStruct::GenerateDestructor(WNCodeModule& _module, WNLogging::WNLog& _compilationLog) {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());

    eWNTypeError err = ok;
    WNScriptType voidReturnType;
    if(ok != (err = _module.GetTypeManager().GetTypeByName("Void", voidReturnType))) {
        return(err);
    }
    WNScriptType scriptType;
    if(ok != (err = _module.GetTypeManager().GetTypeByName(mName, scriptType))) {
        return(err);
    }

    std::vector<WNScriptType> parameterTypes;
    parameterTypes.push_back(scriptType);
    wn_size_t nameLen = WNStrings::WNStrLen(mName);
    wn_char *functionName = static_cast<wn_char*>(WN_STACK_ALLOC(sizeof(wn_char) * (nameLen + 1 + 5)));
    WNMemory::WNMemCpy(functionName, mName, nameLen);
    WNMemory::WNMemCpy(functionName + nameLen, "Destr", 6);

    WNFunctionDefinition * def = _module.GetFunctionDefinition(functionName, parameterTypes);
    if(!def) {
        _compilationLog.Log(WNLogging::eError, 0, "Cannot find destructor for struct: ", scriptType->mName);
        LogLine(_compilationLog, WNLogging::eError);
        return(error);
    }
    llvm::BasicBlock* bb = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", def->mFunction);
    builder->SetInsertPoint(bb);
    llvm::Function::arg_iterator fIter = def->mFunction->arg_begin();
    fIter->setName("this");

    std::vector<llvm::Value*> GepArray;
    GepArray.clear();
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 1));
    llvm::Value* tempValue = builder->CreateLoad(builder->CreateGEP(fIter, GepArray));

    wn_size_t cnt = 0;
    if(mParentType) {
        cnt += mParentType->mStructTypes.size();
    } else {
        cnt += 1; //we have to not care about the vtable
    }
    for(WNScriptLinkedList<WNDeclaration>::WNScriptLinkedListNode* i = mStructMembers.first; i != wn_nullptr; ++cnt, i = i->next) {
        WNScriptType type = wn_nullptr;
        if(ok != (err = i->value->GetType(_module.GetTypeManager(), type, _compilationLog))) {
            return(err);
        }
        const GenerateDestruction* destOp = _module.GetTypeManager().GetDestructionOperation(type);
        if(destOp) {
            GepArray.clear();
            GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
            GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), cnt));
            llvm::Value* valLoc = builder->CreateGEP(tempValue, GepArray);

            eWNTypeError err = ok;
            if(ok != (err = destOp->Execute(_module, type, valLoc, def, _compilationLog))){
                return(err);
            }
        }
    }

    //call parent's destructor if we have a parent
    if(scriptType->mParentClass) {
        wn_size_t nameLen = WNStrings::WNStrLen(scriptType->mParentClass->mName);
        wn_char *functionName = static_cast<wn_char*>(WN_STACK_ALLOC(sizeof(wn_char)* (nameLen + 1 + 5)));
        WNMemory::WNMemCpy(functionName, scriptType->mParentClass->mName, nameLen);
        WNMemory::WNMemCpy(functionName + nameLen, "Destr", 6);
        std::vector<FunctionParam> fParams;
        fParams.push_back(FunctionParam());
        fParams.back().mType = scriptType->mParentClass;

        WNScriptType returnType;

        llvm::Value* returnValue;
        const GenerateCastingOperation* op = _module.GetTypeManager().GetCastingOperation(scriptType, scriptType->mParentClass);
        if(!op) {
            return(eWNInvalidCast);
        }
        llvm::Value* outValue = wn_nullptr;
        if(ok != (err = op->Execute(_module.GetBuilder(), fIter, outValue))) {
            return(err);
        }
        fParams.back().mValue = outValue;


        if(ok != (err = GenerateFunctionCall(_module, def, functionName, fParams, false, returnType, returnValue, _compilationLog))) {
            return(err);
        }
    }

    GenerateVOIDReturn(_module, def);
    _module.GetScopedVariableList().ClearScope();
    builder->ClearInsertionPoint();
    return(ok);
}

WNScriptType WNStruct::GetType() const {
    return(mType);
}
