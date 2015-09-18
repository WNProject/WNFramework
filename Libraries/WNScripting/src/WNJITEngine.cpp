// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Due to a bug in llvm, it must be included before windows.h
#pragma warning(push)
#pragma warning(disable : 4100)
#pragma warning(disable : 4127)
#pragma warning(disable : 4310)
#pragma warning(disable : 4244)
#pragma warning(disable : 4458)
#pragma warning(disable : 4800)
#pragma warning(disable : 4624)
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>

#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#pragma warning(pop)

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNScripting/inc/WNJITEngine.h"
#include "WNScripting/inc/WNASTWalker.h"
#include "WNScripting/inc/WNScriptHelpers.h"
#include "WNLogging/inc/WNLog.h"

#include <algorithm>

namespace {

template <typename T>
typename llvm::ArrayRef<T> make_array_ref(
    const containers::dynamic_array<T>& _array) {
  return llvm::ArrayRef<T>(_array.data(), _array.size());
}

llvm::StringRef make_string_ref(const containers::string_view& _view) {
  return llvm::StringRef(_view.data(), _view.length());
}

llvm::StringRef make_string_ref(const containers::string& _view) {
  return llvm::StringRef(_view.data(), _view.length());
}

struct script_file {
  script_file(memory::allocator* _allocator) : m_functions(_allocator) {}
  ~script_file() {}
  script_file(script_file&& _other)
      : m_functions(std::move(_other.m_functions)) {}
  containers::deque<llvm::Function*> m_functions;
};

struct parameter {
  llvm::Type* m_type;
  containers::string m_name;
};

struct instruction {
  instruction(instruction&& _other)
      : main_block_insertions(std::move(_other.main_block_insertions)),
        additional_basic_blocks(std::move(_other.additional_basic_blocks)),
        end_basic_block(_other.end_basic_block) {
    _other.end_basic_block = wn_nullptr;
  }
  instruction(memory::allocator* _allocator)
      : main_block_insertions(_allocator),
        additional_basic_blocks(_allocator),
        end_basic_block(wn_nullptr) {}
  instruction() {
    WN_RELEASE_ASSERT_DESC(wn_false, "This should never be hit");
  }
  instruction(llvm::Instruction* _inst, memory::allocator* _allocator)
      : instruction(_allocator) {
    main_block_insertions.push_back(_inst);
  }
  containers::list<llvm::Instruction*> main_block_insertions;
  containers::list<llvm::BasicBlock*> additional_basic_blocks;
  llvm::BasicBlock* end_basic_block;
};

struct function_definition {
  function_definition(memory::allocator* _allocator)
      : m_parameters(_allocator) {}
  function_definition(function_definition&& _other)
      : m_type(_other.m_type), m_parameters(std::move(_other.m_parameters)) {
    _other.m_type = wn_nullptr;
  }
  llvm::FunctionType* m_type;
  containers::dynamic_array<containers::string> m_parameters;
};

// Takes a list of instructions and collapses tem into a single
// instruction that may consist of a sequence of llvm instructions,
// a set of basic blocks, and a single new insertion position.
instruction collapse_instructions(
    containers::contiguous_range<instruction> instructions,
    memory::allocator* _allocator) {
  if (instructions.empty()) {
    return instruction(_allocator);
  }
  instruction current_instruction(std::move(instructions[0]));

  for (wn_size_t i = 1; i < instructions.size(); ++i) {
    if (instructions[i - 1].end_basic_block != wn_nullptr) {
      for (auto& instruction : instructions[i].main_block_insertions) {
        instruction->removeFromParent();
        instructions[i - 1].end_basic_block->getInstList().push_back(
            instruction);
      }
    } else {
      instructions[i].main_block_insertions.transfer_to(
          instructions[i].main_block_insertions.begin(),
          instructions[i].main_block_insertions.end(),
          current_instruction.main_block_insertions.end(),
          current_instruction.main_block_insertions);
    }

    instructions[i].additional_basic_blocks.transfer_to(
        instructions[i].additional_basic_blocks.begin(),
        instructions[i].additional_basic_blocks.end(),
        current_instruction.additional_basic_blocks.end(),
        current_instruction.additional_basic_blocks);

    // If the new block does not end on a new basic block
    // then we can continue using the existing basic block.
    if (instructions[i].end_basic_block) {
      current_instruction.end_basic_block = instructions[i].end_basic_block;
      WN_DEBUG_ASSERT_DESC(
          instructions[i].additional_basic_blocks.end() !=
              std::find(instructions[i].additional_basic_blocks.begin(),
                        instructions[i].additional_basic_blocks.end(),
                        instructions[i].end_basic_block),
          "The new end block must be in the list of basic blocks");
    } else {
      WN_DEBUG_ASSERT_DESC(instructions[i].additional_basic_blocks.empty(),
                           "If you have more than one basic block you must have"
                           " transitioned out of the first");
    }
  }
  return std::move(current_instruction);
}

struct ast_jit_traits {
  typedef llvm::Value* expression_type;
  typedef function_definition function_header_type;
  typedef llvm::Function* function_type;
  typedef llvm::Value* assignment_instruction_type;
  typedef parameter parameter_type;
  typedef instruction instruction_type;
  typedef llvm::Value* return_instruction_type;
  typedef llvm::Value* lvalue_type;
  typedef script_file script_file_type;
  typedef llvm::Type* struct_definition_type;
  typedef llvm::Type* type_type;
  typedef containers::string include_type;
};

struct ast_jit_engine {
  ast_jit_engine(memory::allocator* _allocator, llvm::LLVMContext* _context,
                 llvm::Module* _module)
      : m_allocator(_allocator), m_context(_context), m_module(_module) {}

