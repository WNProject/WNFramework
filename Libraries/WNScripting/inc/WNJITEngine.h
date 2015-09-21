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

struct CompiledModule {
  std::unique_ptr<llvm::ExecutionEngine> m_engine;
  llvm::Module* m_module;
};

class jit_engine : public engine {
 public:
  jit_engine(memory::allocator* _allocator, file_manager* _manager,
             WNLogging::WNLog* _log);
  ~jit_engine();
  void_func get_function(containers::string_view _name) const override {
    return m_pointers.find(_name)->second;
  }
  parse_error parse_file(const char* file) override;
 private:
  CompiledModule& add_module(containers::string_view _file);
  memory::allocator* m_allocator;
  file_manager* m_file_manager;
  WNLogging::WNLog* m_compilation_log;
  std::unique_ptr<llvm::LLVMContext> m_context;
  containers::deque<CompiledModule> m_modules;
  containers::hash_map<containers::string_view, void(*)()> m_pointers;
};
}
}

#endif  //__WN_SCRIPTING_JIT_ENGINE_H__
