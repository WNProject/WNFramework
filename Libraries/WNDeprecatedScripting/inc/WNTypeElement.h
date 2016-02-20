// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_SCRIPTING_TYPE_ELEMENT_H__
#define __WN_SCRIPTING_TYPE_ELEMENT_H__

#include "WNCore/inc/WNTypes.h"
#include "WNMemory/inc/WNBasic.h"
#include "WNMemory/inc/WNStringUtility.h"

#include <vector>

namespace llvm {
class Type;
class StructType;
};

namespace WNScripting {
enum WNTypeType { eWNNormalType, eWNStructType, eWNArrayType };

struct WNScriptTypeInternal;
struct WNFunctionDefinition;
struct WNPreprocessedFile;
class WNScriptingEngine;
struct WNContainedStructType {
  char* mName;
  WNScriptTypeInternal* mType;
  int32_t mCppOffset;
};

struct WNScriptTypeInternal {
  char* mName;
  float mPriority;
  WNScriptTypeInternal* mArrayType;
  WNScriptTypeInternal* mArrayParent;
  WNScriptTypeInternal* mParentClass;
  const WNScriptingEngine* mScriptingEngine;
  std::vector<WNContainedStructType> mStructTypes;
  std::vector<WNFunctionDefinition*> mVTable;
  std::vector<WNFunctionDefinition*> mOverridenFunctions;
  void* mStoredVTable;
  size_t mAlignment;
  char mTag[4];
  llvm::Type* mLLVMType;
  llvm::StructType* mLLVMStructType;
  bool mFinalizedStruct;
  WNPreprocessedFile* mContainedFile;
  void (*mCDestructor)(void*);
  size_t mTypeSize;
  WNScriptTypeInternal(
      WNScriptTypeInternal* _internal, const char* _name, const char* _tag)
    : mAlignment(_internal->mAlignment),
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
      mScriptingEngine(_internal->mScriptingEngine) {
    mName = wn::memory::strndup(_name, 256);
    wn::memory::memcpy(mTag, _tag, 4 * sizeof(char));
  }
  WNScriptTypeInternal(const char* _name, const WNScriptingEngine* _engine,
      int32_t _alignment, float _priority, llvm::Type* _type, const char* _tag,
      size_t _typeSize)
    : mAlignment(_alignment),
      mPriority(_priority),
      mLLVMType(_type),
      mLLVMStructType(0),
      mParentClass(nullptr),
      mArrayType(0),
      mArrayParent(0),
      mFinalizedStruct(false),
      mContainedFile(nullptr),
      mStoredVTable(nullptr),
      mCDestructor(nullptr),
      mTypeSize(_typeSize),
      mScriptingEngine(_engine) {
    mName = wn::memory::strndup(_name, 256);
    wn::memory::memcpy(mTag, _tag, 4 * sizeof(char));
  }
  WNScriptTypeInternal(const char* _name, const WNScriptingEngine* _engine,
      int32_t _alignment, float _priority, llvm::Type* _type,
      llvm::StructType* _sType, const char* _tag, size_t _typeSize)
    : mAlignment(_alignment),
      mPriority(_priority),
      mLLVMStructType(_sType),
      mLLVMType(_type),
      mParentClass(nullptr),
      mArrayType(0),
      mArrayParent(0),
      mFinalizedStruct(false),
      mContainedFile(nullptr),
      mStoredVTable(nullptr),
      mCDestructor(nullptr),
      mTypeSize(_typeSize),
      mScriptingEngine(_engine) {
    mName = wn::memory::strndup(_name, 256);
    wn::memory::memcpy(mTag, _tag, 4 * sizeof(char));
  }
  WNScriptTypeInternal(WNScriptTypeInternal* _containedType,
      const WNScriptingEngine* _engine, llvm::Type* _llvmType, float _priority,
      const char* _tag, size_t _typeSize)
    : mArrayType(_containedType),
      mName(nullptr),
      mAlignment(_containedType->mAlignment),
      mPriority(_priority),
      mLLVMType(_llvmType),
      mLLVMStructType(0),
      mParentClass(nullptr),
      mArrayParent(0),
      mFinalizedStruct(false),
      mContainedFile(nullptr),
      mStoredVTable(nullptr),
      mCDestructor(nullptr),
      mTypeSize(_typeSize),
      mScriptingEngine(_engine) {
    _containedType->mArrayParent = this;
    wn::memory::memcpy(mTag, _tag, 4 * sizeof(char));
    size_t mNameSize = wn::memory::strlen(_containedType->mName);
    mName =
        wn::memory::heap_allocate<char>(mNameSize + 1 /*NULL*/ + 2 /*"[]"*/);
    memcpy(mName, _containedType->mName, mNameSize);
    mName[mNameSize] = '[';
    mName[mNameSize + 1] = ']';
    mName[mNameSize + 2] = '\0';
  }
};
typedef WNScriptTypeInternal* WNScriptType;
WN_INLINE bool IsAncestor(WNScriptType _testType, WNScriptType _anc) {
  while (_testType) {
    if (_testType == _anc) {
      return (true);
    }
    _testType = _testType->mParentClass;
  }
  return (false);
}
}
#endif  // __WN_SCRIPTING_TYPE_ELEMENT_H__
