// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNScriptingErrors.h"
#include "WNScripting/inc/WNTypeManager.h"
#include "WNScripting/inc/WNExpression.h"
#include "WNScripting/inc/WNBuiltinTypeInitialization.h"

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

namespace WNScripting {
    struct GenerateFPToIntCast: public GenerateCastingOperation {
        GenerateFPToIntCast(WNScriptType _destType, wn_bool _destSigned) :
            mDestInt(_destType),
            mDestSigned(_destSigned) {
        }
        virtual ~GenerateFPToIntCast() {}
        virtual eWNTypeError Execute(llvm::IRBuilderBase* _builder, llvm::Value* _expr1, llvm::Value*& _outReturnValue) const {
            llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>* >(_builder);
            if(mDestSigned) {
                _outReturnValue = builder->CreateFPToSI(_expr1, mDestInt->mLLVMType, "");
            } else {
                _outReturnValue = builder->CreateFPToUI(_expr1, mDestInt->mLLVMType, "");
            }

            return(ok);
        }
    private:
        WNScriptType mDestInt;
        wn_bool mDestSigned;
    };

    struct GenerateIntToFPCast: public GenerateCastingOperation {
        GenerateIntToFPCast(WNScriptType _destType, wn_bool _srcSigned) :
            mDestInt(_destType),
            mSrcSigned(_srcSigned) {
        }
        virtual ~GenerateIntToFPCast() {}
        virtual eWNTypeError Execute(llvm::IRBuilderBase* _builder, llvm::Value*_expr1, llvm::Value*& _outReturnValue) const {
            llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>* >(_builder);
            if(mSrcSigned) {
                _outReturnValue = builder->CreateSIToFP(_expr1, mDestInt->mLLVMType, "");
            } else {
                _outReturnValue = builder->CreateUIToFP(_expr1, mDestInt->mLLVMType, "");
            }

            return(ok);
        }
    private:
        WNScriptType mDestInt;
        wn_bool mSrcSigned;
    };

    struct GenerateIntToIntCast: public GenerateCastingOperation {
        GenerateIntToIntCast(WNScriptType _destType, wn_bool _srcSigned) :
            mDestInt(_destType),
            mSrcSigned(_srcSigned) {
        }
        virtual ~GenerateIntToIntCast() {}
        virtual eWNTypeError Execute(llvm::IRBuilderBase* _builder, llvm::Value* _expr1, llvm::Value*& _outReturnValue) const {
            llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>* >(_builder);
            _outReturnValue = builder->CreateIntCast(_expr1, mDestInt->mLLVMType, mSrcSigned, "");
            return(ok);
        }
    private:
        WNScriptType mDestInt;
        wn_bool mSrcSigned;
    };

    eWNTypeError WNBuiltInInitializer::InitializeScriptingCasts(WNScriptingEngine*, WNTypeManager& _manager) {
        eWNTypeError err = ok;
        WNScriptType nullType; if((err = _manager.GetTypeByName("-Null", nullType)) != ok) { return err; }
        _manager.RegisterAllocationOperator(nullType, WN_NEW GenerateDefaultAllocation());

        WNScriptType floatType;  if((err = _manager.GetTypeByName("Float", floatType)) != ok) { return err; }
        WNScriptType intType; if((err = _manager.GetTypeByName("Int", intType)) != ok) { return err; }
        WNScriptType charType;  if((err = _manager.GetTypeByName("Char", charType)) != ok) { return err; }
        WNScriptType boolType; if((err = _manager.GetTypeByName("Bool", boolType)) != ok) { return err; }
        WNScriptType sizeTType; if((err = _manager.GetTypeByName("-SizeT", sizeTType)) != ok) { return err; }
        _manager.RegisterCastingOperator(intType, charType,
            WN_NEW GenerateIntToIntCast(charType, true));
        _manager.RegisterCastingOperator(intType, floatType,
            WN_NEW GenerateIntToFPCast(floatType, true));
        _manager.RegisterCastingOperator(intType, boolType,
            WN_NEW GenerateIntToIntCast(boolType, true));
        _manager.RegisterCastingOperator(charType, intType,
            WN_NEW GenerateIntToIntCast(intType, false));
        _manager.RegisterCastingOperator(intType, sizeTType,
            WN_NEW GenerateIntToIntCast(sizeTType, false));
        _manager.RegisterCastingOperator(charType, sizeTType,
            WN_NEW GenerateIntToIntCast(sizeTType, false));
        _manager.RegisterCastingOperator(charType, floatType,
            WN_NEW GenerateIntToFPCast(floatType, false));
        _manager.RegisterCastingOperator(charType, boolType,
            WN_NEW GenerateIntToIntCast(boolType, false));
        _manager.RegisterCastingOperator(floatType, intType,
            WN_NEW GenerateFPToIntCast(intType, true));
        _manager.RegisterCastingOperator(floatType, charType,
            WN_NEW GenerateFPToIntCast(charType, false));
        _manager.RegisterCastingOperator(floatType, boolType,
            WN_NEW GenerateFPToIntCast(boolType, false));

        return(ok);
    }
}
