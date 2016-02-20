// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

// clang-format on
#include "WNDeprecatedScripting/inc/WNIncludeLLVM.h"
#include "llvm/ADT/APInt.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/LinkAllPasses.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Vectorize.h"
#include "WNDeprecatedScripting/inc/WNEndIncludeLLVM.h"

// includes llvm so we need it up top
#include "WNDeprecatedScripting/inc/WNScriptingMemoryManager.h"
// clang-format off

#include "WNDeprecatedFileSystem/inc/WNFile.h"
#include "WNDeprecatedScripting/inc/WNBuiltinTypeInitialization.h"
#include "WNDeprecatedScripting/inc/WNCodeModule.h"
#include "WNDeprecatedScripting/inc/WNParameter.h"
#include "WNDeprecatedScripting/inc/WNScopedVariableList.h"
#include "WNDeprecatedScripting/inc/WNScriptingEngineImplDeprecated.h"
#include "WNDeprecatedScripting/inc/WNScriptingFactoryInternal.h"
#include "WNDeprecatedScripting/inc/WNScriptingInterop.h"
#include "WNDeprecatedScripting/src/WNScriptASTDeprecatedLexer.hpp"
#include "WNDeprecatedScripting/src/WNScriptASTDeprecatedParser.hpp"
#include "WNMemory/inc/WNBasic.h"
#include "WNMemory/inc/WNStringUtility.h"

#include <algorithm>
#include <vector>

using namespace WNScripting;

WNScriptingEngineImplDeprecated::WNScriptingEngineImplDeprecated()
  : mLogLevel(WNLogging::eWarning),
    mInternalLogger(&mConsoleLogger, WNLogging::eError, 1024, true),
    mCompilationLog(&mInternalLogger) {
  mInternalLogger.SetLogLevel(mLogLevel);
}

WNScriptingEngineImplDeprecated::~WNScriptingEngineImplDeprecated() {}

void* WNScriptingEngineImplDeprecated::CompileLazyFile(
    void* _scriptingEngine, void* _requestedFunction) {
  WNScriptingEngine* scriptingEngine =
      reinterpret_cast<WNScriptingEngine*>(_scriptingEngine);
  WNFunctionDefinition* def =
      reinterpret_cast<WNFunctionDefinition*>(_requestedFunction);
  void* funcPtr = nullptr;
  if (ok !=
      scriptingEngine->get_function_pointer(def->mContainedFile->mFileName,
          def->mName, def->mReturn, def->mTypes, funcPtr)) {
    return (nullptr);
  }
  return (funcPtr);
}

eWNTypeError WNScriptingEngineImplDeprecated::initialize() {
  mInternalLogger.Log(
      WNLogging::eInfo, 0, "WNScriptingEngine Initialization Started");
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();
  mTypeManager = WNScriptingFactoryInternal::CreateTypeManager();
  eWNTypeError err = ok;
  WNScriptType throwawayType;
  mTypeManager->RegisterScalarType(
      "-Function", this, 0, throwawayType, 0, sizeof(size_t));
  mTypeManager->RegisterScalarType("-Ptr", this, 0, throwawayType,
      llvm::Type::getInt8PtrTy(llvm::getGlobalContext()), sizeof(size_t));
  mTypeManager->RegisterScalarType("-Object", this, 0, throwawayType,
      llvm::Type::getInt8PtrTy(llvm::getGlobalContext()), sizeof(size_t));
  mTypeManager->RegisterStructType("-Null", this, throwawayType);
  mTypeManager->RegisterScalarType("-Array", this, 0, throwawayType,
      llvm::Type::getInt8PtrTy(llvm::getGlobalContext()), sizeof(size_t));

  if ((err = WNBuiltIninitializer::InitializeIntTypes(this, *mTypeManager)) !=
      ok)
    return err;
  if ((err = WNBuiltIninitializer::InitializeFloatTypes(this, *mTypeManager)) !=
      ok)
    return err;
  if ((err = WNBuiltIninitializer::InitializeScriptingCasts(
           this, *mTypeManager)) != ok)
    return err;
  if ((err = WNBuiltIninitializer::InitializeFunctions(*this)) != ok)
    return err;

  WNScriptType nullType;
  WNScriptType voidType;
  WNScriptType intType;
  if (ok != mTypeManager->get_type_by_name("-Array", nullType) ||
      ok != mTypeManager->get_type_by_name("Void", voidType) ||
      ok != mTypeManager->get_type_by_name("Int", intType)) {
    return (eWNPlatformError);
  }
  std::vector<WNScriptType> params;
  params.push_back(nullType);
  params.push_back(intType);
  register_function(
      "$resizeArray", voidType, params, reinterpret_cast<void*>(&Resize));
  if ((err = register_c_function("CompileLazyFunction", CompileLazyFile)) != ok)
    return err;
  mInternalLogger.Log(
      WNLogging::eInfo, 0, "WNScriptingEngine Initialization Finished");
  return (ok);
}

WNCodeModule* WNScriptingEngineImplDeprecated::GetCompiledModule(
    const char* _file) const {
  char* c = wn::memory::strndup(_file, 1024);
  WNFileSystem::WNFile::CollapseFolderStructure(c);

  for (std::vector<std::pair<char*, WNCodeModule*>>::const_iterator i =
           mFileList.begin();
       i != mFileList.end(); ++i) {
    if (wn::memory::strcmp(c, i->first) == 0) {
      wn::memory::heap_free(c);
      return (i->second);
    }
  }
  wn::memory::heap_free(c);
  return (nullptr);
}

