// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_SCRIPTING_SCRIPTING_ENGINE_IMPL_H__
#define __WN_SCRIPTING_SCRIPTING_ENGINE_IMPL_H__

#include "WNCore/inc/WNTypes.h"
#include "WNDeprecatedScripting/inc/WNScriptingEngine.h"
#include "WNDeprecatedScripting/inc/WNTypeManager.h"
#include "WNDeprecatedScripting/inc/WNScriptingLog.h"
#include "WNLogging/inc/WNConsoleLogger.h"
#include <list>
#include <vector>
#include <deque>

namespace WNScripting {
    class WNCodeModule;
    class WNStruct;
    struct WNPreprocessedFile;
    class WNScriptingMemoryManager;
    class WNScriptingEngineImplDeprecated : public WNScriptingEngine {
    public:
		WNScriptingEngineImplDeprecated();
        virtual ~WNScriptingEngineImplDeprecated();
        virtual eWNTypeError initialize();
        virtual wn_int32 get_virtual_function_index(const wn_char* _functionName, WNScriptType& _type, const std::vector<WNScriptType>& _params);
        virtual eWNTypeError get_function_pointer(const wn_char* _file, const wn_char* _functionName, WNScriptType& _retParam, const std::vector<WNScriptType>& _params, void*& _ptr);
        virtual eWNTypeError GetExistingFunctionPointer(const wn_char* _file, const wn_char* _functionName, WNScriptType& _retParam, const std::vector<WNScriptType>& _params, void*& _ptr) const;
        virtual WNScriptType get_type_by_name(const wn_char* _typeName) const;
        virtual WNScriptType get_array_of(WNScriptType _type);
        virtual WNScriptType get_existing_array_of(WNScriptType _type) const;
        virtual eWNTypeError register_function(const wn_char* _functionName, const WNScriptType _retParam, const std::vector<WNScriptType>& _params, void* _ptr);
        virtual eWNTypeError register_member_function(const wn_char* _functionName, const WNScriptType _thisType, const WNScriptType _retParam, const std::vector<WNScriptType>& _params, void* _ptr);
        virtual eWNTypeError register_member(const wn_char* _varName, const WNScriptType _thisType, const WNScriptType _varType, wn_size_t _offset);
        virtual void* get_registered_function(const wn_char* _functionName) const;
        virtual void set_internal_log_level(WNLogging::WNLogLevel);
        virtual void set_log_level(WNLogging::WNLogLevel);
        virtual void set_compilation_log(WNLogging::WNLog*);
        virtual eWNTypeError compile_file(const wn_char* _file);
        virtual eWNTypeError register_external_type(const wn_char* _typeName, void(*ptr)(void*));
        virtual eWNTypeError register_external_type(const wn_char* _typeName, const wn_char* _parentType, void(*ptr)(void*));
        virtual eWNTypeError construct_scripting_object(WNScriptType _type, wn_void*& _retVal) const;
        virtual eWNTypeError construct_scripting_array(WNScriptType _type, wn_size_t _size, wn_void*& _retVal) const;
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
        eWNTypeError compile_file(const wn_char* _file, WNCodeModule*& _outModule);
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
