// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

// clang-format on
#include "WNDeprecatedScripting/inc/WNIncludeLLVM.h"
#include "llvm/ADT/Triple.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "WNDeprecatedScripting/inc/WNEndIncludeLLVM.h"
// clang-format off

#include "WNDeprecatedScripting/inc/WNScriptingMemoryManager.h"
#include "WNCore/inc/WNAlgorithm.h"
#include "WNDeprecatedScripting/inc/WNCodeModule.h"
#include "WNDeprecatedScripting/inc/WNParameter.h"
#include "WNDeprecatedScripting/inc/WNTypeManager.h"

#include <memory>

#ifndef __WN_HAS_CPP14_STD_MAKE_UNIQUE
namespace std {

template <typename T, typename... Args>
WN_FORCE_INLINE unique_ptr<T> make_unique(Args&&... args) {
  return (unique_ptr<T>(new T(forward<Args>(args)...)));
}
}
#endif

using namespace WNScripting;

WNCodeModule::WNCodeModule(WNTypeManager& _manager,
    WNScopedVariableList& _variableList, WNScriptingEngine* _scriptingEngine)
  : mTypeManager(_manager),
    mScopedVariableList(_variableList),
    mScriptingEngine(_scriptingEngine) {}

const WNScriptingEngine* WNCodeModule::GetScriptingEngine() const {
  return (mScriptingEngine);
}

WNTypeManager& WNCodeModule::GetTypeManager() const {
  return (mTypeManager);
}

WNScopedVariableList& WNCodeModule::GetScopedVariableList() {
  return (mScopedVariableList);
}

eWNTypeError WNCodeModule::initialize(uint32_t flags) {
  std::unique_ptr<llvm::Module> module =
      std::make_unique<llvm::Module>("MCJitModule", llvm::getGlobalContext());

#ifdef _WN_ANDROID
#ifdef _WN_ARM
  module->setTargetTriple(llvm::Triple::normalize("arm-linux-androideabi"));
#else
  module->setTargetTriple(llvm::Triple::normalize("x86-linux-androideabi"));
#endif
#elif defined(_WN_WINDOWS)
#ifdef _WN_64_BIT
  module->setTargetTriple(llvm::Triple::normalize("x86_64-pc-win32-elf"));
#else
  module->setTargetTriple(llvm::Triple::normalize("i686-pc-win32-elf"));
#endif
#endif  // otherwise let it figure itself out
  mModule = module.get();
  llvm::EngineBuilder builder(std::move(module));

  builder.setEngineKind(llvm::EngineKind::JIT);
  builder.setMCJITMemoryManager(
      std::make_unique<WNScriptingMemoryManager>(*mScriptingEngine));

  if (flags & eOptimized) {
    builder.setOptLevel(llvm::CodeGenOpt::Aggressive);
  } else {
    builder.setOptLevel(llvm::CodeGenOpt::None);
  }
  mEngine = builder.create();
  if (!mEngine) {
    mModule = nullptr;
    return (error);
  }
  mBuilder = wn::memory::construct<llvm::IRBuilder<>>(llvm::getGlobalContext());
  if (!mBuilder) {
    wn::memory::destroy(mEngine);
    mModule = nullptr;
    return (error);
  }
  return (ok);
}

llvm::ExecutionEngine* WNCodeModule::GetExecutionEngine() {
  return (mEngine);
}

llvm::Module* WNCodeModule::GetModule() {
  return (mModule);
}

llvm::IRBuilderBase* WNCodeModule::GetBuilder() {
  return (mBuilder);
}

WNFunctionDefinition* WNCodeModule::GetFunctionDefinition(
    const char* _name, const std::vector<WNParameter>& _params) {
  for (std::vector<WNFunctionDefinition*>::iterator i = mFunctions.begin();
       i != mFunctions.end(); ++i) {
    if (wn::memory::strcmp((*i)->mName, _name) == 0) {
      if (_params.size() != (*i)->mTypes.size()) {
        continue;
      }
      size_t j = 0;
      for (; j < _params.size(); ++j) {
        if (_params[j].mType != (*i)->mTypes[j]) {
          break;
        }
      }
      if (j == _params.size()) {
        return (*i);
      }
    }
  }
  return (nullptr);
}

