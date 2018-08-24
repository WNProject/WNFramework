// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

// Due to a bug in llvm, it must be included before windows.h

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4100)
#pragma warning(disable : 4127)
#pragma warning(disable : 4310)
#pragma warning(disable : 4244)
#pragma warning(disable : 4458)
#pragma warning(disable : 4800)
#pragma warning(disable : 4624)
#pragma warning(disable : 4267)
#pragma warning(disable : 4512)
#pragma warning(disable : 4310)
#pragma warning(disable : 4146)
#pragma warning(disable : 4141)
#pragma warning(disable : 4324)
#pragma warning(disable : 4456)
#endif

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/Support/TargetSelect.h>

#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>

#ifdef _MSC_VER
#undef PRIi64
#undef PRId64
#undef PRIo64
#undef PRIu64
#undef PRIx64
#undef PRIX64
#pragma warning(pop)
#endif

#include "WNContainers/inc/WNArray.h"
#include "WNContainers/inc/WNContiguousRange.h"
#include "WNContainers/inc/WNDynamicArray.h"
#include "WNContainers/inc/WNList.h"
#include "WNFileSystem/inc/WNMapping.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/allocator.h"
#include "WNMemory/inc/manipulation.h"
#include "WNScripting/inc/WNJITConfiguration.h"
#include "WNScripting/inc/WNJITEngine.h"
#include "WNScripting/inc/WNScriptHelpers.h"
#include "WNScripting/inc/ast_node_types.h"
#include "WNScripting/inc/jit_compiler.h"

#include <algorithm>
extern "C" {
#if defined(_WN_WINDOWS)
#if defined(_WN_64_BIT)
void __chkstk();
#else
void _chkstk();
#endif
#endif
}

namespace {
llvm::StringRef make_string_ref(const wn::containers::string_view& _view) {
  return llvm::StringRef(_view.data(), _view.length());
}

}  // anonymous namespace

namespace wn {
namespace scripting {

// This is the layout of our object.
struct object {
  std::atomic_size_t ref_count;
  void (*destructor)(void*);
};

// Temp this is going to have to change.
void* do_allocate(wn_size_t i) {
  void* t = memory::malloc(i.val);
  return t;
}

// Temp this is going to have to change.
void* do_allocate_array(wn_size_t i, wn_size_t _count) {
  // For now just return any-old malloc. We REALLY need to
  // start passing in alignment so we can handle this properly.

  void* t = memory::malloc(sizeof(uint32_t) * 2 + (i.val * _count.val));
  (*(uint32_t*)t) = static_cast<uint32_t>(_count.val);
  return t;
}

// Temp this is going to have to change.
void do_free(void* val) {
  if (val == nullptr) {
    return;
  }
  memory::free(val);
}

namespace {
class CustomMemoryManager : public llvm::SectionMemoryManager {
public:
  CustomMemoryManager(memory::allocator* _allocator, logging::log* _log,
      containers::hash_map<containers::string, void (*)(void)>*
          _imported_functions)
    : m_allocator(_allocator),
      m_log(_log),
      m_imported_functions(_imported_functions) {}

