// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_SCRIPTING_TYPE_MANAGER_IMPL_H__
#define __WN_SCRIPTING_TYPE_MANAGER_IMPL_H__

#include "WNScripting/inc/WNTypeManager.h"
#include <list>
namespace WNScripting {
   class WNTypeManagerImpl : public WNTypeManager {
    public:
        WNTypeManagerImpl();
        virtual ~WNTypeManagerImpl();
        virtual eWNTypeError RegisterScalarType(const WN_CHAR* name, const WNScriptingEngine* _engine, WN_FLOAT32 priority, WNScriptType& _outType, llvm::Type* _type, WN_SIZE_T _size);
        virtual eWNTypeError RegisterStructType(const WN_CHAR* name, const WNScriptingEngine* _engine, WNScriptType& _outType);
        virtual eWNTypeError RegisterCStruct(const WNScriptingEngine* _engine, const WN_CHAR* _type, WNScriptType& _outType);
        virtual eWNTypeError RegisterAliasedStruct(const WN_CHAR* name, WNScriptType copyType, WNScriptType& _outType);
        virtual eWNTypeError RegisterArithmeticOperator(WNArithmeticType _type, WNScriptType _operand1, WNScriptType _operand2,  GenerateArithmeticOperation* _operation);
        virtual eWNTypeError RegisterCastingOperator(WNScriptType _fromType, WNScriptType _toType, GenerateCastingOperation* operation);
        virtual eWNTypeError RegisterPreUnaryOperator(WNUnaryType _type, WNScriptType _operand, GeneratePreUnaryOperation* operation);
        virtual eWNTypeError RegisterPostUnaryOperator(WNPostUNType _type, WNScriptType _operand, GeneratePostUnaryOperation* operation);
        virtual eWNTypeError RegisterArrayAccessOperator(WNScriptType _operand1, WNScriptType _operand2, GenerateArrayAccessOperation* operation);
        virtual eWNTypeError RegisterIDAccessOperator(WNScriptType _operand1, GenerateIDAccessOperation* operation);
        virtual eWNTypeError RegisterConstantOperator(WNScriptType _type, GenerateConstantOperation* operation);
        virtual eWNTypeError RegisterAllocationOperator(WNScriptType _type, GenerateAllocation* operation);
        virtual eWNTypeError RegisterAssignmentOperator(WNScriptType _type, WNAssignType _assignType, GenerateAssignment* operation);
        virtual eWNTypeError RegisterDestructionOperator(WNScriptType _type, GenerateDestruction* operation);
        virtual eWNTypeError RegisterConstructionOperator(WNScriptType _type, GenerateConstruction* operation);
        virtual eWNTypeError RegisterCopyConstructionOperator(WNScriptType _type, GenerateCopyConstruction* operation);
        virtual eWNTypeError GetTypeByName(const WN_CHAR* name, WNScriptType& _outType) const;
        virtual eWNTypeError GetArrayOf(WNScriptType& _type, WNScriptType& _outType);
        virtual eWNTypeError GetExistingArrayOf(WNScriptType& _type, WNScriptType& _outType) const;
        virtual const GenerateArithmeticOperation* GetArithmeticOperation(WNArithmeticType _arith, WNScriptType _lhs, WNScriptType _rhs) const;
        virtual const GenerateCastingOperation* GetCastingOperation(WNScriptType _lhs, WNScriptType _rhs) const;
        virtual const GeneratePreUnaryOperation* GetPreUnaryOperation(WNUnaryType _type, WNScriptType _lhs) const;
        virtual const GeneratePostUnaryOperation* GetPostUnaryOperation(WNPostUNType _type, WNScriptType _operand) const;
        virtual const GenerateArrayAccessOperation* GetArrayAccessOperation(WNScriptType _operand1, WNScriptType _operand2) const;
        virtual const GenerateIDAccessOperation* GetIDAccessOperation(WNScriptType _type) const;
        virtual const GenerateConstantOperation* GetConstantOperation(WNScriptType _type) const;
        virtual const GenerateAllocation* GetAllocationOperation(WNScriptType _type) const;
        virtual const GenerateAssignment* GetAssignmentOperation(WNScriptType _type, WNAssignType _assignType) const;
        virtual const GenerateDestruction* GetDestructionOperation(WNScriptType _type) const;
        virtual const GenerateConstruction* GetConstructionOperation(WNScriptType _type) const;
        virtual const GenerateCopyConstruction* GetCopyConstructionOperation(WNScriptType _type) const;


        virtual WN_VOID RemoveType(const WNScriptType _type);
   private:
       void IncrementTag();
       
