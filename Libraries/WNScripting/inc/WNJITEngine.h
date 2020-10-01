// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_SCRIPTING_JIT_ENGINE_H__
#define __WN_SCRIPTING_JIT_ENGINE_H__

#include "WNContainers/inc/WNDeque.h"
#include "WNContainers/inc/WNHashMap.h"
#include "WNCore/inc/utilities.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/allocator.h"
#include "WNScripting/inc/WNEngine.h"
#include "WNScripting/inc/WNScriptHelpers.h"
#include "WNScripting/inc/ast_node_types.h"
#include "WNScripting/inc/type_manager.h"

namespace llvm {
class LLVMContext;
class EngineBuilder;
class ExecutionEngine;
class Module;
}  // namespace llvm

namespace wn {
namespace file_system {
class mapping;
}

namespace scripting {
namespace internal {
struct resource {
  const ast_type* type;
  scripting::resource_manager* resource;
};
}  // namespace internal
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
  jit_engine(memory::allocator* _allocator, file_system::mapping* _mapping,
      logging::log* _log);
  ~jit_engine();
  parse_error parse_file(const containers::string_view file) override;
  void free_shared(void* v) const override;

protected:
  void_f get_function_pointer(containers::string_view _name) const override {
    {
      auto it = m_pointers.find(_name);
      if (it != m_pointers.end()) {
        return it->second;
      }
    }
    auto it = m_c_pointers.find(_name.to_string(m_allocator));
    return it != m_c_pointers.end() ? it->second : nullptr;
  }

  bool register_c_function(containers::string_view name,
      containers::contiguous_range<const ast_type*> _types,
      void_f function) override;

  bool register_mangled_c_function(containers::string_view _name,
      void_f _function, bool _is_virtual) override;

  bool register_resource(
      resource_manager* _resource, const ast_type* _type) override;

  size_t get_vtable_offset(const ast_type* _t) override;
  void* allocate_shared(size_t size) override;

private:
  CompiledModule& add_module(containers::string_view _file);
  file_system::mapping* m_file_mapping;
  logging::log* m_compilation_log;
  std::unique_ptr<llvm::LLVMContext> m_context;
  containers::deque<CompiledModule> m_modules;
  containers::hash_map<containers::string_view, void_f> m_pointers;
  containers::hash_map<containers::string, void_f> m_c_pointers;

  containers::hash_map<containers::string, internal::resource> m_resources;
  containers::hash_map<containers::string, resource_manager*>
      m_extension_handlers;

  containers::hash_map<containers::string, memory::unique_ptr<ast_type>>
      m_external_types;

  containers::hash_map<const ast_type*, struct_info> m_struct_infos;

  containers::deque<containers::string_view> m_started_files;
  containers::hash_set<containers::string> m_finished_files;
};
}  // namespace scripting
}  // namespace wn

#endif  //__WN_SCRIPTING_JIT_ENGINE_H__