eWNTypeError WNScriptingEngineImplDeprecated::DumpIntoModule(
    WNCodeModule*& _module, bool mCurrentFile,
    WNPreprocessedFile* _preprocessedFile,
    std::deque<WNPreprocessedFile*>& _nextToProcess) {
  if (_preprocessedFile->mMarked) {
    return (ok);
  }

  for (std::vector<WNPreprocessedFile*>::iterator i =
           _preprocessedFile->mIncludedFiles.begin();
       i != _preprocessedFile->mIncludedFiles.end(); ++i) {
    _nextToProcess.push_back(*i);
  }

  for (std::vector<WNFunctionDefinition*>::iterator i =
           _preprocessedFile->mFunctionDefinitions.begin();
       i != _preprocessedFile->mFunctionDefinitions.end(); ++i) {
    if (mCurrentFile) {
      (*i)->mCurrentFile = true;
    } else {
      (*i)->mCurrentFile = false;
    }
    (*i)->mFunction = nullptr;
    eWNTypeError err = ok;
    WNFunctionDefinition* def;
    if (ok != (err = _module->AddExternalScriptFunction(*i, def))) {
      if (eWNAlreadyExists == err) {
        mCompilationLog->Log(WNLogging::eError, 0,
            "Error could not add function ", (*i)->mName,
            " because it would hide/be hidden by ", def->mName);
      } else {
        mCompilationLog->Log(
            WNLogging::eError, 0, "Error could not add function ", (*i)->mName);
      }
      return (err);
    }
  }

  _preprocessedFile->mMarked = true;
  return (ok);
}

eWNTypeError WNScriptingEngineImplDeprecated::DumpPreprocessedfile(
    WNCodeModule*& _module, WNPreprocessedFile* _preprocessedFile) {
  for (std::vector<WNPreprocessedFile*>::iterator i =
           mPreprocessedFiles.begin();
       i != mPreprocessedFiles.end(); ++i) {
    (*i)->mMarked = false;
  }

  std::deque<WNPreprocessedFile*> toProcess;

  eWNTypeError err = ok;

  if (ok !=
      (err = DumpIntoModule(_module, true, _preprocessedFile, toProcess))) {
    return (err);
  }
  for (std::vector<WNPreprocessedFile*>::iterator i =
           _preprocessedFile->mIncludedFiles.begin();
       i != _preprocessedFile->mIncludedFiles.end(); ++i) {
    toProcess.push_back(*i);
  }
  while (!toProcess.empty()) {
    if (ok !=
        (err = DumpIntoModule(_module, false, *toProcess.begin(), toProcess))) {
      return (err);
    }
    toProcess.pop_front();
  }
  return (ok);
}

eWNTypeError WNScriptingEngineImplDeprecated::LinkStructs(
    WNCodeModule*& _module, std::list<WNStruct*>& _toBeLinked) {
  size_t listSize = _toBeLinked.size();
  WNTypeManager& manager = _module->GetTypeManager();
  eWNTypeError err = ok;
  mCompilationLog->Log(WNLogging::eInfo, 0, "Starting type link ", listSize);
  while (listSize > 0) {
    for (std::list<WNStruct*>::iterator i = _toBeLinked.begin();
         i != _toBeLinked.end();) {  // intentionally not incrementing here (may
                                     // be removing)
      if (ok == (err = (*i)->FinalizeStruct(manager, *mCompilationLog))) {
        i = _toBeLinked.erase(i);
      } else if (err != eWNUnknownResolution) {
        std::vector<WNPreprocessedFile*> toRemoveFiles;
        std::vector<WNScriptType> toRemoveTypes;
        for (std::list<WNStruct*>::iterator link = _toBeLinked.begin();
             link != _toBeLinked.end(); ++link) {
          if ((*link)->GetType()->mContainedFile != nullptr) {
            for (std::vector<WNScriptType>::iterator j =
                     (*i)->GetType()->mContainedFile->mExposedTypes.begin();
                 j != (*link)->GetType()->mContainedFile->mExposedTypes.end();
                 ++j) {
              toRemoveTypes.push_back(*j);
            }
            toRemoveFiles.push_back((*link)->GetType()->mContainedFile);
          }
        }
        std::sort(toRemoveFiles.begin(), toRemoveFiles.end());
        std::sort(toRemoveTypes.begin(), toRemoveTypes.end());
        std::unique(toRemoveFiles.begin(), toRemoveFiles.end());
        std::unique(toRemoveTypes.begin(), toRemoveTypes.end());
        for (std::vector<WNScriptType>::iterator remove = toRemoveTypes.begin();
             remove != toRemoveTypes.end(); ++remove) {
          mTypeManager->RemoveType(*remove);
        }
        for (std::vector<WNPreprocessedFile*>::iterator remove =
                 toRemoveFiles.begin();
             remove != toRemoveFiles.end(); ++remove) {
          std::vector<WNPreprocessedFile*>::iterator toRemove = std::find(
              mPreprocessedFiles.begin(), mPreprocessedFiles.end(), *remove);
          if (toRemove != mPreprocessedFiles.end()) {
            mPreprocessedFiles.erase(toRemove);
          }

          wn::memory::destroy(*remove);
        }
        return (err);
      } else {
        ++i;
      }
    }

    if (listSize ==
        _toBeLinked.size()) {  // nothing has been removed, this is bad
      mCompilationLog->Log(WNLogging::eError, 0,
          "Error could not find valid resolution order for ", listSize,
          " class");
      for (std::list<WNStruct*>::iterator i = _toBeLinked.begin();
           i != _toBeLinked.end(); ++i) {
        mCompilationLog->Log(WNLogging::eError, 0, "Class ", (*i)->GetName());
      }
      return (eWNBadType);
    }
    listSize = _toBeLinked.size();
    mCompilationLog->Log(WNLogging::eDebug, 0, "After one iteration only ",
        listSize, " structs to be linked");
  }
  mCompilationLog->Log(WNLogging::eInfo, 0, "Completed type link ", listSize);
  return (ok);
}

