// Copyright (c) 2016, WNProject Authors. All rights reserved.
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

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNContainers/inc/WNArray.h"
#include "WNContainers/inc/WNDynamicArray.h"
#include "WNContainers/inc/WNList.h"
#include "WNFileSystem/inc/WNMapping.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNMemory/inc/WNBasic.h"
#include "WNScripting/inc/WNASTCodeGenerator.h"
#include "WNScripting/inc/WNASTWalker.h"
#include "WNScripting/inc/WNJITConfiguration.h"
#include "WNScripting/inc/WNJITEngine.h"
#include "WNScripting/inc/WNJITGenerator.h"
#include "WNScripting/inc/WNScriptHelpers.h"

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

llvm::StringRef make_string_ref(const wn::containers::string& _view) {
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
void* allocate_shared(size_t i, void(*_destructor)(void*)) {
  object* obj = static_cast<object*>(memory::malloc(sizeof(object) + i));
  obj->ref_count = 0;
  obj->destructor = _destructor;
  return (uint8_t*)(obj) + sizeof(object);
}

// Temp this is going to have to change.
void deref_shared(uint8_t* val) {
  if (val == nullptr) return;
  object* obj = (object*)(val - sizeof(object));
  if (obj->ref_count.fetch_sub(1) == 1) {
    obj->destructor(val);
    memory::free(obj);
  }
}

// Temp this is going to have to change.
void* assign_shared(uint8_t* a, uint8_t* b) {
  deref_shared(b);
  object* obj = (object*)(a - sizeof(object));
  obj->ref_count.fetch_add(1);
  return a;
}

// Temp this is going to have to change.
void* return_shared(uint8_t* a) {
  object* obj = (object*)(a - sizeof(object));
  obj->ref_count.fetch_sub(1);
  return a;
}

namespace {
  class CustomMemoryManager : public llvm::SectionMemoryManager {
  public:
    CustomMemoryManager(WNLogging::WNLog* _log): m_log(_log) {
    }

    llvm::RuntimeDyld::SymbolInfo findSymbol(const std::string &Name) override {
      m_log->Log(WNLogging::eInfo, 0, "Resolving ", Name.c_str(), ".");
      if (Name == "_Z3wns16_allocate_sharedEvpsfp") {
        return llvm::RuntimeDyld::SymbolInfo(
            reinterpret_cast<uint64_t>(&allocate_shared),
            llvm::JITSymbolFlags::Exported);
      } else if (Name == "_Z3wns13_deref_sharedEvvp") {
        return llvm::RuntimeDyld::SymbolInfo(
            reinterpret_cast<uint64_t>(&deref_shared),
            llvm::JITSymbolFlags::Exported);
      } else if (Name == "_Z3wns14_assign_sharedEvpvpvp") {
        return llvm::RuntimeDyld::SymbolInfo(
            reinterpret_cast<uint64_t>(&assign_shared),
            llvm::JITSymbolFlags::Exported);
      } else if (Name == "_Z3wns14_return_sharedEvpvp") {
        return llvm::RuntimeDyld::SymbolInfo(
            reinterpret_cast<uint64_t>(&return_shared),
            llvm::JITSymbolFlags::Exported);
      }
#if defined(_WN_WINDOWS)
#if defined(_WN_64_BIT)
      // On windows64, enable _chkstk. LLVM generates calls
      // to this.
      if (Name == "__chkstk") {
        return llvm::RuntimeDyld::SymbolInfo(
            reinterpret_cast<uint64_t>(&__chkstk),
            llvm::JITSymbolFlags::Exported);
      }
#else
      // On windows32, enable _chkstk. LLVM generates calls
      // to this.
      if (Name == "_chkstk") {
        return llvm::RuntimeDyld::SymbolInfo(
            reinterpret_cast<uint64_t>(&_chkstk),
            llvm::JITSymbolFlags::Exported);
      }
#endif
#endif
      return llvm::RuntimeDyld::SymbolInfo(0, llvm::JITSymbolFlags::Exported);
    }

  private:
    WNLogging::WNLog* m_log;
  };
}

CompiledModule::CompiledModule() : m_module(nullptr) {}

CompiledModule::CompiledModule(CompiledModule&& _other)
  : m_engine(core::move(_other.m_engine)), m_module(core::move(_other.m_module)) {
  _other.m_module = nullptr;
}

jit_engine::jit_engine(type_validator* _validator,
    memory::allocator* _allocator, file_system::mapping* _mapping,
    WNLogging::WNLog* _log)
  : engine(_validator, _allocator),
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

  containers::array<uint32_t, 2> allocate_shared_params = {
    static_cast<uint32_t>(type_classification::size_type),
    static_cast<uint32_t>(type_classification::function_ptr_type)
  };

  containers::array<uint32_t, 1> deref_shared_params = {
    static_cast<uint32_t>(type_classification::void_ptr_type)
  };
  containers::array<uint32_t, 2> assign_params = {
    static_cast<uint32_t>(type_classification::void_ptr_type),
    static_cast<uint32_t>(type_classification::void_ptr_type)
  };

  // We have a set of functions that we MUST expose to the scripting
  // engine.
  containers::array<external_function, 4> required_functions;
  required_functions[0] = {"_allocate_shared",
      static_cast<uint32_t>(type_classification::void_ptr_type),
      allocate_shared_params};
  required_functions[1] = {"_deref_shared",
      static_cast<uint32_t>(type_classification::void_type),
      deref_shared_params};
  required_functions[2] = {"_assign_shared",
      static_cast<uint32_t>(type_classification::void_ptr_type),
      assign_params};
  // _return_shared is a little bit special.
  // It dereferences a value, but does not delete the object if it
  // ends up being 0. This will get injected at return sites,
  // since we know that the results will always be incremented.
  required_functions[3] = {"_return_shared",
      static_cast<uint32_t>(type_classification::void_ptr_type),
      deref_shared_params};

  memory::unique_ptr<script_file> parsed_file = parse_script(m_allocator,
      m_validator, _file, required_functions, file->typed_range<char>(),
      m_compilation_log, &m_num_warnings, &m_num_errors);

  if (parsed_file == nullptr) {
    return parse_error::parse_failed;
  }
  CompiledModule& module = add_module(_file);

  ast_code_generator<ast_jit_traits> generator(m_allocator);
  ast_jit_engine engine(
      m_allocator, m_validator, m_context.get(), module.m_module, &generator);
  generator.set_generator(&engine);

  run_ast_pass<ast_code_generator<ast_jit_traits>>(
      &generator, static_cast<const script_file*>(parsed_file.get()));

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
