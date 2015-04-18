// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_NODE_TYPES_H__
#define __WN_NODE_TYPES_H__

#include "WNScripting/inc/WNTypeElement.h"
#include "WNScripting/inc/WNScriptingErrors.h"
#include "WNScripting/inc/WNCodeModule.h"
#include "WNScripting/inc/WNTypeManager.h"
#include "WNScripting/inc/WNScriptingEnums.h"
#include "WNScripting/inc/WNScopedVariableList.h"
#include "WNScripting/inc/WNScriptLinkedList.h"
#include "WNScripting/inc/WNFunctionParam.h"
#include "WNCore/inc/WNAssert.h"
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

#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"

#ifdef _WN_MSVC
    #pragma warning(pop)
#endif

namespace llvm {
    class Value;
};

namespace WNScripting {
    #define START_OFFSET( x ) for (int _i = 0; _i < x; ++_i ) { printf("    "); }

    WN_INLINE eWNTypeError GenerateActualCall(WNCodeModule& _module, const WNFunctionDefinition* _currentFunction, WNFunctionDefinition* fDefinition, std::vector<llvm::Value*> arguments, WNScriptType& _returnType, llvm::Value*& _returnValue, WNLogging::WNLog& _compilationLog) {
        eWNTypeError err;
        llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
        if(fDefinition->mFunction) {
            _returnValue = builder->CreateCall(fDefinition->mFunction, arguments, "");
        } else {
            WNScriptType voidStar = wn_nullptr;
            WNScriptType sizeTType = wn_nullptr;
            if(ok != (err = _module.GetTypeManager().GetTypeByName("-Ptr", voidStar))) {
                _compilationLog.Log(WNLogging::eCritical, 0, "Cannot find Ptr type");
                return(err);
            }
            if(ok != (err = _module.GetTypeManager().GetTypeByName("-SizeT", sizeTType))) {
                _compilationLog.Log(WNLogging::eCritical, 0, "Cannot find SizeT type");
                return(err);
            }


            llvm::Function* f = llvm::Function::Create(fDefinition->mFunctionType, llvm::GlobalValue::ExternalLinkage, "", _module.GetModule());

            llvm::Value* functionPtrLocation = llvm::ConstantInt::get(sizeTType->mLLVMType, reinterpret_cast<size_t>(&(fDefinition->mFunctionPointer)));
            functionPtrLocation = builder->CreateIntToPtr(functionPtrLocation, sizeTType->mLLVMType->getPointerTo(0), "");
            llvm::Value* fPtr = builder->CreateLoad(functionPtrLocation);
            fPtr = builder->CreateIntToPtr(fPtr, sizeTType->mLLVMType->getPointerTo(0));
            fPtr = builder->CreateBitCast(fPtr, f->getType(), "");
            llvm::Value* fPtrStore = builder->CreateAlloca(f->getType());
            builder->CreateStore(fPtr, fPtrStore, false);

            llvm::BasicBlock* doesNotExist = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _currentFunction->mFunction);
            llvm::BasicBlock* end  = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _currentFunction->mFunction);

            llvm::Value* cmpType = builder->CreatePtrToInt(fPtr, sizeTType->mLLVMType, "");
            llvm::Value* cmpValue = builder->CreateICmpEQ(cmpType, llvm::ConstantInt::get(sizeTType->mLLVMType, 0));
            builder->CreateCondBr(cmpValue, doesNotExist, end);
            builder->SetInsertPoint(doesNotExist);

            std::vector<WNScriptType> generateTypes;
            generateTypes.push_back(voidStar);
            generateTypes.push_back(voidStar);
            WNFunctionDefinition* def = _module.GetFunctionDefinition("CompileLazyFunction", generateTypes);
            if(!def) {
                _compilationLog.Log(WNLogging::eCritical, 0, "Cannot find lazy function compiler");
                return(eWNPlatformError);
            }
            llvm::Value* generateValues[2];
            generateValues[0] = builder->CreateIntToPtr(llvm::ConstantInt::get(sizeTType->mLLVMType, reinterpret_cast<size_t>(_module.GetScriptingEngine())), voidStar->mLLVMType, "");
            generateValues[1] = builder->CreateIntToPtr(llvm::ConstantInt::get(sizeTType->mLLVMType, reinterpret_cast<size_t>(fDefinition)), voidStar->mLLVMType, "");
            llvm::Value* fPtrCreated = builder->CreateCall(def->mFunction, llvm::makeArrayRef(generateValues));
            fPtrCreated = builder->CreateBitCast(fPtrCreated, f->getType(), "");
            builder->CreateStore(fPtrCreated, fPtrStore, false);
            builder->CreateBr(end);
            builder->SetInsertPoint(end);

