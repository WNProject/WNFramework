// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNScriptingErrors.h"
#include "WNScripting/inc/WNBuiltinTypeInitialization.h"
#include "WNScripting/inc/WNTypeManager.h"
#include "WNScripting/inc/WNExpression.h"
#include "WNScripting/inc/WNCodeModule.h"
#include "WNMemory/inc/WNAllocation.h"

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
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Function.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"

#ifdef _WN_MSVC
    #pragma warning(pop)
#endif

#include <stdlib.h>

namespace WNScripting {

    template<llvm::Value* (llvm::IRBuilder<>::*T)(llvm::Value*, llvm::Value*, const llvm::Twine&, bool, bool)>
    struct GenerateIntArithmetic : public GenerateArithmeticOperation {
        GenerateIntArithmetic(WNScriptType _destType) :
            mDestFlt(_destType) {
        }
        virtual ~GenerateIntArithmetic() {}
        virtual eWNTypeError Execute(llvm::IRBuilderBase* _builder, llvm::Value* _expr1, llvm::Value* _expr2, WNScriptType& _destType, llvm::Value*& _outReturnVal) const {
            llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>* >(_builder);
            _outReturnVal = ((*builder).*T)(_expr1, _expr2, "", false, false);
            _destType = mDestFlt;
            return(ok);
        }
    private:
        WNScriptType mDestFlt;
    };

    template<llvm::Value* (llvm::IRBuilder<>::*T)(llvm::Value*, llvm::Value*, const llvm::Twine&, bool, bool)>
    struct GenerateCompoundIntAssignment : public GenerateAssignment {
        virtual eWNTypeError Execute(WNCodeModule& _module, bool _declaration, WNScriptType, llvm::Value* _value, llvm::Value* _assignLocation, const WNFunctionDefinition*, WNLogging::WNLog& _compilationLog) const {
            if(_declaration) {
                _compilationLog.Log(WNLogging::eError, 0, "Cannot declare a variable with a compound assignment");
                return(eWNCannotCreateType);
            }
            llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
            llvm::Value* val = builder->CreateLoad(_assignLocation, false, "");
            llvm::Value* computedVal = ((*builder).*T)(val, _value, "", false, false);
            builder->CreateStore(computedVal, _assignLocation);
            return(ok);
        }
    };


    template<llvm::Value* (llvm::IRBuilder<>::*T)(llvm::Value*, llvm::Value*, const llvm::Twine&)>
    struct GenerateIntCompare : public GenerateArithmeticOperation {
        GenerateIntCompare(WNScriptType _destType) :
            mDestFlt(_destType) {
        }
        virtual ~GenerateIntCompare() {}
        virtual eWNTypeError Execute(llvm::IRBuilderBase* _builder, llvm::Value* _expr1, llvm::Value* _expr2, WNScriptType& _destType, llvm::Value*& _outReturnVal) const {
            llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>* >(_builder);
            _outReturnVal = ((*builder).*T)(_expr1, _expr2, "");
            _outReturnVal = builder->CreateIntCast(_outReturnVal, mDestFlt->mLLVMType, false, "");
            _destType = mDestFlt;
            return(ok);
        }
    private:
        WNScriptType mDestFlt;
    };

    template<llvm::Value* (llvm::IRBuilder<>::*T)(llvm::Value*, llvm::Value*, const llvm::Twine&, bool)>
    struct GenerateIntDivArithmetic : public GenerateArithmeticOperation {
        GenerateIntDivArithmetic(WNScriptType _destType) :
            mDestFlt(_destType) {
        }
        virtual ~GenerateIntDivArithmetic() {}
        virtual eWNTypeError Execute(llvm::IRBuilderBase* _builder, llvm::Value* _expr1, llvm::Value* _expr2, WNScriptType& _destType, llvm::Value*& _outReturnVal) const {
            llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>* >(_builder);
            _outReturnVal = ((*builder).*T)(_expr1, _expr2, "", false);
            _destType = mDestFlt;
            return(ok);
        }
    private:
        WNScriptType mDestFlt;
    };

