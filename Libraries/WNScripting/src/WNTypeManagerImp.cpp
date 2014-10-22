// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNTypeManagerImpl.h"
#include "WNScripting/inc/WNCodeModule.h"
#include "WNScripting/inc/WNNodeHelpers.h"
#include "WNScripting/inc/WNExpression.h"
#include "WNLogging/inc/WNLog.h"

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
#include "llvm/IR/Intrinsics.h"

#ifdef _WN_MSVC
    #pragma warning(pop)
#endif

using namespace WNScripting;

WNTypeManagerImpl::WNTypeManagerImpl() {
    mNextTag[0] = 'A';
    mNextTag[1] = 'A';
    mNextTag[2] = 'A';
    mNextTag[3] = 'A';
}

WNTypeManagerImpl::~WNTypeManagerImpl() {

}

void WNTypeManagerImpl::IncrementTag() {
    for(size_t i = 0; i < 4; ++i) {
        if(mNextTag[i] < 'z') {
            mNextTag[i] += 1;
            break;
        }
        else
        {
            mNextTag[i] = 'A';
        }
    }
}


WN_VOID GenerateAssertion(WNCodeModule& _module) {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
    WNFunctionDefinition* func = _module.GetFunctionDefinition("Assert", std::vector<WNScriptType>());
    if(!func) {
        return;
    }
    builder->CreateCall(func->mFunction);
}

eWNTypeError WNTypeManagerImpl::RegisterScalarType(const WN_CHAR* name, const WNScriptingEngine* _engine, WN_FLOAT32 priority, WNScriptType& _outType, llvm::Type* _type, WN_SIZE_T _size) {
    if(GetTypeByName(name, _outType) == eWNOK) {
        return(eWNAlreadyExists);
    }
    WNScripting::WNScriptTypeInternal* type = WN_NEW WNScripting::WNScriptTypeInternal(name, _engine, 0, priority, _type, mNextTag, _size);
    IncrementTag();
    mScriptTypes.push_back(type);
    _outType = type;
    return(eWNOK);
}

eWNTypeError WNTypeManagerImpl::RegisterStructType(const WN_CHAR* name, const WNScriptingEngine* _engine, WNScriptType& _outType) {
    if(GetTypeByName(name, _outType) == eWNOK) {
        return(eWNAlreadyExists);
    }
    llvm::StructType* sType = llvm::StructType::create(llvm::getGlobalContext());
    sType->setName(name);
    llvm::PointerType* ty = sType->getPointerTo(0);
#ifdef _WN_64_BIT
    llvm::Type* structCount = llvm::IntegerType::getInt64Ty(llvm::getGlobalContext());
#else
    llvm::Type* structCount = llvm::IntegerType::getInt32Ty(llvm::getGlobalContext());
#endif
    llvm::Type* types[3] = { llvm::IntegerType::getInt32PtrTy(llvm::getGlobalContext()), ty, structCount };
    llvm::StructType* sContainerType = llvm::StructType::create(llvm::getGlobalContext(), llvm::makeArrayRef(types));

    ty = sContainerType->getPointerTo(0);
    WNScripting::WNScriptTypeInternal* type = WN_NEW WNScripting::WNScriptTypeInternal(name, _engine, 0, 0, ty, sType, mNextTag, sizeof(WN_SIZE_T));
    IncrementTag();
    mScriptTypes.push_back(type);
    _outType = type;
    return(eWNOK);
}

eWNTypeError WNTypeManagerImpl::RegisterAliasedStruct(const WN_CHAR* name, WNScriptType copyType, WNScriptType& _outType) {
    if(GetTypeByName(name, _outType) == eWNOK) {
        return(eWNAlreadyExists);
    }
    WNScripting::WNScriptTypeInternal* type = WN_NEW WNScripting::WNScriptTypeInternal(copyType, name, mNextTag);
    IncrementTag();
    mScriptTypes.push_back(type);
    _outType = type;
    return(eWNOK);
}

eWNTypeError WNTypeManagerImpl::RegisterCStruct(const WNScriptingEngine* _engine, const WN_CHAR* name, WNScriptType& _outType) {
    if(GetTypeByName(name, _outType) == eWNOK) {
        return(eWNAlreadyExists);
    }
    
#ifdef _WN_64_BIT
    llvm::Type* structCount = llvm::IntegerType::getInt64Ty(llvm::getGlobalContext());
#else
    llvm::Type* structCount = llvm::IntegerType::getInt32Ty(llvm::getGlobalContext());
#endif
    llvm::Type* types[3] = { llvm::IntegerType::getInt32PtrTy(llvm::getGlobalContext()), llvm::IntegerType::getInt32PtrTy(llvm::getGlobalContext()), structCount };
    llvm::StructType* sContainerType = llvm::StructType::create(llvm::getGlobalContext(), llvm::makeArrayRef(types));

    llvm::PointerType* ty = sContainerType->getPointerTo(0);
    WNScripting::WNScriptTypeInternal* type = WN_NEW WNScripting::WNScriptTypeInternal(name, _engine, 0, 0, ty, 0, mNextTag, sizeof(WN_SIZE_T));
    IncrementTag();
    mScriptTypes.push_back(type);
    _outType = type;
    return(eWNOK);
}


eWNTypeError WNTypeManagerImpl::RegisterArithmeticOperator(WNArithmeticType _type, WNScriptType _operand1, WNScriptType _operand2,  GenerateArithmeticOperation* _operation) {
    mArithmeticStructs.push_back(GenerateArithmeticStruct());
    mArithmeticStructs.back().mArithmeticType = _type;
    mArithmeticStructs.back().mType1 = _operand1;
    mArithmeticStructs.back().mType2 = _operand2;
    mArithmeticStructs.back().mOperation = _operation;
    return(eWNOK);
}

eWNTypeError WNTypeManagerImpl::RegisterCastingOperator(WNScriptType _fromType, WNScriptType _toType, GenerateCastingOperation* _operation) {
    mCastingStructs.push_back(GenerateCastingStruct());
    mCastingStructs.back().mType1 = _fromType;
    mCastingStructs.back().mType2 = _toType;
    mCastingStructs.back().mOperation = _operation;
    return(eWNOK);
}

eWNTypeError WNTypeManagerImpl::RegisterPreUnaryOperator(WNUnaryType _type, WNScriptType _operand, GeneratePreUnaryOperation* _operation) {
    mPreUnaryStructs.push_back(GeneratePreUnaryStruct());
    mPreUnaryStructs.back().mUnaryType = _type;
    mPreUnaryStructs.back().mType = _operand;
    mPreUnaryStructs.back().mOperation = _operation;
    return(eWNOK);
}

eWNTypeError WNTypeManagerImpl::RegisterPostUnaryOperator(WNPostUNType _type, WNScriptType _operand, GeneratePostUnaryOperation* _operation) {
    mPostUnaryStructs.push_back(GeneratePostUnaryStruct());
    mPostUnaryStructs.back().mPostUnaryType = _type;
    mPostUnaryStructs.back().mType = _operand;
    mPostUnaryStructs.back().mOperation = _operation;
    return(eWNOK);
}

eWNTypeError WNTypeManagerImpl::RegisterArrayAccessOperator(WNScriptType _operand1, WNScriptType _operand2, GenerateArrayAccessOperation* _operation) {
    mArrayAccessStructs.push_back(GenerateArrayAccessStruct());
    mArrayAccessStructs.back().mType1 = _operand1;
    mArrayAccessStructs.back().mType2 = _operand2;
    mArrayAccessStructs.back().mOperation = _operation;
    return(eWNOK);
}

eWNTypeError WNTypeManagerImpl::RegisterIDAccessOperator(WNScriptType _operand1, GenerateIDAccessOperation* _operation) {
    mIDAccessStructs.push_back(GenerateIDAccessStruct());
    mIDAccessStructs.back().mType1 = _operand1;
    mIDAccessStructs.back().mOperation = _operation;
    return(eWNOK);
}

eWNTypeError WNTypeManagerImpl::RegisterConstantOperator(WNScriptType _type, GenerateConstantOperation* _operation) {
    mConstantStructs.push_back(GenerateConstantStruct());
    mConstantStructs.back().mType = _type;
    mConstantStructs.back().mOperation = _operation;
    return(eWNOK);
}

eWNTypeError WNTypeManagerImpl::RegisterAllocationOperator(WNScriptType _type, GenerateAllocation* _operation) {
    mAllocationStructs.push_back(GenerateAllocationStruct());
    mAllocationStructs.back().mType = _type;
    mAllocationStructs.back().mOperation = _operation;
    return(eWNOK);
}

eWNTypeError WNTypeManagerImpl::RegisterAssignmentOperator(WNScriptType _type, WNAssignType _assignType, GenerateAssignment* _operation) {
    mAssignmentStructs.push_back(GenerateAssignmentStruct());
    mAssignmentStructs.back().mType = _type;
    mAssignmentStructs.back().mOperation = _operation;
    mAssignmentStructs.back().mAssignType = _assignType;
    return(eWNOK);
}

eWNTypeError WNTypeManagerImpl::RegisterDestructionOperator(WNScriptType _type, GenerateDestruction* _operation) {
    mDestructionStructs.push_back(GenerateDestructionStruct());
    mDestructionStructs.back().mType = _type;
    mDestructionStructs.back().mOperation = _operation;
    return(eWNOK);
}

