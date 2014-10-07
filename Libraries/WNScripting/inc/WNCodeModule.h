////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
        const WN_CHAR* mFileName;
        bool mMarked;
        std::vector<WNFunctionDefinition*> mFunctionDefinitions;
        std::vector<WNScriptType>          mExposedTypes;
        std::vector<WNPreprocessedFile*>   mIncludedFiles;
    };


    struct WNFunctionDefinition {
        WNFunctionDefinition(): 
            mName(WN_NULL),
            mTag(WN_NULL),
            mReturn(WN_NULL),
            mThisType(WN_NULL),
            mFunction(WN_NULL),
            mFunctionType(WN_NULL),
            mCurrentFile(false),
            mContainedFile(WN_NULL),
            mFunctionPointer(WN_NULL),
            mIsVirtual(false),
            mScriptingEngine(WN_NULL)
        {
        }
        WN_CHAR* mName;
        WN_CHAR* mTag;
        WNScriptType mReturn;
        WNScriptType mThisType;
        std::vector<WNScriptType> mTypes;
        llvm::Function* mFunction;
        llvm::FunctionType* mFunctionType;
        bool mCurrentFile;
        WNPreprocessedFile* mContainedFile;
        void* mFunctionPointer;
        WN_BOOL mIsVirtual;
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
        eWNTypeError Initialize(WN_UINT32 flags, WNScriptingMemoryManager& _manager);
        llvm::ExecutionEngine* GetExecutionEngine();
        llvm::Module* GetModule();
        llvm::IRBuilderBase* GetBuilder();
        WNFunctionDefinition* GetFunctionDefinition(const WN_CHAR* _name, const std::vector<WNParameter>& _params);
        WNFunctionDefinition* GetFunctionDefinition(const WN_CHAR* _name, const std::vector<WNScriptType>& _params);
        eWNTypeError GetCastableFunctionDefinition(const WN_CHAR* _name, const std::vector<WNScriptType>& _params, WNFunctionDefinition*& _definition);
        eWNTypeError GetCastableVirtualFunction(const WN_CHAR* _name, const WNScriptType thisType, const std::vector<FunctionParam>& _params, WN_INT32& _outIndex);
        eWNTypeError GetNonCastableVirtualFunction(const WN_CHAR* _name, const WNScriptType thisType, const std::vector<WNScriptType>& _params, WN_INT32& _outIndex);
        eWNTypeError AddFunctionDefinition(const WN_CHAR* _name, const std::vector<WNScriptType>& _params, const WNScriptType& _return, WNFunctionDefinition*& _outFunctionDefinition, WNFunctionDefinition*& _equivalentFunction, WNScriptType _thisType = WN_NULL, bool _virtual = false);
        eWNTypeError GenerateFunctionDefinition(const WN_CHAR* _name, const std::vector<WNScriptType>& _params, const WNScriptType& _return, WNFunctionDefinition*& _outFunctionDefinition, WNScriptType _thisType = WN_NULL, bool _virtual = false);
        eWNTypeError AddExternalDefinition(const WN_CHAR* _name, const WN_CHAR* _tag, const WNScriptType _thisType, const std::vector<WNScriptType>& _params, const WNScriptType& _return);
        eWNTypeError AddExternalScriptFunction(WNFunctionDefinition* functionDef,  WNFunctionDefinition*& _equivalentFunction);
        eWNTypeError GetThisCallFunction(const WN_CHAR* _name, const WNScriptType thisType, const std::vector<FunctionParam>& _params, WN_INT32& _outIndex, WNFunctionDefinition*& _outDefinition);
        bool NamedFunctionExists(const WN_CHAR* _name) const;
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
