// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


#include "WNDeprecatedScripting/inc/WNIncludeLLVM.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Intrinsics.h"
#include "WNDeprecatedScripting/inc/WNEndIncludeLLVM.h"

#include "WNDeprecatedScripting/inc/WNNodeHelpers.h" // This is intentionally out of order.
// If this is included lower, it will fail because windows.h conflitcs wiht llvm.

#include "WNDeprecatedScripting/inc/WNTypeManagerImpl.h"
#include "WNDeprecatedScripting/inc/WNCodeModule.h"
#include "WNDeprecatedScripting/inc/WNExpression.h"
#include "WNLogging/inc/WNLog.h"

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


wn_void GenerateAssertion(WNCodeModule& _module) {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
    WNFunctionDefinition* func = _module.GetFunctionDefinition("Assert", std::vector<WNScriptType>());
    if(!func) {
        return;
    }
    builder->CreateCall(func->mFunction);
}

eWNTypeError WNTypeManagerImpl::RegisterScalarType(const wn_char* name, const WNScriptingEngine* _engine, wn_float32 priority, WNScriptType& _outType, llvm::Type* _type, wn_size_t _size) {
    if(get_type_by_name(name, _outType) == ok) {
        return(eWNAlreadyExists);
    }
    WNScripting::WNScriptTypeInternal* type = wn::memory::construct<WNScripting::WNScriptTypeInternal>(name, _engine, 0, priority, _type, mNextTag, _size);
    IncrementTag();
    mScriptTypes.push_back(type);
    _outType = type;
    return(ok);
}