eWNTypeError WNScriptingEngineImplDeprecated::PreprocessFile(const char* _file,
    WNCodeModule*& _module, WNPreprocessedFile*& _outFile,
    std::list<WNStruct*>& _toBeLinked) {
  char cBuff[2048];
  wn::memory::strncpy(cBuff, _file, 2047);
  WNFileSystem::WNFile::CollapseFolderStructure(cBuff);

  eWNTypeError err = ok;

  for (std::vector<WNPreprocessedFile*>::iterator i =
           mPreprocessedFiles.begin();
       i != mPreprocessedFiles.end(); ++i) {
    if (wn::memory::strncmp((*i)->mFileName, cBuff, 1024) == 0) {
      _outFile = *i;
      return (ok);
    }
  }
  mInternalLogger.Log(
      WNLogging::eInfo, 0, "WNScriptingEngine Preprocessing File: ", _file);
  WNFileSystem::WNFile f;
  if (WNFileSystem::WNFile::ok !=
      f.OpenFile(cBuff, WNFileSystem::WNFile::eWNFMRead)) {
    return (eWNInvalidFile);
  }
  char* c = wn::memory::strndup(cBuff, 1024);

  const char* buff = f.GetDataBuffer();
  WNScriptASTDeprecatedLexer::InputStreamType input(
      reinterpret_cast<const ANTLR_UINT8*>(buff), ANTLR_ENC_8BIT,
      static_cast<ANTLR_UINT32>(f.GetFileSize()),
      reinterpret_cast<ANTLR_UINT8*>(c));

  WNScriptASTDeprecatedLexer lexer(&input);
  WNScriptASTDeprecatedParser::TokenStreamType tStream(
      ANTLR_SIZE_HINT, lexer.get_tokSource());
  WNScriptASTDeprecatedParser parser(&tStream);

  WNScriptFile* scriptFile = parser.program();
  if (parser.getNumberOfSyntaxErrors() > 0 ||
      lexer.getNumberOfSyntaxErrors() > 0) {
    wn::memory::heap_free(c);
    return (error);
  }

  std::vector<WNScriptType> containedTypes;
  std::vector<WNFunctionDefinition*> containedFunctions;

  WNPreprocessedFile* preFile = wn::memory::construct<WNPreprocessedFile>();
  preFile->mFileName = c;

  if (ok != (err = scriptFile->PreProcess(*_module, preFile->mExposedTypes,
                 preFile->mFunctionDefinitions, *mCompilationLog))) {
    return (err);
  }
  mPreprocessedFiles.push_back(preFile);
  _outFile = preFile;
  char* folderName = WNFileSystem::WNFile::GetFolderName(cBuff);
  for (const WNScriptLinkedList<char>::WNScriptLinkedListNode* i =
           scriptFile->GetFirstInclude();
       i != nullptr; i = i->next) {
    char filename[2048];
    wn::memory::strncpy(
        filename, folderName, wn::memory::strlen(folderName) + 1);
    wn::memory::strncat(
        filename, "/", sizeof(filename) - wn::memory::strlen(filename) - 1);
    wn::memory::strncat(filename, i->value + 1,
        sizeof(filename) - wn::memory::strlen(filename) - 1);
    filename[strlen(filename) - 1] = '\0';
    WNFileSystem::WNFile::CollapseFolderStructure(filename);
    WNPreprocessedFile* includedFile;
    if (ok !=
        (err = PreprocessFile(filename, _module, includedFile, _toBeLinked))) {
      if (preFile) {
        for (std::vector<WNScriptType>::iterator type =
                 preFile->mExposedTypes.begin();
             type != preFile->mExposedTypes.end(); ++type) {
          mTypeManager->RemoveType(*type);
        }
      }
      std::vector<WNPreprocessedFile*>::iterator toRemove = std::find(
          mPreprocessedFiles.begin(), mPreprocessedFiles.end(), preFile);
      if (toRemove != mPreprocessedFiles.end()) {
        mPreprocessedFiles.erase(toRemove);
      }
      delete (preFile);
      _outFile = nullptr;
      return (err);
    }
    preFile->mIncludedFiles.push_back(includedFile);
  }

  if (ok !=
      (err = scriptFile->FinalizePreProcess(*_module,
           preFile->mFunctionDefinitions, _toBeLinked, *mCompilationLog))) {
    return (err);
  }

  for (std::vector<WNFunctionDefinition*>::iterator i =
           preFile->mFunctionDefinitions.begin();
       i != preFile->mFunctionDefinitions.end(); ++i) {
    (*i)->mContainedFile = preFile;
  }
  for (std::vector<WNScriptType>::iterator i = preFile->mExposedTypes.begin();
       i != preFile->mExposedTypes.end(); ++i) {
    (*i)->mContainedFile = preFile;
  }

  return (ok);
}

eWNTypeError WNScriptingEngineImplDeprecated::compile_file(const char* _file) {
  WNCodeModule* module;
  if (GetCompiledModule(_file)) {
    return (ok);
  }
  return (compile_file(_file, module));
}

