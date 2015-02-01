// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_SCRIPTING_TYPE_ELEMENT_H__
#define __WN_SCRIPTING_TYPE_ELEMENT_H__

#include "WNCore/inc/WNTypes.h"
#include "WNStrings/inc/WNStrings.h"
#include "WNMemory/inc/WNManipulation.h"
#include <vector>

namespace llvm {
    class Type;
    class StructType;
};

namespace WNScripting {
    enum WNTypeType {
        eWNNormalType,
        eWNStructType,
        eWNArrayType
    };

    struct WNScriptTypeInternal;
    struct WNFunctionDefinition;
    struct WNPreprocessedFile;
    class WNScriptingEngine;
    struct WNContainedStructType {
        WN_CHAR* mName;
        WNScriptTypeInternal* mType;
        WN_INT32 mCppOffset;
    };

    struct WNScriptTypeInternal {
        WN_CHAR* mName;
        WN_FLOAT32 mPriority;
        WNScriptTypeInternal* mArrayType;
        WNScriptTypeInternal* mArrayParent;
        WNScriptTypeInternal* mParentClass;
        const WNScriptingEngine* mScriptingEngine;
        std::vector<WNContainedStructType> mStructTypes;
        std::vector<WNFunctionDefinition*> mVTable;
        std::vector<WNFunctionDefinition*> mOverridenFunctions;
        void* mStoredVTable;
        WN_SIZE_T mAlignment;
        WN_CHAR mTag[4];
        llvm::Type* mLLVMType;
        llvm::StructType* mLLVMStructType;
        WN_BOOL mFinalizedStruct;
        WNPreprocessedFile* mContainedFile;
        WN_VOID (*mCDestructor)(WN_VOID*);
        WN_SIZE_T mTypeSize;
        WNScriptTypeInternal(WNScriptTypeInternal* _internal, const WN_CHAR* _name, const WN_CHAR* _tag) :
            mAlignment(_internal->mAlignment),
            mPriority(_internal->mPriority),
            mLLVMType(_internal->mLLVMType),
            mLLVMStructType(_internal->mLLVMStructType),
            mParentClass(_internal->mParentClass),
            mArrayType(_internal->mArrayType),
            mArrayParent(_internal->mArrayParent),
            mFinalizedStruct(_internal->mFinalizedStruct),
            mContainedFile(_internal->mContainedFile),
            mStoredVTable(_internal->mStoredVTable),
            mCDestructor(_internal->mCDestructor),
            mTypeSize(_internal->mTypeSize),
            mScriptingEngine(_internal->mScriptingEngine){
            mName = WNStrings::WNStrNDup(_name, 256);
            WNMemory::WNMemCpy(mTag, _tag, 4*sizeof(WN_CHAR));
            
        }
        WNScriptTypeInternal(const WN_CHAR* _name, const WNScriptingEngine* _engine, WN_INT32 _alignment, WN_FLOAT32 _priority, llvm::Type* _type,const WN_CHAR* _tag, WN_SIZE_T _typeSize) :
            mAlignment(_alignment),
            mPriority(_priority),
            mLLVMType(_type),
            mLLVMStructType(0),
            mParentClass(WN_NULL),
            mArrayType(0),
            mArrayParent(0),
            mFinalizedStruct(WN_FALSE),
            mContainedFile(WN_NULL),
            mStoredVTable(WN_NULL),
            mCDestructor(WN_NULL),
            mTypeSize(_typeSize),
            mScriptingEngine(_engine){
            mName = WNStrings::WNStrNDup(_name, 256);
            WNMemory::WNMemCpy(mTag, _tag, 4*sizeof(WN_CHAR));
            
        }
        WNScriptTypeInternal(const WN_CHAR* _name, const WNScriptingEngine* _engine, WN_INT32 _alignment, WN_FLOAT32 _priority, llvm::Type* _type, llvm::StructType* _sType, const WN_CHAR* _tag, WN_SIZE_T _typeSize) :
            mAlignment(_alignment),
            mPriority(_priority),
            mLLVMStructType(_sType),
            mLLVMType(_type),
            mParentClass(WN_NULL),
            mArrayType(0),
            mArrayParent(0),
            mFinalizedStruct(WN_FALSE),
            mContainedFile(WN_NULL),
            mStoredVTable(WN_NULL),
            mCDestructor(WN_NULL),
            mTypeSize(_typeSize),
            mScriptingEngine(_engine){
            mName = WNStrings::WNStrNDup(_name, 256);
            WNMemory::WNMemCpy(mTag, _tag, 4*sizeof(WN_CHAR));
        }
        WNScriptTypeInternal(WNScriptTypeInternal* _containedType, const WNScriptingEngine* _engine, llvm::Type* _llvmType, WN_FLOAT32 _priority, const WN_CHAR* _tag, WN_SIZE_T _typeSize) :
            mArrayType(_containedType),
            mName(WN_NULL),
            mAlignment(_containedType->mAlignment),
            mPriority(_priority),
            mLLVMType(_llvmType),
            mLLVMStructType(0),
            mParentClass(WN_NULL),
            mArrayParent(0),
            mFinalizedStruct(WN_FALSE),
            mContainedFile(WN_NULL),
            mStoredVTable(WN_NULL),
            mCDestructor(WN_NULL),
            mTypeSize(_typeSize),
            mScriptingEngine(_engine){
            _containedType->mArrayParent = this;
            WNMemory::WNMemCpy(mTag, _tag, 4*sizeof(WN_CHAR));
            WN_SIZE_T mNameSize = WNStrings::WNStrLen(_containedType->mName);
            mName = WNMemory::WNMallocT<WN_CHAR>(mNameSize + 1 /*NULL*/ + 2 /*"[]"*/);
            memcpy(mName, _containedType->mName, mNameSize);
            mName[mNameSize] = '[';
            mName[mNameSize+1] = ']';
            mName[mNameSize+2] = '\0';
        }
    };
    typedef WNScriptTypeInternal* WNScriptType;
    WN_INLINE WN_BOOL IsAncestor(WNScriptType _testType, WNScriptType _anc) {
        while(_testType) {
            if(_testType == _anc) {
                return(WN_TRUE);
            }
            _testType = _testType->mParentClass;
        }
        return(WN_FALSE);
    }
    
}
#endif // __WN_SCRIPTING_TYPE_ELEMENT_H__