  void pre_walk_expression(const wn::scripting::expression*) {}
  llvm::Value* walk_expression(
      const wn::scripting::expression*,
      wn::containers::contiguous_range<
          wn::containers::contiguous_range<llvm::Value*>>,
      llvm::Type*) {
    WN_RELEASE_ASSERT_DESC(wn_false, "Not Implemented");
    return wn_nullptr;
  }

  void pre_walk_function(const wn::scripting::function* _function) {}
  void pre_walk_function_header(const wn::scripting::function* _function) {}
  function_definition walk_function_header(
      const wn::scripting::function* _function, parameter& _decl,
      containers::dynamic_array<parameter>& _parameters) {
    function_definition definition(m_allocator);
    definition.m_parameters.reserve(_parameters.size());

    containers::dynamic_array<llvm::Type*> types(m_allocator);
    types.reserve(_parameters.size());
    for (auto& decl : _parameters) {
      types.push_back(decl.m_type);
      definition.m_parameters.push_back(std::move(decl.m_name));
    }
    llvm::FunctionType* type =
        llvm::FunctionType::get(_decl.m_type, make_array_ref(types), false);
    definition.m_type = type;
    return std::move(definition);
  }

  llvm::Function* walk_function(const wn::scripting::function* _function,
                                const function_definition& function_header,
                                containers::dynamic_array<instruction>& _body) {
    llvm::Function* function = llvm::Function::Create(
        function_header.m_type,
        llvm::GlobalValue::LinkageTypes::ExternalLinkage,
        make_string_ref(_function->get_signature()->get_name()));

    auto llvm_args = function->arg_begin();
    auto arg_names = function_header.m_parameters.begin();
    while (llvm_args != function->arg_end()) {
      llvm_args->setName(make_string_ref(*arg_names));
      ++llvm_args;
      ++arg_names;
    }

    llvm::BasicBlock* bb =
        llvm::BasicBlock::Create(*m_context, "body", function);

    instruction inst =
        collapse_instructions(wn::containers::contiguous_range<instruction>(
                                  _body.data(), _body.size()),
                              m_allocator);

    for (auto& val : inst.main_block_insertions) {
      bb->getInstList().push_back(val);
    }

    for (auto& basic_block : inst.additional_basic_blocks) {
      basic_block->insertInto(function);
    }
    return function;
  }

  void pre_walk_declaration(const wn::scripting::declaration* _declaration) {}

  instruction walk_declaration(const wn::scripting::declaration* _declaration,
                               llvm::Type* _type) {
    return instruction(m_allocator);
  }

  llvm::Type* walk_type(const wn::scripting::type* _type) {
    switch (_type->get_classification()) {
      case wn::scripting::type_classification::invalid_type:
        WN_RELEASE_ASSERT_DESC(wn_false, "Cannot classify invalid types");
        break;
      case wn::scripting::type_classification::void_type:
        return llvm::Type::getVoidTy(*m_context);
        break;
      case wn::scripting::type_classification::int_type:
        break;
      case wn::scripting::type_classification::float_type:
        break;
      case wn::scripting::type_classification::char_type:
        break;
      case wn::scripting::type_classification::string_type:
        break;
      case wn::scripting::type_classification::bool_type:
        break;
      case wn::scripting::type_classification::custom_type:
        break;
      case wn::scripting::type_classification::max:
        WN_RELEASE_ASSERT_DESC(wn_false, "Type out of range");
        break;
    }
    WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented type");
    return wn_nullptr;
  }