       std::list<WNScriptType> mScriptTypes;
        struct GenerateArithmeticStruct {
            GenerateArithmeticStruct() : mOperation(WN_NULL) {}
            ~GenerateArithmeticStruct() {
                WN_DELETE(mOperation);
            }
            WNArithmeticType mArithmeticType;
            WNScriptType mType1;
            WNScriptType mType2;
            GenerateArithmeticOperation* mOperation;
        };
        struct GenerateCastingStruct {
            GenerateCastingStruct() : mOperation(WN_NULL) {}
            ~GenerateCastingStruct() {
                WN_DELETE(mOperation);
            }
            WNScriptType mType1;
            WNScriptType mType2;
            GenerateCastingOperation* mOperation;
        };
        struct GeneratePreUnaryStruct {
            GeneratePreUnaryStruct() : mOperation(WN_NULL) {}
            ~GeneratePreUnaryStruct() {
                WN_DELETE(mOperation);
            }
            WNUnaryType mUnaryType;
            WNScriptType mType;
            GeneratePreUnaryOperation* mOperation;
        };
        struct GeneratePostUnaryStruct {
            GeneratePostUnaryStruct() : mOperation(WN_NULL) {}
            ~GeneratePostUnaryStruct() {
                WN_DELETE(mOperation);
            }
            WNPostUNType mPostUnaryType;
            WNScriptType mType;
            GeneratePostUnaryOperation* mOperation;
        };
        struct GenerateArrayAccessStruct {
            GenerateArrayAccessStruct() : mOperation(WN_NULL) {}
            ~GenerateArrayAccessStruct() {
                WN_DELETE(mOperation);
            }
            WNScriptType mType1;
            WNScriptType mType2;
            GenerateArrayAccessOperation* mOperation;
        };
        struct GenerateIDAccessStruct {
            GenerateIDAccessStruct() : mOperation(WN_NULL) {}
            ~GenerateIDAccessStruct() {
                WN_DELETE(mOperation);
            }
            WNScriptType mType1;
            GenerateIDAccessOperation* mOperation;
        };
        struct GenerateConstantStruct {
            GenerateConstantStruct() : mOperation(WN_NULL) {}
            ~GenerateConstantStruct() {
                WN_DELETE(mOperation);
            }
            WNScriptType mType;
            GenerateConstantOperation* mOperation;
        };
        struct GenerateAllocationStruct {
            GenerateAllocationStruct() : mOperation(WN_NULL) {}
            ~GenerateAllocationStruct() {
                WN_DELETE(mOperation);
            }
            WNScriptType mType;
            GenerateAllocation* mOperation;
        };
        struct GenerateAssignmentStruct {
            GenerateAssignmentStruct() : mOperation(WN_NULL) {}
            ~GenerateAssignmentStruct() {
                WN_DELETE(mOperation);
            }
            WNScriptType mType;
            WNAssignType mAssignType;
            GenerateAssignment* mOperation;
        };
        struct GenerateDestructionStruct {
            GenerateDestructionStruct() : mOperation(WN_NULL) {}
            ~GenerateDestructionStruct() {
                WN_DELETE(mOperation);
            }
            WNScriptType mType;
            GenerateDestruction* mOperation;
        };
        struct GenerateConstructionStruct {
            GenerateConstructionStruct() : mOperation(WN_NULL) {}
            ~GenerateConstructionStruct() {
                WN_DELETE(mOperation);
            }
            WNScriptType mType;
            GenerateConstruction* mOperation;
        };
        struct GenerateCopyConstructionStruct{
            GenerateCopyConstructionStruct() : mOperation(WN_NULL) {}
            ~GenerateCopyConstructionStruct() {
                WN_DELETE(mOperation);
            }
            WNScriptType mType;
            GenerateCopyConstruction* mOperation;
        };
        std::list<GenerateArithmeticStruct> mArithmeticStructs;
        std::list<GenerateCastingStruct> mCastingStructs;
        std::list<GeneratePreUnaryStruct> mPreUnaryStructs;
        std::list<GeneratePostUnaryStruct> mPostUnaryStructs;
        std::list<GenerateArrayAccessStruct> mArrayAccessStructs;
        std::list<GenerateIDAccessStruct> mIDAccessStructs;
        std::list<GenerateConstantStruct> mConstantStructs;
        std::list<GenerateAllocationStruct> mAllocationStructs;
        std::list<GenerateAssignmentStruct> mAssignmentStructs;
        std::list<GenerateDestructionStruct> mDestructionStructs;
        std::list<GenerateConstructionStruct> mConstructionStructs;
        std::list<GenerateCopyConstructionStruct> mCopyConstructionStructs;
        WN_CHAR mNextTag[4];
    };
}
#endif // __WN_SCRIPTING_TYPE_MANAGER_IMPL_H__