eWNTypeError WNScriptingEngineImplDeprecated::compile_file(
    const char* _file, WNCodeModule*& _module) {
  if ((_module = GetCompiledModule(_file)) != nullptr) {
    return (ok);
  }
  WNFileSystem::WNFile f;
  if (WNFileSystem::WNFile::ok !=
      f.OpenFile(_file, WNFileSystem::WNFile::eWNFMRead)) {
    return (eWNInvalidFile);
  }
  mInternalLogger.Log(
      WNLogging::eInfo, 0, "WNScriptingEngine Started Compiling File: ", _file);
  char* c = wn::memory::strndup(_file, 1024);
  WNFileSystem::WNFile::CollapseFolderStructure(c);

  const char* buff = f.GetDataBuffer();
  WNScriptASTDeprecatedLexer::InputStreamType input(
      reinterpret_cast<const ANTLR_UINT8*>(buff), ANTLR_ENC_8BIT,
      static_cast<ANTLR_UINT32>(f.GetFileSize()),
      reinterpret_cast<ANTLR_UINT8*>(c));

  WNScriptASTDeprecatedLexer lexer(&input);
  WNScriptASTDeprecatedParser::TokenStreamType tStream(
      ANTLR_SIZE_HINT, lexer.get_tokSource());
  WNScriptASTDeprecatedParser parser(&tStream);

  WNScriptFile* scriptFile = parser.program();
  if (parser.getNumberOfSyntaxErrors() > 0 ||
      lexer.getNumberOfSyntaxErrors() > 0) {
    wn::memory::destroy(scriptFile);
    wn::memory::heap_free(c);
    return (error);
  }

  WNScopedVariableList* variableList =
      WNScriptingFactoryInternal::CreateScopedVariableList();
  _module =
      wn::memory::construct<WNCodeModule>(*mTypeManager, *variableList, this);
  if (ok != _module->initialize(eOptimized)) {
    wn::memory::destroy(scriptFile);
    wn::memory::heap_free(c);
    wn::memory::destroy(_module);
    return (error);
  }

  eWNTypeError err = ok;
  WNPreprocessedFile* preFile = nullptr;
  std::list<WNStruct*> unlinkedStructs;
  if (ok != (err = PreprocessFile(_file, _module, preFile, unlinkedStructs))) {
    if (preFile) {
      for (std::vector<WNScriptType>::iterator i =
               preFile->mExposedTypes.begin();
           i != preFile->mExposedTypes.end(); ++i) {
        mTypeManager->RemoveType(*i);
      }
    }
    return (err);
  }

  if (ok != (err = LinkStructs(_module, unlinkedStructs))) {
    return (err);
  }

  if (ok != (err = DumpPreprocessedfile(_module, preFile))) {
    if (preFile) {
      for (std::vector<WNScriptType>::iterator i =
               preFile->mExposedTypes.begin();
           i != preFile->mExposedTypes.end(); ++i) {
        mTypeManager->RemoveType(*i);
      }
    }
    return (err);
  }

  for (std::vector<WNFunctionRegistry>::const_iterator i =
           mRegisteredFunctions.begin();
       i != mRegisteredFunctions.end(); ++i) {
    if (ok != (err = _module->AddExternalDefinition(i->mFunctionName,
                   i->mRegisteredFunctionTag, i->mThisType, i->mParams,
                   i->mRetParam))) {
      if (preFile) {
        for (std::vector<WNScriptType>::iterator type =
                 preFile->mExposedTypes.begin();
             type != preFile->mExposedTypes.end(); ++type) {
          mTypeManager->RemoveType(*type);
        }
      }
      wn::memory::heap_free(c);
      return (err);
    }
  }

  if (ok != (err = scriptFile->DumpHeaders(*_module, *mCompilationLog))) {
    if (preFile) {
      for (std::vector<WNScriptType>::iterator i =
               preFile->mExposedTypes.begin();
           i != preFile->mExposedTypes.end(); ++i) {
        mTypeManager->RemoveType(*i);
      }
    }
    wn::memory::heap_free(c);
    wn::memory::destroy(_module);

    return (err);
  }

  if (ok != (err = scriptFile->GenerateCode(*_module, *mCompilationLog))) {
    if (preFile) {
      for (std::vector<WNScriptType>::iterator i =
               preFile->mExposedTypes.begin();
           i != preFile->mExposedTypes.end(); ++i) {
        mTypeManager->RemoveType(*i);
      }
    }
    wn::memory::heap_free(c);
    wn::memory::destroy(_module);
    return (err);
  }

  llvm::Module* module = _module->GetModule();
  llvm::legacy::FunctionPassManager mFPM(module);

  module->setDataLayout(*_module->GetExecutionEngine()->getDataLayout());

  mFPM.add(llvm::createBasicAliasAnalysisPass());
  mFPM.add(llvm::createConstantPropagationPass());
  mFPM.add(llvm::createPromoteMemoryToRegisterPass());
  mFPM.add(llvm::createInstructionCombiningPass());
  mFPM.add(llvm::createReassociatePass());
  mFPM.add(llvm::createGVNPass());
  mFPM.add(llvm::createCFGSimplificationPass());
  mFPM.add(llvm::createLoopSimplifyPass());

  mFPM.doInitialization();

  for (llvm::Module::iterator i = _module->GetModule()->begin();
       i != _module->GetModule()->end(); ++i) {
    mFPM.run(*i);
  }
  llvm::legacy::PassManager manager;
  manager.add(llvm::createFunctionAttrsPass());
  manager.add(llvm::createFunctionInliningPass());
  manager.run(*_module->GetModule());

  _module->GetExecutionEngine()->finalizeObject();

  wn::memory::destroy(variableList);
  wn::memory::destroy(scriptFile);
  mFileList.push_back(std::pair<char*, WNCodeModule*>(c, _module));

  for (size_t i = 0; i < preFile->mFunctionDefinitions.size(); ++i) {
    preFile->mFunctionDefinitions[i]->mFunctionPointer =
        _module->GetExecutionEngine()->getPointerToFunction(
            preFile->mFunctionDefinitions[i]->mFunction);
  }

  for (size_t i = 0; i < preFile->mExposedTypes.size(); ++i) {
    char exposedName[1024];
    exposedName[0] = '\0';
    wn::memory::strcat(exposedName, preFile->mExposedTypes[i]->mName);
    wn::memory::strcat(exposedName, "Destr");
    // Find all the destructors
    std::vector<WNFunctionDefinition*>::iterator func =
        std::find_if(preFile->mFunctionDefinitions.begin(),
            preFile->mFunctionDefinitions.end(),
            [&exposedName](WNFunctionDefinition* f) {
              return (wn::memory::strcmp(f->mName, exposedName) == 0);
            });
    if (func == preFile->mFunctionDefinitions.end()) {
      mInternalLogger.Log(WNLogging::eCritical, 0,
          "WNScriptingEngine Failed to find destructor that it would have "
          "generated in: ",
          _file);
    }
    reinterpret_cast<void**>(preFile->mExposedTypes[i]->mStoredVTable)[0] =
        (*func)->mFunctionPointer;
    reinterpret_cast<WNScriptType*>(
        preFile->mExposedTypes[i]->mStoredVTable)[1] =
        preFile->mExposedTypes[i];
    std::vector<WNContainedStructType>& st =
        preFile->mExposedTypes[i]->mStructTypes;
    const llvm::StructLayout* layout =
        _module->GetExecutionEngine()->getDataLayout()->getStructLayout(
            preFile->mExposedTypes[i]->mLLVMStructType);
    for (uint32_t j = 0; j < static_cast<uint32_t>(st.size()); ++j) {
      st[j].mCppOffset = static_cast<int32_t>(layout->getElementOffset(j));
    }
  }
  mInternalLogger.Log(WNLogging::eInfo, 0,
      "WNScriptingEngine Finished Compiling File: ", _file);
  return (ok);
}

