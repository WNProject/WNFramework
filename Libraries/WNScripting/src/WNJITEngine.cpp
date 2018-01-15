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
#endif

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/Support/TargetSelect.h>

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
#include "WNMemory/inc/WNAllocator.h"
#include "WNMemory/inc/WNBasic.h"
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

// TODO(awoloszyn): Use our thread (fiber)-local
// allocator for this.

// Temp this is going to have to change.
void* do_allocate(size_t i) {
  void* t = memory::malloc(i);
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
  CustomMemoryManager(logging::log* _log) : m_log(_log) {}

  llvm::JITSymbol findSymbol(const std::string& Name) override {
    m_log->log_info("Resolving ", Name.c_str(), ".");
    if (Name == "_ZN3wns9_allocateEPvN3wns4sizeE") {
      return llvm::JITSymbol(reinterpret_cast<uint64_t>(&do_allocate),
          llvm::JITSymbolFlags::Exported);
    } else if (Name == "_ZN3wns5_freeEvPv") {
      return llvm::JITSymbol(
          reinterpret_cast<uint64_t>(&do_free), llvm::JITSymbolFlags::Exported);
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
  logging::log* m_log;
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
  : engine(_allocator),
    m_file_mapping(_mapping),
    m_compilation_log(_log),
    m_context(memory::make_std_unique<llvm::LLVMContext>()),
    m_modules(_allocator),
    m_pointers(_allocator) {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();
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
  builder.setMCJITMemoryManager(
      memory::make_std_unique<CustomMemoryManager>(m_compilation_log));

  code_module.m_engine =
      std::unique_ptr<llvm::ExecutionEngine>(builder.create());
  return code_module;
}

parse_error jit_engine::parse_file(const char* _file) {
  file_system::result res;
  file_system::file_ptr file = m_file_mapping->open_file(_file, res);

  if (!file) {
    return parse_error::does_not_exist;
  }

  containers::array<uint32_t, 1> allocate_params = {
      static_cast<uint32_t>(type_classification::size_type)};
  containers::array<uint32_t, 1> free_params = {
      static_cast<uint32_t>(type_classification::void_ptr_type)};

  // We have a set of functions that we MUST expose to the scripting
  // engine.
  containers::array<external_function, 2> required_functions;
  required_functions[0] = {"_allocate",
      static_cast<uint32_t>(type_classification::void_ptr_type),
      allocate_params};
  required_functions[1] = {"_free",
      static_cast<uint32_t>(type_classification::void_type), free_params};

  memory::unique_ptr<ast_script_file> parsed_file = parse_script(m_allocator,
      _file, file->typed_range<char>(), required_functions, false,
      m_compilation_log, &m_num_warnings, &m_num_errors);

  if (parsed_file == nullptr) {
    return parse_error::parse_failed;
  }
  CompiledModule& module = add_module(_file);

  jit_compiler compiler(m_allocator, module.m_module);
  compiler.compile(parsed_file.get());

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

}  // namespace scripting
}  // namespace wn
