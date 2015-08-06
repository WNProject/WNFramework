// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_SCRIPTING_TYPE_MANAGER_H__
#define __WN_SCRIPTING_TYPE_MANAGER_H__

#include "WNCore/inc/WNTypes.h"
#include "WNDeprecatedScripting/inc/WNTypeElement.h"
#include "WNDeprecatedScripting/inc/WNScriptingErrors.h"
#include "WNDeprecatedScripting/inc/WNScriptingEnums.h"
#include "WNDeprecatedScripting/inc/WNScriptLinkedList.h"

namespace llvm{
    class Value;
    class IRBuilderBase;
};

namespace WNLogging {
    class WNLog;
};

namespace WNScripting {
    class WNExpression;
    class WNTypeNode;
    class WNCodeModule;
    struct WNFunctionDefinition;

    struct GenerateArithmeticOperation {
        virtual ~GenerateArithmeticOperation() {}
        virtual eWNTypeError Execute(llvm::IRBuilderBase* _builder, llvm::Value*, llvm::Value*, WNScriptType& _outType, llvm::Value*& _outReturnVal) const = 0;
    };
    template<eWNTypeError (*T)(llvm::IRBuilderBase*, llvm::Value*, llvm::Value*, WNScriptType&, llvm::Value*&)>
    struct ArithmeticFunction : public GenerateArithmeticOperation {
        virtual ~ArithmeticFunction() {}
        virtual eWNTypeError Execute(llvm::IRBuilderBase* _builder, llvm::Value* _expr1, llvm::Value* _expr2, WNScriptType& _outType, llvm::Value*& _outReturnValue) const {
            return(T(_builder, _expr1, _expr2, _outType, _outReturnValue));
        }
    };

    struct GenerateCastingOperation {
        virtual ~GenerateCastingOperation() {};
        virtual eWNTypeError Execute(llvm::IRBuilderBase* _builder, llvm::Value*, llvm::Value*& _outReturnVal) const = 0;
    };
    template<eWNTypeError (*T)(llvm::IRBuilderBase*, llvm::Value*, llvm::Value*&)>
    struct CastFunction : public GenerateCastingOperation {
        virtual ~CastFunction() {}
        virtual eWNTypeError Execute(llvm::IRBuilderBase* _builder, llvm::Value* _expr1, llvm::Value*& _outReturnValue) const {
            return(T(_builder, _expr1, _outReturnValue));
        }
    };
    struct GenerateReinterpretCastOperation : public GenerateCastingOperation {
        GenerateReinterpretCastOperation(WNScriptType scriptType) : mScriptType(scriptType) { };
        virtual ~GenerateReinterpretCastOperation() {}
        virtual eWNTypeError Execute(llvm::IRBuilderBase* _builder, llvm::Value* _expr1, llvm::Value*& _outReturnValue) const;
    private:
        WNScriptType mScriptType;
    };

    struct GenerateStructCompare : public GenerateArithmeticOperation {
        GenerateStructCompare(WNScriptType _boolType, bool _eq) : mBoolType(_boolType), mEq(_eq) {};
        eWNTypeError Execute(llvm::IRBuilderBase* _builder, llvm::Value* _expr1, llvm::Value* _expr2, WNScriptType& _destType, llvm::Value*& _outReturnVal) const;
    private:
        WNScriptType mBoolType;
        wn_bool mEq;
    };

    struct GeneratePreUnaryOperation {
        virtual ~GeneratePreUnaryOperation() {};
        virtual eWNTypeError Execute(llvm::IRBuilderBase* _builder, const WNExpression& , llvm::Value*& _outReturnVal) const = 0;
    };
    template<eWNTypeError (*T)(llvm::IRBuilderBase*, const WNExpression&, llvm::Value*&)>
    struct PreUnaryFunction : public GeneratePreUnaryOperation {
        virtual ~PreUnaryFunction() {}
        virtual eWNTypeError Execute(llvm::IRBuilderBase* _builder, const WNExpression& _expr1, llvm::Value*& _outReturnValue) const {
            return(T(_builder, _expr1, _outReturnValue));
        }
    };

    struct GeneratePostUnaryOperation {
        virtual ~GeneratePostUnaryOperation() {};
        virtual eWNTypeError Execute(llvm::IRBuilderBase* _builder, const WNExpression& , llvm::Value*& _outValue) const = 0;
    };
    template<eWNTypeError (*T)(llvm::IRBuilderBase*, const WNExpression& , llvm::Value*&)>
    struct PostUnaryFunction : public GeneratePostUnaryOperation {
        virtual ~PostUnaryFunction() {}
        virtual eWNTypeError Execute(llvm::IRBuilderBase* _builder, const WNExpression& _expr1, llvm::Value*& _outValue) const {
            return(T(_builder, _expr1, _outValue));
        }
    };