eWNTypeError WNTypeManagerImpl::RegisterConstructionOperator(WNScriptType _type, GenerateConstruction* _operation) {
    mConstructionStructs.push_back(GenerateConstructionStruct());
    mConstructionStructs.back().mType = _type;
    mConstructionStructs.back().mOperation = _operation;
    return(eWNOK);
}
eWNTypeError WNTypeManagerImpl::RegisterCopyConstructionOperator(WNScriptType _type, GenerateCopyConstruction* _operation) {
    mCopyConstructionStructs.push_back(GenerateCopyConstructionStruct());
    mCopyConstructionStructs.back().mType = _type;
    mCopyConstructionStructs.back().mOperation = _operation;
    return(eWNOK);
}

eWNTypeError WNTypeManagerImpl::GetExistingArrayOf(WNScriptType& _type, WNScriptType& _outType) const {
    if(!_type) {
        return(eWNDoesNotExist);
    }
    if(_type->mArrayParent) {
        _outType = _type->mArrayParent;
        return(eWNOK);
    }
    return(eWNDoesNotExist);
}
eWNTypeError WNTypeManagerImpl::GetArrayOf(WNScriptType& _type, WNScriptType& _outType) {
    if(!_type) {
        return(eWNDoesNotExist);
    }
    if(_type->mArrayParent) {
        _outType = _type->mArrayParent;
        return(eWNOK);
    }

    llvm::PointerType* ty = _type->mLLVMType->getPointerTo(0);
#ifdef _WN_64_BIT
    llvm::Type* arrayRefCount = llvm::IntegerType::getInt64Ty(llvm::getGlobalContext());
#else
    llvm::Type* arrayRefCount = llvm::IntegerType::getInt32Ty(llvm::getGlobalContext());
#endif
    llvm::Type* types[3] = { llvm::IntegerType::getInt32PtrTy(llvm::getGlobalContext()), ty, arrayRefCount };
    llvm::StructType* sIndirectionType = llvm::StructType::create(llvm::getGlobalContext(), llvm::makeArrayRef(types));

    ty = sIndirectionType->getPointerTo(0);
    WNScripting::WNScriptTypeInternal* scriptType = WN_NEW WNScriptTypeInternal(_type, _type->mScriptingEngine, ty, 0, mNextTag, sizeof(WN_SIZE_T));
    IncrementTag();
    _outType = scriptType;
    WNScriptType nullType;
    WNScriptType  boolType;
    WNScriptType intType;
    WNScriptType functionType;
    WNScriptType arrayType;
    if((eWNOK != GetTypeByName("-Null", nullType)) || (eWNOK != GetTypeByName("Bool", boolType)) || (eWNOK != GetTypeByName("Int", intType)) || (eWNOK != GetTypeByName("-Function", functionType)) || (eWNOK != GetTypeByName("-Array", arrayType)))
    {
        return(eWNPlatformError);
    }

    RegisterArrayAccessOperator(scriptType, intType, WN_NEW GenerateDefaultArrayAccessOperation(scriptType));
    RegisterDestructionOperator(scriptType, WN_NEW GenerateArrayDestruction());
    RegisterAllocationOperator(scriptType, WN_NEW GenerateDefaultAllocation());
    RegisterAssignmentOperator(scriptType, AT_EQ, WN_NEW GenerateStructAssignment());
    RegisterAssignmentOperator(scriptType, AT_CHOWN, WN_NEW GenerateStructTransfer());
    RegisterCastingOperator(nullType, scriptType, WN_NEW GenerateReinterpretCastOperation(scriptType));
    RegisterCastingOperator(scriptType, arrayType, WN_NEW GenerateReinterpretCastOperation(arrayType));
    RegisterArithmeticOperator(AR_EQ, scriptType, scriptType, WN_NEW GenerateStructCompare(boolType, WN_TRUE));
    RegisterArithmeticOperator(AR_NEQ, scriptType, scriptType, WN_NEW GenerateStructCompare(boolType, WN_FALSE));
    RegisterIDAccessOperator(scriptType, WN_NEW GenerateArrayIDOperator(scriptType, intType, functionType));
    RegisterConstructionOperator(scriptType, WN_NEW GenerateArrayConstruction());
    RegisterCopyConstructionOperator(scriptType, WN_NEW GenerateArrayCopyConstruction());
    return(eWNOK);
}

eWNTypeError WNTypeManagerImpl::GetTypeByName(const WN_CHAR* name, WNScriptType& _outType) const {
    for(std::list<WNScriptType>::const_iterator i = mScriptTypes.begin(); i != mScriptTypes.end(); ++i) {
        if(WNStrings::WNStrNCmp((*i)->mName, name, 256) == 0){
            _outType = *i;
            return(eWNOK);
        }
    }
    return(eWNDoesNotExist);
}

const GenerateArithmeticOperation* WNTypeManagerImpl::GetArithmeticOperation(WNArithmeticType _arith, WNScriptType _lhs, WNScriptType _rhs) const {
    for(std::list<GenerateArithmeticStruct>::const_iterator i = mArithmeticStructs.begin(); i != mArithmeticStructs.end(); ++i) {
        if(i->mType1 == _lhs && i->mType2 == _rhs && i->mArithmeticType == _arith) {
            return(i->mOperation);
        }
    }
    return(WN_NULL);
}

const GenerateCastingOperation* WNTypeManagerImpl::GetCastingOperation(WNScriptType _lhs, WNScriptType _rhs) const {
    for(std::list<GenerateCastingStruct>::const_iterator i = mCastingStructs.begin(); i != mCastingStructs.end(); ++i) {
        if(i->mType1 == _lhs && i->mType2 == _rhs) {
            return(i->mOperation);
        }
    }
    return(WN_NULL);
}

const GeneratePreUnaryOperation* WNTypeManagerImpl::GetPreUnaryOperation(WNUnaryType _type, WNScriptType _lhs) const {
    for(std::list<GeneratePreUnaryStruct>::const_iterator i = mPreUnaryStructs.begin(); i != mPreUnaryStructs.end(); ++i) {
        if(i->mType == _lhs && i->mUnaryType == _type) {
            return(i->mOperation);
        }
    }
    return(WN_NULL);
}

const GeneratePostUnaryOperation* WNTypeManagerImpl::GetPostUnaryOperation(WNPostUNType _type, WNScriptType _operand) const {
    for(std::list<GeneratePostUnaryStruct>::const_iterator i = mPostUnaryStructs.begin(); i != mPostUnaryStructs.end(); ++i) {
        if(i->mPostUnaryType == _type && i->mType == _operand) {
            return(i->mOperation);
        }
    }
    return(WN_NULL);
}

const GenerateArrayAccessOperation* WNTypeManagerImpl::GetArrayAccessOperation(WNScriptType _operand1, WNScriptType _operand2) const {
    for(std::list<GenerateArrayAccessStruct>::const_iterator i = mArrayAccessStructs.begin(); i != mArrayAccessStructs.end(); ++i) {
        if(i->mType1 == _operand1 && i->mType2 == _operand2) {
            return(i->mOperation);
        }
    }
    return(WN_NULL);
}

const GenerateIDAccessOperation* WNTypeManagerImpl::GetIDAccessOperation(WNScriptType _type) const {
    for(std::list<GenerateIDAccessStruct>::const_iterator i = mIDAccessStructs.begin(); i != mIDAccessStructs.end(); ++i) {
        if(i->mType1 == _type) {
            return(i->mOperation);
        }
    }
    return(WN_NULL);
}

const GenerateConstantOperation* WNTypeManagerImpl::GetConstantOperation(WNScriptType _type) const {
    for(std::list<GenerateConstantStruct>::const_iterator i = mConstantStructs.begin(); i != mConstantStructs.end(); ++i) {
        if(i->mType == _type) {
            return(i->mOperation);
        }
    }
    return(WN_NULL);
}

const GenerateAllocation* WNTypeManagerImpl::GetAllocationOperation(WNScriptType _type) const {
    for(std::list<GenerateAllocationStruct>::const_iterator i = mAllocationStructs.begin(); i != mAllocationStructs.end(); ++i) {
        if(i->mType == _type) {
            return(i->mOperation);
        }
    }
    return(WN_NULL);
}

const GenerateAssignment* WNTypeManagerImpl::GetAssignmentOperation(WNScriptType _type, WNAssignType _assignType) const {
    for(std::list<GenerateAssignmentStruct>::const_iterator i = mAssignmentStructs.begin(); i != mAssignmentStructs.end(); ++i) {
        if(i->mType == _type && i->mAssignType == _assignType) {
            return(i->mOperation);
        }
    }
    return(WN_NULL);
}

const GenerateDestruction* WNTypeManagerImpl::GetDestructionOperation(WNScriptType _type) const {
    for(std::list<GenerateDestructionStruct>::const_iterator i = mDestructionStructs.begin(); i != mDestructionStructs.end(); ++i) {
        if(i->mType == _type) {
            return(i->mOperation);
        }
    }
    return(WN_NULL);
}

const GenerateConstruction* WNTypeManagerImpl::GetConstructionOperation(WNScriptType _type) const {
    for(std::list<GenerateConstructionStruct>::const_iterator i = mConstructionStructs.begin(); i != mConstructionStructs.end(); ++i) {
        if(i->mType == _type) {
            return(i->mOperation);
        }
    }
    return(WN_NULL);
}