  llvm::JITSymbol findSymbol(const std::string& Name) override {
    m_log->log_info("Resolving ", Name.c_str(), ".");
    auto it = m_imported_functions->find(
        containers::string(m_allocator, Name.c_str()));
    if (it != m_imported_functions->end()) {
      return llvm::JITSymbol(
          static_cast<uint64_t>(reinterpret_cast<size_t>(it->second)),
          llvm::JITSymbolFlags::Exported);
    }

#if defined(_WN_WINDOWS)
#if defined(_WN_64_BIT)
    // On windows64, enable _chkstk. LLVM generates calls
    // to this.
    if (Name == "__chkstk") {
      return llvm::JITSymbol(reinterpret_cast<uint64_t>(&__chkstk),
          llvm::JITSymbolFlags::Exported);
    }
#else
    // On windows32, enable _chkstk. LLVM generates calls
    // to this.
    if (Name == "_chkstk") {
      return llvm::JITSymbol(
          reinterpret_cast<uint64_t>(&_chkstk), llvm::JITSymbolFlags::Exported);
    }
#endif
#endif
    return llvm::JITSymbol(0, llvm::JITSymbolFlags::Exported);
  }

private:
  memory::allocator* m_allocator;
  logging::log* m_log;
  containers::hash_map<containers::string, void (*)(void)>*
      m_imported_functions;
};
}  // namespace

CompiledModule::CompiledModule() : m_module(nullptr) {}

CompiledModule::CompiledModule(CompiledModule&& _other)
  : m_engine(core::move(_other.m_engine)),
    m_module(core::move(_other.m_module)) {
  _other.m_module = nullptr;
}

jit_engine::jit_engine(memory::allocator* _allocator,
    file_system::mapping* _mapping, logging::log* _log)
  : engine(_allocator, _log),
    m_file_mapping(_mapping),
    m_compilation_log(_log),
    m_context(memory::make_std_unique<llvm::LLVMContext>()),
    m_modules(_allocator),
    m_pointers(_allocator),
    m_c_pointers(_allocator),
    m_external_types(_allocator) {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();
  register_function("_allocate", &do_allocate);
  register_function("_free", &do_free);
  register_function("_allocate_runtime_array", &do_allocate_array);
  m_type_manager.finalize_builtins();
}

jit_engine::~jit_engine() {}

CompiledModule& jit_engine::add_module(containers::string_view _file) {
  m_modules.push_back(CompiledModule());

  std::unique_ptr<llvm::Module> module =
      memory::make_std_unique<llvm::Module>(make_string_ref(_file), *m_context);

  CompiledModule& code_module = m_modules.back();
  code_module.m_module = module.get();

#ifdef WN_LLVM_OVERRIDDEN_TRIPLE
  code_module.m_module->setTargetTriple(
      llvm::Triple::normalize(internal::llvm_triple));
#endif

  llvm::EngineBuilder builder(core::move(module));
  builder.setEngineKind(llvm::EngineKind::JIT);
  builder.setMCJITMemoryManager(memory::make_std_unique<CustomMemoryManager>(
      m_allocator, m_compilation_log, &m_c_pointers));

  code_module.m_engine =
      std::unique_ptr<llvm::ExecutionEngine>(builder.create());
  return code_module;
}

parse_error jit_engine::parse_file(const containers::string_view _file) {
  file_system::result res;
  file_system::file_ptr file = m_file_mapping->open_file(_file, res);

  if (!file) {
    m_compilation_log->log_error("Could not find file", _file);
    return parse_error::does_not_exist;
  }

  memory::unique_ptr<ast_script_file> parsed_file = parse_script(m_allocator,
      _file, file->typed_range<char>(), nullptr, &m_type_manager, false,
      m_compilation_log, &m_num_warnings, &m_num_errors);

  if (parsed_file == nullptr) {
    return parse_error::parse_failed;
  }
  CompiledModule& module = add_module(_file);

  jit_compiler compiler(m_allocator, module.m_module);
  compiler.compile(parsed_file.get());

  // TODO: figure out what optimizations to run eventually
  // auto FPM =
  //     llvm::make_unique<llvm::legacy::FunctionPassManager>(module.m_module);
  //
  // // Add some optimizations.
  // FPM->add(llvm::createPromoteMemoryToRegisterPass());
  // FPM->add(llvm::createInstructionCombiningPass());
  // FPM->add(llvm::createReassociatePass());
  // FPM->add(llvm::createConstantPropagationPass());
  // FPM->add(llvm::createGVNPass());
  // FPM->add(llvm::createCFGSimplificationPass());
  // FPM->doInitialization();
  // for (auto& F : *module.m_module)
  //   FPM->run(F);

  // Uncomment to get debug information about the module out.
  // It is not really needed, but a good place to debug.
  // module.m_module->dump();
  module.m_engine->finalizeObject();
  for (auto& function : module.m_module->getFunctionList()) {
    llvm::StringRef name = function.getName();
    if (!function.isIntrinsic()) {
      m_pointers[containers::string_view(name.data(), name.size())] =
          reinterpret_cast<void (*)()>(
              module.m_engine->getPointerToFunction(&function));
    }
  }

  return parse_error::ok;
}

bool jit_engine::register_c_function(containers::string_view _name,
    containers::contiguous_range<const ast_type*> _types, void_f _function) {
  containers::string s = get_mangled_name(m_allocator, _name);
  for (auto& param : _types) {
    s += param->m_mangled_name;
  }
#if defined(_WN_WINDOWS) && defined(_WN_X86) && !defined(_WN_64_BIT)
  s.insert(s.begin(), '_');
#endif

  m_c_pointers[core::move(s)] = _function;
  return true;
}

bool jit_engine::register_mangled_c_function(
    containers::string_view _name, void_f _function, bool) {
  auto name = _name.to_string(m_allocator);
#if defined(_WN_WINDOWS) && defined(_WN_X86) && !defined(_WN_64_BIT)
  name.insert(name.begin(), '_');
#endif
  m_c_pointers[core::move(name)] = _function;
  return true;
}

}  // namespace scripting
}  // namespace wn