int32_t WNScriptingEngineImplDeprecated::get_virtual_function_index(
    const char* _functionName, WNScriptType& _type,
    const std::vector<WNScriptType>& _params) {
  return (WNScripting::get_virtual_function_index(
      *mTypeManager, _functionName, _params, _type, false));
}

eWNTypeError WNScriptingEngineImplDeprecated::GetExistingFunctionPointer(
    const char* _file, const char* _functionName, WNScriptType& _retParam,
    const std::vector<WNScriptType>& _params, void*& _ptr) const {
  WNCodeModule* codeModule = nullptr;
  if ((codeModule = GetCompiledModule(_file)) == nullptr) {
    return (does_not_exist);
  }
  WNFunctionDefinition* f =
      codeModule->GetFunctionDefinition(_functionName, _params);
  if (!f) {
    mInternalLogger.Log(WNLogging::eWarning, 0, "Function ", _functionName,
        " does not exist in file", _file);
    return (does_not_exist);
  }
  WNScriptType mObjectType = get_type_by_name("-Object");

  if (f->mReturn->mLLVMStructType && _retParam == mObjectType) {
    _retParam = f->mReturn;
    _ptr = f->mFunctionPointer;
    return (ok);
  }
  if (!(f->mReturn == _retParam)) {
    mInternalLogger.Log(WNLogging::eWarning, 0, "Function ", _functionName,
        " does not exist in file", _file);
    return (does_not_exist);
  }

  _ptr = f->mFunctionPointer;

  return (ok);
}

eWNTypeError WNScriptingEngineImplDeprecated::get_function_pointer(
    const char* _file, const char* _functionName, WNScriptType& _retParam,
    const std::vector<WNScriptType>& _params, void*& _ptr) {
  WNCodeModule* codeModule = nullptr;
  eWNTypeError err = compile_file(_file, codeModule);
  if (err != ok) {
    printf("Could not compile file %s\n", _file);
    return (err);
  }

  WNFunctionDefinition* f =
      codeModule->GetFunctionDefinition(_functionName, _params);
  if (!f) {
    mInternalLogger.Log(WNLogging::eWarning, 0, "Function ", _functionName,
        " does not exist in file", _file);
    return (does_not_exist);
  }
  WNScriptType mObjectType = get_type_by_name("-Object");

  if (f->mReturn->mLLVMStructType && _retParam == mObjectType) {
    _retParam = f->mReturn;
    _ptr = f->mFunctionPointer;
    return (ok);
  }
  if (!(f->mReturn == _retParam)) {
    mInternalLogger.Log(WNLogging::eWarning, 0, "Function ", _functionName,
        " does not exist in file", _file);
    return (does_not_exist);
  }

  _ptr = f->mFunctionPointer;

  return (ok);
}

WNScriptType WNScriptingEngineImplDeprecated::get_type_by_name(
    const char* _typeName) const {
  if (!mTypeManager) {
    return (0);
  }
  WNScriptType t;
  if (ok != mTypeManager->get_type_by_name(_typeName, t)) {
    return (0);
  }
  return (t);
}