const GenerateCopyConstruction* WNTypeManagerImpl::GetCopyConstructionOperation(WNScriptType _type) const {
    for(std::list<GenerateCopyConstructionStruct>::const_iterator i = mCopyConstructionStructs.begin(); i != mCopyConstructionStructs.end(); ++i) {
        if(i->mType == _type) {
            return(i->mOperation);
        }
    }
    return(WN_NULL);
}

GenerateDefaultStructAccessor::GenerateDefaultStructAccessor(WNScriptType _structType, WNScriptType _functionType):
    mStructType(_structType) ,
    mFunctionType(_functionType) {
}


eWNTypeError GenerateDefaultStructAccessor::Execute(llvm::IRBuilderBase* _builder, llvm::Value*, llvm::Value* _structLoc, WN_CHAR* _id, WNScriptType& _outType, llvm::Value*& _outValue, llvm::Value*& _outLocation) const
{
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_builder);
    size_t valueNum = 0;

    for(std::vector<WNContainedStructType>::iterator i = mStructType->mStructTypes.begin(); i != mStructType->mStructTypes.end(); ++i, valueNum++) {
        if(WNStrings::WNStrNCmp(i->mName, _id, 256) == 0){
            _outType = i->mType;
            break;
        }
    }
    if(valueNum >= mStructType->mStructTypes.size()) {
        //cannot find a struct with the correct name, what about a function?
        _outValue = reinterpret_cast<llvm::Value*>(WNStrings::WNStrNDup(_id, 2048));
        _outType = mFunctionType;
        return(eWNOK);
    }
    
    std::vector<llvm::Value*> GepArray;
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    llvm::Value* tempValue = builder->CreateLoad(builder->CreateGEP(_structLoc, GepArray));
    
    GepArray.clear();
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 1));
    tempValue = builder->CreateLoad(builder->CreateGEP(tempValue, GepArray));

    GepArray.clear();
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), valueNum));
    
    _outLocation = builder->CreateGEP(tempValue, GepArray);
    
    _outValue = builder->CreateLoad(_outLocation, false, "");
    return(eWNOK);
}

GenerateCPPStructAccessor::GenerateCPPStructAccessor(WNScriptType _structType, WNScriptType _functionType):
    mStructType(_structType) ,
    mFunctionType(_functionType) {
}

eWNTypeError GenerateCPPStructAccessor::Execute(llvm::IRBuilderBase* _builder, llvm::Value*, llvm::Value* _structLoc, WN_CHAR* _id, WNScriptType& _outType, llvm::Value*& _outValue, llvm::Value*& _outLocation) const
{
    
#ifdef _WN_64_BIT
    llvm::Type* ptrType = llvm::IntegerType::getInt64Ty(llvm::getGlobalContext());
#else
    llvm::Type* ptrType = llvm::IntegerType::getInt32Ty(llvm::getGlobalContext());
#endif

    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_builder);
    WN_INT32 valueNum = -1;

    WNScriptType tempType = mStructType;
    while(tempType) {
        for(std::vector<WNContainedStructType>::iterator i = tempType->mStructTypes.begin(); i != tempType->mStructTypes.end(); ++i) {
            if(WNStrings::WNStrNCmp(i->mName, _id, 256) == 0){
                _outType = i->mType;
                valueNum = i->mCppOffset;
                break;
            }
        }   
        tempType = tempType->mParentClass;
    }

    if(valueNum < 0) {
        //cannot find a struct with the correct name, what about a function?
        _outValue = reinterpret_cast<llvm::Value*>(WNStrings::WNStrNDup(_id, 2048));
        _outType = mFunctionType;
        return(eWNOK);
    }
    
    std::vector<llvm::Value*> GepArray;
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    llvm::Value* tempValue = builder->CreateLoad(builder->CreateGEP(_structLoc, GepArray));
    
    GepArray.clear();
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 1));
    tempValue = builder->CreateLoad(builder->CreateGEP(tempValue, GepArray));

    tempValue = builder->CreatePtrToInt(tempValue, ptrType);
    tempValue = builder->CreateAdd(tempValue, llvm::ConstantInt::get(ptrType, valueNum));
    _outLocation = builder->CreateIntToPtr(tempValue, _outType->mLLVMType->getPointerTo());
    _outValue = builder->CreateLoad(_outLocation, false, "");
   
    return(eWNOK);
}

GenerateArrayIDOperator::GenerateArrayIDOperator(WNScriptType _structType, WNScriptType _intType, WNScriptType _functionType):
    mStructType(_structType), 
    mIntType(_intType), 
    mFunctionType(_functionType){
}

eWNTypeError GenerateArrayIDOperator::Execute(llvm::IRBuilderBase* _builder, llvm::Value*, llvm::Value* _structLoc, WN_CHAR* _id, WNScriptType& _outType, llvm::Value*& _outValue, llvm::Value*&) const
{
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_builder);
    if(WNStrings::WNStrCmp(_id, "resize") == 0) {
        _outType = mFunctionType;
        _outValue = reinterpret_cast<llvm::Value*>(const_cast<char*>("$resizeArray"));
        return(eWNOK);
    }
    if(WNStrings::WNStrCmp(_id, "length") != 0) {
        return(eWNDoesNotExist);
    }


#ifdef _WN_64_BIT
    llvm::Type* ptrType = llvm::IntegerType::getInt64Ty(llvm::getGlobalContext());
#else
    llvm::Type* ptrType = llvm::IntegerType::getInt32Ty(llvm::getGlobalContext());
#endif

    std::vector<llvm::Value*> GepArray;
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    llvm::Value* tempValue = builder->CreateLoad(builder->CreateGEP(_structLoc, GepArray));
    
    GepArray.clear();
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 1));
    tempValue = builder->CreateLoad(builder->CreateGEP(tempValue, GepArray));
    
    
    tempValue = builder->CreateLoad(builder->CreatePointerCast(tempValue, ptrType->getPointerTo()));
 
    _outValue = builder->CreateIntCast(tempValue, mIntType->mLLVMType, false);
    _outType = mIntType;
    return(eWNOK);
}


eWNTypeError GenerateDefaultAllocation::Execute(WNCodeModule& _module, WNScriptType _type, const WN_CHAR* _name, WN_BOOL, llvm::Value*& _outLocation) const {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
    if(!_type->mLLVMType) {
        return(eWNBadType);
    }
    _outLocation = builder->CreateAlloca(_type->mLLVMType, 0, _name);
    return(eWNOK);
}

eWNTypeError GenerateStructConstruction::Execute(WNCodeModule& _module, llvm::Value*& _retVal, WNScriptType structType, WNScriptLinkedList<WNExpression>::WNScriptLinkedListNode*, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) const {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
        llvm::Value* allocLocation = WN_NULL;
        _retVal = WN_NULL;
        eWNTypeError err = eWNOK;
        const GenerateAllocation* alloc = _module.GetTypeManager().GetAllocationOperation(structType);
        if(!alloc) {
            return(eWNCannotCreateType);
        }
        if(eWNOK != (err = alloc->Execute(_module, structType, "", false, allocLocation))) {
            return(err);
        }

    #ifdef _WN_64_BIT
        llvm::Type* ptrType = llvm::IntegerType::getInt64Ty(llvm::getGlobalContext());
    #else
        llvm::Type* ptrType = llvm::IntegerType::getInt32Ty(llvm::getGlobalContext());
    #endif
        WNScriptType sizeTType = 0;
        if(eWNOK != (err = _module.GetTypeManager().GetTypeByName("-SizeT", sizeTType))) {
            return(err);
        }
    
        std::vector<WNScriptType> mallocTypes;
        mallocTypes.push_back(sizeTType);
       
        llvm::Type* allocType = allocLocation->getType()->getContainedType(0)->getContainedType(0);
        llvm::Value* v = builder->Insert(llvm::CallInst::CreateMalloc(builder->GetInsertBlock(), ptrType, allocType, llvm::ConstantInt::get(sizeTType->mLLVMType, _module.GetExecutionEngine()->getDataLayout()->getTypeAllocSize(allocType))));

        v = builder->CreatePointerCast(v, allocLocation->getType()->getContainedType(0));

        llvm::Value* actualStruct = builder->Insert(llvm::CallInst::CreateMalloc(builder->GetInsertBlock(), ptrType, structType->mLLVMStructType, llvm::ConstantInt::get(sizeTType->mLLVMType, _module.GetExecutionEngine()->getDataLayout()->getTypeAllocSize(structType->mLLVMStructType))));
        actualStruct = builder->CreatePointerCast(actualStruct, structType->mLLVMStructType->getPointerTo(0));

        std::vector<llvm::Value*> GepArray;
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 1));
        builder->CreateStore(actualStruct, builder->CreateGEP(v, GepArray));

        GepArray.clear();
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 2));
        builder->CreateStore(llvm::ConstantInt::get(ptrType, 0), builder->CreateGEP(v, GepArray));

        GepArray.clear();
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
        ///builder->CreateGEP(v, GepArray);
        builder->CreateStore(builder->CreateIntToPtr(llvm::ConstantInt::get(sizeTType->mLLVMType, 0), llvm::IntegerType::getInt32PtrTy(llvm::getGlobalContext())), builder->CreateGEP(v, GepArray));

        WN_SIZE_T nameLen = WNStrings::WNStrLen(structType->mName);
        WN_CHAR *functionName = static_cast<WN_CHAR*>(WN_STACK_ALLOC(sizeof(WN_CHAR)* (nameLen + 1 + 5)));
        WNMemory::WNMemCpy(functionName, structType->mName, nameLen);
        WNMemory::WNMemCpy(functionName + nameLen, "Const", 5);
        functionName[nameLen + 5] = '\0';
        std::vector<FunctionParam> fParams;
        fParams.push_back(FunctionParam());
        fParams.back().mType = structType;
        fParams.back().mValue = v;
        
        WNScriptType returnType;
        llvm::Value* returnValue;
        if(eWNOK != (err = GenerateFunctionCall(_module, _def, functionName, fParams, false, returnType, returnValue, _compilationLog))) {
            return(err);
        }
        builder->CreateStore(v, allocLocation);
        _retVal = v;        
        return(eWNOK);
}

