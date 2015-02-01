// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNBinExpression.h"
#include "WNScripting/inc/WNExpression.h"
#include "WNScripting/inc/WNCodeModule.h"
#include "WNScripting/inc/WNTypeManager.h"

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

WNBinExpression::WNBinExpression(WNArithmeticType _type, WNExpression* _lhs, WNExpression* _rhs) :
    mType(_type), mLHS(_lhs), mRHS(_rhs) {
}

WNBinExpression::~WNBinExpression() {
    WN_DELETE(mLHS);
    WN_DELETE(mRHS);
}

eWNTypeError WNBinExpression::GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) {
    llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>*>(_module.GetBuilder());
    eWNTypeError err = ok;
    if((ok != (err = mLHS->GenerateCode(_module, _def, _compilationLog))) ||
       (ok != (err = mRHS->GenerateCode(_module, _def, _compilationLog))) ) {
        return(err);
    }
    llvm::Value* lhs = mLHS->GetValue();
    llvm::Value* rhs = mRHS->GetValue();

    const GenerateArithmeticOperation* arith = _module.GetTypeManager().GetArithmeticOperation(mType, mLHS->GetType(), mRHS->GetType());
    if(!arith && (mRHS->GetType()->mPriority >= mLHS->GetType()->mPriority)) {
        const GenerateCastingOperation* cast;
        cast = _module.GetTypeManager().GetCastingOperation(mLHS->GetType(), mRHS->GetType());
        arith = _module.GetTypeManager().GetArithmeticOperation(mType, mRHS->GetType(), mRHS->GetType());
        if(cast && arith) {
            if(ok != (err = cast->Execute(builder, mLHS->GetValue(), lhs))) {
                _compilationLog.Log(WNLogging::eCritical, 0, "Error generating casting operation");
                LogLine(_compilationLog, WNLogging::eCritical);
                return(err);
            }
        } else {
            cast = _module.GetTypeManager().GetCastingOperation(mRHS->GetType(), mLHS->GetType());
            arith = _module.GetTypeManager().GetArithmeticOperation(mType, mLHS->GetType(), mLHS->GetType());
            if(cast && arith) {
                if(ok != (err = cast->Execute(builder, mRHS->GetValue(), rhs))) {
                    _compilationLog.Log(WNLogging::eCritical, 0, "Error generating casting operation");
                    LogLine(_compilationLog, WNLogging::eCritical);
                    return(err);    
                }
            } else {
                _compilationLog.Log(WNLogging::eError, 0, "Error can not cast between types ", mLHS->GetType()->mName, " and ", mRHS->GetType()->mName);
                LogLine(_compilationLog, WNLogging::eError);
                return(eWNBadType);
            }
        }
    } 
    
    if(!arith && (mRHS->GetType()->mPriority <= mLHS->GetType()->mPriority)) {
        const GenerateCastingOperation* cast;
        cast = _module.GetTypeManager().GetCastingOperation(mRHS->GetType(), mLHS->GetType());
        arith = _module.GetTypeManager().GetArithmeticOperation(mType, mLHS->GetType(), mLHS->GetType());
        if(cast && arith) {
            if(ok != (err = cast->Execute(builder, mRHS->GetValue(), rhs))) {
                _compilationLog.Log(WNLogging::eCritical, 0, "Error generating casting operation");
                LogLine(_compilationLog, WNLogging::eCritical);
                return(err);
            }
        } else {
            cast = _module.GetTypeManager().GetCastingOperation(mLHS->GetType(), mRHS->GetType());
            arith = _module.GetTypeManager().GetArithmeticOperation(mType, mRHS->GetType(), mRHS->GetType());
            if(cast && arith) {
                if(ok != (err = cast->Execute(builder, mLHS->GetValue(), lhs))) {
                    _compilationLog.Log(WNLogging::eCritical, 0, "Error generating casting operation");
                    LogLine(_compilationLog, WNLogging::eCritical);
                    return(err);    
                }
            } else {
                _compilationLog.Log(WNLogging::eError, 0, "Error can not cast between types ", mLHS->GetType()->mName, " and ", mRHS->GetType()->mName);
                LogLine(_compilationLog, WNLogging::eError);
                return(eWNBadType);
            }
        }
    }

    if(!arith) {
        _compilationLog.Log(WNLogging::eError, 0, "Error can not cast between types ", mLHS->GetType()->mName, " and ", mRHS->GetType()->mName);
        LogLine(_compilationLog, WNLogging::eError);
        return(eWNInvalidCast);
    }

    if(ok != (err = arith->Execute(builder, lhs, rhs, mScriptType, mValue))) {
        _compilationLog.Log(WNLogging::eCritical, 0, "Error generating arithmetic operation");
        LogLine(_compilationLog, WNLogging::eCritical);
        return(err);
    }

    return(ok);
}