WNFunctionDefinition* WNCodeModule::GetFunctionDefinition(
    const char* _name, const std::vector<WNScriptType>& _params) {
  for (std::vector<WNFunctionDefinition*>::iterator i = mFunctions.begin();
       i != mFunctions.end(); ++i) {
    if (wn::memory::strcmp((*i)->mName, _name) == 0) {
      if (_params.size() != (*i)->mTypes.size()) {
        continue;
      }
      size_t j = 0;
      for (; j < _params.size(); ++j) {
        if (_params[j] != (*i)->mTypes[j]) {
          break;
        }
      }
      if (j == _params.size()) {
        return (*i);
      }
    }
  }
  return (nullptr);
}

struct CountingIntHelper {
  CountingIntHelper(int32_t _def) : def(_def), numCasts(0) {}
  int numCasts;
  uint32_t def;
};

struct CountingStructHelper {
  CountingStructHelper(WNFunctionDefinition* _def) : def(_def), numCasts(0) {}
  int numCasts;
  WNFunctionDefinition* def;
};

eWNTypeError WNCodeModule::GetNonCastableVirtualFunction(const char* _name,
    const WNScriptType thisType, const std::vector<WNScriptType>& _params,
    int32_t& _outIndex) {
  if (thisType->mVTable.size() == 0) {
    return (does_not_exist);
  }
  std::vector<WNFunctionDefinition*>& functions = thisType->mVTable;
  std::vector<CountingIntHelper> sharedNameDefs;
  for (size_t i = 0; i < functions.size(); ++i) {
    const char* lookupFunc = wn::memory::strstr(functions[i]->mName, "$");
    lookupFunc = lookupFunc == NULL ? functions[i]->mName : lookupFunc + 1;
    if (wn::memory::strcmp(lookupFunc, _name) == 0) {
      if (functions[i]->mTypes.size() == _params.size() + 1) {
        sharedNameDefs.push_back(CountingIntHelper(static_cast<int32_t>(i)));
      }
    }
  }

  for (size_t i = 0; i < _params.size(); ++i) {
    for (std::vector<CountingIntHelper>::iterator vIt =
             sharedNameDefs.begin() + 1;
         vIt != sharedNameDefs.end();) {
      if (functions[vIt->def]->mTypes[i + 1] != _params[i]) {
        vIt = sharedNameDefs.erase(vIt);
        continue;
      }
      vIt++;
    }
  }

  if (sharedNameDefs.empty()) {
    return (error);
  }
  if (sharedNameDefs.size() > 1) {
    return (eWNAmbiguous);
  }
  _outIndex = sharedNameDefs.begin()->def;
  return (ok);
}