eWNTypeError GenerateArrayConstruction::Execute(WNCodeModule& _module, llvm::Value*& _retVal, WNScriptType _type, WNScriptLinkedList<WNExpression>::WNScriptLinkedListNode* _initializer, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) const {
     llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
        
    _retVal = WN_NULL;
    eWNTypeError err = eWNOK;
    if(!_initializer) {
        WNScriptType nullType;
        if(eWNOK != (err = GenerateNullAllocation(_module, _def, _retVal, nullType))) {
            return(err);
        }
    }
    if(!_type) {
        //Last level
        return(eWNError);
    }

    llvm::Value* allocLocation = WN_NULL;
    const GenerateAllocation* alloc = _module.GetTypeManager().GetAllocationOperation(_type);
    if(!alloc) {
        return(eWNCannotCreateType);
    }
    if(eWNOK != (err = alloc->Execute(_module, _type, "", false, allocLocation))) {
        return(err);
    }

#ifdef _WN_64_BIT
    llvm::Type* ptrType = llvm::IntegerType::getInt64Ty(llvm::getGlobalContext());
    const WN_SIZE_T ptrSize = 8;
#else
    llvm::Type* ptrType = llvm::IntegerType::getInt32Ty(llvm::getGlobalContext());
    const WN_SIZE_T ptrSize = 4;
#endif
    WNScriptType sizeTType = 0;
    WNScriptType intType = 0;
    if(eWNOK != (err = _module.GetTypeManager().GetTypeByName("-SizeT", sizeTType)) ||
        eWNOK != (err = _module.GetTypeManager().GetTypeByName("Int", intType))) {
        return(err);
    }

    if(eWNOK != (err = _initializer->value->GenerateCode(_module, _def, _compilationLog))) {
        return(err);
    }

    llvm::Value* sizeVal = _initializer->value->GetValue();
    if(_initializer->value->GetType() != intType) {
        const GenerateCastingOperation* cast = _module.GetTypeManager().GetCastingOperation(_initializer->value->GetType(), intType);
        if(!cast) {
            return(eWNInvalidCast);
        }
        if(eWNOK != (err = cast->Execute(builder, sizeVal, sizeVal))) {
            return(err);
        }
    }

    llvm::Type* allocType = _type->mLLVMType->getContainedType(0);
    llvm::Value* v = builder->Insert(llvm::CallInst::CreateMalloc(builder->GetInsertBlock(), ptrType, allocType, llvm::ConstantInt::get(sizeTType->mLLVMType, _module.GetExecutionEngine()->getDataLayout()->getTypeAllocSize(allocType))));
    v = builder->CreatePointerCast(v, allocLocation->getType()->getContainedType(0));

    WN_UINT64 allocSize = _module.GetExecutionEngine()->getDataLayout()->getTypeAllocSize(_type->mArrayType->mLLVMType);
    llvm::Value* allocationSize = builder->CreateMul(sizeVal, llvm::ConstantInt::get(intType->mLLVMType, allocSize));
    allocationSize = builder->CreateAdd(allocationSize, llvm::ConstantInt::get(intType->mLLVMType, (ptrSize * 2)));
        
    allocType = _type->mArrayType->mLLVMType;
    llvm::Value* actualArray = builder->Insert(llvm::CallInst::CreateMalloc(builder->GetInsertBlock(), ptrType, allocType, builder->CreateIntCast(allocationSize, sizeTType->mLLVMType, false)));
    actualArray = builder->CreatePointerCast(actualArray, _type->mArrayType->mLLVMType->getPointerTo(0));

    std::vector<llvm::Value*> GepArray;
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 1));
    builder->CreateStore(actualArray, builder->CreateGEP(v, GepArray));

    GepArray.clear();
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 2));
    builder->CreateStore(llvm::ConstantInt::get(ptrType, 0), builder->CreateGEP(v, GepArray));

    GepArray.clear();
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));

    builder->CreateStore(builder->CreateIntToPtr(llvm::ConstantInt::get(sizeTType->mLLVMType, 0), llvm::IntegerType::getInt32PtrTy(llvm::getGlobalContext())), builder->CreateGEP(v, GepArray));

    llvm::Value* storeSizeLocation = builder->CreatePointerCast(actualArray, ptrType->getPointerTo(0));
    builder->CreateStore(builder->CreateIntCast(sizeVal, ptrType, false), storeSizeLocation);
    llvm::Value* typeLocation = builder->CreatePtrToInt(storeSizeLocation, ptrType);
    typeLocation = builder->CreateAdd(typeLocation, llvm::ConstantInt::get(ptrType, ptrSize));
    typeLocation = builder->CreateIntToPtr(typeLocation, ptrType->getPointerTo(0));
    builder->CreateStore(llvm::ConstantInt::get(ptrType, reinterpret_cast<WN_INT64>(_type)), typeLocation);
    if(!(_type->mArrayType->mLLVMStructType || _type->mArrayType->mArrayType )) {
        _retVal = v;
        return(eWNOK);
    }

    llvm::Value* dataPtr = builder->CreatePtrToInt(actualArray, ptrType);
    dataPtr = builder->CreateAdd(dataPtr, llvm::ConstantInt::get(ptrType, ptrSize * 2));
    dataPtr = builder->CreateIntToPtr(dataPtr, _type->mArrayType->mLLVMType->getPointerTo(0));

    llvm::Value* counterLoc = builder->CreateAlloca(ptrType);
    builder->CreateStore(llvm::ConstantInt::get(ptrType, 0), counterLoc);
    llvm::BasicBlock* counterBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
    llvm::BasicBlock* bodyBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
    llvm::BasicBlock* endBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
    builder->CreateBr(counterBlock);
    builder->SetInsertPoint(counterBlock);
    llvm::Value* cond = builder->CreateICmpUGE(builder->CreateLoad(counterLoc), builder->CreateIntCast(sizeVal, ptrType, false));
    builder->CreateCondBr(cond, endBlock, bodyBlock);
    builder->SetInsertPoint(bodyBlock);

    llvm::Value* gepLocation = builder->CreateGEP(dataPtr, builder->CreateLoad(counterLoc));
    if(_type->mArrayType->mLLVMStructType) {
        llvm::Value* structCreation;
        WNScriptType nullType;
        if(eWNOK != (err = GenerateNullAllocation(_module, _def, structCreation, nullType))) {
            return(err);
        }
        const GenerateCastingOperation* cast = _module.GetTypeManager().GetCastingOperation(nullType, _type->mArrayType);
        if(cast == WN_NULL) {
            return(eWNPlatformError);
        }
        if(eWNOK != (err = cast->Execute(_module.GetBuilder(), structCreation, structCreation))) {
            return(err);
        }
        const GenerateAssignment* assOp =_module.GetTypeManager().GetAssignmentOperation(_type->mArrayType, AT_CHOWN);
        if(eWNOK != (err = assOp->Execute(_module, true, _type->mArrayType, structCreation, gepLocation, _def, _compilationLog))) {
            return(err);
        }
    } else if(_type->mArrayType->mArrayType) {
        const GenerateConstruction* construction = _module.GetTypeManager().GetConstructionOperation(_type->mArrayType);
        if(!construction) {
            _compilationLog.Log(WNLogging::eError, 0, "Could not find construction operation for ", _type->mArrayType->mName);
            return(eWNCannotCreateType);
        }
        llvm::Value* arrayCreation;
        
        if(eWNOK != (err = construction->Execute(_module, arrayCreation, _type->mArrayType, _initializer->next, _def, _compilationLog))) {
            return(err);
        }
        const GenerateAssignment* assOp =_module.GetTypeManager().GetAssignmentOperation(_type->mArrayType, AT_CHOWN);
        if(eWNOK != (err = assOp->Execute(_module, true, _type->mArrayType, arrayCreation, gepLocation, _def, _compilationLog))) {
            return(err);
        }
    }
    builder->CreateStore(builder->CreateAdd(builder->CreateLoad(counterLoc), llvm::ConstantInt::get(ptrType, 1)), counterLoc);
    builder->CreateBr(counterBlock);
    builder->SetInsertPoint(endBlock);
    _retVal = v;
    return(eWNOK);
}

