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
#include "WNContainers/inc/WNDynamicArray.h"
#include "WNContainers/inc/WNList.h"
#include "WNFileSystem/inc/WNMapping.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNScripting/inc/WNASTCodeGenerator.h"
#include "WNScripting/inc/WNASTWalker.h"
#include "WNScripting/inc/WNJITEngine.h"
#include "WNScripting/inc/WNJITGenerator.h"
#include "WNScripting/inc/WNScriptHelpers.h"

#include <algorithm>

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

CompiledModule::CompiledModule() : m_module(nullptr) {}

CompiledModule::CompiledModule(CompiledModule&& _other)
  : m_engine(std::move(_other.m_engine)), m_module(std::move(_other.m_module)) {
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

#ifdef _WN_ANDROID
#ifdef _WN_ARM
  code_module.m_module->setTargetTriple(
      llvm::Triple::normalize("armv7a-linux-androideabi"));
#else
  code_module.m_module->setTargetTriple(
      llvm::Triple::normalize("i686-linux-androideabi"));
#endif
#elif defined(_WN_WINDOWS)
#ifdef _WN_64_BIT
  code_module.m_module->setTargetTriple(
      llvm::Triple::normalize("x86_64-pc-win32-elf"));
#else
  code_module.m_module->setTargetTriple(
      llvm::Triple::normalize("i686-pc-win32-elf"));
#endif
#endif  // otherwise let it figure itself out

  llvm::EngineBuilder builder(std::move(module));
  builder.setEngineKind(llvm::EngineKind::JIT);
  builder.setMCJITMemoryManager(
      memory::make_std_unique<llvm::SectionMemoryManager>());

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

  memory::unique_ptr<script_file> parsed_file =
      parse_script(m_allocator, m_validator, _file, file->typed_range<char>(),
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