            _returnValue = builder->CreateCall(builder->CreateLoad(fPtrStore), arguments, "");
        }
        _returnType = fDefinition->mReturn;
        return(ok);

      }

    inline eWNTypeError GenerateThisFunctionCall(WNCodeModule& _module, const WNFunctionDefinition* _currentFunction, WNFunctionDefinition* fDefinition, const std::vector<FunctionParam>& _funcParams, WNScriptType& _returnType, llvm::Value*& _returnValue, FunctionParam& _thisType, WNLogging::WNLog& _compilationLog) {
        llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());

        std::vector<WNScriptType> scriptTypes;
        scriptTypes.push_back(_thisType.mType);
        for(std::vector<FunctionParam>::const_iterator i = _funcParams.begin(); i != _funcParams.end(); ++i) {
            scriptTypes.push_back(i->mType);
        }
        std::vector<llvm::Value*> arguments;
        eWNTypeError err = ok;

        if(fDefinition->mTypes.size() != (scriptTypes.size())) {
            _compilationLog.Log(WNLogging::eError, 0, "Invalid number of arguments to this-call");
            return(err);
        }
        if(_thisType.mType != fDefinition->mTypes[0]) {
            const GenerateCastingOperation* op = _module.GetTypeManager().GetCastingOperation(_thisType.mType, fDefinition->mTypes[0]);
            if(!op) {
                _compilationLog.Log(WNLogging::eError, 0, "Invalid cast from ", _thisType.mType->mName, " to ", fDefinition->mTypes[0]->mName );
                return(eWNInvalidCast);
            }
            llvm::Value* outType = wn_nullptr;
            if(ok != (err = op->Execute(_module.GetBuilder(), _thisType.mValue, outType)))
            {
                _compilationLog.Log(WNLogging::eError, 0, "Could not generate cast from ", _thisType.mType->mName, " to ", fDefinition->mTypes[0]->mName );
                return(err);
            }
            arguments.push_back(outType);
        } else {
            arguments.push_back(_thisType.mValue);
        }

        for(wn_size_t i = 0; i < _funcParams.size(); ++i) {
            if(_funcParams[i].mType == fDefinition->mTypes[i + 1]) {
                arguments.push_back(_funcParams[i].mValue);
            } else {
                const GenerateCastingOperation* castingOp = _module.GetTypeManager().GetCastingOperation(_funcParams[i].mType, fDefinition->mTypes[i + 1]);
                if(!castingOp) {
                    _compilationLog.Log(WNLogging::eError, 0, "Cannot cast ", _funcParams[i].mType->mName, " to", fDefinition->mTypes[i + 1]->mName);
                    return(eWNInvalidCast);
                }
                llvm::Value* castedVal;
                if(ok != (err = castingOp->Execute(builder, _funcParams[i].mValue, castedVal))) {
                    _compilationLog.Log(WNLogging::eCritical, 0, "Error creating cast ", _funcParams[i].mType->mName, " to", fDefinition->mTypes[i + 1]->mName);
                    return(err);
                }
                arguments.push_back(castedVal);
            }
        }


        return(GenerateActualCall(_module, _currentFunction, fDefinition, arguments, _returnType, _returnValue, _compilationLog));
    }



    inline eWNTypeError GenerateVirtualFunctionCall(WNCodeModule& _module, const WNFunctionDefinition* _currentFunction, wn_int32 _virtualIndex, const std::vector<FunctionParam>& _funcParams, WNScriptType& _returnType, llvm::Value*& _returnValue, FunctionParam& _thisVal, WNLogging::WNLog& _compilationLog) {
        llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
        WNFunctionDefinition* fDefinition = _thisVal.mType->mVTable[_virtualIndex];
        std::vector<llvm::Value*> arguments;
        eWNTypeError err = ok;
        std::vector<WNScriptType> scriptTypes;
        scriptTypes.push_back(_thisVal.mType);
        for(std::vector<FunctionParam>::const_iterator i = _funcParams.begin(); i != _funcParams.end(); ++i) {
            scriptTypes.push_back(i->mType);
        }

        if(fDefinition->mTypes.size() != (scriptTypes.size())) {
                _compilationLog.Log(WNLogging::eError, 0, "Invalid number of arguments to this-call");
                return(err);
            }
            if(_thisVal.mType != fDefinition->mTypes[0]) {
                const GenerateCastingOperation* op = _module.GetTypeManager().GetCastingOperation(_thisVal.mType, fDefinition->mTypes[0]);
                if(!op) {
                    _compilationLog.Log(WNLogging::eError, 0, "Invalid cast from ", _thisVal.mType->mName, " to ", fDefinition->mTypes[0]->mName );
                }
                llvm::Value* outType = wn_nullptr;
                if(ok != (err = op->Execute(_module.GetBuilder(), _thisVal.mValue, outType)))
                {
                    _compilationLog.Log(WNLogging::eError, 0, "Could not generate cast from ", _thisVal.mType->mName, " to ", fDefinition->mTypes[0]->mName );
                    return(err);
                }
                arguments.push_back(outType);
            } else {
                arguments.push_back(_thisVal.mValue);
            }

            for(wn_size_t i = 0; i < _funcParams.size(); ++i) {
                if(_funcParams[i].mType == fDefinition->mTypes[i + 1]) {
                    arguments.push_back(_funcParams[i].mValue);
                } else {
                    const GenerateCastingOperation* castingOp = _module.GetTypeManager().GetCastingOperation(_funcParams[i].mType, fDefinition->mTypes[i + 1]);
                    if(!castingOp) {
                        _compilationLog.Log(WNLogging::eError, 0, "Cannot cast ", _funcParams[i].mType->mName, " to", fDefinition->mTypes[i + 1]->mName);
                        return(eWNInvalidCast);
                    }
                    llvm::Value* castedVal;
                    if(ok != (err = castingOp->Execute(builder, _funcParams[i].mValue, castedVal))) {
                        _compilationLog.Log(WNLogging::eCritical, 0, "Error creating cast ", _funcParams[i].mType->mName, " to", fDefinition->mTypes[i + 1]->mName);
                        return(err);
                    }
                    arguments.push_back(castedVal);
                }
            }


        WNScriptType voidStar = wn_nullptr;
        WNScriptType sizeTType = wn_nullptr;
        if(ok != (err = _module.GetTypeManager().GetTypeByName("-Ptr", voidStar))) {
            _compilationLog.Log(WNLogging::eCritical, 0, "Cannot find Ptr type");
            return(err);
        }
        if(ok != (err = _module.GetTypeManager().GetTypeByName("-SizeT", sizeTType))) {
            _compilationLog.Log(WNLogging::eCritical, 0, "Cannot find SizeT type");
            return(err);
        }


        llvm::Function* f = llvm::Function::Create(fDefinition->mFunctionType, llvm::GlobalValue::ExternalLinkage, "", _module.GetModule());

        std::vector<llvm::Value*> GepArray;
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 1));
        llvm::Value* thisLoc = builder->CreateLoad(builder->CreateGEP(_thisVal.mValue, GepArray), false);

        GepArray.clear();
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
        llvm::Value* vTableLoc = builder->CreateLoad(builder->CreateGEP(thisLoc, GepArray));
        vTableLoc = builder->CreatePointerCast(vTableLoc, sizeTType->mLLVMType->getPointerTo(0));
        GepArray.clear();
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), _virtualIndex * 2 + 2));
        llvm::Value* fPtrLoc = builder->CreateGEP(vTableLoc, GepArray);
        GepArray.clear();
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), _virtualIndex * 2 + 3));
        llvm::Value* fDefLoc = builder->CreateGEP(vTableLoc, GepArray);

        llvm::Value* functionPtrLocation = fPtrLoc;
        llvm::Value* fPtr = builder->CreateLoad(functionPtrLocation);
        fPtr = builder->CreateIntToPtr(fPtr, sizeTType->mLLVMType->getPointerTo(0));
        fPtr = builder->CreateBitCast(fPtr, f->getType(), "");
        functionPtrLocation = builder->CreateBitCast(functionPtrLocation, f->getType()->getPointerTo(0));
        llvm::BasicBlock* doesNotExist = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _currentFunction->mFunction);
        llvm::BasicBlock* end  = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _currentFunction->mFunction);

        llvm::Value* cmpType = builder->CreatePtrToInt(fPtr, sizeTType->mLLVMType, "");
        llvm::Value* cmpValue = builder->CreateICmpEQ(cmpType, llvm::ConstantInt::get(sizeTType->mLLVMType, 0));
        builder->CreateCondBr(cmpValue, doesNotExist, end);
        builder->SetInsertPoint(doesNotExist);

        std::vector<WNScriptType> generateTypes;
        generateTypes.push_back(voidStar);
        generateTypes.push_back(voidStar);
        WNFunctionDefinition* def = _module.GetFunctionDefinition("CompileLazyFunction", generateTypes);
        if(!def) {
            _compilationLog.Log(WNLogging::eCritical, 0, "Cannot find lazy function compiler");
            return(eWNPlatformError);
        }
        llvm::Value* generateValues[2];
        generateValues[0] = builder->CreateIntToPtr(llvm::ConstantInt::get(sizeTType->mLLVMType, reinterpret_cast<size_t>(_module.GetScriptingEngine())), voidStar->mLLVMType, "");
        generateValues[1] = builder->CreateIntToPtr(builder->CreateLoad(fDefLoc), voidStar->mLLVMType, "");
        llvm::Value* fPtrCreated = builder->CreateCall(def->mFunction, llvm::makeArrayRef(generateValues));
        fPtrCreated = builder->CreateBitCast(fPtrCreated, f->getType(), "");
        builder->CreateStore(fPtrCreated, functionPtrLocation, false);
        builder->CreateBr(end);
        builder->SetInsertPoint(end);

        _returnValue = builder->CreateCall(builder->CreateLoad(functionPtrLocation), arguments, "");
        _returnType = fDefinition->mReturn;
        return(ok);
    }

    inline eWNTypeError GenerateFunctionCall(WNCodeModule& _module, const WNFunctionDefinition* _currentFunction, const wn_char* functionName, const std::vector<FunctionParam>& _funcParams, bool _castable, WNScriptType& _returnType, llvm::Value*& _returnValue, WNLogging::WNLog& _compilationLog) {
        llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
        WNFunctionDefinition* fDefinition;
        std::vector<WNScriptType> scriptTypes;
        for(std::vector<FunctionParam>::const_iterator i = _funcParams.begin(); i != _funcParams.end(); ++i) {
            scriptTypes.push_back(i->mType);
        }
        std::vector<llvm::Value*> arguments;
        eWNTypeError err = ok;

        if(_castable) {
            if(ok != (err = _module.GetCastableFunctionDefinition(functionName, scriptTypes, fDefinition))) {
                if(eWNPartiallyAmbiguous == err) {
                    _compilationLog.Log(WNLogging::eWarning, 0, "Function ", fDefinition->mName, " is the closest match for multiple possibilites");
                } else if (eWNAmbiguous == err) {
                    _compilationLog.Log(WNLogging::eError, 0, "Function ", functionName, " is ambiguous");
                    return(err);
                } else {
                    _compilationLog.Log(WNLogging::eError, 0, "Function ", functionName, " does not exist with given types");
                    return(err);
                }
            }
            for(wn_size_t i = 0; i < _funcParams.size(); ++i) {
                if(_funcParams[i].mType == fDefinition->mTypes[i]) {
                    arguments.push_back(_funcParams[i].mValue);
                } else {
                    const GenerateCastingOperation* castingOp = _module.GetTypeManager().GetCastingOperation(_funcParams[i].mType, fDefinition->mTypes[i]);
                    if(!castingOp) {
                        _compilationLog.Log(WNLogging::eError, 0, "Cannot cast ", _funcParams[i].mType->mName, " to", fDefinition->mTypes[i]->mName);
                        return(eWNInvalidCast);
                    }
                    llvm::Value* castedVal;
                    if(ok != (err = castingOp->Execute(builder, _funcParams[i].mValue, castedVal))) {
                        _compilationLog.Log(WNLogging::eCritical, 0, "Error creating cast ", _funcParams[i].mType->mName, " to", fDefinition->mTypes[i]->mName);
                        return(err);
                    }
                    arguments.push_back(castedVal);
                }
            }
        } else {
            fDefinition = _module.GetFunctionDefinition(functionName, scriptTypes);
            if(!fDefinition) {
                _compilationLog.Log(WNLogging::eError, 0, "Function ", functionName, " does not exist with given types");
                return(eWNPlatformError);
            }
            for(wn_size_t i = 0; i < _funcParams.size(); ++i) {
                arguments.push_back(_funcParams[i].mValue);
            }
        }
        return(GenerateActualCall(_module, _currentFunction, fDefinition, arguments, _returnType, _returnValue, _compilationLog));
    }
    inline eWNTypeError GenerateRecursiveThisFunctionCall(WNCodeModule& _module, const WNFunctionDefinition* _currentFunction, const wn_char* _functionName, const std::vector<FunctionParam>& _funcParams, WNScriptType& _returnType, llvm::Value*& _returnValue, FunctionParam& _thisType, WNLogging::WNLog& _compilationLog) {
        eWNTypeError err = error;
        WNScriptType thisType = _thisType.mType;
        wn_int32 castableLocation = -1;
        WNFunctionDefinition* def = wn_nullptr;

        err = _module.GetThisCallFunction(_functionName, thisType, _funcParams, castableLocation, def);
        if(eWNAmbiguous == err) {
            _compilationLog.Log(WNLogging::eError, 0, "Error abiguous function call definition ", _functionName);
            return(err);
        } else if(!((ok == err) || (eWNPartiallyAmbiguous == err))){
             _compilationLog.Log(WNLogging::eError, 0, "Error could not resolve call to ", _functionName);
            return(err);
        } else if (eWNPartiallyAmbiguous == err) {
             _compilationLog.Log(WNLogging::eWarning, 0, "Calling ", _functionName, " but casting may occur");
        }
        if(castableLocation >= 0) {
            return(GenerateVirtualFunctionCall(_module, _currentFunction, castableLocation, _funcParams, _returnType, _returnValue, _thisType, _compilationLog));
        } else {
            return(GenerateThisFunctionCall(_module, _currentFunction, def, _funcParams, _returnType, _returnValue, _thisType, _compilationLog));
        }
    }

      WN_INLINE eWNTypeError GenerateVOIDReturn(WNCodeModule& _module, const WNFunctionDefinition* _def) {
        eWNTypeError err = ok;
        llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
        WNScriptType t = wn_nullptr;
        if(ok != (err = _module.GetTypeManager().GetTypeByName("Void", t))) {
            return(err);
        }
        if(_def->mReturn != t) {
            return(eWNInvalidCast);
        }
        const GenerateConstantOperation*  constOp = _module.GetTypeManager().GetConstantOperation(t);
        if(!constOp) {
            return(eWNBadType);
        }
        llvm::Value* v = wn_nullptr;
        wn_bool _forceUse = wn_false;
        if(ok != (err = constOp->Execute(_module, "", _forceUse, v))) {
            return(err);
        }
        if(_forceUse) {
            return(eWNInvalidConstant);
        }
        builder->CreateRet(v);
        return(ok);
    }

    inline  eWNTypeError GenerateNullAllocation(WNCodeModule& _module, const WNFunctionDefinition* /*_def*/, llvm::Value*& _retVal, WNScriptType& _outType) {
        llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
        eWNTypeError err = ok;
        _retVal = NULL;
        if(ok != (err = _module.GetTypeManager().GetTypeByName("-Null", _outType))) {
            return(err);
        }

        llvm::Value* allocLocation = wn_nullptr;
        const GenerateAllocation* alloc = _module.GetTypeManager().GetAllocationOperation(_outType);
        if(!alloc) {
            return(eWNCannotCreateType);
        }
        if(ok != (err = alloc->Execute(_module, _outType, "", false, allocLocation))) {
            return(err);
        }
#ifdef _WN_64_BIT
        llvm::Type* ptrType = llvm::IntegerType::getInt64Ty(llvm::getGlobalContext());
#else
        llvm::Type* ptrType = llvm::IntegerType::getInt32Ty(llvm::getGlobalContext());
#endif
        WNScriptType sizeTType = 0;
        if(ok != (err = _module.GetTypeManager().GetTypeByName("-SizeT", sizeTType))) {
            return(err);
        }

        llvm::Type* allocType = allocLocation->getType()->getContainedType(0)->getContainedType(0);
        llvm::Value* v = builder->Insert(llvm::CallInst::CreateMalloc(builder->GetInsertBlock(), ptrType, allocType, llvm::ConstantInt::get(sizeTType->mLLVMType, _module.GetExecutionEngine()->getDataLayout()->getTypeAllocSize(allocType))));
        v = builder->CreatePointerCast(v, allocLocation->getType()->getContainedType(0));

        std::vector<llvm::Value*> GepArray;
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 1));
        builder->CreateStore(builder->CreateIntToPtr(llvm::ConstantInt::get(sizeTType->mLLVMType, 0), _outType->mLLVMStructType->getPointerTo(0)), builder->CreateGEP(v, GepArray));

        GepArray.clear();
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 2));
        builder->CreateStore(llvm::ConstantInt::get(ptrType, 0), builder->CreateGEP(v, GepArray));

        GepArray.clear();
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
        GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
        ///builder->CreateGEP(v, GepArray);
        builder->CreateStore(builder->CreateIntToPtr(llvm::ConstantInt::get(sizeTType->mLLVMType, 0), llvm::IntegerType::getInt32PtrTy(llvm::getGlobalContext())), builder->CreateGEP(v, GepArray));

        builder->CreateStore(v, allocLocation);
        _retVal = v;
        return(ok);
    }

}
#endif//__WN_NODE_TYPES_H__