eWNTypeError GenerateArrayCopyConstruction::Execute(WNCodeModule& _module, llvm::Value*& _retVal, WNScriptType _type, llvm::Value* _copyValue, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) const {
    WN_UNUSED_ARG(_compilationLog);
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
        
    _retVal = WN_NULL;
    eWNTypeError err = eWNOK;
    llvm::Value* allocLocation = WN_NULL;
    const GenerateAllocation* alloc = _module.GetTypeManager().GetAllocationOperation(_type);
    if(!alloc) {
        return(eWNCannotCreateType);
    }
    if(eWNOK != (err = alloc->Execute(_module, _type, "", false, allocLocation))) {
        return(err);
    }

#ifdef _WN_64_BIT
    llvm::Type* ptrType = llvm::IntegerType::getInt64Ty(llvm::getGlobalContext());
    const WN_SIZE_T ptrSize = 8;
#else
    llvm::Type* ptrType = llvm::IntegerType::getInt32Ty(llvm::getGlobalContext());
    const WN_SIZE_T ptrSize = 4;
#endif
    WNScriptType sizeTType = 0;
    WNScriptType intType = 0;
    if(eWNOK != (err = _module.GetTypeManager().GetTypeByName("-SizeT", sizeTType)) ||
        eWNOK != (err = _module.GetTypeManager().GetTypeByName("Int", intType))) {
        return(err);
    }
    std::vector<llvm::Value*> GepArray;

    llvm::Type* allocType = _type->mLLVMType->getContainedType(0);
    llvm::Value* v = builder->Insert(llvm::CallInst::CreateMalloc(builder->GetInsertBlock(), ptrType, allocType, llvm::ConstantInt::get(sizeTType->mLLVMType, _module.GetExecutionEngine()->getDataLayout()->getTypeAllocSize(allocType))));
    v = builder->CreatePointerCast(v, allocLocation->getType()->getContainedType(0));

    GepArray.clear();
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 2));
    builder->CreateStore(llvm::ConstantInt::get(ptrType, 0), builder->CreateGEP(v, GepArray));

    GepArray.clear();
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));

    builder->CreateStore(builder->CreateIntToPtr(llvm::ConstantInt::get(sizeTType->mLLVMType, 0), llvm::IntegerType::getInt32PtrTy(llvm::getGlobalContext())), builder->CreateGEP(v, GepArray));


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    GepArray.clear();
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 1));
    llvm::Value* oldSizeLocation = builder->CreateGEP(_copyValue, GepArray);
    oldSizeLocation = builder->CreateLoad(builder->CreateBitCast(oldSizeLocation, ptrType->getPointerTo(0)->getPointerTo(0)));
    
    llvm::BasicBlock* copyBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "copyBlock", _def->mFunction);
    llvm::BasicBlock* nullBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "nullBlock", _def->mFunction);
    llvm::BasicBlock* exitBlock= llvm::BasicBlock::Create(llvm::getGlobalContext(), "exitBlock", _def->mFunction);
    builder->CreateCondBr(builder->CreateICmpNE(builder->CreatePtrToInt(oldSizeLocation, ptrType), llvm::ConstantInt::get(ptrType, 0)), copyBlock, nullBlock);
    builder->SetInsertPoint(copyBlock);

    
    {/////////////WE ACTUALLY HAVE ANARRAY
            llvm::Value* arraySize = builder->CreateLoad(oldSizeLocation);
            WN_UINT64 allocSize = _module.GetExecutionEngine()->getDataLayout()->getTypeAllocSize(_type->mArrayType->mLLVMType);
            llvm::Value* allocationSize = builder->CreateMul(arraySize, llvm::ConstantInt::get(ptrType, allocSize));
            allocationSize = builder->CreateAdd(allocationSize, llvm::ConstantInt::get(ptrType, (ptrSize * 2)));
        
            allocType = _type->mArrayType->mLLVMType;
            llvm::Value* actualArray = builder->Insert(llvm::CallInst::CreateMalloc(builder->GetInsertBlock(), ptrType, allocType, builder->CreateIntCast(allocationSize, sizeTType->mLLVMType, false)));
            actualArray = builder->CreatePointerCast(actualArray, _type->mArrayType->mLLVMType->getPointerTo(0));

            GepArray.clear();
            GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
            GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 1));
            builder->CreateStore(actualArray, builder->CreateGEP(v, GepArray));

            llvm::Type* types[3] = {llvm::IntegerType::getInt8PtrTy(llvm::getGlobalContext()), 
                                    llvm::IntegerType::getInt8PtrTy(llvm::getGlobalContext()), 
                                    ptrType
                                    };
            llvm::Function* fnc = llvm::Intrinsic::getDeclaration(_module.GetModule(), llvm::Intrinsic::memcpy, llvm::makeArrayRef(types));
            llvm::Value*  pointers[5] = {  builder->CreatePointerCast(actualArray, llvm::IntegerType::getInt8PtrTy(llvm::getGlobalContext())), 
                                           builder->CreatePointerCast(oldSizeLocation, llvm::IntegerType::getInt8PtrTy(llvm::getGlobalContext())), 
                                           allocationSize,
                                           llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0),
                                           llvm::ConstantInt::get(llvm::IntegerType::getInt1Ty(llvm::getGlobalContext()), 0)
                            };
            builder->CreateCall(fnc, llvm::makeArrayRef(pointers));

            llvm::Value* typeLocation = builder->CreatePtrToInt(actualArray, ptrType);
            typeLocation = builder->CreateAdd(typeLocation, llvm::ConstantInt::get(ptrType, ptrSize));
            typeLocation = builder->CreateIntToPtr(typeLocation, ptrType->getPointerTo(0));
            builder->CreateStore(llvm::ConstantInt::get(ptrType, reinterpret_cast<WN_INT64>(_type)), typeLocation);
            
            if(_type->mArrayType->mLLVMStructType || _type->mArrayType->mArrayType ) {
                llvm::Value* dataPtr = builder->CreatePtrToInt(actualArray, ptrType);
                dataPtr = builder->CreateAdd(dataPtr, llvm::ConstantInt::get(ptrType, ptrSize * 2));
                dataPtr = builder->CreateIntToPtr(dataPtr, _type->mArrayType->mLLVMType->getPointerTo(0));
                llvm::Value* counterLoc = builder->CreateAlloca(ptrType);
                llvm::BasicBlock* counterBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "counterBlock", _def->mFunction);
                llvm::BasicBlock* bodyBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "bodyBlock", _def->mFunction);
                builder->CreateBr(counterBlock);
                builder->SetInsertPoint(counterBlock);
                llvm::Value* cond = builder->CreateICmpUGE(builder->CreateLoad(counterLoc), builder->CreateIntCast(arraySize, ptrType, false));
                builder->CreateCondBr(cond, exitBlock, bodyBlock);
                builder->SetInsertPoint(bodyBlock);

                llvm::Value* gepLocation = builder->CreateGEP(dataPtr, builder->CreateLoad(counterLoc));
                llvm::Value* structNodeLocation = builder->CreateLoad(gepLocation);
                GepArray.clear();
                GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
                GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 2));
                builder->CreateAtomicRMW(llvm::AtomicRMWInst::Add, builder->CreateGEP(structNodeLocation, GepArray), llvm::ConstantInt::get(ptrType, 1), llvm::Monotonic);

                builder->CreateStore(builder->CreateAdd(builder->CreateLoad(counterLoc), llvm::ConstantInt::get(ptrType, 1)), counterLoc);
                builder->CreateBr(counterBlock);
        } else {
            builder->CreateBr(exitBlock);        
        }
    }
    builder->SetInsertPoint(nullBlock);
    {////////////WE DO NOT HAVE AN ARRAY
        GepArray.clear();
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 1));
        builder->CreateStore(builder->CreateIntToPtr(llvm::ConstantInt::get(ptrType, 0), _type->mArrayType->mLLVMType->getPointerTo(0)), builder->CreateGEP(v, GepArray));
    }


    builder->CreateBr(exitBlock);
    builder->SetInsertPoint(exitBlock);
    ////////////////////////////////////
    _retVal = v;
    return(eWNOK);
}

eWNTypeError GenerateStructCopyConstruction::Execute(WNCodeModule& _module, llvm::Value*& _retVal, WNScriptType structType, llvm::Value* _copyValue, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) const {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
        llvm::Value* allocLocation = WN_NULL;
        _retVal = WN_NULL;
        eWNTypeError err = eWNOK;
        const GenerateAllocation* alloc = _module.GetTypeManager().GetAllocationOperation(structType);
        if(!alloc) {
            return(eWNCannotCreateType);
        }
        if(eWNOK != (err = alloc->Execute(_module, structType, "", false, allocLocation))) {
            return(err);
        }

    #ifdef _WN_64_BIT
        llvm::Type* ptrType = llvm::IntegerType::getInt64Ty(llvm::getGlobalContext());
    #else
        llvm::Type* ptrType = llvm::IntegerType::getInt32Ty(llvm::getGlobalContext());
    #endif
        WNScriptType sizeTType = 0;
        if(eWNOK != (err = _module.GetTypeManager().GetTypeByName("-SizeT", sizeTType))) {
            return(err);
        }
    
        std::vector<WNScriptType> mallocTypes;
        mallocTypes.push_back(sizeTType);
       
        llvm::Type* allocType = allocLocation->getType()->getContainedType(0)->getContainedType(0);
        llvm::Value* v = builder->Insert(llvm::CallInst::CreateMalloc(builder->GetInsertBlock(), ptrType, allocType, llvm::ConstantInt::get(sizeTType->mLLVMType, _module.GetExecutionEngine()->getDataLayout()->getTypeAllocSize(allocType))));

        v = builder->CreatePointerCast(v, allocLocation->getType()->getContainedType(0));

        llvm::Value* actualStruct = builder->Insert(llvm::CallInst::CreateMalloc(builder->GetInsertBlock(), ptrType, structType->mLLVMStructType, llvm::ConstantInt::get(sizeTType->mLLVMType, _module.GetExecutionEngine()->getDataLayout()->getTypeAllocSize(structType->mLLVMStructType))));
        actualStruct = builder->CreatePointerCast(actualStruct, structType->mLLVMStructType->getPointerTo(0));

        std::vector<llvm::Value*> GepArray;
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 1));
        builder->CreateStore(actualStruct, builder->CreateGEP(v, GepArray));

        GepArray.clear();
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 2));
        builder->CreateStore(llvm::ConstantInt::get(ptrType, 0), builder->CreateGEP(v, GepArray));

        GepArray.clear();
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
        builder->CreateStore(builder->CreateIntToPtr(llvm::ConstantInt::get(sizeTType->mLLVMType, 0), llvm::IntegerType::getInt32PtrTy(llvm::getGlobalContext())), builder->CreateGEP(v, GepArray));

        WN_SIZE_T nameLen = WNStrings::WNStrLen(structType->mName);
        WN_CHAR *functionName = static_cast<WN_CHAR*>(WN_STACK_ALLOC(sizeof(WN_CHAR)* (nameLen + 1 + 9)));
        WNMemory::WNMemCpy(functionName, structType->mName, nameLen);
        WNMemory::WNMemCpy(functionName + nameLen, "CopyConst", 10);
        std::vector<FunctionParam> fParams;
        fParams.push_back(FunctionParam());
        fParams.back().mType = structType;
        fParams.back().mValue = v;
        fParams.push_back(FunctionParam());
        fParams.back().mType = structType;
        fParams.back().mValue = _copyValue;
            
        WNScriptType returnType;
        llvm::Value* returnValue;
        if(eWNOK != (err = GenerateFunctionCall(_module, _def, functionName, fParams, false, returnType, returnValue, _compilationLog))) {
            return(err);
        }
        
        builder->CreateStore(v, allocLocation);
        _retVal = v;        
        return(eWNOK);
}

