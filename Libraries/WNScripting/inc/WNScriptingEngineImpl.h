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
        virtual wn_int32 GetVirtualFunctionIndex(const wn_char* _functionName, WNScriptType& _type, const std::vector<WNScriptType>& _params);
        virtual eWNTypeError GetFunctionPointer(const wn_char* _file, const wn_char* _functionName, WNScriptType& _retParam, const std::vector<WNScriptType>& _params, void*& _ptr);
        virtual eWNTypeError GetExistingFunctionPointer(const wn_char* _file, const wn_char* _functionName, WNScriptType& _retParam, const std::vector<WNScriptType>& _params, void*& _ptr) const;
        virtual WNScriptType GetTypeByName(const wn_char* _typeName) const;
        virtual WNScriptType GetArrayOf(WNScriptType _type);
        virtual WNScriptType GetExistingArrayOf(WNScriptType _type) const;
        virtual eWNTypeError RegisterFunction(const wn_char* _functionName, const WNScriptType _retParam, const std::vector<WNScriptType>& _params, void* _ptr);
        virtual eWNTypeError RegisterMemberFunction(const wn_char* _functionName, const WNScriptType _thisType, const WNScriptType _retParam, const std::vector<WNScriptType>& _params, void* _ptr);
        virtual eWNTypeError RegisterMember(const wn_char* _varName, const WNScriptType _thisType, const WNScriptType _varType, wn_size_t _offset);
        virtual void* GetRegisteredFunction(const wn_char* _functionName) const;
        virtual void SetInternalLogLevel(WNLogging::WNLogLevel);
        virtual void SetLogLevel(WNLogging::WNLogLevel);
        virtual void SetCompilationLog(WNLogging::WNLog*);
        virtual eWNTypeError CompileFile(const wn_char* _file);
        virtual eWNTypeError RegisterExternalType(const wn_char* _typeName, void(*ptr)(void*));
        virtual eWNTypeError RegisterExternalType(const wn_char* _typeName, const wn_char* _parentType, void(*ptr)(void*));
        virtual eWNTypeError AddExternalLibs(eWNTypeError(*)(WNTypeManager&, void*), void*);
        virtual eWNTypeError ConstructScriptingObject(WNScriptType _type, wn_void*& _retVal) const;
        virtual eWNTypeError ConstructScriptingArray(WNScriptType _type, wn_size_t _size, wn_void*& _retVal) const;
    private:
        static wn_void BufferFlushCallback(wn_void* _context, const wn_char* buffer, wn_size_t bufferLength, const std::vector<WNLogging::WNLogColorElement>& colors);
        struct WNFunctionRegistry {
            const wn_char* mFunctionName;
            const wn_char* mRegisteredFunctionTag;
            WNScriptType mRetParam;
            std::vector<WNScriptType> mParams;
            void* functionPointer;
            WNScriptType mThisType;
        };
        static wn_void* CompileLazyFile(wn_void* _scriptingEngine, wn_void* _requestedFunction);
        eWNTypeError CompileFile(const wn_char* _file, WNCodeModule*& _outModule);
        WNCodeModule* GetCompiledModule(const wn_char* _file) const;
        eWNTypeError PreprocessFile(const wn_char* _file, WNCodeModule*& _module, WNPreprocessedFile*& _outFile, std::list<WNStruct*>& _toBeLinked);
        eWNTypeError DumpIntoModule(WNCodeModule*& _module, bool currentFile, WNPreprocessedFile* _preprocessedFile, std::deque<WNPreprocessedFile*>& _nextToProcess);
        eWNTypeError DumpPreprocessedfile(WNCodeModule*& _module, WNPreprocessedFile* _preprocessedFile);
        eWNTypeError LinkStructs(WNCodeModule*& _module, std::list<WNStruct*>& _toBeLinked);
        WNScriptingLogCallback mErrorCallback;
        mutable WNLogging::WNConsoleLogger<> mConsoleLogger;
        mutable WNLogging::WNLog mInternalLogger;
        WNLogging::WNLogLevel mLogLevel;
        WNLogging::WNLog* mCompilationLog;
        std::vector<std::pair<wn_char*, WNCodeModule*> > mFileList;
        std::vector<WNFunctionRegistry> mRegisteredFunctions;
        std::vector<WNPreprocessedFile*> mPreprocessedFiles;
        WNTypeManager* mTypeManager;
    };
}
#endif // __WN_SCRIPTING_SCRIPTING_ENGINE_H__
