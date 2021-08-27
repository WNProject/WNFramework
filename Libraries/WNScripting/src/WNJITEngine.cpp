// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
#pragma warning(disable : 4245)
#pragma warning(disable : 4459)
#endif

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
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
#include "WNFunctional/inc/WNDefer.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/allocator.h"
#include "WNMemory/inc/manipulation.h"
#include "WNScripting/inc/WNJITConfiguration.h"
#include "WNScripting/inc/WNJITEngine.h"
#include "WNScripting/inc/WNScriptHelpers.h"
#include "WNScripting/inc/ast_node_types.h"
#include "WNScripting/inc/jit_compiler.h"
#include "profiling/inc/profiling.h"

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
          _imported_functions,
      containers::hash_map<containers::string_view, void_f>*
          _external_functions,
      containers::hash_map<containers::string, internal::resource>*
          _resource_functions)
    : m_allocator(_allocator),
      m_log(_log),
      m_imported_functions(_imported_functions),
      m_external_functions(_external_functions),
      m_resource_functions(_resource_functions) {}

  llvm::JITSymbol findSymbol(const std::string& Name) override {
    PROFILE_REGION(SymbolLookup);
    if (Name.empty()) {
      return llvm::JITSymbol(0, llvm::JITSymbolFlags::Exported);
    }
    m_log->log_debug("Resolving ", Name.c_str(), ".");
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
    std::string Nm = Name;
// LLVM for win32 x86 is surprising as it will automatically
// prepend _ to some symbols. If we see __, then remove a _
#if defined(_WN_WINDOWS)
#if !defined(_WN_64_BIT)
    if (Nm.size() > 2 && Nm[0] == '_' && Nm[1] == '_') {
      Nm = Nm.substr(1);
    }
#endif
#endif
#if defined(_WN_MACOS)
    Nm = Nm.substr(1);
#endif
    {
      auto it = m_imported_functions->find(
          containers::string(m_allocator, Nm.c_str()));
      if (it != m_imported_functions->end()) {
        return llvm::JITSymbol(
            static_cast<uint64_t>(reinterpret_cast<size_t>(it->second)),
            llvm::JITSymbolFlags::Exported);
      }
    }

    {
      auto it = m_external_functions->find(containers::string_view(Nm.c_str()));
      if (it != m_external_functions->end()) {
        return llvm::JITSymbol(
            static_cast<uint64_t>(reinterpret_cast<size_t>(it->second)),
            llvm::JITSymbolFlags::Exported);
      }
    }
    m_log->log_error("Could not resolve ", Name.c_str(), ".");
    return llvm::JITSymbol(0, llvm::JITSymbolFlags::Exported);
  }

private:
  memory::allocator* m_allocator;
  logging::log* m_log;
  containers::hash_map<containers::string, void (*)(void)>*
      m_imported_functions;
  containers::hash_map<containers::string_view, void_f>* m_external_functions;
  containers::hash_map<containers::string, internal::resource>*
      m_resource_functions;
};
}  // namespace

CompiledModule::CompiledModule() : m_module(nullptr) {}

CompiledModule::CompiledModule(CompiledModule&& _other)
  : m_engine(core::move(_other.m_engine)),
    m_module(core::move(_other.m_module)) {
  _other.m_module = nullptr;
}

jit_engine::jit_engine(memory::allocator* _allocator,
    file_system::mapping* _mapping, logging::log* _log,
    memory::allocator* _support_allocator)
  : engine(_allocator, _log, _support_allocator),
    m_file_mapping(_mapping),
    m_compilation_log(_log),
    m_context(memory::make_std_unique<llvm::LLVMContext>()),
    m_modules(_allocator),
    m_pointers(_allocator),
    m_c_pointers(_allocator),
    m_resources(_allocator),
    m_extension_handlers(_allocator),
    m_external_types(_allocator),
    m_struct_infos(_allocator),
    m_started_files(_allocator),
    m_finished_files(_allocator) {
  PROFILE_REGION(JitInit);
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();
  register_function<decltype(&do_allocate), &do_allocate>("_allocate");
  register_function<decltype(&do_free), &do_free>("_free");
  register_function<decltype(&do_allocate_array), &do_allocate_array>(
      "_allocate_runtime_array");
  m_type_manager.finalize_builtins();
}

jit_engine::~jit_engine() {}

CompiledModule& jit_engine::add_module(containers::string_view _file) {
  PROFILE_REGION(JitAddModule);

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
      memory::make_std_unique<CustomMemoryManager>(m_allocator,
          m_compilation_log, &m_c_pointers, &m_pointers, &m_resources));

  code_module.m_engine =
      std::unique_ptr<llvm::ExecutionEngine>(builder.create());
  return code_module;
}

void* jit_engine::allocate_shared(size_t size) {
  void* v = do_allocate(wn_size_t{sizeof(object) + size});
  memset(v, 0x00, sizeof(object));
  return static_cast<void*>(static_cast<uint8_t*>(v) + sizeof(object));
}

void jit_engine::free_shared(void* v) const {
  do_free(static_cast<uint8_t*>(v) - sizeof(object));
}