eWNTypeError WNCodeModule::GetThisCallFunction(const char* _name,
    WNScriptType thisType, const std::vector<FunctionParam>& _params,
    int32_t& _outIndex, WNFunctionDefinition*& _outDefinition) {
  int32_t outIndex = -1;

  std::vector<CountingStructHelper> sharedNameDefs;
  eWNTypeError virtualErr =
      GetCastableVirtualFunction(_name, thisType, _params, outIndex);

  char funcName[2048];
  while (thisType) {
    funcName[0] = '\0';
    if (_name[0] !=
        '$') {  // names starting with $ are special this means they are exposed
      // to scripting but have arbitrary base types
      wn::memory::strncat(funcName, thisType->mName, 1024);
      WN_DEBUG_ASSERT(
          wn::memory::strlen(funcName) == wn::memory::strlen(thisType->mName));
      wn::memory::strcat(funcName, "$");
    }
    wn::memory::strncat(funcName, _name, 1024);
    for (std::vector<WNFunctionDefinition*>::iterator i = mFunctions.begin();
         i != mFunctions.end(); ++i) {
      if (_name[0] != '$') {
        if ((*i)->mIsVirtual || !(*i)->mThisType) {
          continue;
        }
      }
      if (wn::memory::strcmp((*i)->mName, funcName) == 0) {
        if ((*i)->mTypes.size() == (_params.size() + 1)) {
          sharedNameDefs.push_back(CountingStructHelper(*i));
        }
      }
    }
    thisType = (_name[0] == '$' ? 0 : thisType->mParentClass);
  }

  for (size_t i = 0; i < _params.size(); ++i) {
    for (std::vector<CountingStructHelper>::iterator vIt =
             sharedNameDefs.begin();
         vIt != sharedNameDefs.end();) {
      if (vIt->def->mTypes[i + 1] !=
          _params[i].mType) {  // we do not current care about the base type, so
                               // start with i+1
        if (!mTypeManager.GetCastingOperation(
                _params[i].mType, vIt->def->mTypes[i + 1])) {
          vIt = sharedNameDefs.erase(vIt);
          continue;
        }
        vIt->numCasts++;
        vIt++;
        continue;
      }
      vIt++;
    }
  }

  if (sharedNameDefs.empty()) {
    _outIndex = outIndex;
    return (virtualErr);
  }

  WNFunctionDefinition* perfectFunction = nullptr;
  std::vector<CountingStructHelper>::iterator i =
      std::find_if(sharedNameDefs.begin(), sharedNameDefs.end(),
          [](CountingStructHelper& val) { return (val.numCasts == 0); });
  if (i != sharedNameDefs.end()) {
    perfectFunction = (*i).def;
  }
  if (ok == virtualErr) {  // if we found a perfect match above us, then its
                           // either ambiguous, or it was good
    if (perfectFunction) {
      return (eWNAmbiguous);
    } else {
      _outIndex = outIndex;
      return (eWNPartiallyAmbiguous);
    }
  }
  if (sharedNameDefs.size() > 1) {
    if (!perfectFunction) {
      return (eWNAmbiguous);
    }
    _outDefinition = perfectFunction;
    return (eWNPartiallyAmbiguous);
  }

  if (!perfectFunction &&
      eWNPartiallyAmbiguous == virtualErr) {  // If we do not have a "perfect
                                              // match" and neither does our
                                              // virtual
    // we would have to cast, ambiguous cast
    return (eWNAmbiguous);
  }
  _outDefinition =
      perfectFunction ? perfectFunction : sharedNameDefs.begin()->def;
  if (perfectFunction && eWNPartiallyAmbiguous == virtualErr) {
    return (eWNPartiallyAmbiguous);
  }
  return (perfectFunction ? ok : eWNPartiallyAmbiguous);
}

eWNTypeError WNCodeModule::GetCastableVirtualFunction(const char* _name,
    const WNScriptType thisType, const std::vector<FunctionParam>& _params,
    int32_t& _outIndex) {
  if (thisType->mVTable.size() == 0) {
    return (does_not_exist);
  }
  std::vector<WNFunctionDefinition*>& functions = thisType->mVTable;
  std::vector<CountingIntHelper> sharedNameDefs;
  for (size_t i = 0; i < functions.size(); ++i) {
    const char* lookupFunc = wn::memory::strstr(functions[i]->mName, "$");
    lookupFunc = lookupFunc == NULL ? functions[i]->mName : lookupFunc + 1;
    if (wn::memory::strcmp(lookupFunc, _name) == 0) {
      if (functions[i]->mTypes.size() == _params.size() + 1) {
        sharedNameDefs.push_back(CountingIntHelper(static_cast<int32_t>(i)));
      }
    }
  }

  for (size_t i = 0; i < _params.size(); ++i) {
    for (std::vector<CountingIntHelper>::iterator vIt = sharedNameDefs.begin();
         vIt != sharedNameDefs.end();) {
      if (functions[vIt->def]->mTypes[i + 1] != _params[i].mType) {
        if (!mTypeManager.GetCastingOperation(
                _params[i].mType, functions[vIt->def]->mTypes[i + 1])) {
          vIt = sharedNameDefs.erase(vIt);
          continue;
        }
        ++(vIt->numCasts);
        vIt++;
        continue;
      }
      vIt++;
    }
  }

  if (sharedNameDefs.empty()) {
    return (error);
  }
  int32_t perfectFunction = -1;
  std::vector<CountingIntHelper>::iterator i =
      std::find_if(sharedNameDefs.begin(), sharedNameDefs.end(),
          [](CountingIntHelper& val) { return (val.numCasts == 0); });
  if (i != sharedNameDefs.end()) {
    perfectFunction = (*i).def;
  }
  if (sharedNameDefs.size() > 1) {
    _outIndex = perfectFunction;
    if (-1 == perfectFunction) {
      return (eWNAmbiguous);
    } else {
      return (eWNPartiallyAmbiguous);
    }
  }

  _outIndex =
      (perfectFunction >= 0) ? perfectFunction : sharedNameDefs.begin()->def;

  return ((perfectFunction >= 0) ? ok : eWNPartiallyAmbiguous);
}