    template<llvm::Value* (llvm::IRBuilder<>::*T)(llvm::Value*, llvm::Value*, const llvm::Twine&, bool)>
    struct GenerateCompoundIntDivAssignment : public GenerateAssignment {
        virtual eWNTypeError Execute(WNCodeModule& _module, bool _declaration, WNScriptType, llvm::Value* _value, llvm::Value* _assignLocation, const WNFunctionDefinition*, WNLogging::WNLog& _compilationLog) const {
            if(_declaration) {
                _compilationLog.Log(WNLogging::eError, 0, "Cannot declare a variable with a compound assignment");
                return(eWNCannotCreateType);
            }
            llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
            llvm::Value* val = builder->CreateLoad(_assignLocation, false, "");
            llvm::Value* computedVal = ((*builder).*T)(val, _value, "", false);
            builder->CreateStore(computedVal, _assignLocation);
            return(ok);
        }
    };



    template<llvm::Value* (llvm::IRBuilder<>::*T)(llvm::Value*, llvm::Value*, const llvm::Twine&)>
    struct GenerateIntModArithmetic : public GenerateArithmeticOperation {
        GenerateIntModArithmetic (WNScriptType _destType) :
            mDestFlt(_destType) {
        }
        virtual ~GenerateIntModArithmetic() {}
        virtual eWNTypeError Execute(llvm::IRBuilderBase* _builder, llvm::Value* _expr1, llvm::Value* _expr2, WNScriptType& _destType, llvm::Value*& _outReturnVal) const {
            llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>* >(_builder);
            _outReturnVal = ((*builder).*T)(_expr1, _expr2, "");
            _destType = mDestFlt;
            return(ok);
        }
    private:
        WNScriptType mDestFlt;
    };

    template<llvm::Value* (llvm::IRBuilder<>::*T)(llvm::Value*, llvm::Value*, const llvm::Twine&)>
    struct GenerateCompoundIntModAssignment : public GenerateAssignment {
        virtual eWNTypeError Execute(WNCodeModule& _module, bool _declaration, WNScriptType, llvm::Value* _value, llvm::Value* _assignLocation, const WNFunctionDefinition*, WNLogging::WNLog& _compilationLog) const {
            if(_declaration) {
                _compilationLog.Log(WNLogging::eError, 0, "Cannot declare a variable with a compound assignment");
                return(eWNCannotCreateType);
            }
            llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
            llvm::Value* val = builder->CreateLoad(_assignLocation, false, "");
            llvm::Value* computedVal = ((*builder).*T)(val, _value, "");
            builder->CreateStore(computedVal, _assignLocation);
            return(ok);
        }
    };

    template<llvm::Value* (llvm::IRBuilder<>::*T)(llvm::Value*, llvm::Value*, const llvm::Twine&, bool, bool)>
    eWNTypeError GeneratePreValue(llvm::IRBuilderBase* _builder, const WNExpression& _expr1, llvm::Value*& _value) {
        llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>* >(_builder);
        llvm::Type* type = _expr1.GetType()->mLLVMType;
        llvm::Value* val = llvm::ConstantInt::get(type, 1);
        _value = ((*builder).*T)(_expr1.GetValue(), val, "", false, false);
        if(wn_nullptr != _expr1.GetValueLocation()) {
            builder->CreateStore(_value, _expr1.GetValueLocation(), false);
        }
        return(ok);
    }

    template<llvm::Value* (llvm::IRBuilder<>::*T)(llvm::Value*, llvm::Value*, const llvm::Twine&, bool, bool)>
    eWNTypeError GeneratePostValue(llvm::IRBuilderBase* _builder, const WNExpression& _expr1, llvm::Value*& _value) {
        llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>* >(_builder);
        if(wn_nullptr == _expr1.GetValueLocation()) {
            return(invalid_parameters);
        }
        _value = _expr1.GetValue();
        llvm::Type* type = _expr1.GetType()->mLLVMType;
        llvm::Value* val = llvm::ConstantInt::get(type, 1);
        llvm::Value* postVal = ((*builder).*T)(_expr1.GetValue(), val, "", false, false);
        builder->CreateStore(postVal, _expr1.GetValueLocation(), false);
        return(ok);
    }

    eWNTypeError GenerateBoolNegation(llvm::IRBuilderBase* _builder, const WNExpression& _expr1, llvm::Value*& _value) {
        llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>* >(_builder);
        llvm::Type* type = _expr1.GetType()->mLLVMType;
        llvm::Value* val = llvm::ConstantInt::get(type, 0x1);
        _value = builder->CreateAnd(_expr1.GetValue(), val, "");
        _value = builder->CreateXor(_value, val, "");
        return(ok);
    }

