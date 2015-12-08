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
#include "WNScripting/inc/WNNodeTypes.h"
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
}

namespace wn {
namespace scripting {

void ast_jit_engine::walk_type(const wn::scripting::type* _type,
                               llvm::Type** _value) {
  switch (_type->get_classification()) {
    case wn::scripting::type_classification::invalid_type:
      WN_RELEASE_ASSERT_DESC(wn_false, "Cannot classify invalid types");
      break;
    case wn::scripting::type_classification::void_type:
      *_value = llvm::Type::getVoidTy(*m_context);
      return;
    case wn::scripting::type_classification::int_type:
      *_value = llvm::IntegerType::getInt32Ty(*m_context);
      return;
    case wn::scripting::type_classification::float_type:
      *_value = llvm::Type::getFloatTy(*m_context);
      return;
    case wn::scripting::type_classification::char_type:
      *_value = llvm::IntegerType::get(*m_context, 8);
      return;
    case wn::scripting::type_classification::string_type:
      WN_RELEASE_ASSERT_DESC(wn_false, "Not implemented: string type");
      break;
    case wn::scripting::type_classification::bool_type:
      *_value = llvm::IntegerType::get(*m_context, 1);
      return;
      break;
    case wn::scripting::type_classification::custom_type:
      WN_RELEASE_ASSERT_DESC(wn_false, "Not implemented: custom types");
      break;
    case wn::scripting::type_classification::max:
      WN_RELEASE_ASSERT_DESC(wn_false, "Type out of range");
      break;
  }
}

void ast_jit_engine::walk_expression(const constant_expression* _const,
                                     expression_dat* _val) {
  switch (_const->get_type()->get_classification()) {
    case wn::scripting::type_classification::int_type: {
      long long val = atoll(_const->get_type_text().c_str());
      _val->value = llvm::ConstantInt::getSigned(
          m_generator->get_data(_const->get_type()), val);
      return;
    }
    default:
      WN_RELEASE_ASSERT_DESC(wn_false, "Not Implemented");
  }
}

void ast_jit_engine::walk_expression(const id_expression* id_expr,
                                     expression_dat* _val) {
  if (id_expr->get_id_source().param_source) {
    _val->instructions =
        wn::containers::dynamic_array<llvm::Instruction*>(m_allocator);
    _val->instructions.push_back(new llvm::LoadInst(
        m_generator->get_data(id_expr->get_id_source().param_source),
        make_string_ref(id_expr->get_id_source().param_source->get_name())));
    _val->value = _val->instructions.back();
  }
}

void ast_jit_engine::walk_instruction(
    const return_instruction* _inst,
    containers::dynamic_array<llvm::Instruction*>* _val) {
  *_val = containers::dynamic_array<llvm::Instruction*>(m_allocator);
  if (_inst->get_expression()) {
    const expression_dat& dat = m_generator->get_data(_inst->get_expression());
    _val->insert(_val->end(), dat.instructions.data(),
                 dat.instructions.data() + dat.instructions.size());
    _val->push_back(llvm::ReturnInst::Create(*m_context, dat.value));
  } else {
    _val->push_back(llvm::ReturnInst::Create(*m_context));
  }
}

void ast_jit_engine::walk_parameter(const parameter* _param,
                                    llvm::Instruction** _val) {
  *_val = new llvm::AllocaInst(m_generator->get_data(_param->get_type()),
                               make_string_ref(_param->get_name()));
}

void ast_jit_engine::walk_function(const function* _func, llvm::Function** _f) {
  wn::containers::dynamic_array<llvm::Type*> parameters;

  llvm::BasicBlock* bb = llvm::BasicBlock::Create(*m_context);
  if (_func->get_parameters()) {
    for (auto& a : _func->get_parameters()->get_parameters()) {
      parameters.push_back(m_generator->get_data(a->get_type()));
    }
  }

  llvm::FunctionType* t = llvm::FunctionType::get(
      m_generator->get_data(_func->get_signature()->get_type()),
      make_array_ref(parameters), wn_false);
  *_f = llvm::Function::Create(
      t, llvm::GlobalValue::LinkageTypes::ExternalLinkage,
      make_string_ref(_func->get_signature()->get_name()));
  if (_func->get_parameters()) {
    auto llvm_args = (*_f)->arg_begin();
    auto arg_names = _func->get_parameters()->get_parameters().begin();
    while (llvm_args != (*_f)->arg_end()) {
      llvm_args->setName(make_string_ref((*arg_names)->get_name()));
      llvm::Instruction* loc = m_generator->get_data((*arg_names).get());
      bb->getInstList().push_back(loc);
      bb->getInstList().push_back(new llvm::StoreInst(&(*llvm_args), loc));
      ++llvm_args;
      ++arg_names;
    }
  }

  for (auto& inst : _func->get_body()->get_instructions()) {
    for (auto& i : m_generator->get_data(inst.get())) {
      bb->getInstList().push_back(i);
    }
  }

  (*_f)->getBasicBlockList().push_front(bb);
}

void ast_jit_engine::walk_script_file(const script_file* _file) {
  for (auto& function : _file->get_functions()) {
    llvm::Function* func = m_generator->get_data(function.get());
    m_module->getFunctionList().push_back(func);
    llvm::verifyFunction(*func);
  }
  llvm::verifyModule(*m_module);
}

}  // namespace scripting
}  // namespace wn