eWNTypeError WNCodeModule::GetCastableFunctionDefinition(const char* _name,
    const std::vector<WNScriptType>& _params,
    WNFunctionDefinition*& _outDefinition) {
  std::vector<CountingStructHelper> sharedNameDefs;

  for (std::vector<WNFunctionDefinition*>::iterator i = mFunctions.begin();
       i != mFunctions.end(); ++i) {
    if (wn::memory::strcmp((*i)->mName, _name) == 0) {
      if ((*i)->mTypes.size() == _params.size()) {
        sharedNameDefs.push_back(CountingStructHelper(*i));
      }
    }
  }

  for (size_t i = 0; i < _params.size(); ++i) {
    for (std::vector<CountingStructHelper>::iterator vIt =
             sharedNameDefs.begin();
         vIt != sharedNameDefs.end();) {
      if (vIt->def->mTypes[i] != _params[i]) {
        if (!mTypeManager.GetCastingOperation(
                _params[i], vIt->def->mTypes[i])) {
          vIt = sharedNameDefs.erase(vIt);
          continue;
        }
        vIt->numCasts++;
        vIt++;
        continue;
      }
      vIt++;
    }
  }

  if (sharedNameDefs.empty()) {
    return (error);
  }

  WNFunctionDefinition* perfectFunction = nullptr;
  std::vector<CountingStructHelper>::iterator i =
      std::find_if(sharedNameDefs.begin(), sharedNameDefs.end(),
          [](CountingStructHelper& val) { return (val.numCasts == 0); });
  if (i != sharedNameDefs.end()) {
    perfectFunction = (*i).def;
  }
  if (sharedNameDefs.size() > 1) {
    _outDefinition = perfectFunction;
    if (!perfectFunction) {
      return (eWNAmbiguous);
    } else {
      return (eWNPartiallyAmbiguous);
    }
  }

  _outDefinition =
      perfectFunction ? perfectFunction : sharedNameDefs.begin()->def;

  return (perfectFunction ? ok : eWNPartiallyAmbiguous);
}

eWNTypeError WNCodeModule::GenerateFunctionDefinition(const char* _name,
    const std::vector<WNScriptType>& _params, const WNScriptType& _return,
    WNFunctionDefinition*& _outFunctionDefinition, WNScriptType _thisType,
    bool _virtual) {
  _outFunctionDefinition = GetFunctionDefinition(_name, _params);
  if (_outFunctionDefinition) {
    return (eWNAlreadyExists);
  }
  WNFunctionDefinition* def = wn::memory::construct<WNFunctionDefinition>();
  def->mName = wn::memory::strndup(_name, 256);
  def->mReturn = _return;
  def->mTypes.assign(_params.begin(), _params.end());
  def->mFunction = nullptr;
  def->mFunctionType = nullptr;
  def->mFunctionPointer = nullptr;
  def->mThisType = _thisType;
  def->mIsVirtual = _virtual;
  def->mScriptingEngine = mScriptingEngine;
  _outFunctionDefinition = def;
  return (ok);
}