eWNTypeError GenerateDefaultAssignment::Execute(WNCodeModule& _module, bool, WNScriptType, llvm::Value* _value, llvm::Value* _assignLocation, const WNFunctionDefinition*, WNLogging::WNLog&) const {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
    builder->CreateStore(_value, _assignLocation);
    return(eWNOK);
}

eWNTypeError GenerateStructAssignment::Execute(WNCodeModule& _module, bool _declaration, WNScriptType _type, llvm::Value* _value, llvm::Value* _assignLocation, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) const {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
    std::vector<llvm::Value*> GepArray;

    if(!_declaration) {
        //First check what was currently in there, if we were the owner then destroy it
        eWNTypeError err = eWNOK;
        const GenerateDestruction * op = _module.GetTypeManager().GetDestructionOperation(_type);
        if(op) {
            if(eWNOK != (err = op->Execute(_module, _type, _assignLocation, _def, _compilationLog))) {
                return(err);
            }
        }

        //Now reduce the ref-count on the smart-ptr if that goes to 0, then delete it
    }

    GepArray.clear();
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    llvm::Value* ownerLocation = builder->CreateGEP(_value, GepArray);
    llvm::Value* tempValue = builder->CreateLoad(ownerLocation);

    llvm::BasicBlock* assignBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "AssignStruct", _def->mFunction);
    llvm::BasicBlock* endBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "Continue", _def->mFunction);
    
#ifdef _WN_64_BIT
    llvm::Type* ptrType = llvm::IntegerType::getInt64Ty(llvm::getGlobalContext());
#else
    llvm::Type* ptrType = llvm::IntegerType::getInt32Ty(llvm::getGlobalContext());
#endif

    llvm::Value* cmp = builder->CreateICmpEQ(builder->CreatePtrToInt(tempValue, ptrType), llvm::ConstantInt::get(ptrType, 0));
    builder->CreateCondBr(cmp, assignBlock, endBlock);
    builder->SetInsertPoint(assignBlock);
    llvm::Value* ptrVal = builder->CreatePointerCast(_assignLocation, llvm::IntegerType::getInt32PtrTy(llvm::getGlobalContext()));
    builder->CreateStore(ptrVal, ownerLocation);
    builder->CreateBr(endBlock);
    builder->SetInsertPoint(endBlock);
    GepArray.clear();
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 2));
    llvm::Value* countLocation = builder->CreateGEP(_value, GepArray);
    builder->CreateAtomicRMW(llvm::AtomicRMWInst::Add, countLocation, llvm::ConstantInt::get(ptrType, 1), llvm::Monotonic);
    builder->CreateStore(_value, _assignLocation);
    return(eWNOK);
}

eWNTypeError GenerateStructTransfer::Execute(WNCodeModule& _module, bool _declaration, WNScriptType _type, llvm::Value* _value, llvm::Value* _assignLocation, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) const {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
    std::vector<llvm::Value*> GepArray;

    llvm::Value* assignLocationTemp = NULL;
    if(!_declaration) {
        assignLocationTemp = builder->CreateAlloca(_type->mLLVMType, 0);
        builder->CreateStore(builder->CreateLoad(_assignLocation), assignLocationTemp);
    }

            
#ifdef _WN_64_BIT
    llvm::Type* ptrType = llvm::IntegerType::getInt64Ty(llvm::getGlobalContext());
#else
    llvm::Type* ptrType = llvm::IntegerType::getInt32Ty(llvm::getGlobalContext());
#endif

    GepArray.clear();
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    llvm::Value* ownerLocation = builder->CreateGEP(_value, GepArray);
    llvm::Value* cmp = NULL;

    llvm::Value* ptrVal = builder->CreatePointerCast(_assignLocation, llvm::IntegerType::getInt32PtrTy(llvm::getGlobalContext()));
    builder->CreateStore(ptrVal, ownerLocation);

    GepArray.clear();
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 2));
    llvm::Value* countLocation = builder->CreateGEP(_value, GepArray);
    builder->CreateAtomicRMW(llvm::AtomicRMWInst::Add, countLocation, llvm::ConstantInt::get(ptrType, 1), llvm::Monotonic);
    builder->CreateStore(_value, _assignLocation);
    if(!_declaration) {
        
        llvm::BasicBlock* transferToTemp = llvm::BasicBlock::Create(llvm::getGlobalContext(), "TransferToTemp", _def->mFunction);
        llvm::BasicBlock* endBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "EndTransferToTemp", _def->mFunction);
        
        GepArray.clear();
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
        llvm::Value* tempValueLocation =  builder->CreateLoad(builder->CreateGEP(assignLocationTemp, GepArray));
        GepArray.clear();
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
        llvm::Value* tempOwnerLocation = builder->CreateGEP(tempValueLocation, GepArray);
        cmp = builder->CreateICmpEQ(builder->CreatePtrToInt(builder->CreateLoad(tempOwnerLocation), ptrType), builder->CreatePtrToInt(_assignLocation, ptrType));

        builder->CreateCondBr(cmp, transferToTemp, endBlock);
        builder->SetInsertPoint(transferToTemp);
    
        
        builder->CreateStore(builder->CreatePointerCast(assignLocationTemp, llvm::IntegerType::getInt32PtrTy(llvm::getGlobalContext())), tempOwnerLocation);
        builder->CreateBr(endBlock);
        builder->SetInsertPoint(endBlock);
        //First check what was currently in there, if we were the owner then destroy it
        eWNTypeError err = eWNOK;
        const GenerateDestruction * op = _module.GetTypeManager().GetDestructionOperation(_type);
        if(op) {
            if(eWNOK != (err = op->Execute(_module, _type, assignLocationTemp, _def, _compilationLog))) {
                return(err);
            }
        }
    }
    return(eWNOK);
}


eWNTypeError GenerateStructDestruction::Execute(WNCodeModule& _module, WNScriptType _node, llvm::Value* _structLoc, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) const {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
    
    std::vector<llvm::Value*> GepArray;
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    llvm::Value* spLoc = builder->CreateLoad(builder->CreateGEP(_structLoc, GepArray));
    
    GepArray.clear();
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    llvm::Value* tempValue = builder->CreateLoad(builder->CreateGEP(spLoc, GepArray));

    llvm::BasicBlock* destroyBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "DestroyStruct", _def->mFunction);
    llvm::BasicBlock* endBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "Continue", _def->mFunction);

#ifdef _WN_64_BIT
    llvm::Type* ptrType = llvm::IntegerType::getInt64Ty(llvm::getGlobalContext());
#else
    llvm::Type* ptrType = llvm::IntegerType::getInt32Ty(llvm::getGlobalContext());