    struct GenerateArrayAccessOperation {
        virtual ~GenerateArrayAccessOperation() {};
        virtual eWNTypeError Execute(WNCodeModule& _module, llvm::Value* _lhs, llvm::Value* _rhs, llvm::Value*& _outValue, llvm::Value*& _outLocation, const WNFunctionDefinition* _def) const = 0;
    };
    template<eWNTypeError (*T)(WNCodeModule& _module, llvm::Value*, llvm::Value*, llvm::Value*&, llvm::Value*&, const WNFunctionDefinition*)>
    struct ArrayAccessFunction : public GenerateArrayAccessOperation {
        virtual ~ArrayAccessFunction() {}
        virtual eWNTypeError Execute(WNCodeModule& _module, llvm::Value* _expr1, llvm::Value* _expr2, llvm::Value*& _outReturnValue, llvm::Value*& _outLocation, const WNFunctionDefinition* _def) const {
            return(T(_module, _expr1, _expr2, _outReturnValue, _outLocation, _def));
        }
    };
    struct GenerateDefaultArrayAccessOperation : public GenerateArrayAccessOperation {
        GenerateDefaultArrayAccessOperation(WNScriptType _type):mScriptType(_type) { }
        virtual ~GenerateDefaultArrayAccessOperation () {};
        virtual eWNTypeError Execute(WNCodeModule& _module, llvm::Value*, llvm::Value*, llvm::Value*& _outValue, llvm::Value*& _outLocation, const WNFunctionDefinition* _def) const;
    private:
        WNScriptType mScriptType;
    };

    struct GenerateIDAccessOperation {
        virtual ~GenerateIDAccessOperation() {};
        virtual eWNTypeError Execute(llvm::IRBuilderBase* _builder, llvm::Value*, llvm::Value*, wn_char*, WNScriptType &_outType, llvm::Value*& _outValue, llvm::Value*& _outLocation) const = 0;
    };
    template<eWNTypeError (*T)(llvm::IRBuilderBase*, llvm::Value*, llvm::Value*, wn_char*, WNScriptType &, llvm::Value*&, llvm::Value*&)>
    struct IDAccessFunction : public GenerateIDAccessOperation {
        virtual ~IDAccessFunction () {}
        virtual eWNTypeError Execute(llvm::IRBuilderBase* _builder, llvm::Value* _expr1, llvm::Value* _expr2, wn_char* _id, WNScriptType &_outType, llvm::Value*& _outReturnValue, llvm::Value*& _outLocation) const {
            return(T(_builder, _expr1, _expr2, _id, _outType, _outReturnValue, _outLocation));
        }
    };
    struct GenerateDefaultStructAccessor : public GenerateIDAccessOperation {
        virtual ~GenerateDefaultStructAccessor() {};
        GenerateDefaultStructAccessor(WNScriptType _scriptType, WNScriptType _functionType);
        virtual eWNTypeError Execute(llvm::IRBuilderBase* _builder, llvm::Value*, llvm::Value*, wn_char*, WNScriptType &_outType, llvm::Value*& _outValue, llvm::Value*& _outLocation) const;
    private:
        WNScriptType mStructType;
        WNScriptType mFunctionType;
    };

    struct GenerateCPPStructAccessor : public GenerateIDAccessOperation {
        virtual ~GenerateCPPStructAccessor () {};
        GenerateCPPStructAccessor (WNScriptType _scriptType, WNScriptType _functionType);
        virtual eWNTypeError Execute(llvm::IRBuilderBase* _builder, llvm::Value*, llvm::Value*, wn_char*, WNScriptType &_outType, llvm::Value*& _outValue, llvm::Value*& _outLocation) const;
    private:
        WNScriptType mStructType;
        WNScriptType mFunctionType;
    };

    struct GenerateArrayIDOperator : public GenerateIDAccessOperation {
        virtual ~GenerateArrayIDOperator() {};
        GenerateArrayIDOperator(WNScriptType _scriptType, WNScriptType _intType, WNScriptType _functionType);
        virtual eWNTypeError Execute(llvm::IRBuilderBase* _builder, llvm::Value*, llvm::Value*, wn_char*, WNScriptType &_outType, llvm::Value*& _outValue, llvm::Value*& _outLocation) const;
    private:
        WNScriptType mStructType;
        WNScriptType mIntType;
        WNScriptType mFunctionType;
    };

