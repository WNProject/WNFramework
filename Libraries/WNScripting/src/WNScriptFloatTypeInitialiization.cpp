
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

#ifdef _WN_MSVC
    #pragma warning(pop)
#endif

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

    template<llvm::Value* (llvm::IRBuilder<>::*T)(llvm::Value*, llvm::Value*, const llvm::Twine&)>
    struct GenerateFloatCompare : public GenerateArithmeticOperation {
        GenerateFloatCompare(WNScriptType _destType) :
            mDestFlt(_destType) {
        }
        virtual ~GenerateFloatCompare() {}
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

    eWNTypeError WNBuiltInInitializer::InitializeFloatTypes(WNScriptingEngine* _engine, WNTypeManager& _manager) {
        WNScriptType scriptType;
        WNScriptType boolType;
        WN_RELEASE_ASSERT(_manager.RegisterScalarType("Float", _engine, 100.0f, scriptType, llvm::Type::getFloatTy(llvm::getGlobalContext()), sizeof(wn_float32)) == ok);


        _manager.RegisterArithmeticOperator(AR_ADD, scriptType, scriptType,
            WN_NEW GenerateFloatArithmetic<&llvm::IRBuilder<>::CreateFAdd>(scriptType));
        _manager.RegisterArithmeticOperator(AR_SUB, scriptType, scriptType,
            WN_NEW GenerateFloatArithmetic<&llvm::IRBuilder<>::CreateFSub>(scriptType));
        _manager.RegisterArithmeticOperator(AR_MULT, scriptType, scriptType,
            WN_NEW GenerateFloatArithmetic<&llvm::IRBuilder<>::CreateFMul>(scriptType));
        _manager.RegisterArithmeticOperator(AR_DIV, scriptType, scriptType,
            WN_NEW GenerateFloatArithmetic<&llvm::IRBuilder<>::CreateFDiv>(scriptType));
        _manager.RegisterArithmeticOperator(AR_MOD, scriptType, scriptType,
            WN_NEW GenerateFloatArithmetic<&llvm::IRBuilder<>::CreateFRem>(scriptType));
        if(ok == _manager.GetTypeByName("Bool", boolType)) {
            _manager.RegisterArithmeticOperator(AR_EQ, scriptType, scriptType,
                WN_NEW GenerateFloatCompare<&llvm::IRBuilder<>::CreateFCmpOEQ>(boolType));
            _manager.RegisterArithmeticOperator(AR_NEQ, scriptType, scriptType,
                WN_NEW GenerateFloatCompare<&llvm::IRBuilder<>::CreateFCmpONE>(boolType));
            _manager.RegisterArithmeticOperator(AR_LEQ, scriptType, scriptType,
                WN_NEW GenerateFloatCompare<&llvm::IRBuilder<>::CreateFCmpOLE>(boolType));
            _manager.RegisterArithmeticOperator(AR_GEQ, scriptType, scriptType,
                WN_NEW GenerateFloatCompare<&llvm::IRBuilder<>::CreateFCmpOGE>(boolType));
            _manager.RegisterArithmeticOperator(AR_LT, scriptType, scriptType,
                WN_NEW GenerateFloatCompare<&llvm::IRBuilder<>::CreateFCmpOLT>(boolType));
            _manager.RegisterArithmeticOperator(AR_GT, scriptType, scriptType,
                WN_NEW GenerateFloatCompare<&llvm::IRBuilder<>::CreateFCmpOGT>(boolType));
        }
        _manager.RegisterAssignmentOperator(scriptType, AT_ADD_EQ,
                WN_NEW GenerateCompoundFloatAssignment <&llvm::IRBuilder<>::CreateFAdd>());
        _manager.RegisterAssignmentOperator(scriptType, AT_SUB_EQ,
                WN_NEW GenerateCompoundFloatAssignment <&llvm::IRBuilder<>::CreateFSub>());
        _manager.RegisterAssignmentOperator(scriptType, AT_MULT_EQ,
                WN_NEW GenerateCompoundFloatAssignment <&llvm::IRBuilder<>::CreateFMul>());
        _manager.RegisterAssignmentOperator(scriptType, AT_DIV_EQ,
            WN_NEW GenerateCompoundFloatAssignment <&llvm::IRBuilder<>::CreateFDiv>());
        _manager.RegisterAssignmentOperator(scriptType, AT_MOD_EQ,
            WN_NEW GenerateCompoundFloatAssignment<&llvm::IRBuilder<>::CreateFRem>());


        _manager.RegisterPreUnaryOperator(UN_NEG, scriptType,
            WN_NEW PreUnaryFunction<&GenerateFloatNegation>());
        _manager.RegisterConstantOperator(scriptType,
            WN_NEW GenerateFloatConstant(scriptType));
        _manager.RegisterAllocationOperator(scriptType,
            WN_NEW GenerateDefaultAllocation());
        _manager.RegisterAssignmentOperator(scriptType, AT_EQ,
            WN_NEW GenerateDefaultAssignment());
        _manager.RegisterAssignmentOperator(scriptType, AT_CHOWN,
            WN_NEW GenerateDefaultAssignment());
        return(ok);
    }
}
