// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_SCRIPTING_JIT_GENERATOR_H__
#define __WN_SCRIPTING_JIT_GENERATOR_H__

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNContainers/inc/WNDeque.h"
#include "WNContainers/inc/WNDynamicArray.h"
#include "WNContainers/inc/WNList.h"
#include "WNContainers/inc/WNString.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNScripting/inc/WNNodeTypes.h"
namespace llvm {
class Instruction;
class Type;
class Value;
}

namespace wn {
namespace scripting {

struct jitted_file {
  jitted_file(memory::allocator* _allocator) : m_functions(_allocator) {}
  ~jitted_file() {}
  jitted_file(jitted_file&& _other)
      : m_functions(std::move(_other.m_functions)) {}
  wn::containers::deque<llvm::Function*> m_functions;
};

struct jit_parameter {
  llvm::Type* m_type;
  llvm::Value* m_value;
  wn::containers::string m_name;
};

struct jitted_instruction {
  jitted_instruction(jitted_instruction&& _other)
      : main_block_insertions(std::move(_other.main_block_insertions)),
        additional_basic_blocks(std::move(_other.additional_basic_blocks)),
        end_basic_block(_other.end_basic_block) {
    _other.end_basic_block = wn_nullptr;
  }

  jitted_instruction(memory::allocator* _allocator)
      : main_block_insertions(_allocator),
        additional_basic_blocks(_allocator),
        end_basic_block(wn_nullptr) {}

  jitted_instruction(memory::allocator* _allocator,
              containers::dynamic_array<llvm::Instruction*>&
                  current_expression_instructions)
      : jitted_instruction(_allocator) {
    for (auto& it : current_expression_instructions) {
      main_block_insertions.push_back(it);
    }
    current_expression_instructions.clear();
  }

  jitted_instruction() {
    WN_RELEASE_ASSERT_DESC(wn_false, "This should never be hit");
  }

  jitted_instruction(llvm::Instruction* _inst, wn::memory::allocator* _allocator,
              containers::dynamic_array<llvm::Instruction*>&
                  current_expression_instructions)
      : jitted_instruction(_allocator, current_expression_instructions) {
    main_block_insertions.push_back(_inst);
  }

  wn::containers::list<llvm::Instruction*> main_block_insertions;
  wn::containers::list<llvm::BasicBlock*> additional_basic_blocks;
  llvm::BasicBlock* end_basic_block;
};

struct jitted_function {
  jitted_function(memory::allocator* _allocator)
      : m_parameters(_allocator) {}
  jitted_function(jitted_function&& _other)
      : m_type(_other.m_type),
        m_function(_other.m_function),
        m_parameters(std::move(_other.m_parameters)) {
    _other.m_type = wn_nullptr;
    _other.m_function = wn_nullptr;
  }
  llvm::FunctionType* m_type;
  llvm::Function* m_function;
  wn::containers::dynamic_array<containers::string> m_parameters;
};

struct ast_jit_traits {
  typedef llvm::Value* expression_type;
  typedef jitted_function function_header_type;
  typedef llvm::Function* function_type;
  typedef llvm::Value* assignment_instruction_type;
  typedef jit_parameter parameter_type;
  typedef jit_parameter function_type_name;
  typedef jitted_instruction instruction_type;
  typedef llvm::Value* return_instruction_type;
  typedef llvm::Value* lvalue_type;
  typedef jitted_file script_file_type;
  typedef llvm::Type* struct_definition_type;
  typedef llvm::Type* type_type;
  typedef wn::containers::string include_type;
};

struct ast_jit_engine {
  ast_jit_engine(memory::allocator* _allocator, llvm::LLVMContext* _context,
                 llvm::Module* _module)
      : m_allocator(_allocator), m_context(_context), m_module(_module) {}

  llvm::Value* walk_expression(
      const wn::scripting::expression*,
      wn::containers::contiguous_range<
          wn::containers::contiguous_range<llvm::Value*>>,
      llvm::Type*);

  llvm::Value* walk_expression(
      const wn::scripting::constant_expression* expr,
      wn::containers::contiguous_range<
          wn::containers::contiguous_range<llvm::Value*>>,
      llvm::Type* _type);

  llvm::Value* walk_expression(
      const wn::scripting::id_expression*,
      wn::containers::contiguous_range<
          wn::containers::contiguous_range<llvm::Value*>> _value,
      llvm::Type* _type);

  jitted_function walk_function_header(
      const wn::scripting::function* _function, jit_parameter& _decl,
      wn::containers::dynamic_array<
          std::pair<jit_parameter, llvm::Type*>>& _parameters);

  llvm::Value* walk_parameter_instantiation(
      const wn::scripting::parameter* _parameter,
      jitted_function& function,
      std::pair<jit_parameter, llvm::Type*>& _parameter_value,
      wn_size_t parameter_number);

  llvm::Function* walk_function(
      const wn::scripting::function* _function,
      const jitted_function& function_header,
      wn::containers::dynamic_array<jitted_instruction>& _body);

  jitted_instruction walk_declaration(
      const wn::scripting::declaration* _declaration, llvm::Type* _type);

  llvm::Type* walk_type(const wn::scripting::type* _type);

  jitted_instruction walk_return_instruction(
      const wn::scripting::return_instruction* _return_instruction,
      llvm::Value* _expression);

  jitted_instruction walk_return_instruction(
      const wn::scripting::return_instruction* _return_instruction);

  jitted_file walk_script_file(
      const wn::scripting::script_file* _file,
      const wn::containers::contiguous_range<llvm::Function*>& _functions,
      const wn::containers::contiguous_range<containers::string>& _includes,
      const wn::containers::contiguous_range<llvm::Type*>& _structs);

  jit_parameter walk_parameter_name(
      const wn::scripting::parameter* _parameter, llvm::Type* _type);

  jit_parameter walk_function_name(
      const wn::scripting::parameter* _parameter, llvm::Type* _type);

 private:
  wn::memory::allocator* m_allocator;
  llvm::Module* m_module;
  llvm::LLVMContext* m_context;
  // This holds all of the instrutions that are inside a nested expression.
  // When these finally get "resolved" they can be passed to the instruction()
  // to make sure they end up in the same basic_block as the instruction
  // that needs them.
  containers::dynamic_array<llvm::Instruction*> current_expression_instructions;
};

}  // namespace scripting
}  // namespace wn

#endif  // __WN_SCRIPTING_JIT_GENERATOR_H__