eWNTypeError WNTypeManagerImpl::RegisterStructType(const wn_char* name, const WNScriptingEngine* _engine, WNScriptType& _outType) {
    if(get_type_by_name(name, _outType) == ok) {
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
    WNScripting::WNScriptTypeInternal* type = wn::memory::construct<WNScripting::WNScriptTypeInternal>(name, _engine, 0, 0.0f, ty, sType, mNextTag, sizeof(wn_size_t));
    IncrementTag();
    mScriptTypes.push_back(type);
    _outType = type;
    return(ok);
}

eWNTypeError WNTypeManagerImpl::RegisterAliasedStruct(const wn_char* name, WNScriptType copyType, WNScriptType& _outType) {
    if(get_type_by_name(name, _outType) == ok) {
        return(eWNAlreadyExists);
    }
    WNScripting::WNScriptTypeInternal* type = wn::memory::construct<WNScripting::WNScriptTypeInternal>(copyType, name, mNextTag);
    IncrementTag();
    mScriptTypes.push_back(type);
    _outType = type;
    return(ok);
}

eWNTypeError WNTypeManagerImpl::RegisterCStruct(const WNScriptingEngine* _engine, const wn_char* name, WNScriptType& _outType) {
    if(get_type_by_name(name, _outType) == ok) {
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
    WNScripting::WNScriptTypeInternal* type = wn::memory::construct<WNScripting::WNScriptTypeInternal>(name, _engine, 0, 0.0f, ty, static_cast<llvm::StructType*>(wn_nullptr), mNextTag, sizeof(wn_size_t));
    IncrementTag();
    mScriptTypes.push_back(type);
    _outType = type;
    return(ok);
}


eWNTypeError WNTypeManagerImpl::RegisterArithmeticOperator(WNArithmeticType _type, WNScriptType _operand1, WNScriptType _operand2,  GenerateArithmeticOperation* _operation) {
    mArithmeticStructs.push_back(GenerateArithmeticStruct());
    mArithmeticStructs.back().mArithmeticType = _type;
    mArithmeticStructs.back().mType1 = _operand1;
    mArithmeticStructs.back().mType2 = _operand2;
    mArithmeticStructs.back().mOperation = _operation;
    return(ok);
}

eWNTypeError WNTypeManagerImpl::RegisterCastingOperator(WNScriptType _fromType, WNScriptType _toType, GenerateCastingOperation* _operation) {
    mCastingStructs.push_back(GenerateCastingStruct());
    mCastingStructs.back().mType1 = _fromType;
    mCastingStructs.back().mType2 = _toType;
    mCastingStructs.back().mOperation = _operation;
    return(ok);
}

eWNTypeError WNTypeManagerImpl::RegisterPreUnaryOperator(WNUnaryType _type, WNScriptType _operand, GeneratePreUnaryOperation* _operation) {
    mPreUnaryStructs.push_back(GeneratePreUnaryStruct());
    mPreUnaryStructs.back().mUnaryType = _type;
    mPreUnaryStructs.back().mType = _operand;
    mPreUnaryStructs.back().mOperation = _operation;
    return(ok);
}

eWNTypeError WNTypeManagerImpl::RegisterPostUnaryOperator(WNPostUNType _type, WNScriptType _operand, GeneratePostUnaryOperation* _operation) {
    mPostUnaryStructs.push_back(GeneratePostUnaryStruct());
    mPostUnaryStructs.back().mPostUnaryType = _type;
    mPostUnaryStructs.back().mType = _operand;
    mPostUnaryStructs.back().mOperation = _operation;
    return(ok);
}

eWNTypeError WNTypeManagerImpl::RegisterArrayAccessOperator(WNScriptType _operand1, WNScriptType _operand2, GenerateArrayAccessOperation* _operation) {
    mArrayAccessStructs.push_back(GenerateArrayAccessStruct());
    mArrayAccessStructs.back().mType1 = _operand1;
    mArrayAccessStructs.back().mType2 = _operand2;
    mArrayAccessStructs.back().mOperation = _operation;
    return(ok);
}

eWNTypeError WNTypeManagerImpl::RegisterIDAccessOperator(WNScriptType _operand1, GenerateIDAccessOperation* _operation) {
    mIDAccessStructs.push_back(GenerateIDAccessStruct());
    mIDAccessStructs.back().mType1 = _operand1;
    mIDAccessStructs.back().mOperation = _operation;
    return(ok);
}

eWNTypeError WNTypeManagerImpl::RegisterConstantOperator(WNScriptType _type, GenerateConstantOperation* _operation) {
    mConstantStructs.push_back(GenerateConstantStruct());
    mConstantStructs.back().mType = _type;
    mConstantStructs.back().mOperation = _operation;
    return(ok);
}

eWNTypeError WNTypeManagerImpl::RegisterAllocationOperator(WNScriptType _type, GenerateAllocation* _operation) {
    mAllocationStructs.push_back(GenerateAllocationStruct());
    mAllocationStructs.back().mType = _type;
    mAllocationStructs.back().mOperation = _operation;
    return(ok);
}

eWNTypeError WNTypeManagerImpl::RegisterAssignmentOperator(WNScriptType _type, WNAssignType _assignType, GenerateAssignment* _operation) {
    mAssignmentStructs.push_back(GenerateAssignmentStruct());
    mAssignmentStructs.back().mType = _type;
    mAssignmentStructs.back().mOperation = _operation;
    mAssignmentStructs.back().mAssignType = _assignType;
    return(ok);
}

eWNTypeError WNTypeManagerImpl::RegisterDestructionOperator(WNScriptType _type, GenerateDestruction* _operation) {
    mDestructionStructs.push_back(GenerateDestructionStruct());
    mDestructionStructs.back().mType = _type;
    mDestructionStructs.back().mOperation = _operation;
    return(ok);
}

eWNTypeError WNTypeManagerImpl::RegisterConstructionOperator(WNScriptType _type, GenerateConstruction* _operation) {
    mConstructionStructs.push_back(GenerateConstructionStruct());
    mConstructionStructs.back().mType = _type;
    mConstructionStructs.back().mOperation = _operation;
    return(ok);
}
eWNTypeError WNTypeManagerImpl::RegisterCopyConstructionOperator(WNScriptType _type, GenerateCopyConstruction* _operation) {
    mCopyConstructionStructs.push_back(GenerateCopyConstructionStruct());
    mCopyConstructionStructs.back().mType = _type;
    mCopyConstructionStructs.back().mOperation = _operation;
    return(ok);
}

eWNTypeError WNTypeManagerImpl::get_existing_array_of(WNScriptType& _type, WNScriptType& _outType) const {
    if(!_type) {
        return(does_not_exist);
    }
    if(_type->mArrayParent) {
        _outType = _type->mArrayParent;
        return(ok);
    }
    return(does_not_exist);
}
eWNTypeError WNTypeManagerImpl::get_array_of(WNScriptType& _type, WNScriptType& _outType) {
    if(!_type) {
        return(does_not_exist);
    }
    if(_type->mArrayParent) {
        _outType = _type->mArrayParent;
        return(ok);
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
    WNScripting::WNScriptTypeInternal* scriptType = wn::memory::construct<WNScriptTypeInternal>(_type, _type->mScriptingEngine, ty, 0.0f, mNextTag, sizeof(wn_size_t));
    IncrementTag();
    _outType = scriptType;
    WNScriptType nullType;
    WNScriptType  boolType;
    WNScriptType intType;
    WNScriptType functionType;
    WNScriptType arrayType;
    if((ok != get_type_by_name("-Null", nullType)) || (ok != get_type_by_name("Bool", boolType)) || (ok != get_type_by_name("Int", intType)) || (ok != get_type_by_name("-Function", functionType)) || (ok != get_type_by_name("-Array", arrayType)))
    {
        return(eWNPlatformError);
    }

    RegisterArrayAccessOperator(scriptType, intType, wn::memory::construct<GenerateDefaultArrayAccessOperation>(scriptType));
    RegisterDestructionOperator(scriptType, wn::memory::construct<GenerateArrayDestruction>());
    RegisterAllocationOperator(scriptType, wn::memory::construct<GenerateDefaultAllocation>());
    RegisterAssignmentOperator(scriptType, AT_EQ, wn::memory::construct<GenerateStructAssignment>());
    RegisterAssignmentOperator(scriptType, AT_CHOWN, wn::memory::construct<GenerateStructTransfer>());
    RegisterCastingOperator(nullType, scriptType, wn::memory::construct<GenerateReinterpretCastOperation>(scriptType));
    RegisterCastingOperator(scriptType, arrayType, wn::memory::construct<GenerateReinterpretCastOperation>(arrayType));
    RegisterArithmeticOperator(AR_EQ, scriptType, scriptType, wn::memory::construct<GenerateStructCompare>(boolType, wn_true));
    RegisterArithmeticOperator(AR_NEQ, scriptType, scriptType, wn::memory::construct<GenerateStructCompare>(boolType, wn_false));
    RegisterIDAccessOperator(scriptType, wn::memory::construct<GenerateArrayIDOperator>(scriptType, intType, functionType));
    RegisterConstructionOperator(scriptType, wn::memory::construct<GenerateArrayConstruction>());
    RegisterCopyConstructionOperator(scriptType, wn::memory::construct<GenerateArrayCopyConstruction>());
    return(ok);
}

eWNTypeError WNTypeManagerImpl::get_type_by_name(const wn_char* name, WNScriptType& _outType) const {
    for(std::list<WNScriptType>::const_iterator i = mScriptTypes.begin(); i != mScriptTypes.end(); ++i) {
        if(wn::memory::strncmp((*i)->mName, name, 256) == 0){
            _outType = *i;
            return(ok);
        }
    }
    return(does_not_exist);
}

const GenerateArithmeticOperation* WNTypeManagerImpl::GetArithmeticOperation(WNArithmeticType _arith, WNScriptType _lhs, WNScriptType _rhs) const {
    for(std::list<GenerateArithmeticStruct>::const_iterator i = mArithmeticStructs.begin(); i != mArithmeticStructs.end(); ++i) {
        if(i->mType1 == _lhs && i->mType2 == _rhs && i->mArithmeticType == _arith) {
            return(i->mOperation);
        }
    }
    return(wn_nullptr);
}

const GenerateCastingOperation* WNTypeManagerImpl::GetCastingOperation(WNScriptType _lhs, WNScriptType _rhs) const {
    for(std::list<GenerateCastingStruct>::const_iterator i = mCastingStructs.begin(); i != mCastingStructs.end(); ++i) {
        if(i->mType1 == _lhs && i->mType2 == _rhs) {
            return(i->mOperation);
        }
    }
    return(wn_nullptr);
}

const GeneratePreUnaryOperation* WNTypeManagerImpl::GetPreUnaryOperation(WNUnaryType _type, WNScriptType _lhs) const {
    for(std::list<GeneratePreUnaryStruct>::const_iterator i = mPreUnaryStructs.begin(); i != mPreUnaryStructs.end(); ++i) {
        if(i->mType == _lhs && i->mUnaryType == _type) {
            return(i->mOperation);
        }
    }
    return(wn_nullptr);
}

const GeneratePostUnaryOperation* WNTypeManagerImpl::GetPostUnaryOperation(WNPostUNType _type, WNScriptType _operand) const {
    for(std::list<GeneratePostUnaryStruct>::const_iterator i = mPostUnaryStructs.begin(); i != mPostUnaryStructs.end(); ++i) {
        if(i->mPostUnaryType == _type && i->mType == _operand) {
            return(i->mOperation);
        }
    }
    return(wn_nullptr);
}

const GenerateArrayAccessOperation* WNTypeManagerImpl::GetArrayAccessOperation(WNScriptType _operand1, WNScriptType _operand2) const {
    for(std::list<GenerateArrayAccessStruct>::const_iterator i = mArrayAccessStructs.begin(); i != mArrayAccessStructs.end(); ++i) {
        if(i->mType1 == _operand1 && i->mType2 == _operand2) {
            return(i->mOperation);
        }
    }
    return(wn_nullptr);
}

const GenerateIDAccessOperation* WNTypeManagerImpl::GetIDAccessOperation(WNScriptType _type) const {
    for(std::list<GenerateIDAccessStruct>::const_iterator i = mIDAccessStructs.begin(); i != mIDAccessStructs.end(); ++i) {
        if(i->mType1 == _type) {
            return(i->mOperation);
        }
    }
    return(wn_nullptr);
}

const GenerateConstantOperation* WNTypeManagerImpl::GetConstantOperation(WNScriptType _type) const {
    for(std::list<GenerateConstantStruct>::const_iterator i = mConstantStructs.begin(); i != mConstantStructs.end(); ++i) {
        if(i->mType == _type) {
            return(i->mOperation);
        }
    }
    return(wn_nullptr);
}

const GenerateAllocation* WNTypeManagerImpl::GetAllocationOperation(WNScriptType _type) const {
    for(std::list<GenerateAllocationStruct>::const_iterator i = mAllocationStructs.begin(); i != mAllocationStructs.end(); ++i) {
        if(i->mType == _type) {
            return(i->mOperation);
        }
    }
    return(wn_nullptr);
}

const GenerateAssignment* WNTypeManagerImpl::GetAssignmentOperation(WNScriptType _type, WNAssignType _assignType) const {
    for(std::list<GenerateAssignmentStruct>::const_iterator i = mAssignmentStructs.begin(); i != mAssignmentStructs.end(); ++i) {
        if(i->mType == _type && i->mAssignType == _assignType) {
            return(i->mOperation);
        }
    }
    return(wn_nullptr);
}

const GenerateDestruction* WNTypeManagerImpl::GetDestructionOperation(WNScriptType _type) const {
    for(std::list<GenerateDestructionStruct>::const_iterator i = mDestructionStructs.begin(); i != mDestructionStructs.end(); ++i) {
        if(i->mType == _type) {
            return(i->mOperation);
        }
    }
    return(wn_nullptr);
}

const GenerateConstruction* WNTypeManagerImpl::GetConstructionOperation(WNScriptType _type) const {
    for(std::list<GenerateConstructionStruct>::const_iterator i = mConstructionStructs.begin(); i != mConstructionStructs.end(); ++i) {
        if(i->mType == _type) {
            return(i->mOperation);
        }
    }
    return(wn_nullptr);
}

const GenerateCopyConstruction* WNTypeManagerImpl::GetCopyConstructionOperation(WNScriptType _type) const {
    for(std::list<GenerateCopyConstructionStruct>::const_iterator i = mCopyConstructionStructs.begin(); i != mCopyConstructionStructs.end(); ++i) {
        if(i->mType == _type) {
            return(i->mOperation);
        }
    }
    return(wn_nullptr);
}

GenerateDefaultStructAccessor::GenerateDefaultStructAccessor(WNScriptType _structType, WNScriptType _functionType):
    mStructType(_structType) ,
    mFunctionType(_functionType) {
}


eWNTypeError GenerateDefaultStructAccessor::Execute(llvm::IRBuilderBase* _builder, llvm::Value*, llvm::Value* _structLoc, wn_char* _id, WNScriptType& _outType, llvm::Value*& _outValue, llvm::Value*& _outLocation) const
{
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_builder);
    size_t valueNum = 0;

    for(std::vector<WNContainedStructType>::iterator i = mStructType->mStructTypes.begin(); i != mStructType->mStructTypes.end(); ++i, valueNum++) {
        if(wn::memory::strncmp(i->mName, _id, 256) == 0){
            _outType = i->mType;
            break;
        }
    }
    if(valueNum >= mStructType->mStructTypes.size()) {
        //cannot find a struct with the correct name, what about a function?
        _outValue = reinterpret_cast<llvm::Value*>(wn::memory::strndup(_id, 2048));
        _outType = mFunctionType;
        return(ok);
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
    return(ok);
}

GenerateCPPStructAccessor::GenerateCPPStructAccessor(WNScriptType _structType, WNScriptType _functionType):
    mStructType(_structType) ,
    mFunctionType(_functionType) {
}

eWNTypeError GenerateCPPStructAccessor::Execute(llvm::IRBuilderBase* _builder, llvm::Value*, llvm::Value* _structLoc, wn_char* _id, WNScriptType& _outType, llvm::Value*& _outValue, llvm::Value*& _outLocation) const
{

#ifdef _WN_64_BIT
    llvm::Type* ptrType = llvm::IntegerType::getInt64Ty(llvm::getGlobalContext());
#else
    llvm::Type* ptrType = llvm::IntegerType::getInt32Ty(llvm::getGlobalContext());
#endif

    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_builder);
    wn_int32 valueNum = -1;

    WNScriptType tempType = mStructType;
    while(tempType) {
        for(std::vector<WNContainedStructType>::iterator i = tempType->mStructTypes.begin(); i != tempType->mStructTypes.end(); ++i) {
            if(wn::memory::strncmp(i->mName, _id, 256) == 0){
                _outType = i->mType;
                valueNum = i->mCppOffset;
                break;
            }
        }
        tempType = tempType->mParentClass;
    }

    if(valueNum < 0) {
        //cannot find a struct with the correct name, what about a function?
        _outValue = reinterpret_cast<llvm::Value*>(wn::memory::strndup(_id, 2048));
        _outType = mFunctionType;
        return(ok);
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

    return(ok);
}

GenerateArrayIDOperator::GenerateArrayIDOperator(WNScriptType _structType, WNScriptType _intType, WNScriptType _functionType):
    mStructType(_structType),
    mIntType(_intType),
    mFunctionType(_functionType){
}

eWNTypeError GenerateArrayIDOperator::Execute(llvm::IRBuilderBase* _builder, llvm::Value*, llvm::Value* _structLoc, wn_char* _id, WNScriptType& _outType, llvm::Value*& _outValue, llvm::Value*&) const
{
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_builder);
    if(wn::memory::strcmp(_id, "resize") == 0) {
        _outType = mFunctionType;
        _outValue = reinterpret_cast<llvm::Value*>(const_cast<char*>("$resizeArray"));
        return(ok);
    }
    if(wn::memory::strcmp(_id, "length") != 0) {
        return(does_not_exist);
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
    return(ok);
}


eWNTypeError GenerateDefaultAllocation::Execute(WNCodeModule& _module, WNScriptType _type, const wn_char* _name, wn_bool, llvm::Value*& _outLocation) const {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
    if(!_type->mLLVMType) {
        return(eWNBadType);
    }
    _outLocation = builder->CreateAlloca(_type->mLLVMType, 0, _name);
    return(ok);
}

eWNTypeError GenerateStructConstruction::Execute(WNCodeModule& _module, llvm::Value*& _retVal, WNScriptType structType, WNScriptLinkedList<WNExpression>::WNScriptLinkedListNode*, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) const {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
        llvm::Value* allocLocation = wn_nullptr;
        _retVal = wn_nullptr;
        eWNTypeError err = ok;
        const GenerateAllocation* alloc = _module.GetTypeManager().GetAllocationOperation(structType);
        if(!alloc) {
            return(eWNCannotCreateType);
        }
        if(ok != (err = alloc->Execute(_module, structType, "", false, allocLocation))) {
            return(err);
        }

    #ifdef _WN_64_BIT
        llvm::Type* ptrType = llvm::IntegerType::getInt64Ty(llvm::getGlobalContext());
    #else
        llvm::Type* ptrType = llvm::IntegerType::getInt32Ty(llvm::getGlobalContext());
    #endif
        WNScriptType sizeTType = 0;
        if(ok != (err = _module.GetTypeManager().get_type_by_name("-SizeT", sizeTType))) {
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

        wn_size_t nameLen = wn::memory::strlen(structType->mName);
        wn_char *functionName = static_cast<wn_char*>(WN_STACK_ALLOC(sizeof(wn_char)* (nameLen + 1 + 5)));
        wn::memory::memcpy(functionName, structType->mName, nameLen);
        wn::memory::memcpy(functionName + nameLen, "Const", 5);
        functionName[nameLen + 5] = '\0';
        std::vector<FunctionParam> fParams;
        fParams.push_back(FunctionParam());
        fParams.back().mType = structType;
        fParams.back().mValue = v;

        WNScriptType returnType;
        llvm::Value* returnValue;
        if(ok != (err = GenerateFunctionCall(_module, _def, functionName, fParams, false, returnType, returnValue, _compilationLog))) {
            return(err);
        }
        builder->CreateStore(v, allocLocation);
        _retVal = v;
        return(ok);
}

eWNTypeError GenerateArrayConstruction::Execute(WNCodeModule& _module, llvm::Value*& _retVal, WNScriptType _type, WNScriptLinkedList<WNExpression>::WNScriptLinkedListNode* _initializer, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) const {
     llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());

    _retVal = wn_nullptr;
    eWNTypeError err = ok;
    if(!_initializer) {
        WNScriptType nullType;
        if(ok != (err = GenerateNullAllocation(_module, _def, _retVal, nullType))) {
            return(err);
        }
    }
    if(!_type) {
        //Last level
        return(error);
    }

    llvm::Value* allocLocation = wn_nullptr;
    const GenerateAllocation* alloc = _module.GetTypeManager().GetAllocationOperation(_type);
    if(!alloc) {
        return(eWNCannotCreateType);
    }
    if(ok != (err = alloc->Execute(_module, _type, "", false, allocLocation))) {
        return(err);
    }

#ifdef _WN_64_BIT
    llvm::Type* ptrType = llvm::IntegerType::getInt64Ty(llvm::getGlobalContext());
    const wn_size_t ptrSize = 8;
#else
    llvm::Type* ptrType = llvm::IntegerType::getInt32Ty(llvm::getGlobalContext());
    const wn_size_t ptrSize = 4;
#endif
    WNScriptType sizeTType = 0;
    WNScriptType intType = 0;
    if(ok != (err = _module.GetTypeManager().get_type_by_name("-SizeT", sizeTType)) ||
        ok != (err = _module.GetTypeManager().get_type_by_name("Int", intType))) {
        return(err);
    }

    if(ok != (err = _initializer->value->GenerateCode(_module, _def, _compilationLog))) {
        return(err);
    }

    llvm::Value* sizeVal = _initializer->value->GetValue();
    if(_initializer->value->GetType() != intType) {
        const GenerateCastingOperation* cast = _module.GetTypeManager().GetCastingOperation(_initializer->value->GetType(), intType);
        if(!cast) {
            return(eWNInvalidCast);
        }
        if(ok != (err = cast->Execute(builder, sizeVal, sizeVal))) {
            return(err);
        }
    }

    llvm::Type* allocType = _type->mLLVMType->getContainedType(0);
    llvm::Value* v = builder->Insert(llvm::CallInst::CreateMalloc(builder->GetInsertBlock(), ptrType, allocType, llvm::ConstantInt::get(sizeTType->mLLVMType, _module.GetExecutionEngine()->getDataLayout()->getTypeAllocSize(allocType))));
    v = builder->CreatePointerCast(v, allocLocation->getType()->getContainedType(0));

    wn_uint64 allocSize = _module.GetExecutionEngine()->getDataLayout()->getTypeAllocSize(_type->mArrayType->mLLVMType);
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
    builder->CreateStore(llvm::ConstantInt::get(ptrType, reinterpret_cast<wn_int64>(_type)), typeLocation);
    if(!(_type->mArrayType->mLLVMStructType || _type->mArrayType->mArrayType )) {
        _retVal = v;
        return(ok);
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
        if(ok != (err = GenerateNullAllocation(_module, _def, structCreation, nullType))) {
            return(err);
        }
        const GenerateCastingOperation* cast = _module.GetTypeManager().GetCastingOperation(nullType, _type->mArrayType);
        if(cast == wn_nullptr) {
            return(eWNPlatformError);
        }
        if(ok != (err = cast->Execute(_module.GetBuilder(), structCreation, structCreation))) {
            return(err);
        }
        const GenerateAssignment* assOp =_module.GetTypeManager().GetAssignmentOperation(_type->mArrayType, AT_CHOWN);
        if(ok != (err = assOp->Execute(_module, true, _type->mArrayType, structCreation, gepLocation, _def, _compilationLog))) {
            return(err);
        }
    } else if(_type->mArrayType->mArrayType) {
        const GenerateConstruction* construction = _module.GetTypeManager().GetConstructionOperation(_type->mArrayType);
        if(!construction) {
            _compilationLog.Log(WNLogging::eError, 0, "Could not find construction operation for ", _type->mArrayType->mName);
            return(eWNCannotCreateType);
        }
        llvm::Value* arrayCreation;

        if(ok != (err = construction->Execute(_module, arrayCreation, _type->mArrayType, _initializer->next, _def, _compilationLog))) {
            return(err);
        }
        const GenerateAssignment* assOp =_module.GetTypeManager().GetAssignmentOperation(_type->mArrayType, AT_CHOWN);
        if(ok != (err = assOp->Execute(_module, true, _type->mArrayType, arrayCreation, gepLocation, _def, _compilationLog))) {
            return(err);
        }
    }
    builder->CreateStore(builder->CreateAdd(builder->CreateLoad(counterLoc), llvm::ConstantInt::get(ptrType, 1)), counterLoc);
    builder->CreateBr(counterBlock);
    builder->SetInsertPoint(endBlock);
    _retVal = v;
    return(ok);
}

eWNTypeError GenerateArrayCopyConstruction::Execute(WNCodeModule& _module, llvm::Value*& _retVal, WNScriptType _type, llvm::Value* _copyValue, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) const {
    WN_UNUSED_ARGUMENT(_compilationLog);
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());

    _retVal = wn_nullptr;
    eWNTypeError err = ok;
    llvm::Value* allocLocation = wn_nullptr;
    const GenerateAllocation* alloc = _module.GetTypeManager().GetAllocationOperation(_type);
    if(!alloc) {
        return(eWNCannotCreateType);
    }
    if(ok != (err = alloc->Execute(_module, _type, "", false, allocLocation))) {
        return(err);
    }

#ifdef _WN_64_BIT
    llvm::Type* ptrType = llvm::IntegerType::getInt64Ty(llvm::getGlobalContext());
    const wn_size_t ptrSize = 8;
#else
    llvm::Type* ptrType = llvm::IntegerType::getInt32Ty(llvm::getGlobalContext());
    const wn_size_t ptrSize = 4;
#endif
    WNScriptType sizeTType = 0;
    WNScriptType intType = 0;
    if(ok != (err = _module.GetTypeManager().get_type_by_name("-SizeT", sizeTType)) ||
        ok != (err = _module.GetTypeManager().get_type_by_name("Int", intType))) {
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
            wn_uint64 allocSize = _module.GetExecutionEngine()->getDataLayout()->getTypeAllocSize(_type->mArrayType->mLLVMType);
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
            builder->CreateStore(llvm::ConstantInt::get(ptrType, reinterpret_cast<wn_int64>(_type)), typeLocation);

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
    return(ok);
}

eWNTypeError GenerateStructCopyConstruction::Execute(WNCodeModule& _module, llvm::Value*& _retVal, WNScriptType structType, llvm::Value* _copyValue, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) const {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
        llvm::Value* allocLocation = wn_nullptr;
        _retVal = wn_nullptr;
        eWNTypeError err = ok;
        const GenerateAllocation* alloc = _module.GetTypeManager().GetAllocationOperation(structType);
        if(!alloc) {
            return(eWNCannotCreateType);
        }
        if(ok != (err = alloc->Execute(_module, structType, "", false, allocLocation))) {
            return(err);
        }

    #ifdef _WN_64_BIT
        llvm::Type* ptrType = llvm::IntegerType::getInt64Ty(llvm::getGlobalContext());
    #else
        llvm::Type* ptrType = llvm::IntegerType::getInt32Ty(llvm::getGlobalContext());
    #endif
        WNScriptType sizeTType = 0;
        if(ok != (err = _module.GetTypeManager().get_type_by_name("-SizeT", sizeTType))) {
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

        wn_size_t nameLen = wn::memory::strlen(structType->mName);
        wn_char *functionName = static_cast<wn_char*>(WN_STACK_ALLOC(sizeof(wn_char)* (nameLen + 1 + 9)));
        wn::memory::memcpy(functionName, structType->mName, nameLen);
        wn::memory::memcpy(functionName + nameLen, "CopyConst", 10);
        std::vector<FunctionParam> fParams;
        fParams.push_back(FunctionParam());
        fParams.back().mType = structType;
        fParams.back().mValue = v;
        fParams.push_back(FunctionParam());
        fParams.back().mType = structType;
        fParams.back().mValue = _copyValue;

        WNScriptType returnType;
        llvm::Value* returnValue;
        if(ok != (err = GenerateFunctionCall(_module, _def, functionName, fParams, false, returnType, returnValue, _compilationLog))) {
            return(err);
        }

        builder->CreateStore(v, allocLocation);
        _retVal = v;
        return(ok);
}

eWNTypeError GenerateDefaultAssignment::Execute(WNCodeModule& _module, bool, WNScriptType, llvm::Value* _value, llvm::Value* _assignLocation, const WNFunctionDefinition*, WNLogging::WNLog&) const {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
    builder->CreateStore(_value, _assignLocation);
    return(ok);
}

eWNTypeError GenerateStructAssignment::Execute(WNCodeModule& _module, bool _declaration, WNScriptType _type, llvm::Value* _value, llvm::Value* _assignLocation, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) const {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
    std::vector<llvm::Value*> GepArray;

    if(!_declaration) {
        //First check what was currently in there, if we were the owner then destroy it
        eWNTypeError err = ok;
        const GenerateDestruction * op = _module.GetTypeManager().GetDestructionOperation(_type);
        if(op) {
            if(ok != (err = op->Execute(_module, _type, _assignLocation, _def, _compilationLog))) {
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
    return(ok);
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
        eWNTypeError err = ok;
        const GenerateDestruction * op = _module.GetTypeManager().GetDestructionOperation(_type);
        if(op) {
            if(ok != (err = op->Execute(_module, _type, assignLocationTemp, _def, _compilationLog))) {
                return(err);
            }
        }
    }
    return(ok);
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

    if(ok != _module.GetTypeManager().get_type_by_name("-Ptr", voidStarType)) {
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
    if(ok != _module.GetTypeManager().get_type_by_name("Void", voidReturnType)) {
        _compilationLog.Log(WNLogging::eCritical, 0, "Error cannot find void type");
        return(error);
    }

    llvm::FunctionType* t = llvm::FunctionType::get(voidReturnType->mLLVMType, _node->mLLVMType, wn_false);

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
    return(ok);
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

    if(ok != _module.GetTypeManager().get_type_by_name("-Ptr", voidStarType)) {
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
    if(ok != _module.GetTypeManager().get_type_by_name("Void", voidReturnType)) {
        _compilationLog.Log(WNLogging::eCritical, 0, "Error cannot find void type");
        return(error);
    }

    llvm::FunctionType* t = llvm::FunctionType::get(voidReturnType->mLLVMType, _node->mLLVMType, wn_false);

    llvm::Value* v = builder->CreateIntToPtr(llvm::ConstantInt::get(ptrType, reinterpret_cast<wn_size_t>(_node->mCDestructor)), t->getPointerTo(0));
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
    return(ok);
}


eWNTypeError GenerateReinterpretCastOperation::Execute(llvm::IRBuilderBase* _builder, llvm::Value* _expr1, llvm::Value*& _outReturnValue) const {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_builder);
    _outReturnValue = builder->CreatePointerCast(_expr1, mScriptType->mLLVMType);
    return(ok);
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
    return(ok);
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
    return(ok);
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
    wn_size_t ptrSize = 8;
#else
    llvm::Type* ptrType = llvm::IntegerType::getInt32Ty(llvm::getGlobalContext());
    wn_size_t ptrSize = 4;
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
    eWNTypeError err = ok;
    if(ok != _module.GetTypeManager().get_type_by_name("-Ptr", voidStarType)) {
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

        if(ok != (err = destOp->Execute(_module, _node->mArrayType, currentElement, _def, _compilationLog))) {
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
    return(ok);
}

wn_void WNTypeManagerImpl::RemoveType(WNScriptType _type) {
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

wn_int32 WNScripting::get_virtual_function_index(WNTypeManager& manager, const wn_char* _functionName, std::vector<WNScriptType> mTypes, WNScriptType mLookupClass, bool allowCast) {
    const wn_char* lookupFunc = wn::memory::strstr(_functionName, "$");
    lookupFunc = lookupFunc == NULL? _functionName: lookupFunc + 1;
    int counter = 0;
    for(std::vector<WNScripting::WNFunctionDefinition*>::iterator i = mLookupClass->mVTable.begin(); i != mLookupClass->mVTable.end(); ++i, ++counter) {

        //mName will be of form ClassName$functionName, so we only want to compare functionName for all, rather than also class name
        if(0 == wn::memory::strcmp(lookupFunc , (*i)->mName + wn::memory::strlen((*i)->mThisType->mName) + 1)) {
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
