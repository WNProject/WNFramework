// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_SCRIPTING_SCRIPTING_ENGINE_IMPL_H__
#define __WN_SCRIPTING_SCRIPTING_ENGINE_IMPL_H__

#include <deque>
#include <list>
#include <vector>
#include "WNCore/inc/WNTypes.h"
#include "WNDeprecatedScripting/inc/WNScriptingEngine.h"
#include "WNDeprecatedScripting/inc/WNScriptingLog.h"
#include "WNDeprecatedScripting/inc/WNTypeManager.h"
#include "WNLogging/inc/WNConsoleLogger.h"

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
  virtual int32_t get_virtual_function_index(const char* _functionName,
      WNScriptType& _type, const std::vector<WNScriptType>& _params);
  virtual eWNTypeError get_function_pointer(const char* _file,
      const char* _functionName, WNScriptType& _retParam,
      const std::vector<WNScriptType>& _params, void*& _ptr);
  virtual eWNTypeError GetExistingFunctionPointer(const char* _file,
      const char* _functionName, WNScriptType& _retParam,
      const std::vector<WNScriptType>& _params, void*& _ptr) const;
  virtual WNScriptType get_type_by_name(const char* _typeName) const;
  virtual WNScriptType get_array_of(WNScriptType _type);
  virtual WNScriptType get_existing_array_of(WNScriptType _type) const;
  virtual eWNTypeError register_function(const char* _functionName,
      const WNScriptType _retParam, const std::vector<WNScriptType>& _params,
      void* _ptr);
  virtual eWNTypeError register_member_function(const char* _functionName,
      const WNScriptType _thisType, const WNScriptType _retParam,
      const std::vector<WNScriptType>& _params, void* _ptr);
  virtual eWNTypeError register_member(const char* _varName,
      const WNScriptType _thisType, const WNScriptType _varType,
      size_t _offset);
  virtual void* get_registered_function(const char* _functionName) const;
  virtual void set_internal_log_level(WNLogging::WNLogLevel);
  virtual void set_log_level(WNLogging::WNLogLevel);
  virtual void set_compilation_log(WNLogging::WNLog*);
  virtual eWNTypeError compile_file(const char* _file);
  virtual eWNTypeError register_external_type(
      const char* _typeName, void (*ptr)(void*));
  virtual eWNTypeError register_external_type(
      const char* _typeName, const char* _parentType, void (*ptr)(void*));
  virtual eWNTypeError construct_scripting_object(
      WNScriptType _type, void*& _retVal) const;
  virtual eWNTypeError construct_scripting_array(
      WNScriptType _type, size_t _size, void*& _retVal) const;

private:
  static void BufferFlushCallback(void* _context, const char* buffer,
      size_t bufferLength,
      const std::vector<WNLogging::WNLogColorElement>& colors);
  struct WNFunctionRegistry {
    const char* mFunctionName;
    const char* mRegisteredFunctionTag;
    WNScriptType mRetParam;
    std::vector<WNScriptType> mParams;
    void* functionPointer;
    WNScriptType mThisType;
  };
  static void* CompileLazyFile(
      void* _scriptingEngine, void* _requestedFunction);
  eWNTypeError compile_file(const char* _file, WNCodeModule*& _outModule);
  WNCodeModule* GetCompiledModule(const char* _file) const;
  eWNTypeError PreprocessFile(const char* _file, WNCodeModule*& _module,
      WNPreprocessedFile*& _outFile, std::list<WNStruct*>& _toBeLinked);
  eWNTypeError DumpIntoModule(WNCodeModule*& _module, bool currentFile,
      WNPreprocessedFile* _preprocessedFile,
      std::deque<WNPreprocessedFile*>& _nextToProcess);
  eWNTypeError DumpPreprocessedfile(
      WNCodeModule*& _module, WNPreprocessedFile* _preprocessedFile);
  eWNTypeError LinkStructs(
      WNCodeModule*& _module, std::list<WNStruct*>& _toBeLinked);
  WNScriptingLogCallback mErrorCallback;
  mutable WNLogging::WNConsoleLogger<> mConsoleLogger;
  mutable WNLogging::WNLog mInternalLogger;
  WNLogging::WNLogLevel mLogLevel;
  WNLogging::WNLog* mCompilationLog;
  std::vector<std::pair<char*, WNCodeModule*>> mFileList;
  std::vector<WNFunctionRegistry> mRegisteredFunctions;
  std::vector<WNPreprocessedFile*> mPreprocessedFiles;
  WNTypeManager* mTypeManager;
};
}
#endif  // __WN_SCRIPTING_SCRIPTING_ENGINE_H__
