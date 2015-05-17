// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_SCRIPTING_CODE_MODULE_H__
#define __WN_SCRIPTING_CODE_MODULE_H__

#include "WNCore/inc/WNTypes.h"
#include "WNScripting/inc/WNTypeElement.h"
#include "WNScripting/inc/WNScriptingErrors.h"
#include "WNScripting/inc/WNFunctionParam.h"

namespace llvm {
    class Function;
    class FunctionType;
    class ExecutionEngine;
    class Module;
    class IRBuilderBase;
};

namespace WNScripting {
    struct WNParameter;
    class WNTypeManager;
    class WNScopedVariableList;
    class WNScriptingMemoryManager;
    struct WNFunctionDefinition;
    class WNScriptingEngine;

    struct WNPreprocessedFile {
        const wn_char* mFileName;
        bool mMarked;
        std::vector<WNFunctionDefinition*> mFunctionDefinitions;
        std::vector<WNScriptType>          mExposedTypes;
        std::vector<WNPreprocessedFile*>   mIncludedFiles;
    };


    struct WNFunctionDefinition {
        WNFunctionDefinition():
            mName(wn_nullptr),
            mTag(wn_nullptr),
            mReturn(wn_nullptr),
            mThisType(wn_nullptr),
            mFunction(wn_nullptr),
            mFunctionType(wn_nullptr),
            mCurrentFile(false),
            mContainedFile(wn_nullptr),
            mFunctionPointer(wn_nullptr),
            mIsVirtual(false),
            mScriptingEngine(wn_nullptr)
        {
        }
        wn_char* mName;
        wn_char* mTag;
        WNScriptType mReturn;
        WNScriptType mThisType;
        std::vector<WNScriptType> mTypes;
        llvm::Function* mFunction;
        llvm::FunctionType* mFunctionType;
        bool mCurrentFile;
        WNPreprocessedFile* mContainedFile;
        void* mFunctionPointer;
        wn_bool mIsVirtual;
        WNScriptingEngine* mScriptingEngine;
    };

    enum ModuleFlags {
        eOptimized = 1 << 0,
        eGenerateDebugData = 1 << 1
     };
    class WNScriptingEngine;
    class WNCodeModule {
    public:
        WNCodeModule(WNTypeManager& _typeManager, WNScopedVariableList& _scopedVariableList, WNScriptingEngine* _scriptingEngine);
        eWNTypeError Initialize(wn_uint32 flags);
        llvm::ExecutionEngine* GetExecutionEngine();
        llvm::Module* GetModule();
        llvm::IRBuilderBase* GetBuilder();
        WNFunctionDefinition* GetFunctionDefinition(const wn_char* _name, const std::vector<WNParameter>& _params);
        WNFunctionDefinition* GetFunctionDefinition(const wn_char* _name, const std::vector<WNScriptType>& _params);
        eWNTypeError GetCastableFunctionDefinition(const wn_char* _name, const std::vector<WNScriptType>& _params, WNFunctionDefinition*& _definition);
        eWNTypeError GetCastableVirtualFunction(const wn_char* _name, const WNScriptType thisType, const std::vector<FunctionParam>& _params, wn_int32& _outIndex);
        eWNTypeError GetNonCastableVirtualFunction(const wn_char* _name, const WNScriptType thisType, const std::vector<WNScriptType>& _params, wn_int32& _outIndex);
        eWNTypeError AddFunctionDefinition(const wn_char* _name, const std::vector<WNScriptType>& _params, const WNScriptType& _return, WNFunctionDefinition*& _outFunctionDefinition, WNFunctionDefinition*& _equivalentFunction, WNScriptType _thisType = wn_nullptr, bool _virtual = false);
        eWNTypeError GenerateFunctionDefinition(const wn_char* _name, const std::vector<WNScriptType>& _params, const WNScriptType& _return, WNFunctionDefinition*& _outFunctionDefinition, WNScriptType _thisType = wn_nullptr, bool _virtual = false);
        eWNTypeError AddExternalDefinition(const wn_char* _name, const wn_char* _tag, const WNScriptType _thisType, const std::vector<WNScriptType>& _params, const WNScriptType& _return);
        eWNTypeError AddExternalScriptFunction(WNFunctionDefinition* functionDef,  WNFunctionDefinition*& _equivalentFunction);
        eWNTypeError GetThisCallFunction(const wn_char* _name, const WNScriptType thisType, const std::vector<FunctionParam>& _params, wn_int32& _outIndex, WNFunctionDefinition*& _outDefinition);
        bool NamedFunctionExists(const wn_char* _name) const;
        WNTypeManager& GetTypeManager() const;
        WNScopedVariableList& GetScopedVariableList();
        const WNScriptingEngine* GetScriptingEngine() const;
    private:
        WNFunctionDefinition* GetCallableEquivalent(WNFunctionDefinition* def) const;
        WNCodeModule(const WNCodeModule &other);
        WNCodeModule& operator=(WNCodeModule &other);
        llvm::ExecutionEngine* mEngine;
        llvm::Module* mModule;
        llvm::IRBuilderBase* mBuilder;
        WNTypeManager& mTypeManager;
        std::vector<WNFunctionDefinition*> mFunctions;
        WNScopedVariableList& mScopedVariableList;
        WNScriptingEngine* mScriptingEngine;
    };
}
#endif//__WN_SCRIPTING_CODE_MODULE_H__
