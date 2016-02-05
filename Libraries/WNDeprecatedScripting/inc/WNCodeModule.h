// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_SCRIPTING_CODE_MODULE_H__
#define __WN_SCRIPTING_CODE_MODULE_H__

#include "WNCore/inc/WNTypes.h"
#include "WNDeprecatedScripting/inc/WNTypeElement.h"
#include "WNDeprecatedScripting/inc/WNScriptingErrors.h"
#include "WNDeprecatedScripting/inc/WNFunctionParam.h"

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
        const char* mFileName;
        bool mMarked;
        std::vector<WNFunctionDefinition*> mFunctionDefinitions;
        std::vector<WNScriptType>          mExposedTypes;
        std::vector<WNPreprocessedFile*>   mIncludedFiles;
    };


    struct WNFunctionDefinition {
        WNFunctionDefinition():
            mName(nullptr),
            mTag(nullptr),
            mReturn(nullptr),
            mThisType(nullptr),
            mFunction(nullptr),
            mFunctionType(nullptr),
            mCurrentFile(false),
            mContainedFile(nullptr),
            mFunctionPointer(nullptr),
            mIsVirtual(false),
            mScriptingEngine(nullptr)
        {
        }
        char* mName;
        char* mTag;
        WNScriptType mReturn;
        WNScriptType mThisType;
        std::vector<WNScriptType> mTypes;
        llvm::Function* mFunction;
        llvm::FunctionType* mFunctionType;
        bool mCurrentFile;
        WNPreprocessedFile* mContainedFile;
        void* mFunctionPointer;
        bool mIsVirtual;
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
        eWNTypeError initialize(uint32_t flags);
        llvm::ExecutionEngine* GetExecutionEngine();
        llvm::Module* GetModule();
        llvm::IRBuilderBase* GetBuilder();
        WNFunctionDefinition* GetFunctionDefinition(const char* _name, const std::vector<WNParameter>& _params);
        WNFunctionDefinition* GetFunctionDefinition(const char* _name, const std::vector<WNScriptType>& _params);
        eWNTypeError GetCastableFunctionDefinition(const char* _name, const std::vector<WNScriptType>& _params, WNFunctionDefinition*& _definition);
        eWNTypeError GetCastableVirtualFunction(const char* _name, const WNScriptType thisType, const std::vector<FunctionParam>& _params, int32_t& _outIndex);
        eWNTypeError GetNonCastableVirtualFunction(const char* _name, const WNScriptType thisType, const std::vector<WNScriptType>& _params, int32_t& _outIndex);
        eWNTypeError AddFunctionDefinition(const char* _name, const std::vector<WNScriptType>& _params, const WNScriptType& _return, WNFunctionDefinition*& _outFunctionDefinition, WNFunctionDefinition*& _equivalentFunction, WNScriptType _thisType = nullptr, bool _virtual = false);
        eWNTypeError GenerateFunctionDefinition(const char* _name, const std::vector<WNScriptType>& _params, const WNScriptType& _return, WNFunctionDefinition*& _outFunctionDefinition, WNScriptType _thisType = nullptr, bool _virtual = false);
        eWNTypeError AddExternalDefinition(const char* _name, const char* _tag, const WNScriptType _thisType, const std::vector<WNScriptType>& _params, const WNScriptType& _return);
        eWNTypeError AddExternalScriptFunction(WNFunctionDefinition* functionDef,  WNFunctionDefinition*& _equivalentFunction);
        eWNTypeError GetThisCallFunction(const char* _name, const WNScriptType thisType, const std::vector<FunctionParam>& _params, int32_t& _outIndex, WNFunctionDefinition*& _outDefinition);
        bool NamedFunctionExists(const char* _name) const;
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
