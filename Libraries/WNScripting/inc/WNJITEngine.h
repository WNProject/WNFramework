// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_SCRIPTING_JIT_ENGINE_H__
#define __WN_SCRIPTING_JIT_ENGINE_H__

#include "WNContainers/inc/WNDeque.h"
#include "WNContainers/inc/WNHashMap.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNScripting/inc/WNEngineFileManager.h"
#include "WNScripting/inc/WNEngine.h"
#include "WNCore/inc/WNUtility.h"

namespace WNLogging {
class WNLogger;
}

namespace llvm {
  class LLVMContext;
  class EngineBuilder;
  class ExecutionEngine;
  class Module;
}

namespace wn {
namespace scripting {

// All of the llvm data needed for a single compiled module.
class CompiledModule : public core::non_copyable {
public:
  CompiledModule();
  CompiledModule(CompiledModule&& _other);

  std::unique_ptr<llvm::ExecutionEngine> m_engine;
  llvm::Module* m_module;
};

// A specialization of engine that compiles WNScript files
// using LLVM during calls to parse_file.
class jit_engine : public engine {
 public:
   jit_engine(type_validator* _validator, memory::allocator* _allocator,
       file_manager* _manager, WNLogging::WNLog* _log);
   ~jit_engine();
   parse_error parse_file(const char* file) override;

 protected:
   void_func get_function(containers::string_view _name) const override {
     return m_pointers.find(_name)->second;
  }

private:
  CompiledModule& add_module(containers::string_view _file);
  file_manager* m_file_manager;
  WNLogging::WNLog* m_compilation_log;
  std::unique_ptr<llvm::LLVMContext> m_context;
  containers::deque<CompiledModule> m_modules;
  containers::hash_map<containers::string_view, void(*)()> m_pointers;
};
}  // namespace scripting
}  // namespace wn

#endif  //__WN_SCRIPTING_JIT_ENGINE_H__