WNScriptType WNScriptingEngineImplDeprecated::get_array_of(WNScriptType _type) {
  if (!mTypeManager) {
    return (0);
  }
  WNScriptType t;
  if (ok != mTypeManager->get_array_of(_type, t)) {
    return (0);
  }
  return (t);
}

WNScriptType WNScriptingEngineImplDeprecated::get_existing_array_of(
    WNScriptType _type) const {
  if (!mTypeManager) {
    return (0);
  }
  WNScriptType t;
  if (ok != mTypeManager->get_existing_array_of(_type, t)) {
    return (0);
  }
  return (t);
}

eWNTypeError WNScriptingEngineImplDeprecated::register_function(
    const char* _functionName, const WNScriptType _retParam,
    const std::vector<WNScriptType>& _params, void* _ptr) {
  if (!mTypeManager) {
    mInternalLogger.Log(WNLogging::eCritical, 0,
        "type manager was not correctly created ", _functionName);
    return (error);
  }

  for (std::vector<WNFunctionRegistry>::const_iterator i =
           mRegisteredFunctions.begin();
       i != mRegisteredFunctions.end(); ++i) {
    if (wn::memory::strncmp(_functionName, i->mFunctionName, 256) == 0) {
      if (i->mParams.size() != _params.size()) {
        continue;
      }
      std::vector<WNScriptType>::const_iterator newIterator = _params.begin();
      std::vector<WNScriptType>::const_iterator oldIterator =
          i->mParams.begin();
      for (; newIterator != _params.end() && oldIterator != i->mParams.end();
           ++newIterator, ++oldIterator) {
        if (*newIterator != *oldIterator) {
          break;
        }
        if (!mTypeManager->GetCastingOperation(*newIterator, *oldIterator)) {
          if (!mTypeManager->GetCastingOperation(*oldIterator, *newIterator)) {
            break;
          }
        }
      }
      if (newIterator == _params.end() || oldIterator == i->mParams.end()) {
        mInternalLogger.Log(
            WNLogging::eWarning, 0, "Function already exists ", _functionName);
        return (eWNAlreadyExists);
      }
    }
  }
  mInternalLogger.Log(WNLogging::eDebug, 0,
      "WNScriptingEngine Function Registered: ", _functionName);
  size_t len = wn::memory::strlen(_functionName);
  char* functionName =
      wn::memory::heap_allocate<char>(len + 4 * _params.size() + 2);
  functionName[len + 4 * _params.size() + 1] = '\0';
  wn::memory::memcpy(functionName, _functionName, len);
  char* loc = functionName + len + 1;
  functionName[len] = '@';
  for (std::vector<WNScriptType>::const_iterator i = _params.begin();
       i != _params.end(); ++i) {
    wn::memory::memcpy(loc, (*i)->mTag, 4);
    loc += 4;
  }
  mRegisteredFunctions.push_back(WNFunctionRegistry());
  mRegisteredFunctions.back().functionPointer = _ptr;
  mRegisteredFunctions.back().mFunctionName =
      wn::memory::strndup(_functionName, 256);
  ;
  mRegisteredFunctions.back().mRegisteredFunctionTag = functionName;
  mRegisteredFunctions.back().mParams.assign(_params.begin(), _params.end());
  mRegisteredFunctions.back().mRetParam = _retParam;
  mRegisteredFunctions.back().mThisType = nullptr;
  return (ok);
}
eWNTypeError WNScriptingEngineImplDeprecated::register_member(
    const char* _varName, const WNScriptType _thisType,
    const WNScriptType _varType, size_t _offset) {
  bool err = false;
  std::vector<WNContainedStructType>::iterator i =
      std::find_if(_thisType->mStructTypes.begin(),
          _thisType->mStructTypes.end(), [_varName](WNContainedStructType& t) {
            return (wn::memory::strcmp(_varName, t.mName) == 0);
          });
  if (i != _thisType->mStructTypes.end()) {
    return (eWNAlreadyExists);
  }
  WNContainedStructType t = {wn::memory::strndup(_varName, 1024), _varType,
      static_cast<int32_t>(_offset)};
  _thisType->mStructTypes.push_back(t);
  return (ok);
}

