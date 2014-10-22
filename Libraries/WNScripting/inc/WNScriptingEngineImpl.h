// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_SCRIPTING_SCRIPTING_ENGINE_IMPL_H__
#define __WN_SCRIPTING_SCRIPTING_ENGINE_IMPL_H__

#include "WNCore/inc/WNTypes.h"
#include "WNScripting/inc/WNScriptingEngine.h"
#include "WNScripting/inc/WNTypeManager.h"
#include "WNScripting/inc/WNScriptingLog.h"
#include "WNLogging/inc/WNConsoleLogger.h"
#include <list>
#include <vector>
#include <deque>

namespace WNScripting {
    class WNCodeModule;
    class WNStruct;
    struct WNPreprocessedFile;
    class WNScriptingMemoryManager;
    class WNScriptingEngineImpl : public WNScriptingEngine {
    public:
        WNScriptingEngineImpl();
        virtual ~WNScriptingEngineImpl();
        virtual eWNTypeError Initialize();
        virtual WN_INT32 GetVirtualFunctionIndex(const WN_CHAR* _functionName, WNScriptType& _type, const std::vector<WNScriptType>& _params);
        virtual eWNTypeError GetFunctionPointer(const WN_CHAR* _file, const WN_CHAR* _functionName, WNScriptType& _retParam, const std::vector<WNScriptType>& _params, void*& _ptr);
        virtual eWNTypeError GetExistingFunctionPointer(const WN_CHAR* _file, const WN_CHAR* _functionName, WNScriptType& _retParam, const std::vector<WNScriptType>& _params, void*& _ptr) const;
        virtual WNScriptType GetTypeByName(const WN_CHAR* _typeName) const;
        virtual WNScriptType GetArrayOf(WNScriptType _type);
        virtual WNScriptType GetExistingArrayOf(WNScriptType _type) const;
        virtual eWNTypeError RegisterFunction(const WN_CHAR* _functionName, const WNScriptType _retParam, const std::vector<WNScriptType>& _params, void* _ptr);
        virtual eWNTypeError RegisterMemberFunction(const WN_CHAR* _functionName, const WNScriptType _thisType, const WNScriptType _retParam, const std::vector<WNScriptType>& _params, void* _ptr);
        virtual eWNTypeError RegisterMember(const WN_CHAR* _varName, const WNScriptType _thisType, const WNScriptType _varType, WN_SIZE_T _offset);
        virtual void* GetRegisteredFunction(const WN_CHAR* _functionName) const;
        virtual void SetInternalLogLevel(WNLogging::WNLogLevel);
        virtual void SetLogLevel(WNLogging::WNLogLevel);
        virtual void SetCompilationLog(WNLogging::WNLog*);
        virtual eWNTypeError CompileFile(const WN_CHAR* _file);
        virtual eWNTypeError RegisterExternalType(const WN_CHAR* _typeName, void(*ptr)(void*));
        virtual eWNTypeError RegisterExternalType(const WN_CHAR* _typeName, const WN_CHAR* _parentType, void(*ptr)(void*));
        virtual eWNTypeError AddExternalLibs(eWNTypeError(*)(WNTypeManager&, void*), void*);
        virtual eWNTypeError ConstructScriptingObject(WNScriptType _type, WN_VOID*& _retVal) const;
        virtual eWNTypeError ConstructScriptingArray(WNScriptType _type, WN_SIZE_T _size, WN_VOID*& _retVal) const;
    private:
        static WN_VOID BufferFlushCallback(WN_VOID* _context, const WN_CHAR* buffer, WN_SIZE_T bufferLength, const std::vector<WNLogging::WNLogColorElement>& colors);
        struct WNFunctionRegistry {
            const WN_CHAR* mFunctionName;
            const WN_CHAR* mRegisteredFunctionTag;
            WNScriptType mRetParam;
            std::vector<WNScriptType> mParams;
            void* functionPointer;
            WNScriptType mThisType;
        };
        static WN_VOID* CompileLazyFile(WN_VOID* _scriptingEngine, WN_VOID* _requestedFunction);
        eWNTypeError CompileFile(const WN_CHAR* _file, WNCodeModule*& _outModule);
        WNCodeModule* GetCompiledModule(const WN_CHAR* _file) const;
        eWNTypeError PreprocessFile(const WN_CHAR* _file, WNCodeModule*& _module, WNPreprocessedFile*& _outFile, std::list<WNStruct*>& _toBeLinked);
        eWNTypeError DumpIntoModule(WNCodeModule*& _module, bool currentFile, WNPreprocessedFile* _preprocessedFile, std::deque<WNPreprocessedFile*>& _nextToProcess);
        eWNTypeError DumpPreprocessedfile(WNCodeModule*& _module, WNPreprocessedFile* _preprocessedFile);
        eWNTypeError LinkStructs(WNCodeModule*& _module, std::list<WNStruct*>& _toBeLinked);
        WNScriptingLogCallback mErrorCallback;
        mutable WNLogging::WNConsoleLogger<> mConsoleLogger;
        mutable WNLogging::WNLog mInternalLogger;
        WNLogging::WNLogLevel mLogLevel;
        WNLogging::WNLog* mCompilationLog;
        std::vector<std::pair<WN_CHAR*, WNCodeModule*> > mFileList;
        std::vector<WNFunctionRegistry> mRegisteredFunctions;
        std::vector<WNPreprocessedFile*> mPreprocessedFiles;
        WNTypeManager* mTypeManager;
        WNScriptingMemoryManager* mMemoryManager;
    };
}
#endif // __WN_SCRIPTING_SCRIPTING_ENGINE_H__
