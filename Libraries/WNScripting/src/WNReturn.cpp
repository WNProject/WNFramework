// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNReturn.h"
#include "WNScripting/inc/WNCodeModule.h"
#include "WNScripting/inc/WNExpression.h"
#include "WNScripting/inc/WNTypeManager.h"
#include "WNScripting/inc/WNScopedVariableList.h"
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

#ifdef _WN_MSVC
    #pragma warning(pop)
#endif

using namespace WNScripting;

WNReturn::WNReturn(WNExpression* _expr, bool _changeOwnership):
    mExpression(_expr),
    mChangeOwnership(_changeOwnership) {
}

WNReturn::WNReturn() :
    mExpression(wn_nullptr) {
}

WNReturn::~WNReturn() {
    if(mExpression) {
        WN_DELETE(mExpression);
    }
}

eWNTypeError WNReturn::GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
    mReturns = true;
    eWNTypeError err;
    if(mExpression) {
        llvm::Value* v = wn_nullptr;
        if(ok != (err = mExpression->GenerateCode(_module, _def, _compilationLog)) ) {
            return(err);
        }
        v  = mExpression->GetValue();
        if(_def->mReturn != mExpression->GetType()) {
            const GenerateCastingOperation* castOp = _module.GetTypeManager().GetCastingOperation(mExpression->GetType(), _def->mReturn);
            if(!castOp) {
                _compilationLog.Log(WNLogging::eError, 0, "Cannot cast ", mExpression->GetType(), " to ", _def->mReturn->mName);
                LogLine(_compilationLog, WNLogging::eError);
                return(eWNInvalidCast);
            }
            if(ok != (err = castOp->Execute(_module.GetBuilder(), mExpression->GetValue(), v))) {
                _compilationLog.Log(WNLogging::eCritical, 0, "Error generating casting operation ", mExpression->GetType()->mName, " to ", _def->mReturn->mName);
                LogLine(_compilationLog, WNLogging::eCritical);
                return(err);
            }
        }
         #ifdef _WN_64_BIT
                llvm::Type* ptrType = llvm::IntegerType::getInt64Ty(llvm::getGlobalContext());
            #else
                llvm::Type* ptrType = llvm::IntegerType::getInt32Ty(llvm::getGlobalContext());
            #endif

        if(mChangeOwnership){
            if(!((_def->mReturn->mLLVMStructType || _def->mReturn->mArrayType) && mExpression->GetValueLocation())) {
                _compilationLog.Log(WNLogging::eCritical, 0, "Cannot change of type ", mExpression->GetType()->mName);
                LogLine(_compilationLog, WNLogging::eCritical);
                return(error);
            }
            llvm::Value* structLoc = mExpression->GetValueLocation();
            std::vector<llvm::Value*> GepArray;
            GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
            llvm::Value* spLoc = builder->CreateLoad(builder->CreateGEP(structLoc, GepArray));

            GepArray.clear();
            GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
            GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
            llvm::Value* valueLoc = builder->CreateGEP(spLoc, GepArray);
            llvm::Value* tempValue = builder->CreateLoad(valueLoc);

            llvm::BasicBlock* destroyBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "DestroyStruct", _def->mFunction);
            llvm::BasicBlock* endBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "Continue", _def->mFunction);

            llvm::Value* cmp = builder->CreateICmpEQ(builder->CreatePtrToInt(tempValue, ptrType), builder->CreatePtrToInt(structLoc, ptrType));

            builder->CreateCondBr(cmp, destroyBlock, endBlock);
            builder->SetInsertPoint(destroyBlock);
            builder->CreateStore(llvm::ConstantInt::get(ptrType, 0), builder->CreatePointerCast(valueLoc, ptrType->getPointerTo(0)));
            GepArray.clear();
            GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
            GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 2));
            llvm::Value* countLocation = builder->CreateGEP(spLoc, GepArray);
            builder->CreateAtomicRMW(llvm::AtomicRMWInst::Add, countLocation, llvm::ConstantInt::get(ptrType, 1), llvm::Monotonic);


            builder->CreateBr(endBlock);
            builder->SetInsertPoint(endBlock);
        }
        else if ((_def->mReturn->mLLVMStructType || _def->mReturn->mArrayType) && mExpression->GetValueLocation()) {
            llvm::Value* structLoc = mExpression->GetValueLocation();
            std::vector<llvm::Value*> GepArray;
            GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
            llvm::Value* spLoc = builder->CreateLoad(builder->CreateGEP(structLoc, GepArray));
            GepArray.clear();
            GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
            GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 2));
            llvm::Value* countLocation = builder->CreateGEP(spLoc, GepArray);
            builder->CreateAtomicRMW(llvm::AtomicRMWInst::Add, countLocation, llvm::ConstantInt::get(ptrType, 1), llvm::Monotonic);
        }


        if(ok != (err = _module.GetScopedVariableList().GenerateReturn(_module, _def, _compilationLog))) {
            _compilationLog.Log(WNLogging::eCritical, 0, "Error generating return");
            LogLine(_compilationLog, WNLogging::eCritical);
            return(err);
        }

        if ((_def->mReturn->mLLVMStructType || _def->mReturn->mArrayType) && mExpression->GetValueLocation()) {
            llvm::Value* structLoc = mExpression->GetValueLocation();
            std::vector<llvm::Value*> GepArray;
            GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
            llvm::Value* spLoc = builder->CreateLoad(builder->CreateGEP(structLoc, GepArray));
            GepArray.clear();
            GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 0));
            GepArray.push_back(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), 2));
            llvm::Value* countLocation = builder->CreateGEP(spLoc, GepArray);
            builder->CreateAtomicRMW(llvm::AtomicRMWInst::Sub, countLocation, llvm::ConstantInt::get(ptrType, 1), llvm::Monotonic);
        }

        builder->CreateRet(v);
    } else {
        if(ok != (err = _module.GetScopedVariableList().GenerateReturn(_module, _def, _compilationLog))) {
            _compilationLog.Log(WNLogging::eCritical, 0, "Error generating return");
            LogLine(_compilationLog, WNLogging::eCritical);
            return(err);
        }
        if(ok != (err = GenerateVOIDReturn(_module, _def))) {
            _compilationLog.Log(WNLogging::eCritical, 0, "Error generating void return");
            LogLine(_compilationLog, WNLogging::eCritical);
            return(err);
        }
    }
    return(ok);
}