#endif

    llvm::Value* cmp = builder->CreateICmpEQ(builder->CreatePtrToInt(tempValue, ptrType), builder->CreatePtrToInt(_structLoc, ptrType));
    builder->CreateCondBr(cmp, destroyBlock, endBlock);
    builder->SetInsertPoint(destroyBlock);

    GepArray.clear();
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 1));
    llvm::Value* objectLocationLocation = builder->CreateGEP(spLoc, GepArray);
    tempValue = builder->CreateLoad(objectLocationLocation);

    WNScriptType voidStarType;

    if(eWNOK != _module.GetTypeManager().GetTypeByName("-Ptr", voidStarType)) {
        return(eWNPlatformError);
    }

    std::vector<FunctionParam> fParams;
    fParams.push_back(FunctionParam());
    fParams.back().mType = _node;
    fParams.back().mValue = spLoc;
    
    llvm::BasicBlock* notNull = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
    cmp = builder->CreateICmpNE(builder->CreatePtrToInt(tempValue, ptrType, ""), llvm::ConstantInt::get(ptrType, 0));
    builder->CreateCondBr(cmp, notNull, endBlock);
    builder->SetInsertPoint(notNull);


    WNScriptType voidReturnType;
    if(eWNOK != _module.GetTypeManager().GetTypeByName("Void", voidReturnType)) {
        _compilationLog.Log(WNLogging::eCritical, 0, "Error cannot find void type");
        return(eWNError);
    }
    
    llvm::FunctionType* t = llvm::FunctionType::get(voidReturnType->mLLVMType, _node->mLLVMType, WN_FALSE);

    GepArray.clear();
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    llvm::Value* fPtrLoc = builder->CreateLoad(builder->CreateGEP(tempValue, GepArray));
    fPtrLoc = builder->CreateLoad(builder->CreatePointerCast(fPtrLoc, ptrType->getPointerTo(0)));
    llvm::Value* v = builder->CreateIntToPtr(fPtrLoc, t->getPointerTo(0));
    builder->CreateCall(v, spLoc);
    builder->Insert(llvm::CallInst::CreateFree(tempValue, builder->GetInsertBlock()));
    builder->CreateStore(builder->CreateIntToPtr(llvm::ConstantInt::get(ptrType, 0), _node->mLLVMStructType->getPointerTo(0)), objectLocationLocation);

    builder->CreateBr(endBlock);
    builder->SetInsertPoint(endBlock);
    
    GepArray.clear();
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 2));
    tempValue = builder->CreateGEP(spLoc, GepArray);
    
    llvm::Value* val = builder->CreateAtomicRMW(llvm::AtomicRMWInst::Sub, tempValue, llvm::ConstantInt::get(ptrType, 1), llvm::Monotonic);
    llvm::BasicBlock* deleteScopeBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
    endBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
    
    cmp = builder->CreateICmpSLE(val, llvm::ConstantInt::get(ptrType, 1)); 

    builder->CreateCondBr(cmp, deleteScopeBlock, endBlock);
    builder->SetInsertPoint(deleteScopeBlock);

    builder->Insert(llvm::CallInst::CreateFree(spLoc, builder->GetInsertBlock()));
    builder->CreateBr(endBlock);

    builder->SetInsertPoint(endBlock);
    return(eWNOK);
}

eWNTypeError GenerateCStructDestruction::Execute(WNCodeModule& _module, WNScriptType _node, llvm::Value* _structLoc, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) const {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
    
    std::vector<llvm::Value*> GepArray;
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    llvm::Value* spLoc = builder->CreateLoad(builder->CreateGEP(_structLoc, GepArray));
    
    GepArray.clear();
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    llvm::Value* tempValue = builder->CreateLoad(builder->CreateGEP(spLoc, GepArray));

    llvm::BasicBlock* destroyBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "DestroyStruct", _def->mFunction);
    llvm::BasicBlock* endBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "Continue", _def->mFunction);
    llvm::Type* i32Type = llvm::IntegerType::getInt32Ty(llvm::getGlobalContext());
#ifdef _WN_64_BIT
    llvm::Type* ptrType = llvm::IntegerType::getInt64Ty(llvm::getGlobalContext());
#else
    llvm::Type* ptrType = llvm::IntegerType::getInt32Ty(llvm::getGlobalContext());
#endif

    llvm::Value* cmp = builder->CreateICmpEQ(builder->CreatePtrToInt(tempValue, ptrType), builder->CreatePtrToInt(_structLoc, ptrType));
    builder->CreateCondBr(cmp, destroyBlock, endBlock);
    builder->SetInsertPoint(destroyBlock);

    GepArray.clear();
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 1));
    llvm::Value* objectLocationLocation = builder->CreateGEP(spLoc, GepArray);
    tempValue = builder->CreateLoad(objectLocationLocation);

    WNScriptType voidStarType;

    if(eWNOK != _module.GetTypeManager().GetTypeByName("-Ptr", voidStarType)) {
        return(eWNPlatformError);
    }

    std::vector<FunctionParam> fParams;
    fParams.push_back(FunctionParam());
    fParams.back().mType = _node;
    fParams.back().mValue = spLoc;
    
    llvm::BasicBlock* notNull = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
    cmp = builder->CreateICmpNE(builder->CreatePtrToInt(tempValue, ptrType, ""), llvm::ConstantInt::get(ptrType, 0));
    builder->CreateCondBr(cmp, notNull, endBlock);
    builder->SetInsertPoint(notNull);


    WNScriptType voidReturnType;
    if(eWNOK != _module.GetTypeManager().GetTypeByName("Void", voidReturnType)) {
        _compilationLog.Log(WNLogging::eCritical, 0, "Error cannot find void type");
        return(eWNError);
    }
    
    llvm::FunctionType* t = llvm::FunctionType::get(voidReturnType->mLLVMType, _node->mLLVMType, WN_FALSE);

    llvm::Value* v = builder->CreateIntToPtr(llvm::ConstantInt::get(ptrType, reinterpret_cast<WN_SIZE_T>(_node->mCDestructor)), t->getPointerTo(0));
    builder->CreateCall(v, spLoc);
    builder->Insert(llvm::CallInst::CreateFree(tempValue, builder->GetInsertBlock()));
    llvm::Value* itp = builder->CreateIntToPtr(llvm::ConstantInt::get(i32Type, 0), i32Type->getPointerTo(0));
    builder->CreateStore(itp, objectLocationLocation);
    
    builder->CreateBr(endBlock);
    builder->SetInsertPoint(endBlock);
    
    GepArray.clear();
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 2));
    tempValue = builder->CreateGEP(spLoc, GepArray);
    
    llvm::Value* val = builder->CreateAtomicRMW(llvm::AtomicRMWInst::Sub, tempValue, llvm::ConstantInt::get(ptrType, 1), llvm::Monotonic);
    llvm::BasicBlock* deleteScopeBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
    endBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
    
    cmp = builder->CreateICmpSLE(val, llvm::ConstantInt::get(ptrType, 1)); 

    builder->CreateCondBr(cmp, deleteScopeBlock, endBlock);
    builder->SetInsertPoint(deleteScopeBlock);

    builder->Insert(llvm::CallInst::CreateFree(spLoc, builder->GetInsertBlock()));
    builder->CreateBr(endBlock);

    builder->SetInsertPoint(endBlock);
    return(eWNOK);
}


eWNTypeError GenerateReinterpretCastOperation::Execute(llvm::IRBuilderBase* _builder, llvm::Value* _expr1, llvm::Value*& _outReturnValue) const {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_builder);
    _outReturnValue = builder->CreatePointerCast(_expr1, mScriptType->mLLVMType);
    return(eWNOK);
}


eWNTypeError GenerateStructCompare::Execute(llvm::IRBuilderBase* _builder, llvm::Value* _expr1, llvm::Value* _expr2, WNScriptType& _destType, llvm::Value*& _outReturnVal) const {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>* >(_builder);
    
            
#ifdef _WN_64_BIT
    llvm::Type* ptrType = llvm::IntegerType::getInt64Ty(llvm::getGlobalContext());
#else
    llvm::Type* ptrType = llvm::IntegerType::getInt32Ty(llvm::getGlobalContext());
#endif

    std::vector<llvm::Value*> GepArray;
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 1));
    llvm::Value* lhsValue = builder->CreateLoad(builder->CreateGEP(_expr1, GepArray));
    llvm::Value* rhsValue = builder->CreateLoad(builder->CreateGEP(_expr2, GepArray));
    lhsValue = builder->CreatePtrToInt(lhsValue, ptrType);
    rhsValue = builder->CreatePtrToInt(rhsValue, ptrType);
    
    llvm::Value* cmp = NULL;
    if(mEq) {
        cmp = builder->CreateICmpEQ(lhsValue, rhsValue);
    } else {
        cmp = builder->CreateICmpNE(lhsValue, rhsValue);
    }
    _outReturnVal = builder->CreateIntCast(cmp, mBoolType->mLLVMType, false);
    
    _destType = mBoolType;
    return(eWNOK);
}

eWNTypeError GenerateDefaultArrayAccessOperation::Execute(WNCodeModule& _module, llvm::Value* _arrayLoc, llvm::Value* _elemNumber, llvm::Value*& _outValue, llvm::Value*& _outLocation, const WNFunctionDefinition* _def) const {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());

#ifdef _WN_64_BIT
    llvm::Type* ptrType = llvm::IntegerType::getInt64Ty(llvm::getGlobalContext());
    const size_t ptrSize = 8;
#else
    llvm::Type* ptrType = llvm::IntegerType::getInt32Ty(llvm::getGlobalContext());
    const size_t ptrSize = 4;
#endif

    std::vector<llvm::Value*> GepArray;
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    llvm::Value* tempValue = _arrayLoc;//

    GepArray.clear();
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 1));
    llvm::Value* arrayLocation = builder->CreateLoad(builder->CreateGEP(tempValue, GepArray));
    llvm::Value* intPtr = builder->CreatePointerCast(arrayLocation, ptrType->getPointerTo(0));
    arrayLocation = builder->CreatePtrToInt(arrayLocation, ptrType);
    arrayLocation = builder->CreateAdd(arrayLocation , llvm::ConstantInt::get(ptrType, ptrSize*2));
    arrayLocation = builder->CreateIntToPtr(arrayLocation, mScriptType->mArrayType->mLLVMType->getPointerTo(0));
   
    llvm::Value* arraySize = builder->CreateLoad(intPtr);
    llvm::Value* elemNumber = builder->CreateIntCast(_elemNumber, ptrType, true);
    
    llvm::Value* cmp = builder->CreateICmpSGE(elemNumber, llvm::ConstantInt::get(ptrType, 0)); 
    cmp = builder->CreateAnd(cmp, builder->CreateICmpSLT(elemNumber, arraySize));
    llvm::BasicBlock* oobBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "oobBlock", _def->mFunction);
    llvm::BasicBlock* endBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
    builder->CreateCondBr(cmp, endBlock, oobBlock);
    builder->SetInsertPoint(oobBlock);
    GenerateAssertion(_module);
    builder->CreateBr(endBlock);
    builder->SetInsertPoint(endBlock);
    GepArray.clear();
    GepArray.push_back(elemNumber);
    
    _outLocation = builder->CreateGEP(arrayLocation, GepArray);
    
    _outValue = builder->CreateLoad(_outLocation, false, "");
    return(eWNOK);   
}