eWNTypeError WNCodeModule::AddFunctionDefinition(const char* _name,
    const std::vector<WNScriptType>& _params, const WNScriptType& _return,
    WNFunctionDefinition*& _outFunctionDefinition,
    WNFunctionDefinition*& _equivalentFunction, WNScriptType _thisType,
    bool _virtual) {
  eWNTypeError err = GenerateFunctionDefinition(
      _name, _params, _return, _outFunctionDefinition, _thisType, _virtual);
  if (ok != err) {
    return (err);
  }
  if (nullptr !=
      (_equivalentFunction = GetCallableEquivalent(_outFunctionDefinition))) {
    return (eWNAlreadyExists);
  }
  mFunctions.push_back(_outFunctionDefinition);
  return (ok);
}

eWNTypeError WNCodeModule::AddExternalScriptFunction(
    WNFunctionDefinition* _functionDefinition,
    WNFunctionDefinition*& _equivalentFunction) {
  if (nullptr !=
      (_equivalentFunction = GetCallableEquivalent(_functionDefinition))) {
    return (eWNAlreadyExists);
  }
  mFunctions.push_back(_functionDefinition);

  return (ok);
}

bool WNCodeModule::NamedFunctionExists(const char* _name) const {
  for (std::vector<WNFunctionDefinition*>::const_iterator i =
           mFunctions.begin();
       i != mFunctions.end(); ++i) {
    if (wn::memory::strcmp((*i)->mName, _name) == 0) {
      return (true);
    }
  }
  return (false);
}

eWNTypeError WNCodeModule::AddExternalDefinition(const char* _name,
    const char* _tag, WNScriptType _thisType,
    const std::vector<WNScriptType>& _params, const WNScriptType& _return) {
  if (GetFunctionDefinition(_name, _params)) {
    return (eWNAlreadyExists);
  }

  WNFunctionDefinition* def = wn::memory::construct<WNFunctionDefinition>();
  def->mName = wn::memory::strndup(_name, 256);
  def->mTag = wn::memory::strndup(_tag, 1024);
  def->mReturn = _return;
  def->mTypes.assign(_params.begin(), _params.end());
  def->mThisType = _thisType;
  std::vector<llvm::Type*> scriptTypes;
  for (std::vector<WNScriptType>::const_iterator i = _params.begin();
       i != _params.end(); ++i) {
    scriptTypes.push_back((*i)->mLLVMType);
  }

  llvm::FunctionType* fTy =
      llvm::FunctionType::get(_return->mLLVMType, scriptTypes, false);
  llvm::Function* func = llvm::Function::Create(
      fTy, llvm::GlobalValue::ExternalLinkage, def->mTag, mModule);
  def->mFunctionType = fTy;
  def->mFunction = func;
  mFunctions.push_back(def);
  return (ok);
}

WNFunctionDefinition* WNCodeModule::GetCallableEquivalent(
    WNFunctionDefinition* def) const {
  const char* _name = wn::memory::strstr(def->mName, "$");
  _name = _name == NULL ? def->mName : _name + 1;

  for (size_t i = 0; i < mFunctions.size(); ++i) {
    const char* lookupFunc = wn::memory::strstr(mFunctions[i]->mName, "$");
    lookupFunc = lookupFunc == NULL ? mFunctions[i]->mName : lookupFunc + 1;
    if (wn::memory::strcmp(lookupFunc, _name) == 0) {
      if (mFunctions[i]->mTypes.size() == def->mTypes.size()) {
        if ((mFunctions[i]->mThisType != 0) != (def->mThisType != 0)) {
          continue;  // if one is a this-func and the other is not
        }
        int adder = 0;
        if (mFunctions[i]->mThisType) {
          adder = 1;
        }
        if (!wn::core::equal(mFunctions[i]->mTypes.begin() + adder,
                mFunctions[i]->mTypes.end(), def->mTypes.begin() + adder,
                def->mTypes.end())) {
          continue;
        }
        if (mFunctions[i]->mIsVirtual &&
            def->mIsVirtual) {  // If they are both virtual then by definition
                                // they cannot hide each other
          continue;
        }
        if (IsAncestor(mFunctions[i]->mThisType, def->mThisType) ||
            IsAncestor(def->mThisType, mFunctions[i]->mThisType)) {
          return (mFunctions[i]);
        }
      }
    }
  }
  return (nullptr);
}