  void pre_walk_type(const wn::scripting::type* _type) {}

  void pre_walk_return_instruction(
      const wn::scripting::return_instruction* _return_instruction) {}

  instruction walk_return_instruction(
      const wn::scripting::return_instruction* _return_instruction,
      llvm::Value* _expression) {
    WN_RELEASE_ASSERT_DESC(wn_false, "Not implemneted: non-void return");
    return instruction(m_allocator);
  }

  instruction walk_return_instruction(
      const wn::scripting::return_instruction* _return_instruction) {
    return instruction(llvm::ReturnInst::Create(*m_context), m_allocator);
  }

  void pre_walk_script_file(const wn::scripting::script_file* _file) {}
  script_file walk_script_file(
      const wn::scripting::script_file* _file,
      const wn::containers::contiguous_range<llvm::Function*>& _functions,
      const wn::containers::contiguous_range<containers::string>& _includes,
      const wn::containers::contiguous_range<llvm::Type*>& _structs) {
    script_file file(m_allocator);
    for (auto& function : _functions) {
      m_module->getFunctionList().push_back(function);
      file.m_functions.push_back(function);
      llvm::verifyFunction(*function);
    }
    llvm::verifyModule(*m_module);
    return std::move(file);
  }

  void pre_walk_parameter(const wn::scripting::parameter* _parameter) {}

  parameter walk_parameter(const wn::scripting::parameter* _parameter,
                           llvm::Type* _type) {
    return parameter({_type, _parameter->get_name().to_string(m_allocator)});
  }

 private:
  memory::allocator* m_allocator;
  llvm::Module* m_module;
  llvm::LLVMContext* m_context;
};
}  // anonymous

namespace wn {
namespace scripting {

jit_engine::jit_engine(memory::allocator* _allocator, file_manager* _manager,
                       WNLogging::WNLog* _log)
    : m_allocator(_allocator),
      m_file_manager(_manager),
      m_compilation_log(_log),
      m_modules(_allocator),
      m_context(wn::memory::make_std_unique<llvm::LLVMContext>()) {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();
}

jit_engine::~jit_engine() {}

CompiledModule& jit_engine::add_module(containers::string_view _file) {
  m_modules.push_back(CompiledModule());

  std::unique_ptr<llvm::Module> module =
      wn::memory::make_std_unique<llvm::Module>(make_string_ref(_file),
                                                *m_context);

  CompiledModule& code_module = m_modules.back();
  code_module.m_module = module.get();

#ifdef _WN_ANDROID
#ifdef _WN_ARM
  code_module.m_module->setTargetTriple(
      llvm::Triple::normalize("arm-linux-androideabi"));
#else
  code_module.m_module->setTargetTriple(
      llvm::Triple::normalize("x86-linux-androideabi"));
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

  llvm::EngineBuilder builder((std::move(module)));
  builder.setEngineKind(llvm::EngineKind::JIT);
  builder.setMCJITMemoryManager(
      wn::memory::make_std_unique<llvm::SectionMemoryManager>());

  code_module.m_engine =
      std::unique_ptr<llvm::ExecutionEngine>(builder.create());
  return code_module;
}

parse_error jit_engine::parse_file(const char* _file) {
  memory::allocated_ptr<file_buffer> buff = m_file_manager->get_file(_file);

  if (!buff) {
    return parse_error::does_not_exist;
  }

  memory::allocated_ptr<script_file> parsed_file = parse_script(
      m_allocator, _file, containers::string_view(buff->data(), buff->size()),
      m_compilation_log);

  if (parsed_file == wn_nullptr) {
    return wn::scripting::parse_error::parse_failed;
  }
  CompiledModule& module = add_module(_file);
  type_validator validator(m_allocator);
  ast_jit_engine ast_jit(m_allocator, m_context.get(), module.m_module);
  wn::scripting::ast_walker<ast_jit_engine, ast_jit_traits> walker(
      &ast_jit, WNLogging::get_null_logger(), &validator, m_allocator);

  ::script_file f = walker.walk_script_file(parsed_file.get());
  module.m_engine->finalizeObject();

  for (auto& function : f.m_functions) {
    llvm::StringRef name = function->getName();
    m_pointers[containers::string_view(name.data(), name.size())] =
        reinterpret_cast<void (*)()>(
            module.m_engine->getPointerToFunction(function));
  }

  return parse_error::ok;
}

}  // scripting
}  // wn
