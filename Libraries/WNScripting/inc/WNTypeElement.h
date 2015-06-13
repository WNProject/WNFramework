// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_SCRIPTING_TYPE_ELEMENT_H__
#define __WN_SCRIPTING_TYPE_ELEMENT_H__

#include "WNCore/inc/WNTypes.h"
#include "WNStrings/inc/WNStrings.h"
#include "WNMemory/inc/WNBasic.h"

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
        wn_char* mName;
        WNScriptTypeInternal* mType;
        wn_int32 mCppOffset;
    };

    struct WNScriptTypeInternal {
        wn_char* mName;
        wn_float32 mPriority;
        WNScriptTypeInternal* mArrayType;
        WNScriptTypeInternal* mArrayParent;
        WNScriptTypeInternal* mParentClass;
        const WNScriptingEngine* mScriptingEngine;
        std::vector<WNContainedStructType> mStructTypes;
        std::vector<WNFunctionDefinition*> mVTable;
        std::vector<WNFunctionDefinition*> mOverridenFunctions;
        void* mStoredVTable;
        wn_size_t mAlignment;
        wn_char mTag[4];
        llvm::Type* mLLVMType;
        llvm::StructType* mLLVMStructType;
        wn_bool mFinalizedStruct;
        WNPreprocessedFile* mContainedFile;
        wn_void (*mCDestructor)(wn_void*);
        wn_size_t mTypeSize;
        WNScriptTypeInternal(WNScriptTypeInternal* _internal, const wn_char* _name, const wn_char* _tag) :
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
            wn::memory::memcpy(mTag, _tag, 4*sizeof(wn_char));

        }
        WNScriptTypeInternal(const wn_char* _name, const WNScriptingEngine* _engine, wn_int32 _alignment, wn_float32 _priority, llvm::Type* _type,const wn_char* _tag, wn_size_t _typeSize) :
            mAlignment(_alignment),
            mPriority(_priority),
            mLLVMType(_type),
            mLLVMStructType(0),
            mParentClass(wn_nullptr),
            mArrayType(0),
            mArrayParent(0),
            mFinalizedStruct(wn_false),
            mContainedFile(wn_nullptr),
            mStoredVTable(wn_nullptr),
            mCDestructor(wn_nullptr),
            mTypeSize(_typeSize),
            mScriptingEngine(_engine){
            mName = WNStrings::WNStrNDup(_name, 256);
            wn::memory::memcpy(mTag, _tag, 4*sizeof(wn_char));

        }
        WNScriptTypeInternal(const wn_char* _name, const WNScriptingEngine* _engine, wn_int32 _alignment, wn_float32 _priority, llvm::Type* _type, llvm::StructType* _sType, const wn_char* _tag, wn_size_t _typeSize) :
            mAlignment(_alignment),
            mPriority(_priority),
            mLLVMStructType(_sType),
            mLLVMType(_type),
            mParentClass(wn_nullptr),
            mArrayType(0),
            mArrayParent(0),
            mFinalizedStruct(wn_false),
            mContainedFile(wn_nullptr),
            mStoredVTable(wn_nullptr),
            mCDestructor(wn_nullptr),
            mTypeSize(_typeSize),
            mScriptingEngine(_engine){
            mName = WNStrings::WNStrNDup(_name, 256);
            wn::memory::memcpy(mTag, _tag, 4*sizeof(wn_char));
        }
        WNScriptTypeInternal(WNScriptTypeInternal* _containedType, const WNScriptingEngine* _engine, llvm::Type* _llvmType, wn_float32 _priority, const wn_char* _tag, wn_size_t _typeSize) :
            mArrayType(_containedType),
            mName(wn_nullptr),
            mAlignment(_containedType->mAlignment),
            mPriority(_priority),
            mLLVMType(_llvmType),
            mLLVMStructType(0),
            mParentClass(wn_nullptr),
            mArrayParent(0),
            mFinalizedStruct(wn_false),
            mContainedFile(wn_nullptr),
            mStoredVTable(wn_nullptr),
            mCDestructor(wn_nullptr),
            mTypeSize(_typeSize),
            mScriptingEngine(_engine){
            _containedType->mArrayParent = this;
            wn::memory::memcpy(mTag, _tag, 4*sizeof(wn_char));
            wn_size_t mNameSize = WNStrings::WNStrLen(_containedType->mName);
            mName = wn::memory::heap_allocate<wn_char>(mNameSize + 1 /*NULL*/ + 2 /*"[]"*/);
            memcpy(mName, _containedType->mName, mNameSize);
            mName[mNameSize] = '[';
            mName[mNameSize+1] = ']';
            mName[mNameSize+2] = '\0';
        }
    };
    typedef WNScriptTypeInternal* WNScriptType;
    WN_INLINE wn_bool IsAncestor(WNScriptType _testType, WNScriptType _anc) {
        while(_testType) {
            if(_testType == _anc) {
                return(wn_true);
            }
            _testType = _testType->mParentClass;
        }
        return(wn_false);
    }

}
#endif // __WN_SCRIPTING_TYPE_ELEMENT_H__