eWNTypeError WNScriptingEngineImplDeprecated::register_member_function(
    const char* _functionName, const WNScriptType _thisType,
    const WNScriptType _retParam, const std::vector<WNScriptType>& _params,
    void* _ptr) {
  if (!mTypeManager) {
    mInternalLogger.Log(WNLogging::eCritical, 0,
        "type manager was not correctly created ", _functionName);
    return (error);
  }
  if (!_thisType->mCDestructor) {
    mCompilationLog->Log(
        WNLogging::eError, 0, "Error type is not a registered C++ type");
    return (invalid_parameters);
  }

  for (std::vector<WNFunctionRegistry>::const_iterator i =
           mRegisteredFunctions.begin();
       i != mRegisteredFunctions.end(); ++i) {
    if (!IsAncestor(i->mThisType, _thisType)) {
      continue;
    }
    const char* funcName = i->mFunctionName;
    funcName = strstr(funcName, "$");
    funcName = (funcName == nullptr) ? i->mFunctionName : funcName + 1;
    if (wn::memory::strncmp(_functionName, funcName, 256) == 0) {
      if (i->mParams.size() != _params.size()) {
        continue;
      }
      std::vector<WNScriptType>::const_iterator newIterator = _params.begin();
      std::vector<WNScriptType>::const_iterator oldIterator =
          i->mParams.begin();
      for (; newIterator != _params.end() && oldIterator != i->mParams.end();
           ++newIterator, ++oldIterator) {
        if (*newIterator != *oldIterator) {
          break;
        }
        if (!mTypeManager->GetCastingOperation(*newIterator, *oldIterator)) {
          if (!mTypeManager->GetCastingOperation(*oldIterator, *newIterator)) {
            break;
          }
        }
      }
      if (newIterator == _params.end() || oldIterator == i->mParams.end()) {
        mInternalLogger.Log(
            WNLogging::eWarning, 0, "Function already exists ", _functionName);
        return (eWNAlreadyExists);
      }
    }
  }

  size_t len = wn::memory::strlen(_functionName);
  size_t prefixLen = wn::memory::strlen(_thisType->mName);
  char* functionName = wn::memory::heap_allocate<char>(
      prefixLen + 1 + len + 4 * _params.size() + 2);
  char* exportedfunctionName =
      wn::memory::heap_allocate<char>(prefixLen + 1 + len + 1);
  functionName[prefixLen + 1 + len + 4 * _params.size() + 1] = '\0';
  wn::memory::memcpy(functionName, _thisType->mName, len);
  functionName[prefixLen] = '$';
  wn::memory::memcpy(functionName + prefixLen + 1, _functionName, len);
  functionName[prefixLen + 1 + len] =
      '\0';  // so we can print out the name so far
  wn::memory::memcpy(
      exportedfunctionName, functionName, prefixLen + 1 + len + 1);
  mInternalLogger.Log(WNLogging::eDebug, 0,
      "WNScriptingEngine Function Registered: ", functionName);
  char* loc = functionName + len + 1 + prefixLen + 1;
  functionName[len + prefixLen + 1] = '@';
  for (std::vector<WNScriptType>::const_iterator i = _params.begin();
       i != _params.end(); ++i) {
    wn::memory::memcpy(loc, (*i)->mTag, 4);
    loc += 4;
  }

  mRegisteredFunctions.push_back(WNFunctionRegistry());
  mRegisteredFunctions.back().functionPointer = _ptr;
  mRegisteredFunctions.back().mFunctionName = exportedfunctionName;
  mRegisteredFunctions.back().mRegisteredFunctionTag = functionName;
  mRegisteredFunctions.back().mParams.push_back(_thisType);
  mRegisteredFunctions.back().mParams.insert(
      mRegisteredFunctions.back().mParams.end(), _params.begin(),
      _params.end());
  mRegisteredFunctions.back().mRetParam = _retParam;
  mRegisteredFunctions.back().mThisType = _thisType;
  return (ok);
}

void* WNScriptingEngineImplDeprecated::get_registered_function(
    const char* _functionName) const {
  for (std::vector<WNFunctionRegistry>::const_iterator i =
           mRegisteredFunctions.begin();
       i != mRegisteredFunctions.end(); ++i) {
    if (wn::memory::strncmp(_functionName, i->mRegisteredFunctionTag, 1024) ==
        0) {
      return (i->functionPointer);
    }
  }
  mCompilationLog->Log(WNLogging::eInfo, 0,
      "WNScripting: No Explicit function: ", _functionName);
  return (nullptr);
}

void WNScriptingEngineImplDeprecated::set_internal_log_level(
    WNLogging::WNLogLevel _level) {
  mInternalLogger.SetLogLevel(_level);
  mLogLevel = _level;
}

void WNScriptingEngineImplDeprecated::set_log_level(
    WNLogging::WNLogLevel _level) {
  mCompilationLog->SetLogLevel(_level);
}

void WNScriptingEngineImplDeprecated::set_compilation_log(
    WNLogging::WNLog* _compilationLog) {
  if (!_compilationLog) {
    mCompilationLog = &mInternalLogger;
  } else {
    mCompilationLog = _compilationLog;
  }
}

void WNScriptingEngineImplDeprecated::BufferFlushCallback(void* _context,
    const char* buffer, size_t bufferLength,
    const std::vector<WNLogging::WNLogColorElement>& colors) {
  WNScriptingEngineImplDeprecated* impl =
      reinterpret_cast<WNScriptingEngineImplDeprecated*>(_context);
  if (!(impl->mErrorCallback)) {
    return;
  }

  impl->mErrorCallback(buffer, bufferLength, colors);
}

eWNTypeError WNScriptingEngineImplDeprecated::register_external_type(
    const char* _typeName, void (*ptr)(void*)) {
  WNScriptType outType;
  eWNTypeError err = ok;
  if (ok != (err = mTypeManager->RegisterCStruct(this, _typeName, outType))) {
    return (err);
  }
  WNScriptType nullType = NULL;
  if (ok != (err = mTypeManager->get_type_by_name("-Null", nullType))) {
    return (err);
  }
  WNScriptType boolType = NULL;
  if (ok != (err = mTypeManager->get_type_by_name("Bool", boolType))) {
    return (err);
  }
  WNScriptType functionType = NULL;
  if (ok != (err = mTypeManager->get_type_by_name("-Function", functionType))) {
    return (err);
  }
  outType->mCDestructor = ptr;
  mTypeManager->RegisterAllocationOperator(
      outType, wn::memory::construct<GenerateDefaultAllocation>());
  mTypeManager->RegisterAssignmentOperator(
      outType, AT_EQ, wn::memory::construct<GenerateStructAssignment>());
  mTypeManager->RegisterAssignmentOperator(
      outType, AT_CHOWN, wn::memory::construct<GenerateStructTransfer>());
  mTypeManager->RegisterDestructionOperator(
      outType, wn::memory::construct<GenerateCStructDestruction>());
  mTypeManager->RegisterCastingOperator(nullType, outType,
      wn::memory::construct<GenerateReinterpretCastOperation>(outType));
  mTypeManager->RegisterArithmeticOperator(AR_EQ, outType, outType,
      wn::memory::construct<GenerateStructCompare>(boolType, true));
  mTypeManager->RegisterArithmeticOperator(AR_NEQ, outType, outType,
      wn::memory::construct<GenerateStructCompare>(boolType, false));
  mTypeManager->RegisterIDAccessOperator(outType,
      wn::memory::construct<GenerateCPPStructAccessor>(outType, functionType));
  return (ok);
}

