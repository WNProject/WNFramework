
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WNDeprecatedScripting/inc/WNIncludeLLVM.h"
#include "llvm/IR/IRBuilder.h"
#include "WNDeprecatedScripting/inc/WNEndIncludeLLVM.h"

#include "WNDeprecatedScripting/inc/WNScriptingErrors.h"
#include "WNDeprecatedScripting/inc/WNBuiltinTypeInitialization.h"
#include "WNDeprecatedScripting/inc/WNTypeManager.h"
#include "WNDeprecatedScripting/inc/WNExpression.h"
#include "WNDeprecatedScripting/inc/WNCodeModule.h"
#include "WNMemory/inc/WNBasic.h"

namespace WNScripting {

    template<llvm::Value* (llvm::IRBuilder<>::*T)(llvm::Value*, llvm::Value*, const llvm::Twine&, llvm::MDNode*)>
    struct GenerateFloatArithmetic : public GenerateArithmeticOperation {
        GenerateFloatArithmetic(WNScriptType _destType) :
            mDestFlt(_destType) {
        }
        virtual ~GenerateFloatArithmetic() {}
        virtual eWNTypeError Execute(llvm::IRBuilderBase* _builder, llvm::Value* _expr1, llvm::Value* _expr2, WNScriptType& _outType, llvm::Value*& _outReturnVal) const {
            llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>* >(_builder);
            _outReturnVal = ((*builder).*T)(_expr1, _expr2, "", 0);
            _outType = mDestFlt;
            return(ok);
        }
    private:
        WNScriptType mDestFlt;
    };

    template<llvm::Value* (llvm::IRBuilder<>::*T)(llvm::Value*, llvm::Value*, const llvm::Twine&, llvm::MDNode*)>
    struct GenerateCompoundFloatAssignment : public GenerateAssignment {
        GenerateCompoundFloatAssignment() {
        }
        virtual ~GenerateCompoundFloatAssignment() {}
        virtual eWNTypeError Execute(WNCodeModule& _module, bool _declaration, WNScriptType, llvm::Value* value, llvm::Value* assignLocation, const WNFunctionDefinition*, WNLogging::WNLog& _compilationLog) const {
            if(_declaration) {
                _compilationLog.Log(WNLogging::eError, 0, "Cannot declare a variable with a compound assignment");
                return(eWNCannotCreateType);
            }
            llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>* >(_module.GetBuilder());
            llvm::Value* inVal = builder->CreateLoad(assignLocation, false, "");
            llvm::Value* tempVal = ((*builder).*T)(inVal, value, "", 0);
            builder->CreateStore(tempVal, assignLocation, false);
            return(ok);
        }
    private:
        WNScriptType mDestFlt;
    };

    template<llvm::Value* (llvm::IRBuilder<>::*T)(llvm::Value*, llvm::Value*, const llvm::Twine&,
        llvm::MDNode*)>
    struct GenerateFloatCompare : public GenerateArithmeticOperation {
        GenerateFloatCompare(WNScriptType _destType) :
            mDestFlt(_destType) {
        }
        virtual ~GenerateFloatCompare() {}
        virtual eWNTypeError Execute(llvm::IRBuilderBase* _builder, llvm::Value* _expr1, llvm::Value* _expr2, WNScriptType& _destType, llvm::Value*& _outReturnVal) const {
            llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>* >(_builder);
            _outReturnVal = ((*builder).*T)(_expr1, _expr2, "", nullptr);
            _outReturnVal = builder->CreateIntCast(_outReturnVal, mDestFlt->mLLVMType, false, "");
            _destType = mDestFlt;
            return(ok);
        }
    private:
        WNScriptType mDestFlt;
    };


    struct GenerateFloatConstant: public GenerateConstantOperation {
        GenerateFloatConstant(WNScriptType _destType) :
            mDestFlt(_destType) {
        }
        virtual ~GenerateFloatConstant() {}
        virtual eWNTypeError Execute(WNCodeModule&, const wn_char* _constant, bool&, llvm::Value*& _outLocation) const {
            wn_float32 flt = WNStrings::WNStrToFlt(_constant);
            _outLocation = llvm::ConstantFP::get(mDestFlt->mLLVMType, flt);
             return(ok);
        }
    private:
        WNScriptType mDestFlt;
    };


    eWNTypeError GenerateFloatNegation(llvm::IRBuilderBase* _builder, const WNExpression& _expr1, llvm::Value*& _value) {
        llvm::IRBuilder<>* builder = reinterpret_cast<llvm::IRBuilder<>* >(_builder);
        llvm::Type* type = _expr1.GetType()->mLLVMType;
        llvm::Value* val = llvm::ConstantFP::get(type, -1.0);
        _value = builder->CreateFMul(_expr1.GetValue(), val, "", 0);
        return(ok);
    }

