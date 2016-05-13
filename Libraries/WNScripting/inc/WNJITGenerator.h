// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_SCRIPTING_JIT_GENERATOR_H__
#define __WN_SCRIPTING_JIT_GENERATOR_H__

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNContainers/inc/WNDeque.h"
#include "WNContainers/inc/WNDynamicArray.h"
#include "WNContainers/inc/WNHashMap.h"
#include "WNContainers/inc/WNString.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNScripting/inc/WNASTCodeGenerator.h"
#include "WNScripting/inc/WNNodeTypes.h"
namespace llvm {
class Instruction;
class Type;
class Value;
}

namespace wn {
namespace scripting {

class type_validator;
struct expression_dat {
  containers::dynamic_array<llvm::Instruction*> instructions;
  llvm::Value* value;
};

struct instruction_dat {
  containers::dynamic_array<llvm::Instruction*> instructions;
  containers::dynamic_array<llvm::BasicBlock*> blocks;
};

// Implements the ast_walker interface for creating
// llvm modules from WNScript ASTs.
class ast_jit_engine;
struct ast_jit_traits {
  using instruction_data = instruction_dat;
  using expression_data = expression_dat;
  using parameter_data = llvm::Instruction*;
  using function_data = llvm::Function*;
  using type_data = llvm::Type*;
  using struct_definition_data = llvm::Type*;
  using code_gen = ast_jit_engine;
};

class ast_jit_engine {
public:
  ast_jit_engine(memory::allocator* _allocator, type_validator* _validator,
      llvm::LLVMContext* _context, llvm::Module* _module,
      ast_code_generator<ast_jit_traits>* _generator)
    : m_allocator(_allocator),
      m_validator(_validator),
      m_struct_map(m_allocator),
      m_context(_context),
      m_module(_module),
      m_generator(_generator) {}

  void walk_expression(const expression*, expression_dat*) {}
  void walk_expression(const cast_expression* _const, expression_dat* _val);
  void walk_expression(const constant_expression* _const, expression_dat* _str);
  void walk_expression(const id_expression* _const, expression_dat* _str);
  void walk_expression(const binary_expression* _binary, expression_dat* _str);
  void walk_expression(
      const struct_allocation_expression* _alloc, expression_dat* _str);
  void walk_expression(
      const member_access_expression* _alloc, expression_dat* _str);
  void walk_expression(
      const function_call_expression* _call, expression_dat* _str);

  void walk_type(const type* _type, llvm::Type** _val);
  void walk_instruction(const instruction*, instruction_dat*) {
    WN_RELEASE_ASSERT_DESC(false, "Unimplemented instruction type implemented");
  }
  void walk_instruction_list(const instruction_list*, instruction_dat*);
  void walk_instruction(const expression_instruction* _inst, instruction_dat*);
  void walk_instruction(const return_instruction* _inst, instruction_dat*);
  void walk_instruction(const declaration* _inst, instruction_dat*);
  void walk_instruction(const assignment_instruction* _inst, instruction_dat*);
  void walk_instruction(const if_instruction*, instruction_dat*);

  void walk_parameter(const parameter* _param, llvm::Instruction**);
  void walk_struct_definition(const struct_definition* _def, llvm::Type**);
  void walk_function(const function* _func, llvm::Function**);
  void walk_script_file(const script_file* _file);

private:
  memory::allocator* m_allocator;
  type_validator* m_validator;
  containers::hash_map<uint32_t, llvm::Type*> m_struct_map;
  ast_code_generator<ast_jit_traits>* m_generator;
  llvm::Module* m_module;
  llvm::LLVMContext* m_context;
};

}  // namespace scripting
}  // namespace wn

#endif  // __WN_SCRIPTING_JIT_GENERATOR_H__