    eWNTypeError GenerateIntNegation(llvm::IRBuilderBase* _builder, const WNExpression&  _expr1, llvm::Value*& _value) {
        llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>* >(_builder);
        llvm::Type* type = _expr1.GetType()->mLLVMType;
        llvm::Value* val = llvm::ConstantInt::get(type, static_cast<wn_uint64>(-1));
        _value = builder->CreateMul(_expr1.GetValue(), val, "", false, false);
        return(ok);
    }

    struct GenerateIntConstant: public GenerateConstantOperation {
        GenerateIntConstant(WNScriptType _destType) :
            mDestInt(_destType) {
        }
        virtual ~GenerateIntConstant() {}
        virtual eWNTypeError Execute(WNCodeModule&, const wn_char* _constant, bool&, llvm::Value*& _outLocation) const {
            wn_bool consumedString = false;
            wn_int64 constVal = WNStrings::WNStrToLL(_constant, consumedString);
            if(!consumedString) {
                return(eWNInvalidConstant);
            }
            uint64_t tVal = static_cast<uint64_t>(constVal);
            _outLocation = llvm::ConstantInt::get(mDestInt->mLLVMType, tVal);
            return(ok);
        }
    private:
        WNScriptType mDestInt;
    };

    struct GenerateStringConstant: public GenerateConstantOperation {
        GenerateStringConstant(WNScriptType _destType) :
            mDestInt(_destType) {
        }
        virtual eWNTypeError Execute(WNCodeModule& _module, const wn_char* _constant, bool& mForceUse, llvm::Value*& _outLocation) const {
            //string constant time
            wn_int32 val = 0;
            std::vector<llvm::Constant*> mConstantValues;
            const wn_char* ptr = _constant;
            ++ptr; //consume opening "
            while(*ptr != '\0') {
                if(*ptr == '\\') {
                    ptr++;
                    switch(*ptr) {
                    case 'a':
                        val = '\a';
                        break;
                    case 'b':
                         val = '\b';
                         break;
                    case 'f':
                        val = '\f';
                        break;
                    case 'n':
                        val = '\n';
                        break;
                    case 'r':
                        val = '\r';
                        break;
                    case 't':
                        val = '\t';
                        break;
                    case '\\':
                        val = '\\';
                        break;
                    case '\'':
                        val = '\'';
                        break;
                    case '\"':
                        val = '\"';
                        break;
                    case '0':
                        val = '\0';
                        break;
                    default:
                        return(eWNInvalidConstant);
                    }
                } else {
                    val = *ptr;
                }
                mConstantValues.push_back(llvm::ConstantInt::get(mDestInt->mArrayType->mLLVMType, val));
                ++ptr;
            }
            mConstantValues.pop_back(); //get rid of ending "

            //////////create actual constant values to insert
            wn_size_t len = mConstantValues.size();
            wn_size_t zLen = 0;
            llvm::Constant* preConstants[2 * sizeof(wn_size_t)];
            wn_char* ch = reinterpret_cast<wn_char*>(&len);
            for(int i = 0; i < sizeof(wn_size_t); ++i) {
                preConstants[i] = llvm::ConstantInt::get(mDestInt->mArrayType->mLLVMType, ch[i]);
            }
            ch = reinterpret_cast<wn_char*>(&zLen);
            for(int i = 0; i < sizeof(wn_size_t); ++i) {
                preConstants[i+sizeof(wn_size_t)] = llvm::ConstantInt::get(mDestInt->mArrayType->mLLVMType, ch[i]);
            }
            mConstantValues.insert(mConstantValues.begin(), &preConstants[0], &preConstants[2*sizeof(wn_size_t)]);
            llvm::ArrayType* arrType = llvm::ArrayType::get(mDestInt->mArrayType->mLLVMType, mConstantValues.size());
            llvm::Constant* cArray = llvm::ConstantArray::get(arrType, mConstantValues);
            llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>* >(_module.GetBuilder());
            llvm::GlobalVariable* globalString = new llvm::GlobalVariable(*_module.GetModule(), arrType, true, llvm::GlobalValue::InternalLinkage, 0);
            llvm::GlobalVariable* globalPtr = new llvm::GlobalVariable(*_module.GetModule(), mDestInt->mArrayType->mLLVMType->getPointerTo(0), true, llvm::GlobalValue::InternalLinkage, 0);
            std::vector<llvm::Constant*> gepConstants;
            gepConstants.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
            gepConstants.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
            llvm::Constant* cst = llvm::ConstantExpr::getGetElementPtr(globalString, gepConstants);
            globalString->setInitializer(cArray);
            globalPtr->setInitializer(cst);
            //////

            eWNTypeError err = ok;
            llvm::Value* allocLocation = wn_nullptr;
            const GenerateAllocation* alloc = _module.GetTypeManager().GetAllocationOperation(mDestInt);
            if(!alloc) {
                return(eWNCannotCreateType);
            }
            if(ok != (err = alloc->Execute(_module, mDestInt, "", false, allocLocation))) {
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
            if(ok != (err = _module.GetTypeManager().GetTypeByName("-SizeT", sizeTType)) ||
                ok != (err = _module.GetTypeManager().GetTypeByName("Int", intType))) {
                return(err);
            }
            std::vector<llvm::Value*> GepArray;

            llvm::Type* allocType = mDestInt->mLLVMType->getContainedType(0);
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
            llvm::Value* oldSizeLocation = globalPtr;

            {
                        llvm::Value* arraySize = llvm::ConstantInt::get(ptrType, len);
                        wn_uint64 allocSize = _module.GetExecutionEngine()->getDataLayout()->getTypeAllocSize(mDestInt->mArrayType->mLLVMType);
                        llvm::Value* allocationSize = builder->CreateMul(arraySize, llvm::ConstantInt::get(ptrType, allocSize));
                        allocationSize = builder->CreateAdd(allocationSize, llvm::ConstantInt::get(ptrType, (ptrSize * 2)));

                        allocType = mDestInt->mArrayType->mLLVMType;
                        llvm::Value* actualArray = builder->Insert(llvm::CallInst::CreateMalloc(builder->GetInsertBlock(), ptrType, allocType, builder->CreateIntCast(allocationSize, sizeTType->mLLVMType, false)));
                        actualArray = builder->CreatePointerCast(actualArray, mDestInt->mArrayType->mLLVMType->getPointerTo(0));

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
                                                        builder->CreatePointerCast(builder->CreateLoad(oldSizeLocation), llvm::IntegerType::getInt8PtrTy(llvm::getGlobalContext())),
                                                        allocationSize,
                                                        llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0),
                                                        llvm::ConstantInt::get(llvm::IntegerType::getInt1Ty(llvm::getGlobalContext()), 0)
                                        };
                        builder->CreateCall(fnc, llvm::makeArrayRef(pointers));

                        llvm::Value* typeLocation = builder->CreatePtrToInt(actualArray, ptrType);
                        typeLocation = builder->CreateAdd(typeLocation, llvm::ConstantInt::get(ptrType, ptrSize));
                        typeLocation = builder->CreateIntToPtr(typeLocation, ptrType->getPointerTo(0));
                        builder->CreateStore(llvm::ConstantInt::get(ptrType, reinterpret_cast<wn_int64>(mDestInt)), typeLocation);
            }
        ////////////////////////////////////
        _outLocation = v;
        mForceUse = true;
        return(ok);
        }
        WNScriptType mDestInt;
    };

    struct GenerateCharConstant: public GenerateConstantOperation {
        GenerateCharConstant(WNScriptType _destType) :
            mDestInt(_destType) {
        }
        virtual ~GenerateCharConstant() {}
        virtual eWNTypeError Execute(WNCodeModule&, const wn_char* _constant, bool&, llvm::Value*& _outLocation) const {
            wn_int32 val = 0;
            const wn_char* ptr = _constant;
            if(*ptr == '\\') {
                ptr++;
                switch(*ptr) {
                case 'a':
                    val = '\a';
                    break;
                case 'b':
                     val = '\b';
                     break;
                case 'f':
                    val = '\f';
                    break;
                case 'n':
                    val = '\n';
                    break;
                case 'r':
                    val = '\r';
                    break;
                case 't':
                    val = '\t';
                    break;
                case '\\':
                    val = '\\';
                    break;
                case '\'':
                    val = '\'';
                    break;
                case '\"':
                    val = '\"';
                    break;
                case '0':
                    val = '\0';
                    break;
                default:
                    return(eWNInvalidConstant);
                }
            } else {
                val = *ptr;
            }
            _outLocation = llvm::ConstantInt::get(mDestInt->mLLVMType, val);
            return(ok);
        }
    private:
        WNScriptType mDestInt;
    };

    struct GenerateBoolConstant: public GenerateConstantOperation {
        GenerateBoolConstant(WNScriptType _destType) :
            mDestInt(_destType) {
        }
        virtual ~GenerateBoolConstant() {}
        virtual eWNTypeError Execute(WNCodeModule&, const wn_char* _constant, bool&, llvm::Value*& _outLocation) const {
            wn_bool bConst = WNStrings::WNStrNCmp(_constant, "true", 4) == 0;
            _outLocation = llvm::ConstantInt::get(mDestInt->mLLVMType, bConst? 1 : 0);
            return(ok);
        }
    private:
        WNScriptType mDestInt;
    };

    struct GenerateVoidConstant: public GenerateConstantOperation {
        GenerateVoidConstant(WNScriptType _destType) :
            mDestInt(_destType) {
        }
        virtual ~GenerateVoidConstant() {}
        virtual eWNTypeError Execute(WNCodeModule&, const wn_char*, bool&, llvm::Value*& _outLocation) const {
            _outLocation = llvm::ConstantInt::get(mDestInt->mLLVMType, 0);
            return(ok);
        }
    private:
        WNScriptType mDestInt;
    };

    eWNTypeError WNBuiltInInitializer::InitializeIntTypes(WNScriptingEngine* _engine, WNTypeManager& _manager) {
        WNScriptType intType;
        WNScriptType charType;
        WNScriptType boolType;
        WNScriptType voidType;
        WNScriptType sizeTType;
        WNScriptType charArrayType;
        WN_RELEASE_ASSERT(_manager.RegisterScalarType("Int", _engine, 50.0f, intType, llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), sizeof(wn_int32)) == ok);
        WN_RELEASE_ASSERT(_manager.RegisterScalarType("Char", _engine, 25.0f, charType, llvm::IntegerType::getInt8Ty(llvm::getGlobalContext()), sizeof(wn_int8)) == ok);
        WN_RELEASE_ASSERT(_manager.RegisterScalarType("Bool", _engine, 10.0f, boolType, llvm::IntegerType::getInt8Ty(llvm::getGlobalContext()), sizeof(wn_int8)) == ok);
        WN_RELEASE_ASSERT(_manager.RegisterScalarType("Void", _engine, 0.0f, voidType, llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), sizeof(wn_int32) ) == ok);