    struct GenerateConstantOperation {
        virtual ~GenerateConstantOperation() {};
        virtual eWNTypeError Execute(WNCodeModule& _builder, const wn_char*, bool& mForceUse, llvm::Value*& _outLocation) const = 0;
    };
    template<eWNTypeError (*T)(llvm::IRBuilderBase*, const wn_char*, llvm::Value*& _outLocation)>
    struct ConstantFunction : public GenerateConstantOperation {
        virtual ~ConstantFunction() {}
        virtual eWNTypeError Execute(llvm::IRBuilderBase* _builder, const wn_char* _constant, llvm::Value*& _outLocation) const {
            return(T(_builder, _constant, _outLocation));
        }
    };

    struct GenerateAllocation {
        virtual ~GenerateAllocation() {};
        virtual eWNTypeError Execute(WNCodeModule& _builder, WNScriptType _node, const wn_char* _name, wn_bool _onHeap, llvm::Value*& _outLocation) const = 0;
    };
    struct GenerateDefaultAllocation : public GenerateAllocation {
        virtual eWNTypeError Execute(WNCodeModule& _module, WNScriptType _node, const wn_char* _name, wn_bool _onHeap, llvm::Value*& _outLocation) const;
    };

    struct GenerateConstruction {
        virtual ~GenerateConstruction()  {};
        virtual eWNTypeError Execute(WNCodeModule& _module, llvm::Value*& _retVal, WNScriptType structType, WNScriptLinkedList<WNExpression>::WNScriptLinkedListNode* _initializer, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) const = 0;
    };

    struct GenerateCopyConstruction {
        virtual ~GenerateCopyConstruction() {};
        virtual eWNTypeError Execute(WNCodeModule& _module, llvm::Value*& _retVal, WNScriptType structType, llvm::Value* _copyValue, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) const = 0;
    };

    struct GenerateStructConstruction : public GenerateConstruction {
        virtual eWNTypeError Execute(WNCodeModule& _module, llvm::Value*& _retVal, WNScriptType structType, WNScriptLinkedList<WNExpression>::WNScriptLinkedListNode* _initializer, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) const;
    };

    struct GenerateArrayConstruction : public GenerateConstruction {
        virtual eWNTypeError Execute(WNCodeModule& _module, llvm::Value*& _retVal, WNScriptType structType, WNScriptLinkedList<WNExpression>::WNScriptLinkedListNode* _initializer, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) const;
    };


    struct GenerateStructCopyConstruction : public GenerateCopyConstruction {
        virtual eWNTypeError Execute(WNCodeModule& _module, llvm::Value*& _retVal, WNScriptType structType, llvm::Value* _copyValue, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) const;
    };

    struct GenerateArrayCopyConstruction : public GenerateCopyConstruction {
        virtual eWNTypeError Execute(WNCodeModule& _module, llvm::Value*& _retVal, WNScriptType structType, llvm::Value* _copyValue, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) const;
    };

    struct GenerateAssignment {
        virtual ~GenerateAssignment() {};
        virtual eWNTypeError Execute(WNCodeModule& _module, bool _declaration, WNScriptType _type, llvm::Value* value, llvm::Value* assignLocation, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) const = 0;
    };
    struct GenerateDefaultAssignment : public GenerateAssignment {
        virtual eWNTypeError Execute(WNCodeModule& _module, bool _declaration, WNScriptType _type, llvm::Value* value, llvm::Value* assignLocation, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) const;
    };
    struct GenerateStructAssignment: public GenerateAssignment {
        virtual eWNTypeError Execute(WNCodeModule& _module, bool _declaration, WNScriptType _type, llvm::Value* value, llvm::Value* assignLocation, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) const;
    };
    struct     GenerateStructTransfer: public GenerateAssignment {
        virtual eWNTypeError Execute(WNCodeModule& _module, bool _declaration, WNScriptType _type, llvm::Value* value, llvm::Value* assignLocation, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) const;
    };
    struct GenerateDestruction {
        virtual ~GenerateDestruction() {}
        virtual eWNTypeError Execute(WNCodeModule& _builder, WNScriptType _node, llvm::Value* _elementLocation, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) const = 0;
    };

    struct GenerateStructDestruction : public GenerateDestruction {
        virtual eWNTypeError Execute(WNCodeModule& _builder, WNScriptType _node, llvm::Value* _elementLocation, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) const;
    };

    struct GenerateCStructDestruction : public GenerateDestruction {
        virtual eWNTypeError Execute(WNCodeModule& _builder, WNScriptType _node, llvm::Value* _elementLocation, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) const;
    };

    struct GenerateArrayDestruction: public GenerateDestruction {
        virtual eWNTypeError Execute(WNCodeModule& _builder, WNScriptType _node, llvm::Value* _elementLocation, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) const;
    };