    eWNTypeError WNBuiltIninitializer::InitializeFloatTypes(WNScriptingEngine* _engine, WNTypeManager& _manager) {
        WNScriptType scriptType;
        WNScriptType boolType;
        WN_RELEASE_ASSERT(_manager.RegisterScalarType("Float", _engine, 100.0f, scriptType, llvm::Type::getFloatTy(llvm::getGlobalContext()), sizeof(wn_float32)) == ok);


        _manager.RegisterArithmeticOperator(AR_ADD, scriptType, scriptType,
            wn::memory::construct<GenerateFloatArithmetic<&llvm::IRBuilder<>::CreateFAdd>>(scriptType));
        _manager.RegisterArithmeticOperator(AR_SUB, scriptType, scriptType,
            wn::memory::construct<GenerateFloatArithmetic<&llvm::IRBuilder<>::CreateFSub>>(scriptType));
        _manager.RegisterArithmeticOperator(AR_MULT, scriptType, scriptType,
            wn::memory::construct<GenerateFloatArithmetic<&llvm::IRBuilder<>::CreateFMul>>(scriptType));
        _manager.RegisterArithmeticOperator(AR_DIV, scriptType, scriptType,
            wn::memory::construct<GenerateFloatArithmetic<&llvm::IRBuilder<>::CreateFDiv>>(scriptType));
        _manager.RegisterArithmeticOperator(AR_MOD, scriptType, scriptType,
            wn::memory::construct<GenerateFloatArithmetic<&llvm::IRBuilder<>::CreateFRem>>(scriptType));
        if(ok == _manager.get_type_by_name("Bool", boolType)) {
            _manager.RegisterArithmeticOperator(AR_EQ, scriptType, scriptType,
                wn::memory::construct<GenerateFloatCompare<&llvm::IRBuilder<>::CreateFCmpOEQ>>(boolType));
            _manager.RegisterArithmeticOperator(AR_NEQ, scriptType, scriptType,
                wn::memory::construct<GenerateFloatCompare<&llvm::IRBuilder<>::CreateFCmpONE>>(boolType));
            _manager.RegisterArithmeticOperator(AR_LEQ, scriptType, scriptType,
                wn::memory::construct<GenerateFloatCompare<&llvm::IRBuilder<>::CreateFCmpOLE>>(boolType));
            _manager.RegisterArithmeticOperator(AR_GEQ, scriptType, scriptType,
                wn::memory::construct<GenerateFloatCompare<&llvm::IRBuilder<>::CreateFCmpOGE>>(boolType));
            _manager.RegisterArithmeticOperator(AR_LT, scriptType, scriptType,
                wn::memory::construct<GenerateFloatCompare<&llvm::IRBuilder<>::CreateFCmpOLT>>(boolType));
            _manager.RegisterArithmeticOperator(AR_GT, scriptType, scriptType,
                wn::memory::construct<GenerateFloatCompare<&llvm::IRBuilder<>::CreateFCmpOGT>>(boolType));
        }
        _manager.RegisterAssignmentOperator(scriptType, AT_ADD_EQ,
            wn::memory::construct<GenerateCompoundFloatAssignment<&llvm::IRBuilder<>::CreateFAdd>>());
        _manager.RegisterAssignmentOperator(scriptType, AT_SUB_EQ,
            wn::memory::construct<GenerateCompoundFloatAssignment<&llvm::IRBuilder<>::CreateFSub>>());
        _manager.RegisterAssignmentOperator(scriptType, AT_MULT_EQ,
            wn::memory::construct<GenerateCompoundFloatAssignment<&llvm::IRBuilder<>::CreateFMul>>());
        _manager.RegisterAssignmentOperator(scriptType, AT_DIV_EQ,
            wn::memory::construct< GenerateCompoundFloatAssignment<&llvm::IRBuilder<>::CreateFDiv>>());
        _manager.RegisterAssignmentOperator(scriptType, AT_MOD_EQ,
            wn::memory::construct<GenerateCompoundFloatAssignment<&llvm::IRBuilder<>::CreateFRem>>());


        _manager.RegisterPreUnaryOperator(UN_NEG, scriptType,
            wn::memory::construct<PreUnaryFunction<&GenerateFloatNegation>>());
        _manager.RegisterConstantOperator(scriptType,
            wn::memory::construct<GenerateFloatConstant>(scriptType));
        _manager.RegisterAllocationOperator(scriptType,
            wn::memory::construct<GenerateDefaultAllocation>());
        _manager.RegisterAssignmentOperator(scriptType, AT_EQ,
            wn::memory::construct<GenerateDefaultAssignment>());
        _manager.RegisterAssignmentOperator(scriptType, AT_CHOWN,
            wn::memory::construct<GenerateDefaultAssignment>());
        return(ok);
    }
}