#ifdef _WN_64_BIT
        WN_RELEASE_ASSERT(_manager.RegisterScalarType("-SizeT", _engine, 0.f, sizeTType, llvm::IntegerType::getInt64Ty(llvm::getGlobalContext()), sizeof(wn_size_t) ) == ok);
#else
        WN_RELEASE_ASSERT(_manager.RegisterScalarType("-SizeT", _engine, 0.f, sizeTType, llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), sizeof(wn_size_t)  ) == ok);
#endif
        WN_RELEASE_ASSERT(_manager.GetArrayOf(charType, charArrayType) == ok);
        _manager.RegisterArithmeticOperator(AR_ADD, intType, intType,
                WN_NEW GenerateIntArithmetic<&llvm::IRBuilder<>::CreateAdd >(intType));
        _manager.RegisterArithmeticOperator(AR_SUB, intType, intType,
                WN_NEW GenerateIntArithmetic<&llvm::IRBuilder<>::CreateSub >(intType));
        _manager.RegisterArithmeticOperator(AR_DIV, intType, intType,
                WN_NEW GenerateIntDivArithmetic<&llvm::IRBuilder<>::CreateSDiv> (intType));
        _manager.RegisterArithmeticOperator(AR_MULT, intType, intType,
                WN_NEW GenerateIntArithmetic<&llvm::IRBuilder<>::CreateMul>(intType));
        _manager.RegisterArithmeticOperator(AR_MOD, intType, intType,
                WN_NEW GenerateIntModArithmetic<&llvm::IRBuilder<>::CreateSRem>(intType));
        _manager.RegisterArithmeticOperator(AR_EQ, intType, intType,
                WN_NEW GenerateIntCompare<&llvm::IRBuilder<>::CreateICmpEQ>(boolType));

        _manager.RegisterAssignmentOperator(intType, AT_ADD_EQ,
                WN_NEW GenerateCompoundIntAssignment <&llvm::IRBuilder<>::CreateAdd>());
        _manager.RegisterAssignmentOperator(intType, AT_SUB_EQ,
                WN_NEW GenerateCompoundIntAssignment <&llvm::IRBuilder<>::CreateSub>());
        _manager.RegisterAssignmentOperator(intType, AT_MULT_EQ,
                WN_NEW GenerateCompoundIntAssignment <&llvm::IRBuilder<>::CreateMul>());
        _manager.RegisterAssignmentOperator(intType, AT_DIV_EQ,
            WN_NEW GenerateCompoundIntDivAssignment <&llvm::IRBuilder<>::CreateSDiv>());
        _manager.RegisterAssignmentOperator(intType, AT_MOD_EQ,
            WN_NEW GenerateCompoundIntModAssignment<&llvm::IRBuilder<>::CreateSRem>());

        _manager.RegisterArithmeticOperator(AR_NEQ, intType, intType,
                WN_NEW GenerateIntCompare<&llvm::IRBuilder<>::CreateICmpNE>(boolType));
        _manager.RegisterArithmeticOperator(AR_LEQ, intType, intType,
                WN_NEW GenerateIntCompare<&llvm::IRBuilder<>::CreateICmpSLE>(boolType));
        _manager.RegisterArithmeticOperator(AR_GEQ, intType, intType,
                WN_NEW GenerateIntCompare<&llvm::IRBuilder<>::CreateICmpSGE>(boolType));
        _manager.RegisterArithmeticOperator(AR_LT, intType, intType,
                WN_NEW GenerateIntCompare<&llvm::IRBuilder<>::CreateICmpSLT>(boolType));
        _manager.RegisterArithmeticOperator(AR_GT, intType, intType,
                WN_NEW GenerateIntCompare<&llvm::IRBuilder<>::CreateICmpSGT>(boolType));
        _manager.RegisterPreUnaryOperator(UN_PREDEC, intType,
                WN_NEW PreUnaryFunction<&GeneratePreValue<&llvm::IRBuilder<>::CreateSub> >());
        _manager.RegisterPreUnaryOperator(UN_PREINC, intType,
                WN_NEW PreUnaryFunction<&GeneratePreValue<&llvm::IRBuilder<>::CreateAdd> >());
        _manager.RegisterPostUnaryOperator(UN_POSTDEC, intType,
                WN_NEW PostUnaryFunction<&GeneratePostValue<&llvm::IRBuilder<>::CreateSub> >());
        _manager.RegisterPostUnaryOperator(UN_POSTINC, intType,
                WN_NEW PostUnaryFunction<&GeneratePostValue<&llvm::IRBuilder<>::CreateAdd> >());
        _manager.RegisterPreUnaryOperator(UN_NEG, intType,
            WN_NEW PreUnaryFunction<&GenerateIntNegation>());
        _manager.RegisterConstantOperator(intType,
            WN_NEW GenerateIntConstant(intType));
        _manager.RegisterAllocationOperator(intType,
            WN_NEW GenerateDefaultAllocation());
        _manager.RegisterAssignmentOperator(intType, AT_EQ,
            WN_NEW GenerateDefaultAssignment());
        _manager.RegisterAssignmentOperator(intType, AT_CHOWN,
            WN_NEW GenerateDefaultAssignment());

        _manager.RegisterArithmeticOperator(AR_ADD, charType, charType,
                WN_NEW GenerateIntArithmetic<&llvm::IRBuilder<>::CreateAdd >(charType));
        _manager.RegisterArithmeticOperator(AR_SUB, charType, charType,
                WN_NEW GenerateIntArithmetic<&llvm::IRBuilder<>::CreateSub >(charType));
        _manager.RegisterArithmeticOperator(AR_DIV, charType, charType,
                WN_NEW GenerateIntDivArithmetic<&llvm::IRBuilder<>::CreateSDiv> (charType));
        _manager.RegisterArithmeticOperator(AR_MULT, charType, charType,
                WN_NEW GenerateIntArithmetic<&llvm::IRBuilder<>::CreateMul>(charType));
        _manager.RegisterArithmeticOperator(AR_MOD, charType, charType,
                WN_NEW GenerateIntModArithmetic<&llvm::IRBuilder<>::CreateSRem>(charType));
        _manager.RegisterArithmeticOperator(AR_EQ, charType, charType,
                WN_NEW GenerateIntCompare<&llvm::IRBuilder<>::CreateICmpEQ>(boolType));

        _manager.RegisterAssignmentOperator(charType, AT_ADD_EQ,
                WN_NEW GenerateCompoundIntAssignment <&llvm::IRBuilder<>::CreateAdd>());
        _manager.RegisterAssignmentOperator(charType, AT_SUB_EQ,
                WN_NEW GenerateCompoundIntAssignment <&llvm::IRBuilder<>::CreateSub>());
        _manager.RegisterAssignmentOperator(charType, AT_MULT_EQ,
                WN_NEW GenerateCompoundIntAssignment <&llvm::IRBuilder<>::CreateMul>());
        _manager.RegisterAssignmentOperator(charType, AT_DIV_EQ,
            WN_NEW GenerateCompoundIntDivAssignment <&llvm::IRBuilder<>::CreateSDiv>());
        _manager.RegisterAssignmentOperator(charType, AT_MOD_EQ,
            WN_NEW GenerateCompoundIntModAssignment<&llvm::IRBuilder<>::CreateSRem>());

        _manager.RegisterArithmeticOperator(AR_NEQ, charType, charType,
                WN_NEW GenerateIntCompare<&llvm::IRBuilder<>::CreateICmpNE>(boolType));
        _manager.RegisterArithmeticOperator(AR_LEQ, charType, charType,
                WN_NEW GenerateIntCompare<&llvm::IRBuilder<>::CreateICmpSLE>(boolType));
        _manager.RegisterArithmeticOperator(AR_GEQ, charType, charType,
                WN_NEW GenerateIntCompare<&llvm::IRBuilder<>::CreateICmpSGE>(boolType));
        _manager.RegisterArithmeticOperator(AR_LT, charType, charType,
                WN_NEW GenerateIntCompare<&llvm::IRBuilder<>::CreateICmpSLT>(boolType));
        _manager.RegisterArithmeticOperator(AR_GT, charType, charType,
                WN_NEW GenerateIntCompare<&llvm::IRBuilder<>::CreateICmpSGT>(boolType));
        _manager.RegisterPreUnaryOperator(UN_PREDEC, charType,
                WN_NEW PreUnaryFunction<&GeneratePreValue<&llvm::IRBuilder<>::CreateSub> >());
        _manager.RegisterPreUnaryOperator(UN_PREINC, charType,
                WN_NEW PreUnaryFunction<&GeneratePreValue<&llvm::IRBuilder<>::CreateAdd> >());
        _manager.RegisterPostUnaryOperator(UN_POSTDEC, charType,
                WN_NEW PostUnaryFunction<&GeneratePostValue<&llvm::IRBuilder<>::CreateSub> >());
        _manager.RegisterPostUnaryOperator(UN_POSTINC, charType,
                WN_NEW PostUnaryFunction<&GeneratePostValue<&llvm::IRBuilder<>::CreateAdd> >());
        _manager.RegisterPreUnaryOperator(UN_NEG, charType,
            WN_NEW PreUnaryFunction<&GenerateIntNegation>());
        _manager.RegisterConstantOperator(charType,
            WN_NEW GenerateCharConstant(charType));
        _manager.RegisterConstantOperator(charArrayType,
            WN_NEW GenerateStringConstant(charArrayType));
        _manager.RegisterAllocationOperator(charType,
            WN_NEW GenerateDefaultAllocation());
        _manager.RegisterAssignmentOperator(charType, AT_EQ,
            WN_NEW GenerateDefaultAssignment());
        _manager.RegisterAssignmentOperator(charType, AT_CHOWN,
            WN_NEW GenerateDefaultAssignment());

        _manager.RegisterConstantOperator(boolType,
            WN_NEW GenerateBoolConstant(boolType));
        _manager.RegisterPreUnaryOperator(UN_NEG, boolType,
            WN_NEW PreUnaryFunction<&GenerateBoolNegation>());
        _manager.RegisterAllocationOperator(boolType,
            WN_NEW GenerateDefaultAllocation());
        _manager.RegisterAssignmentOperator(boolType, AT_EQ,
            WN_NEW GenerateDefaultAssignment());
        _manager.RegisterAssignmentOperator(boolType, AT_CHOWN,
            WN_NEW GenerateDefaultAssignment());

        _manager.RegisterConstantOperator(voidType,
            WN_NEW GenerateVoidConstant(voidType));

        return(ok);
    }
}