eWNTypeError WNScriptingEngineImplDeprecated::register_external_type(
    const char* _typeName, const char* _parentType, void (*ptr)(void*)) {
  WNScriptType outType;
  eWNTypeError err = ok;
  WNScriptType parentType;
  if (ok != (err = mTypeManager->get_type_by_name(_parentType, parentType))) {
    return (does_not_exist);
  }
  if (ok != (err = mTypeManager->RegisterCStruct(this, _typeName, outType))) {
    return (err);
  }
  WNScriptType nullType = NULL;
  if (ok != (err = mTypeManager->get_type_by_name("-Null", nullType))) {
    return (err);
  }
  WNScriptType boolType = NULL;
  if (ok != (err = mTypeManager->get_type_by_name("Bool", boolType))) {
    return (err);
  }
  WNScriptType functionType = NULL;
  if (ok != (err = mTypeManager->get_type_by_name("-Function", functionType))) {
    return (err);
  }
  outType->mCDestructor = ptr;
  outType->mParentClass = parentType;
  mTypeManager->RegisterAllocationOperator(
      outType, wn::memory::construct<GenerateDefaultAllocation>());
  mTypeManager->RegisterAssignmentOperator(
      outType, AT_EQ, wn::memory::construct<GenerateStructAssignment>());
  mTypeManager->RegisterAssignmentOperator(
      outType, AT_CHOWN, wn::memory::construct<GenerateStructTransfer>());
  mTypeManager->RegisterDestructionOperator(
      outType, wn::memory::construct<GenerateCStructDestruction>());
  mTypeManager->RegisterCastingOperator(nullType, outType,
      wn::memory::construct<GenerateReinterpretCastOperation>(outType));
  mTypeManager->RegisterArithmeticOperator(AR_EQ, outType, outType,
      wn::memory::construct<GenerateStructCompare>(boolType, true));
  mTypeManager->RegisterArithmeticOperator(AR_NEQ, outType, outType,
      wn::memory::construct<GenerateStructCompare>(boolType, false));
  mTypeManager->RegisterIDAccessOperator(outType,
      wn::memory::construct<GenerateCPPStructAccessor>(outType, functionType));
  while (parentType) {
    mTypeManager->RegisterCastingOperator(outType, parentType,
        wn::memory::construct<GenerateReinterpretCastOperation>(parentType));
    parentType = parentType->mParentClass;
  }
  return (ok);
}

// virtual eWNTypeError get_function_pointer(const char* _file, const char*
// _functionName, WNScriptType& _retParam, const std::vector<WNScriptType>&
// _params, void*& _ptr);

eWNTypeError WNScriptingEngineImplDeprecated::construct_scripting_object(
    WNScriptType _type, void*& _retVal) const {
  StructInternalType* tp = wn::memory::heap_allocate<StructInternalType>();
  wn::memory::memory_zero(tp);
  size_t size = 0;
  for (size_t i = 0; i < _type->mStructTypes.size(); ++i) {
    size += _type->mStructTypes[i].mType->mTypeSize;
  }
  void* sType = wn::memory::malloc(size);
  tp->structLoc = sType;
  void* fPtr;
  char strName[1024];
  size_t nameLen = wn::memory::strlen(_type->mName);
  wn::memory::memcpy(strName, _type->mName, nameLen);
  wn::memory::memcpy(strName + nameLen, "Const", 6);
  std::vector<WNScriptType> params;
  params.push_back(_type);
  WNScriptType retType = nullptr;
  eWNTypeError err = ok;
  if (nullptr == (retType = get_type_by_name("Void"))) {
    return (eWNPlatformError);
  }
  if (ok != (err = GetExistingFunctionPointer(_type->mContainedFile->mFileName,
                 strName, retType, params, fPtr))) {
    return (err);
  }
  void (*castedPtr)(void*) = reinterpret_cast<void (*)(void*)>(fPtr);
  castedPtr(tp);
  _retVal = tp;
  return (ok);
}

eWNTypeError WNScriptingEngineImplDeprecated::construct_scripting_array(
    WNScriptType _type, size_t _size, void*& _retVal) const {
  StructInternalType* tp = wn::memory::heap_allocate<StructInternalType>();
  wn::memory::memory_zero(tp);
  size_t typeSize = _type->mArrayType->mTypeSize;
  void* sType = wn::memory::malloc(sizeof(size_t) * 2 + (_size * typeSize));
  tp->structLoc = sType;
  *reinterpret_cast<size_t*>(sType) = _size;
  reinterpret_cast<void**>(sType)[1] = _type;
  if (_type->mArrayType->mArrayType || _type->mArrayType->mLLVMStructType) {
    void** items = reinterpret_cast<void**>(sType);
    for (size_t i = 0; i < _size; ++i) {
      StructInternalType* nt = wn::memory::heap_allocate<StructInternalType>();
      nt->owner = &(items[i + 2]);
      nt->refCount++;
      items[i + 2] = nt;
    }
  }
  _retVal = tp;
  return (ok);
}