eWNTypeError GenerateArrayDestruction::Execute(WNCodeModule& _module, WNScriptType _node, llvm::Value* _structLoc, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) const {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
    
    std::vector<llvm::Value*> GepArray;
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    llvm::Value* spLoc = builder->CreateLoad(builder->CreateGEP(_structLoc, GepArray));
    
    GepArray.clear();
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));

    llvm::Value* tempValue = builder->CreateLoad(builder->CreateGEP(spLoc, GepArray));

    llvm::BasicBlock* destroyBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "DestroyArray", _def->mFunction);
    llvm::BasicBlock* endBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "Continue", _def->mFunction);

#ifdef _WN_64_BIT
    llvm::Type* ptrType = llvm::IntegerType::getInt64Ty(llvm::getGlobalContext());
    WN_SIZE_T ptrSize = 8;
#else
    llvm::Type* ptrType = llvm::IntegerType::getInt32Ty(llvm::getGlobalContext());
    WN_SIZE_T ptrSize = 4;
#endif

    llvm::Value* cmp = builder->CreateICmpEQ(builder->CreatePtrToInt(tempValue, ptrType), builder->CreatePtrToInt(_structLoc, ptrType));
    builder->CreateCondBr(cmp, destroyBlock, endBlock);
    builder->SetInsertPoint(destroyBlock);

    GepArray.clear();
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 1));
    llvm::Value* objectLocationLocation = builder->CreateGEP(spLoc, GepArray);
    tempValue = builder->CreateLoad(objectLocationLocation);

    WNScriptType voidStarType;
    eWNTypeError err = eWNOK;
    if(eWNOK != _module.GetTypeManager().GetTypeByName("-Ptr", voidStarType)) {
        return(eWNPlatformError);
    }
    

    llvm::BasicBlock* notNull = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
    cmp = builder->CreateICmpNE(builder->CreatePtrToInt(tempValue, ptrType, ""), llvm::ConstantInt::get(ptrType, 0));
    builder->CreateCondBr(cmp, notNull, endBlock);
    builder->SetInsertPoint(notNull);

    const GenerateDestruction* destOp = _module.GetTypeManager().GetDestructionOperation(_node->mArrayType);
    if(destOp)
    {
        llvm::BasicBlock* loopHeader = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
        llvm::BasicBlock* loopBody   = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
        llvm::BasicBlock* loopEnd    = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
        llvm::Value* loopLoc = builder->CreateAlloca(ptrType);
        llvm::Value* arraySize = builder->CreateLoad(builder->CreatePointerCast(tempValue, ptrType->getPointerTo(0)));
        llvm::Value* arrayBase = builder->CreatePtrToInt(tempValue, ptrType);
        arrayBase = builder->CreateAdd(arrayBase, llvm::ConstantInt::get(ptrType, ptrSize * 2));
        arrayBase = builder->CreateIntToPtr(arrayBase, _node->mArrayType->mLLVMType->getPointerTo(0));
        builder->CreateStore(llvm::ConstantInt::get(ptrType, 0), loopLoc);
        builder->CreateBr(loopHeader);
        builder->SetInsertPoint(loopHeader);
        llvm::Value* validSize = builder->CreateICmpULT(builder->CreateLoad(loopLoc, false), arraySize); 
        builder->CreateCondBr(validSize, loopBody, loopEnd);
        builder->SetInsertPoint(loopBody);

        llvm::Value* currentElement = builder->CreateGEP(arrayBase, builder->CreateLoad(loopLoc) );
        
        if(eWNOK != (err = destOp->Execute(_module, _node->mArrayType, currentElement, _def, _compilationLog))) {
            return(err);
        }

        builder->CreateStore(builder->CreateAdd(builder->CreateLoad(loopLoc), llvm::ConstantInt::get(ptrType, 1)), loopLoc);
        builder->CreateBr(loopHeader);
        builder->SetInsertPoint(loopEnd);
    }

    builder->Insert(llvm::CallInst::CreateFree(tempValue, builder->GetInsertBlock()));
    builder->CreateStore(builder->CreateIntToPtr(llvm::ConstantInt::get(ptrType, 0), _node->mArrayType->mLLVMType->getPointerTo(0)), objectLocationLocation);

    builder->CreateBr(endBlock);
    builder->SetInsertPoint(endBlock);
    
    GepArray.clear();
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
    GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 2));
    tempValue = builder->CreateGEP(spLoc, GepArray);
    
    llvm::Value* val = builder->CreateAtomicRMW(llvm::AtomicRMWInst::Sub, tempValue, llvm::ConstantInt::get(ptrType, 1), llvm::Monotonic);
    llvm::BasicBlock* deleteScopeBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
    endBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _def->mFunction);
    
    cmp = builder->CreateICmpSLE(val, llvm::ConstantInt::get(ptrType, 1)); 

    builder->CreateCondBr(cmp, deleteScopeBlock, endBlock);
    builder->SetInsertPoint(deleteScopeBlock);

    builder->Insert(llvm::CallInst::CreateFree(spLoc, builder->GetInsertBlock()));
    builder->CreateBr(endBlock);

    builder->SetInsertPoint(endBlock);
    return(eWNOK);
}

WN_VOID WNTypeManagerImpl::RemoveType(WNScriptType _type) {
    mArithmeticStructs.remove_if([_type](GenerateArithmeticStruct& t){ return(t.mType1 == _type || t.mType2 == _type);});
    mCastingStructs.remove_if([_type](GenerateCastingStruct& t){ return(t.mType1 == _type || t.mType2 == _type); });
    mPreUnaryStructs.remove_if([_type](GeneratePreUnaryStruct& t){ return(t.mType == _type); });
    mPostUnaryStructs.remove_if([_type](GeneratePostUnaryStruct& t){ return(t.mType == _type); });
    mArrayAccessStructs.remove_if([_type](GenerateArrayAccessStruct& t){ return(t.mType1 == _type || t.mType2 == _type); });
    mIDAccessStructs.remove_if([_type](GenerateIDAccessStruct& t){ return(t.mType1 == _type); });
    mConstantStructs.remove_if([_type](GenerateConstantStruct& t){ return(t.mType == _type); });
    mAllocationStructs.remove_if([_type](GenerateAllocationStruct& t){ return(t.mType == _type); });
    mAssignmentStructs.remove_if([_type](GenerateAssignmentStruct& t){ return(t.mType == _type); });
    mDestructionStructs.remove_if([_type](GenerateDestructionStruct& t){ return(t.mType == _type); });
    mScriptTypes.remove_if([_type](WNScriptType& t){ return(t == _type); });
}

WN_INT32 WNScripting::GetVirtualFunctionIndex(WNTypeManager& manager, const WN_CHAR* _functionName, std::vector<WNScriptType> mTypes, WNScriptType mLookupClass, bool allowCast) {
    const WN_CHAR* lookupFunc = WNStrings::WNStrStr(_functionName, "$");
    lookupFunc = lookupFunc == NULL? _functionName: lookupFunc + 1;
    int counter = 0;
    for(std::vector<WNScripting::WNFunctionDefinition*>::iterator i = mLookupClass->mVTable.begin(); i != mLookupClass->mVTable.end(); ++i, ++counter) {
        
        //mName will be of form ClassName$functionName, so we only want to compare functionName for all, rather than also class name
        if(0 == WNStrings::WNStrCmp(lookupFunc , (*i)->mName + WNStrings::WNStrLen((*i)->mThisType->mName) + 1)) {
            if(mTypes.size() != (*i)->mTypes.size()) {
                continue;
            }
            std::vector<WNScriptType>::iterator thisTypes = mTypes.begin();
            std::vector<WNScriptType>::iterator otherTypes = (*i)->mTypes.begin();
            //the first type will be "this", since we are virtual, we know we are castable to it
            WN_DEBUG_ASSERT(thisTypes != mTypes.end());
            WN_DEBUG_ASSERT(otherTypes != (*i)->mTypes.end());
            bool enableCast = true; //always allow cast for first (this)
            while(thisTypes != mTypes.end()) {
                if( (*thisTypes) != (*otherTypes)) {
                    if(!enableCast) {
                        break;
                    }
                    const GenerateCastingOperation* castOp = manager.GetCastingOperation(*thisTypes, *otherTypes);
                    if(!castOp) {
                        castOp = manager.GetCastingOperation(*otherTypes, *thisTypes);
                    }
                    if(!castOp) {
                        break;
                    }
                    enableCast = allowCast;
                }
                ++thisTypes;
                ++otherTypes;
            }
            if(thisTypes == mTypes.end()) {
                return(counter);
            }
        }
    }
    return(-1);
}