parse_error jit_engine::parse_file(const containers::string_view _file) {
  PROFILE_REGION(JitFile);

  auto additional_includes = core::move(m_additional_includes);
  for (auto& it : additional_includes) {
    parse_error err = parse_file(it);
    if (err != parse_error::ok) {
      return err;
    }
  }

  if (m_finished_files.find(_file.to_string(m_allocator)) !=
      m_finished_files.end()) {
    return parse_error::ok;
  }
  for (size_t i = 0; i < m_started_files.size(); ++i) {
    if (m_started_files[i] == _file) {
      m_compilation_log->log_error("Recursive include detected");
      m_compilation_log->log_error("        ", _file);
      for (int32_t j = int32_t(m_started_files.size() - 1); j >= 0; --j) {
        m_compilation_log->log_error("        ->", m_started_files[j]);
      }
      return parse_error::invalid_parameters;
    }
  }
  m_started_files.push_back(_file);
  functional::defer clean(functional::function<void()>(
      m_allocator, [this]() { m_started_files.pop_back(); }));

  file_system::result res;
  file_system::file_ptr file;
  containers::string synthetic_contents(m_allocator);
  bool use_synthetic_contents = false;

  if (!_file.ends_with(".wns")) {
    auto pt = _file.find_last_of(".");
    if (pt == containers::string_view::npos) {
      m_compilation_log->log_error("Missing file extension");
      return parse_error::eUnsupported;
    }
    if (m_extension_handlers.find(_file.substr(pt).to_string(m_allocator)) ==
        m_extension_handlers.end()) {
      m_compilation_log->log_error(
          "Unhandled file extension ", _file.substr(pt));
      return parse_error::eUnsupported;
    }
    auto ext_handler =
        m_extension_handlers[_file.substr(pt).to_string(m_allocator)];
    containers::string out_data(m_allocator);
    convert_type convert_res = ext_handler->convert_file(m_compilation_log,
        m_file_mapping, _file, &out_data, &synthetic_contents);
    if (convert_res == convert_type::failed) {
      m_compilation_log->log_error(
          "Resource file ", _file, " was not handled by resource handler");
      return parse_error::eWNInvalidFile;
    }
    if (!m_type_manager.register_resource_data(_file, out_data)) {
      return parse_error::eWNInvalidFile;
    }
    use_synthetic_contents = convert_res == convert_type::success;
  }
  if (!use_synthetic_contents) {
    file = m_file_mapping->open_file(_file, res);

    if (!file) {
      m_compilation_log->log_error("Could not find file", _file);
      return parse_error::does_not_exist;
    }
  }

  memory::unique_ptr<ast_script_file> parsed_file;
  {
    PROFILE_REGION(JitParse);
    parsed_file = parse_script(m_allocator, _file,
        use_synthetic_contents ? synthetic_contents.to_contiguous_range()
                               : file->typed_range<char>(),
        functional::function<bool(containers::string_view)>(m_allocator,
            [this](containers::string_view include) {
              return parse_file(include) == scripting::parse_error::ok ? true
                                                                       : false;
            }),
        functional::function<bool(containers::string_view,
            containers::string_view, bool)>(m_allocator,
            [this](containers::string_view resource_type,
                containers::string_view resource_name, bool instantiated) {
              auto it = m_resources.find(resource_type.to_string(m_allocator));
              if (it == m_resources.end()) {
                m_log->log_error("Cannot find resource ", resource_type);
                return false;
              }
              if (it->second.resource->must_be_instantiated() &&
                  !instantiated) {
                m_log->log_error("Resource ", resource_type,
                    " can only be used in an instantiated context");
                return false;
              }
              if (!it->second.resource->can_be_instantiated() && instantiated) {
                m_log->log_error("Resource ", resource_type,
                    " can only not be used in an instantiated context");
                return false;
              }
              containers::string str(m_allocator);
              if (!it->second.resource->setup_resource(resource_name, &str)) {
                m_log->log_error("Resource @", resource_type, "(",
                    resource_name, ") failed initialization");

                return false;
              }
              if (str.empty()) {
                return true;
              }
              return parse_file(str) == scripting::parse_error::ok ? true
                                                                   : false;
            }),
        &m_type_manager, false, m_compilation_log, &m_num_warnings,
        &m_num_errors);
  }
  if (parsed_file == nullptr) {
    if (use_synthetic_contents) {
      m_log->log_info("Synthetic file contents: ", synthetic_contents);
    }
    return parse_error::parse_failed;
  }
  m_finished_files.insert(_file.to_string(m_allocator));

  CompiledModule& module = add_module(_file);

  jit_compiler compiler(m_allocator, module.m_module, &m_struct_infos);
  {
    PROFILE_REGION(JitCompile);
    compiler.compile(parsed_file.get());
  }
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
  {
    PROFILE_REGION(JitFinalize);
    module.m_engine->finalizeObject();
    for (auto& function : module.m_module->getFunctionList()) {
      llvm::StringRef name = function.getName();
      if (!function.isIntrinsic()) {
        m_pointers[containers::string_view(name.data(), name.size())] =
            reinterpret_cast<void (*)()>(
                module.m_engine->getPointerToFunction(&function));
      }
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
  m_c_pointers[core::move(s)] = _function;
  return true;
}

bool jit_engine::register_mangled_c_function(
    containers::string_view _name, void_f _function, bool) {
  auto name = _name.to_string(m_allocator);
  m_c_pointers[core::move(name)] = _function;
  return true;
}

size_t jit_engine::get_vtable_offset(const ast_type* _type) {
  auto it = m_struct_infos.find(_type);
  if (it != m_struct_infos.end()) {
    return it->second.vtable_offset;
  }
  return static_cast<size_t>(-1);
}

bool jit_engine::register_resource(
    resource_manager* _resource, const ast_type* _type) {
  PROFILE_REGION(JitRegisterRes);
  auto name = _resource->get_name().to_string(m_allocator);
  if (m_resources.find(name) != m_resources.end()) {
    return false;
  }
  m_resources[core::move(name)] = {_type, _resource};
  if (!_resource->get_file_extension().empty()) {
    m_extension_handlers[_resource->get_file_extension().to_string(
        m_allocator)] = _resource;
  }
  return true;
}

}  // namespace scripting
}  // namespace wn
