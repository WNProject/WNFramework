// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


#include "WNScripting/inc/WNIncludeLLVM.h"
#include "llvm/IR/IRBuilder.h"
#include "WNScripting/inc/WNEndIncludeLLVM.h"

#include "WNScripting/inc/WNScriptingErrors.h"
#include "WNScripting/inc/WNTypeManager.h"
#include "WNScripting/inc/WNExpression.h"
#include "WNScripting/inc/WNBuiltinTypeInitialization.h"

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
        _manager.RegisterAllocationOperator(nullType, wn::memory::construct<GenerateDefaultAllocation>());

        WNScriptType floatType;  if((err = _manager.GetTypeByName("Float", floatType)) != ok) { return err; }
        WNScriptType intType; if((err = _manager.GetTypeByName("Int", intType)) != ok) { return err; }
        WNScriptType charType;  if((err = _manager.GetTypeByName("Char", charType)) != ok) { return err; }
        WNScriptType boolType; if((err = _manager.GetTypeByName("Bool", boolType)) != ok) { return err; }
        WNScriptType sizeTType; if((err = _manager.GetTypeByName("-SizeT", sizeTType)) != ok) { return err; }
        _manager.RegisterCastingOperator(intType, charType,
            wn::memory::construct<GenerateIntToIntCast>(charType, true));
        _manager.RegisterCastingOperator(intType, floatType,
            wn::memory::construct<GenerateIntToFPCast>(floatType, true));
        _manager.RegisterCastingOperator(intType, boolType,
            wn::memory::construct<GenerateIntToIntCast>(boolType, true));
        _manager.RegisterCastingOperator(charType, intType,
            wn::memory::construct<GenerateIntToIntCast>(intType, false));
        _manager.RegisterCastingOperator(intType, sizeTType,
            wn::memory::construct<GenerateIntToIntCast>(sizeTType, false));
        _manager.RegisterCastingOperator(charType, sizeTType,
            wn::memory::construct<GenerateIntToIntCast>(sizeTType, false));
        _manager.RegisterCastingOperator(charType, floatType,
            wn::memory::construct<GenerateIntToFPCast>(floatType, false));
        _manager.RegisterCastingOperator(charType, boolType,
            wn::memory::construct<GenerateIntToIntCast>(boolType, false));
        _manager.RegisterCastingOperator(floatType, intType,
            wn::memory::construct<GenerateFPToIntCast>(intType, true));
        _manager.RegisterCastingOperator(floatType, charType,
            wn::memory::construct<GenerateFPToIntCast>(charType, false));
        _manager.RegisterCastingOperator(floatType, boolType,
            wn::memory::construct<GenerateFPToIntCast>(boolType, false));

        return(ok);
    }
}