    class WNTypeManager {
    public:
        virtual eWNTypeError RegisterScalarType(const wn_char* name, const WNScriptingEngine* _engine, wn_float32 priority, WNScriptType& _outType, llvm::Type* _type, wn_size_t _size) = 0;
        virtual eWNTypeError RegisterStructType(const wn_char* name, const WNScriptingEngine* _engine, WNScriptType& _outType) = 0;
        virtual eWNTypeError RegisterCStruct(const WNScriptingEngine* _engine, const wn_char* _type, WNScriptType& _outType) = 0;
        virtual eWNTypeError RegisterAliasedStruct(const wn_char* name, WNScriptType copyType, WNScriptType& _outType) = 0;

        virtual eWNTypeError RegisterArithmeticOperator(WNArithmeticType _type, WNScriptType _operand1, WNScriptType _operand2,  GenerateArithmeticOperation* _operation) = 0;
        virtual eWNTypeError RegisterCastingOperator(WNScriptType _fromType, WNScriptType _toType, GenerateCastingOperation* operation) = 0;
        virtual eWNTypeError RegisterPreUnaryOperator(WNUnaryType _type, WNScriptType _operand, GeneratePreUnaryOperation* operation) = 0;
        virtual eWNTypeError RegisterPostUnaryOperator(WNPostUNType _type, WNScriptType _operand, GeneratePostUnaryOperation* operation) = 0;
        virtual eWNTypeError RegisterArrayAccessOperator(WNScriptType _operand1, WNScriptType _operand2, GenerateArrayAccessOperation* operation) = 0;
        virtual eWNTypeError RegisterIDAccessOperator(WNScriptType _operand1, GenerateIDAccessOperation* operation) = 0;
        virtual eWNTypeError RegisterConstantOperator(WNScriptType _operand1, GenerateConstantOperation* opeartion) = 0;
        virtual eWNTypeError RegisterAllocationOperator(WNScriptType _operand1, GenerateAllocation* operation) = 0;
        virtual eWNTypeError RegisterAssignmentOperator(WNScriptType _operand1, WNAssignType _assignType, GenerateAssignment* operation) = 0;
        virtual eWNTypeError RegisterDestructionOperator(WNScriptType _operand1, GenerateDestruction* operation) = 0;
        virtual eWNTypeError RegisterConstructionOperator(WNScriptType _type, GenerateConstruction* operation) = 0;
        virtual eWNTypeError RegisterCopyConstructionOperator(WNScriptType _type, GenerateCopyConstruction* operation) = 0;

        virtual const GenerateArithmeticOperation* GetArithmeticOperation(WNArithmeticType _arith, WNScriptType _lhs, WNScriptType _rhs) const = 0;
        virtual const GenerateCastingOperation* GetCastingOperation(WNScriptType _lhs, WNScriptType _rhs) const = 0;
        virtual const GeneratePreUnaryOperation* GetPreUnaryOperation(WNUnaryType _type, WNScriptType _lhs) const = 0;
        virtual const GeneratePostUnaryOperation* GetPostUnaryOperation(WNPostUNType _type, WNScriptType _operand) const = 0;
        virtual const GenerateArrayAccessOperation* GetArrayAccessOperation(WNScriptType _operand1, WNScriptType _operand2) const = 0;
        virtual const GenerateIDAccessOperation* GetIDAccessOperation(WNScriptType _type) const = 0;
        virtual const GenerateConstantOperation* GetConstantOperation(WNScriptType _type) const = 0;
        virtual const GenerateAllocation* GetAllocationOperation(WNScriptType _type) const = 0;
        virtual const GenerateAssignment* GetAssignmentOperation(WNScriptType _type, WNAssignType _assignType) const = 0;
        virtual const GenerateDestruction* GetDestructionOperation(WNScriptType _type) const = 0;
        virtual const GenerateConstruction* GetConstructionOperation(WNScriptType _type) const = 0;
        virtual const GenerateCopyConstruction* GetCopyConstructionOperation(WNScriptType _type) const = 0;

        virtual eWNTypeError get_type_by_name(const wn_char* name, WNScriptType& _outType) const = 0;
        virtual eWNTypeError get_array_of(WNScriptType& _type, WNScriptType& _outType) = 0;
        virtual eWNTypeError get_existing_array_of(WNScriptType& _type, WNScriptType& _outType) const = 0;
        virtual wn_void RemoveType(const WNScriptType _type) = 0;
    };

    wn_int32 get_virtual_function_index(WNTypeManager& manager, const wn_char* _functionName, std::vector<WNScriptType> mTypes, WNScriptType mLookupClass, bool allowCast = true);
}
#endif // __WN_SCRIPTING_TYPE_MANAGER_H__
