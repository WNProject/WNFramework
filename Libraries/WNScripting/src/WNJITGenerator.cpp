// Copyright (c) 2015, WNProject Authors. All rights reserved.
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

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNContainers/inc/WNDynamicArray.h"
#include "WNContainers/inc/WNList.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNScripting/inc/WNJITGenerator.h"
#include "WNScripting/inc/WNASTWalker.h"
#include "WNScripting/inc/WNScriptHelpers.h"
#include "WNLogging/inc/WNLog.h"

#include <algorithm>

namespace {

template <typename T>
typename llvm::ArrayRef<T> make_array_ref(
    const wn::containers::dynamic_array<T>& _array) {
  return llvm::ArrayRef<T>(_array.data(), _array.size());
}

llvm::StringRef make_string_ref(const wn::containers::string_view& _view) {
  return llvm::StringRef(_view.data(), _view.length());
}

llvm::StringRef make_string_ref(const wn::containers::string& _view) {
  return llvm::StringRef(_view.data(), _view.length());
}

// Takes a list of instructions and collapses tem into a single
// instruction that may consist of a sequence of llvm instructions,
// a set of basic blocks, and a single new insertion position.
wn::scripting::jitted_instruction collapse_instructions(
    wn::containers::contiguous_range<wn::scripting::jitted_instruction>
        instructions,
    wn::memory::allocator* _allocator) {
  if (instructions.empty()) {
    return wn::scripting::jitted_instruction(_allocator);
  }
  wn::scripting::jitted_instruction current_instruction(
      std::move(instructions[0]));

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
}  // anonymous

namespace wn {
namespace scripting {
llvm::Value* ast_jit_engine::walk_expression(
    const wn::scripting::expression*,
    wn::containers::contiguous_range<
        wn::containers::contiguous_range<llvm::Value*>>,
    llvm::Type*) {
  WN_RELEASE_ASSERT_DESC(wn_false, "Not Implemented");
  return wn_nullptr;
}

llvm::Value* ast_jit_engine::walk_expression(
    const wn::scripting::constant_expression* expr,
    wn::containers::contiguous_range<
        wn::containers::contiguous_range<llvm::Value*>>,
    llvm::Type* _type) {
  switch (expr->get_classification()) {
    case wn::scripting::type_classification::int_type: {
      WN_DEBUG_ASSERT_DESC(_type->isIntegerTy(32),
                           "Integer types must be int32 types");
      long long val = atoll(expr->get_type_text().c_str());
      return llvm::ConstantInt::getSigned(_type, val);
      break;
    }
    default:
      WN_RELEASE_ASSERT_DESC(wn_false, "Not Implemented");
  }
  return wn_nullptr;
}

llvm::Value* ast_jit_engine::walk_expression(
    const wn::scripting::id_expression*,
    wn::containers::contiguous_range<
        wn::containers::contiguous_range<llvm::Value*>> _value,
    llvm::Type* _type) {
  current_expression_instructions.push_back(new llvm::LoadInst(_value[0][0]));
  return current_expression_instructions.back();
}

jitted_function ast_jit_engine::walk_function_header(
    const wn::scripting::function* _function, jit_parameter& _decl,
    wn::containers::dynamic_array<std::pair<jit_parameter, llvm::Type*>>&
        _parameters) {
  jitted_function definition(m_allocator);
  definition.m_parameters.reserve(_parameters.size());

  wn::containers::dynamic_array<llvm::Type*> types(m_allocator);
  types.reserve(_parameters.size());
  for (auto& decl : _parameters) {
    types.push_back(std::get<1>(decl));
    definition.m_parameters.push_back(std::get<0>(decl).m_name);
  }

  llvm::FunctionType* type =
      llvm::FunctionType::get(_decl.m_type, make_array_ref(types), false);
  definition.m_type = type;

  llvm::Function* function = llvm::Function::Create(
      type, llvm::GlobalValue::LinkageTypes::ExternalLinkage,
      make_string_ref(_function->get_signature()->get_name()));

  definition.m_function = function;

  // As a rule, we don't want to generate unwind tables
  // for any functions, furthermore this will cause
  // unexpected symbol resolution on Android.
  function->addFnAttr(llvm::Attribute::NoUnwind);
  llvm::BasicBlock* initial =
      llvm::BasicBlock::Create(*m_context, "initial", function);

  auto llvm_args = function->arg_begin();
  auto arg_names = _parameters.begin();
  while (llvm_args != function->arg_end()) {
    llvm_args->setName(make_string_ref(std::get<0>(*arg_names).m_name));
    ++llvm_args;
    ++arg_names;
  }

  return std::move(definition);
}

llvm::Value* ast_jit_engine::walk_parameter_instantiation(
    const wn::scripting::parameter* _parameter, jitted_function& function,
    std::pair<jit_parameter, llvm::Type*>& _parameter_value,
    wn_size_t parameter_number) {
  llvm::Instruction* inst = new llvm::AllocaInst(
      std::get<1>(_parameter_value),
      make_string_ref(std::get<0>(_parameter_value).m_name));
  llvm::BasicBlock& block = function.m_function->getBasicBlockList().back();

  // walk_parameter_instantiation gets called once per parameter,
  // making this n^2 in the number of params. This is non-ideal,
  // but should be ok given we do not expect the number of
  // params to be too large.
  auto arg = function.m_function->arg_begin();
  for (size_t i = 0; i < parameter_number; ++i, ++arg)
    ;

  block.getInstList().push_back(inst);
  block.getInstList().push_back(
      new llvm::StoreInst(function.m_function->arg_begin(), inst));
  return inst;
}

llvm::Function* ast_jit_engine::walk_function(
    const wn::scripting::function* _function,
    const jitted_function& function_header,
    wn::containers::dynamic_array<jitted_instruction>& _body) {
  llvm::BasicBlock& last_bb =
      function_header.m_function->getBasicBlockList().back();

  llvm::BasicBlock* bb =
      llvm::BasicBlock::Create(*m_context, "body", function_header.m_function);
  last_bb.getInstList().push_back(llvm::BranchInst::Create(bb));

  jitted_instruction inst = collapse_instructions(
      wn::containers::contiguous_range<jitted_instruction>(_body.data(),
                                                           _body.size()),
      m_allocator);

  for (auto& val : inst.main_block_insertions) {
    bb->getInstList().push_back(val);
  }

  for (auto& basic_block : inst.additional_basic_blocks) {
    basic_block->insertInto(function_header.m_function);
  }
  return function_header.m_function;
}

jitted_instruction ast_jit_engine::walk_declaration(
    const wn::scripting::declaration* _declaration, llvm::Type* _type) {
  return jitted_instruction(m_allocator, current_expression_instructions);
}

llvm::Type* ast_jit_engine::walk_type(const wn::scripting::type* _type) {
  switch (_type->get_classification()) {
    case wn::scripting::type_classification::invalid_type:
      WN_RELEASE_ASSERT_DESC(wn_false, "Cannot classify invalid types");
      break;
    case wn::scripting::type_classification::void_type:
      return llvm::Type::getVoidTy(*m_context);
    case wn::scripting::type_classification::int_type:
      return llvm::IntegerType::getInt32Ty(*m_context);
    case wn::scripting::type_classification::float_type:
      return llvm::Type::getFloatTy(*m_context);
    case wn::scripting::type_classification::char_type:
      return llvm::IntegerType::get(*m_context, 8);
    case wn::scripting::type_classification::string_type:
      break;
    case wn::scripting::type_classification::bool_type:
      return llvm::IntegerType::get(*m_context, 1);
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

jitted_instruction ast_jit_engine::walk_return_instruction(
    const wn::scripting::return_instruction* _return_instruction,
    llvm::Value* _expression) {
  return jitted_instruction(llvm::ReturnInst::Create(*m_context, _expression),
                            m_allocator, current_expression_instructions);
}

jitted_instruction ast_jit_engine::walk_return_instruction(
    const wn::scripting::return_instruction* _return_instruction) {
  return jitted_instruction(llvm::ReturnInst::Create(*m_context), m_allocator,
                            current_expression_instructions);
}

jitted_file ast_jit_engine::walk_script_file(
    const wn::scripting::script_file* _file,
    const wn::containers::contiguous_range<llvm::Function*>& _functions,
    const wn::containers::contiguous_range<containers::string>& _includes,
    const wn::containers::contiguous_range<llvm::Type*>& _structs) {
  jitted_file file(m_allocator);
  for (auto& function : _functions) {
    m_module->getFunctionList().push_back(function);
    file.m_functions.push_back(function);
    llvm::verifyFunction(*function);
  }
  llvm::verifyModule(*m_module);
  return std::move(file);
}

jit_parameter ast_jit_engine::walk_parameter_name(
    const wn::scripting::parameter* _parameter, llvm::Type* _type) {
  return jit_parameter(
      {_type, wn_nullptr, _parameter->get_name().to_string(m_allocator)});
}

jit_parameter ast_jit_engine::walk_function_name(
    const wn::scripting::parameter* _parameter, llvm::Type* _type) {
  return jit_parameter(
      {_type, wn_nullptr, _parameter->get_name().to_string(m_allocator)});
}

}  // scripting
}  // wn
