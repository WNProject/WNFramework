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
#include <llvm/IR/IRBuilder.h>
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

#include "WNScripting/inc/WNNodeTypes.h"
#include "WNScripting/inc/internal/ast_convertor_context.h"
#include "WNScripting/inc/jit_compiler.h"
#include "WNScripting/inc/parse_ast_convertor.h"

namespace wn {
namespace scripting {
namespace {
template <typename T>
llvm::ArrayRef<T> array_ref(containers::dynamic_array<T>& t) {
  return llvm::ArrayRef<T>(t.data(), t.data() + t.size());
}
}  // namespace

namespace internal {
struct jit_compiler_context {
  jit_compiler_context(memory::allocator* _allocator, llvm::Module* _module)
    : m_allocator(_allocator),
      m_module(_module),
      m_data_layout(_module),
      m_context(_module->getContext()),
      m_types(_allocator),
      m_function_types(_allocator),
      m_functions(_allocator),
      m_declarations(_allocator),
      m_function_parameters(_allocator),
      m_vtables(_allocator),
      m_loops(_allocator) {
    m_int32_t = llvm::IntegerType::getInt32Ty(m_context);
    m_float_t = llvm::Type::getFloatTy(m_context);
    m_bool_t = llvm::IntegerType::getInt1Ty(m_context);
    m_uint8_t = llvm::IntegerType::getInt8Ty(m_context);
    m_size_t = llvm::Type::getIntNTy(m_context, sizeof(size_t) * 8);
    m_void_t = llvm::Type::getVoidTy(m_context);
    m_void_ptr_t = m_uint8_t->getPointerTo(0);
    m_voidfn_ptr_t = llvm::FunctionType::get(m_void_t, false)->getPointerTo(0);
  }

  bool compile(const ast_script_file* _file);

  // Types
  bool decode_type(const ast_type* _type);
  llvm::Type* get_type(const ast_type* _type);
  llvm::Type* get_function_ptr_type(const ast_type* _func);
  bool prepare_vtable(const ast_vtable* _vtable);

  // Functions
  bool forward_declare_function(const ast_function* _function);
  bool declare_function(const ast_function* _function);

  // Expressions
  llvm::Value* get_expression(const ast_expression* _expression);
  llvm::Value* get_id(const ast_id* _id);
  llvm::Value* get_constant(const ast_constant* _const);
  llvm::Value* get_binary_expression(const ast_binary_expression* _bin);
  llvm::Value* get_function_call(const ast_function_call_expression* _func);
  llvm::Function* get_function(const ast_function* _func);
  llvm::Value* get_function_from_ptr(const ast_expression* _func);
  llvm::Value* get_function_pointer(
      const ast_function_pointer_expression* _func);
  llvm::Value* get_virtual_function(
      const ast_function_call_expression* _call, llvm::Value* _this);
  llvm::Value* get_cast(const ast_cast_expression* _expr);
  llvm::Value* get_member_access(const ast_member_access_expression* _access);
  llvm::Value* get_builtin(const ast_builtin_expression* _builtin);

  // Statements
  bool write_statement(const ast_statement* _statement);
  bool write_assignment(const ast_assignment* _block);
  bool write_declaration(const ast_declaration* _block);
  bool write_return(const ast_return_instruction* _return);
  bool write_scope_block(const ast_scope_block* _block);
  bool write_if_chain(const ast_if_chain* _if);
  bool write_evaluate_expression(const ast_evaluate_expression* _expr);
  bool write_builtin_statement(const ast_builtin_statement* _st);
  bool write_loop(const ast_loop* _loop);
  bool write_break(const ast_break* _break);
  bool write_continue(const ast_continue_instruction* _continue);

  // Helpers
  bool write_temporaries(const ast_expression* _expression);
  bool write_temporary_cleanup(const ast_expression* _expression);
  llvm::Constant* i32(int32_t _x) {
    return llvm::ConstantInt::get(m_int32_t, _x);
  }

  llvm::IRBuilder<>* m_function_builder;
  memory::allocator* m_allocator;
  llvm::Module* m_module;
  llvm::DataLayout m_data_layout;
  llvm::LLVMContext& m_context;
  containers::hash_map<const ast_type*, llvm::Type*> m_types;
  containers::hash_map<const ast_function*, llvm::FunctionType*>
      m_function_types;
  containers::hash_map<const ast_function*, llvm::Function*> m_functions;
  containers::hash_map<const ast_declaration*, llvm::Value*> m_declarations;

  containers::hash_map<const ast_function::parameter*, llvm::Value*>
      m_function_parameters;
  containers::hash_map<const ast_vtable*, llvm::GlobalValue*> m_vtables;

  struct loop_data {
    llvm::BasicBlock* continue_target;
    llvm::BasicBlock* break_target;
  };

  containers::hash_map<const ast_loop*, loop_data> m_loops;

  llvm::Function* m_current_function;
  llvm::IntegerType* m_int32_t;
  llvm::Type* m_float_t;
  llvm::IntegerType* m_bool_t;
  llvm::IntegerType* m_uint8_t;
  llvm::IntegerType* m_size_t;
  llvm::Type* m_void_t;
  llvm::Type* m_void_ptr_t;
  llvm::Type* m_voidfn_ptr_t;
};
}  // namespace internal

jit_compiler::jit_compiler(memory::allocator* _allocator, llvm::Module* _module)
  : m_compiler_context(memory::make_unique<internal::jit_compiler_context>(
        _allocator, _allocator, _module)) {}

jit_compiler::~jit_compiler() {}

bool jit_compiler::compile(ast_script_file* _file) {
  return m_compiler_context->compile(_file);
}

bool internal::jit_compiler_context::compile(const ast_script_file* _file) {
  for (auto& function : _file->m_functions) {
    if (!forward_declare_function(function.get())) {
      return false;
    }
  }

  for (auto& t : _file->m_initialization_order) {
    if (t->m_vtable) {
      if (!prepare_vtable(t->m_vtable)) {
        return false;
      }
    }
  }

  for (auto& function : _file->m_functions) {
    if (!declare_function(function.get())) {
      return false;
    }
  }
  return true;
}

bool internal::jit_compiler_context::forward_declare_function(
    const ast_function* _function) {
  containers::dynamic_array<llvm::Type*> types(m_allocator);

  llvm::Type* return_type = get_type(_function->m_return_type);
  if (!return_type) {
    return false;
  }

  for (auto& t : _function->m_parameters) {
    llvm::Type* param_type = get_type(t.m_type);
    if (!param_type) {
      return false;
    }
    types.push_back(param_type);
  }

  llvm::FunctionType* ft = llvm::FunctionType::get(return_type,
      llvm::ArrayRef<llvm::Type*>(types.data(), types.data() + types.size()),
      false);

  m_function_types[_function] = ft;

  llvm::Function* f = llvm::cast<llvm::Function>(
      m_module->getOrInsertFunction(_function->m_mangled_name.c_str(), ft));
  f->addFnAttr(llvm::Attribute::NoUnwind);
  m_functions[_function] = f;
  return true;
}

bool internal::jit_compiler_context::declare_function(
    const ast_function* _function) {
  if (!_function->m_scope) {
    return true;
  }

  llvm::Function* function = m_functions[_function];
  m_current_function = function;
  m_function_parameters.clear();

  llvm::BasicBlock* entry =
      llvm::BasicBlock::Create(m_context, "entry", function);
  llvm::IRBuilder<> builder(entry);
  m_function_builder = &builder;

  auto arg_it = function->arg_begin();
  for (const auto& arg : _function->m_parameters) {
    llvm::Type* t = get_type(arg.m_type);
    if (!t) {
      return false;
    }
    containers::string param_name = containers::string(m_allocator, "_");
    param_name += arg.m_name;
    arg_it->setName(param_name.c_str());
    llvm::Value* v = m_function_builder->CreateAlloca(t, 0, arg.m_name.c_str());
    llvm::Value* x = &(*arg_it);
    ++arg_it;
    m_function_builder->CreateStore(x, v);
    m_function_parameters[&arg] = v;
  }

  return write_scope_block(_function->m_scope.get());
}

bool internal::jit_compiler_context::prepare_vtable(const ast_vtable* _vtable) {
  llvm::ArrayType* vtable_type =
      llvm::ArrayType::get(m_voidfn_ptr_t, _vtable->m_functions.size());
  llvm::GlobalVariable* c = llvm::dyn_cast<llvm::GlobalVariable>(
      m_module->getOrInsertGlobal(_vtable->m_name.c_str(), vtable_type));

  containers::dynamic_array<llvm::Constant*> init(m_allocator);
  init.reserve(_vtable->m_functions.size());
  for (size_t i = 0; i < _vtable->m_functions.size(); ++i) {
    init.push_back(llvm::ConstantExpr::getCast(llvm::Instruction::BitCast,
        m_functions[_vtable->m_functions[i]], m_voidfn_ptr_t));
  }
  c->setConstant(true);
  c->setInitializer(llvm::ConstantArray::get(vtable_type, array_ref(init)));
  m_vtables[_vtable] = c;
  return true;
}

llvm::Type* internal::jit_compiler_context::get_type(const ast_type* _type) {
  auto type = m_types.find(_type);
  if (type == m_types.end()) {
    if (!decode_type(_type)) {
      return nullptr;
    }
    return m_types[_type];
  }
  return type->second;
}

llvm::Type* internal::jit_compiler_context::get_function_ptr_type(
    const ast_type* _type) {
  auto type = m_types.find(_type);
  if (type == m_types.end()) {
    if (!decode_type(_type)) {
      return nullptr;
    }
  }
  return m_types[_type];
}

bool internal::jit_compiler_context::decode_type(const ast_type* _type) {
  if (m_types.find(_type) != m_types.end()) {
    return true;
  }
  switch (_type->m_classification) {
    case ast_type_classification::primitive:
      switch (_type->m_builtin) {
        case builtin_type::bool_type:
          m_types[_type] = m_bool_t;
          return true;
        case builtin_type::integral_type:
          switch (_type->m_bit_width) {
            case 8:
              m_types[_type] = m_uint8_t;
              return true;
            case 32:
              m_types[_type] = m_int32_t;
              return true;
            default:
              WN_RELEASE_ASSERT(false, "Unimplemented bit width");
              return false;
          }
        case builtin_type::floating_point_type:
          switch (_type->m_bit_width) {
            case 32:
              m_types[_type] = m_float_t;
              return true;
            default:
              WN_RELEASE_ASSERT(false, "Unimplemented bit width");
              return false;
          }
        case builtin_type::nullptr_type:
          m_types[_type] = m_size_t;
          return true;
        case builtin_type::size_type:
          m_types[_type] = m_size_t;
          return true;
        case builtin_type::void_type:
          m_types[_type] = m_void_t;
          return true;
        case builtin_type::void_ptr_type:
          m_types[_type] = m_void_ptr_t;
          return true;
        case builtin_type::vtable_type:
          m_types[_type] = m_voidfn_ptr_t->getPointerTo(0);
          return true;
        case builtin_type::not_builtin:
          WN_RELEASE_ASSERT(false, "You shouldn't get here");
          return false;
        default:
          WN_RELEASE_ASSERT(false, "Unhandled builtin type");
          return false;
      }
    case ast_type_classification::static_array: {
      if (!decode_type(_type->m_implicitly_contained_type)) {
        return false;
      }
      auto child_type = m_types[_type->m_implicitly_contained_type];
      llvm::StructType* s =
          llvm::StructType::create({llvm::IntegerType::getInt32Ty(m_context),
              llvm::ArrayType::get(child_type, 0)});
      s->setName(_type->m_name.c_str());
      m_types[_type] = s->getPointerTo(0);
    }
      return true;
    case ast_type_classification::dynamic_array: {
      if (!decode_type(_type->m_implicitly_contained_type)) {
        return false;
      }
      auto child_type = m_types[_type->m_implicitly_contained_type];
      llvm::StructType* s =
          llvm::StructType::create({llvm::IntegerType::getInt32Ty(m_context),
              child_type->getPointerTo()});
      s->setName(_type->m_name.c_str());
      m_types[_type] = s->getPointerTo(0);
    }
      return true;
    case ast_type_classification::reference: {
      if (!decode_type(_type->m_implicitly_contained_type)) {
        return false;
      }
      m_types[_type] =
          m_types[_type->m_implicitly_contained_type]->getPointerTo();
    }
      return true;
    case ast_type_classification::shared_reference: {
      if (!decode_type(_type->m_implicitly_contained_type)) {
        return false;
      }
      m_types[_type] =
          m_types[_type->m_implicitly_contained_type]->getPointerTo(0);
    }
      return true;
    case ast_type_classification::weak_reference: {
      WN_RELEASE_ASSERT(false, "Todo: Handle weak pointers");
      return false;
    }
    case ast_type_classification::struct_type: {
      containers::dynamic_array<llvm::Type*> types(m_allocator);
      for (auto& t : _type->m_structure_members) {
        if (!decode_type(t->m_type)) {
          return false;
        }
        types.push_back(m_types[t->m_type]);
      }
      llvm::StructType* s =
          llvm::StructType::create(llvm::ArrayRef<llvm::Type*>(
              types.data(), types.data() + types.size()));
      s->setName(_type->m_name.c_str());
      m_types[_type] = s;
    }
      return true;
    case ast_type_classification::function_pointer: {
      containers::dynamic_array<llvm::Type*> types(m_allocator);
      bool skip = true;
      llvm::Type* ret_type = nullptr;
      for (auto& t : _type->m_contained_types) {
        if (!decode_type(t)) {
          return false;
        }
        if (skip) {
          ret_type = m_types[t];
          skip = false;
          continue;
        }

        types.push_back(m_types[t]);
      }
      llvm::FunctionType* t = llvm::FunctionType::get(ret_type,
          llvm::ArrayRef<llvm::Type*>(
              types.data(), types.data() + types.size()),
          false);
      m_types[_type] = t->getPointerTo(0);
      return true;
    }
    default:
      WN_RELEASE_ASSERT(false, "We shouldn't get here");
      return false;
  }
}

llvm::Value* internal::jit_compiler_context::get_expression(
    const ast_expression* _expression) {
  if (!write_temporaries(_expression)) {
    return nullptr;
  }
  llvm::Value* v = nullptr;
  switch (_expression->m_node_type) {
    case ast_node_type::ast_binary_expression: {
      v = get_binary_expression(cast_to<ast_binary_expression>(_expression));
    } break;
    case ast_node_type::ast_constant: {
      v = get_constant(cast_to<ast_constant>(_expression));
    } break;
    case ast_node_type::ast_id:
      v = get_id(cast_to<ast_id>(_expression));
      break;
    case ast_node_type::ast_array_access_expression: {
      WN_RELEASE_ASSERT(false, "Unimplemented statement type");
    } break;
    case ast_node_type::ast_member_access_expression: {
      v = get_member_access(cast_to<ast_member_access_expression>(_expression));
    } break;
    case ast_node_type::ast_unary_expression: {
      WN_RELEASE_ASSERT(false, "Unimplemented statement type");
    } break;
    case ast_node_type::ast_cast_expression: {
      v = get_cast(cast_to<ast_cast_expression>(_expression));
    } break;
    case ast_node_type::ast_builtin_expression: {
      v = get_builtin(cast_to<ast_builtin_expression>(_expression));
    } break;
    case ast_node_type::ast_function_call_expression: {
      v = get_function_call(cast_to<ast_function_call_expression>(_expression));
    } break;
    case ast_node_type::ast_function_pointer_expression: {
      v = get_function_pointer(
          cast_to<ast_function_pointer_expression>(_expression));
      break;
    }
    default:
      WN_RELEASE_ASSERT(false, "You should never get here");
  }
  if (!write_temporary_cleanup(_expression)) {
    return nullptr;
  }
  return v;
}

llvm::Value* internal::jit_compiler_context::get_binary_expression(
    const ast_binary_expression* _bin) {
  llvm::Value* lhs = get_expression(_bin->m_lhs.get());
  llvm::Value* rhs = get_expression(_bin->m_rhs.get());

  llvm::Type* lhs_type = get_type(_bin->m_lhs->m_type);
  const bool is_float = lhs_type == m_float_t;
  const bool is_signed = lhs_type == m_int32_t;

  switch (_bin->m_binary_type) {
    case ast_binary_type::add:
      return m_function_builder->CreateAdd(lhs, rhs);
    case ast_binary_type::sub:
      return m_function_builder->CreateSub(lhs, rhs);
    case ast_binary_type::div:
      if (is_float) {
        return m_function_builder->CreateFDiv(lhs, rhs);
      }
      if (is_signed) {
        return m_function_builder->CreateSDiv(lhs, rhs);
      }
      return m_function_builder->CreateUDiv(lhs, rhs);
    case ast_binary_type::mult:
      if (is_float) {
        return m_function_builder->CreateFMul(lhs, rhs);
      }
      return m_function_builder->CreateMul(lhs, rhs);
    case ast_binary_type::mod:
      if (is_float) {
        return m_function_builder->CreateFRem(lhs, rhs);
      }
      if (is_signed) {
        return m_function_builder->CreateSRem(lhs, rhs);
      }
      return m_function_builder->CreateURem(lhs, rhs);
      break;
    case ast_binary_type::lt:
      if (is_float) {
        return m_function_builder->CreateFCmpOLT(lhs, rhs);
      }
      if (is_signed) {
        return m_function_builder->CreateICmpSLT(lhs, rhs);
      }
      return m_function_builder->CreateICmpULT(lhs, rhs);
    case ast_binary_type::gt:
      if (is_float) {
        return m_function_builder->CreateFCmpOGT(lhs, rhs);
      }
      if (is_signed) {
        return m_function_builder->CreateICmpSGT(lhs, rhs);
      }
      return m_function_builder->CreateICmpUGT(lhs, rhs);
    case ast_binary_type::lte:
      if (is_float) {
        return m_function_builder->CreateFCmpOLE(lhs, rhs);
      }
      if (is_signed) {
        return m_function_builder->CreateICmpSLE(lhs, rhs);
      }
      return m_function_builder->CreateICmpULE(lhs, rhs);
    case ast_binary_type::gte:
      if (is_float) {
        return m_function_builder->CreateFCmpOGE(lhs, rhs);
      }
      if (is_signed) {
        return m_function_builder->CreateICmpUGE(lhs, rhs);
      }
      return m_function_builder->CreateICmpUGE(lhs, rhs);
    case ast_binary_type::eq:
      if (is_float) {
        return m_function_builder->CreateFCmpOEQ(lhs, rhs);
      }
      return m_function_builder->CreateICmpEQ(lhs, rhs);
    case ast_binary_type::neq:
      if (is_float) {
        return m_function_builder->CreateFCmpONE(lhs, rhs);
      }
      return m_function_builder->CreateICmpNE(lhs, rhs);
      break;
  }
  return nullptr;
}

llvm::Function* internal::jit_compiler_context::get_function(
    const ast_function* _func) {
  return m_functions[_func];
}

llvm::Value* internal::jit_compiler_context::get_function_from_ptr(
    const ast_expression* _func) {
  llvm::Value* expr = get_expression(_func);

  llvm::Value* fn = m_function_builder->CreateIntToPtr(
      expr, get_function_ptr_type(_func->m_type));

  return fn;
}

llvm::Value* internal::jit_compiler_context::get_function_pointer(
    const ast_function_pointer_expression* _expr) {
  return m_functions[_expr->m_function];
}

llvm::Value* internal::jit_compiler_context::get_virtual_function(
    const ast_function_call_expression* _call, llvm::Value* _this) {
  uint32_t vtable_offset =
      _call->m_parameters[0]
          ->m_type->m_implicitly_contained_type->m_vtable_index;

  llvm::Value* gep[2] = {i32(0), i32(vtable_offset)};
  llvm::Value* gep2[1] = {i32(_call->m_function->m_virtual_index)};

  return m_function_builder->CreateBitCast(
      m_function_builder->CreateLoad(m_function_builder->CreateInBoundsGEP(
          m_function_builder->CreateLoad(m_function_builder->CreateInBoundsGEP(
              _this, llvm::ArrayRef<llvm::Value*>(&gep[0], 2))),
          llvm::ArrayRef<llvm::Value*>(&gep2[0], 1))),
      m_functions[_call->m_function]->getType());
}

llvm::Value* internal::jit_compiler_context::get_function_call(
    const ast_function_call_expression* _bin) {
  containers::dynamic_array<llvm::Value*> expressions =
      containers::dynamic_array<llvm::Value*>(m_allocator);
  expressions.resize(_bin->m_parameters.size());

  for (size_t i = 0; i < _bin->m_parameters.size(); ++i) {
    expressions[i] = get_expression(_bin->m_parameters[i].get());
  }

  llvm::Value* fn = nullptr;
  if (_bin->m_function) {
    if (_bin->m_function->m_is_override || _bin->m_function->m_is_virtual) {
      fn = get_virtual_function(_bin, expressions[0]);
    } else {
      fn = get_function(_bin->m_function);
    }
  } else if (_bin->m_function_ptr) {
    fn = get_function_from_ptr(_bin->m_function_ptr.get());
  } else {
    WN_DEBUG_ASSERT(false, "Should never arrive here");
  }

  if (!fn) {
    return nullptr;
  }

  // We cannot name void values, so split this based on return type.
  if (get_type(_bin->m_type) != m_void_t) {
    return m_function_builder->CreateCall(fn,
        llvm::ArrayRef<llvm::Value*>(expressions.data(), expressions.size()),
        _bin->m_function->m_name.c_str());
  } else {
    return m_function_builder->CreateCall(fn,
        llvm::ArrayRef<llvm::Value*>(expressions.data(), expressions.size()));
  }
}

llvm::Value* internal::jit_compiler_context::get_cast(
    const ast_cast_expression* _expression) {
  llvm::Value* expr = get_expression(_expression->m_base_expression.get());
  if (_expression->m_type->m_classification ==
          ast_type_classification::reference &&
      _expression->m_base_expression->m_type->m_classification ==
          ast_type_classification::struct_type) {
    if (_expression->m_type->m_implicitly_contained_type ==
        _expression->m_base_expression->m_type) {
      llvm::LoadInst* load = llvm::dyn_cast<llvm::LoadInst>(expr);
      if (!load) {
        return nullptr;
      }
      expr = load->getOperand(0);
      load->removeFromParent();
      delete load;
      return expr;
    }
  }
  if (_expression->m_type->m_classification ==
          ast_type_classification::struct_type &&
      _expression->m_base_expression->m_type->m_classification ==
          ast_type_classification::reference) {
    if (_expression->m_base_expression->m_type->m_implicitly_contained_type ==
        _expression->m_type) {
      return m_function_builder->CreateLoad(expr);
    }
  }

  bool is_int_to_ptr = _expression->m_type->m_classification ==
                           ast_type_classification::reference &&
                       _expression->m_base_expression->m_type->m_builtin ==
                           builtin_type::nullptr_type;
  is_int_to_ptr |= _expression->m_type->m_classification ==
                       ast_type_classification::shared_reference &&
                   _expression->m_base_expression->m_type->m_builtin ==
                       builtin_type::nullptr_type;

  is_int_to_ptr |=
      _expression->m_type->m_builtin == builtin_type::void_ptr_type &&
      _expression->m_base_expression->m_type->m_builtin ==
          builtin_type::nullptr_type;

  is_int_to_ptr |= _expression->m_type->m_classification ==
                       ast_type_classification::function_pointer &&
                   _expression->m_base_expression->m_type->m_builtin ==
                       builtin_type::nullptr_type;

  bool is_bitcast = _expression->m_type->m_classification ==
                        ast_type_classification::reference &&
                    _expression->m_base_expression->m_type->m_classification ==
                        ast_type_classification::reference;
  is_bitcast |= _expression->m_type->m_classification ==
                    ast_type_classification::shared_reference &&
                _expression->m_base_expression->m_type->m_classification ==
                    ast_type_classification::shared_reference;
  is_bitcast |= _expression->m_type->m_classification ==
                    ast_type_classification::reference &&
                _expression->m_base_expression->m_type->m_classification ==
                    ast_type_classification::shared_reference;
  is_bitcast |= _expression->m_type->m_builtin == builtin_type::void_ptr_type &&
                _expression->m_base_expression->m_type->m_classification ==
                    ast_type_classification::reference;
  is_bitcast |= _expression->m_type->m_builtin == builtin_type::void_ptr_type &&
                _expression->m_base_expression->m_type->m_classification ==
                    ast_type_classification::shared_reference;
  is_bitcast |= _expression->m_type->m_classification ==
                    ast_type_classification::reference &&
                _expression->m_base_expression->m_type->m_builtin ==
                    builtin_type::void_ptr_type;
  is_bitcast |= _expression->m_type->m_classification ==
                    ast_type_classification::shared_reference &&
                _expression->m_base_expression->m_type->m_builtin ==
                    builtin_type::void_ptr_type;
  is_bitcast |= _expression->m_type->m_classification ==
                    ast_type_classification::function_pointer &&
                _expression->m_base_expression->m_type->m_classification ==
                    ast_type_classification::function_pointer;

  is_bitcast |= _expression->m_type->m_builtin == builtin_type::void_ptr_type &&
                _expression->m_base_expression->m_type->m_classification ==
                    ast_type_classification::shared_reference;

  if (is_bitcast) {
    return m_function_builder->CreateBitCast(
        expr, get_type(_expression->m_type));
  }

  if (is_int_to_ptr) {
    return m_function_builder->CreateIntToPtr(
        expr, get_type(_expression->m_type));
  }

  WN_RELEASE_ASSERT(false, "Unknown cast");
  return nullptr;
}

llvm::Value* internal::jit_compiler_context::get_builtin(
    const ast_builtin_expression* _builtin) {
  switch (_builtin->m_builtin_type) {
    case builtin_expression_type::size_of: {
      llvm::Type* t = get_type(_builtin->m_extra_types[0]);
      if (!t) {
        return nullptr;
      }
      return llvm::ConstantInt::get(
          m_size_t, m_data_layout.getTypeAllocSize(t));
    }
    case builtin_expression_type::atomic_inc: {
      llvm::Value* v = get_expression(_builtin->m_expressions[0].get());
      if (!v) {
        return nullptr;
      }
      llvm::LoadInst* load = llvm::dyn_cast<llvm::LoadInst>(v);
      if (!v) {
        return nullptr;
      }
      v = load->getOperand(0);
      load->removeFromParent();
      delete load;

      return m_function_builder->CreateAtomicRMW(
          llvm::AtomicRMWInst::BinOp::Add, v,
          llvm::ConstantInt::get(m_size_t, 1), llvm::AtomicOrdering::Monotonic,
          llvm::SynchronizationScope::CrossThread);
      break;
    }
    case builtin_expression_type::atomic_dec: {
      llvm::Value* v = get_expression(_builtin->m_expressions[0].get());
      if (!v) {
        return nullptr;
      }
      llvm::LoadInst* load = llvm::dyn_cast<llvm::LoadInst>(v);
      if (!v) {
        return nullptr;
      }
      v = load->getOperand(0);
      load->removeFromParent();
      delete load;

      return m_function_builder->CreateAtomicRMW(
          llvm::AtomicRMWInst::BinOp::Sub, v,
          llvm::ConstantInt::get(m_size_t, 1), llvm::AtomicOrdering::Release,
          llvm::SynchronizationScope::CrossThread);
    }
    case builtin_expression_type::pointer_to_shared: {
      llvm::Value* v = get_expression(_builtin->m_expressions[0].get());
      if (!v) {
        return nullptr;
      }
      llvm::Type* shared_obj_type =
          get_type(_builtin->m_type->m_implicitly_contained_type);
      llvm::Type* shared_obj_ptr_type = get_type(_builtin->m_type);
      if (!shared_obj_type || !shared_obj_ptr_type) {
        return nullptr;
      }

      v = m_function_builder->CreatePtrToInt(v, m_size_t);
      llvm::Value* s = llvm::ConstantInt::get(
          m_size_t, m_data_layout.getTypeAllocSize(shared_obj_type));
      v = m_function_builder->CreateSub(v, s);
      return m_function_builder->CreateIntToPtr(v, shared_obj_ptr_type);
    }
    case builtin_expression_type::shared_to_pointer: {
      llvm::Value* v = get_expression(_builtin->m_expressions[0].get());
      if (!v) {
        return nullptr;
      }
      llvm::Type* shared_obj_type = get_type(
          _builtin->m_expressions[0]->m_type->m_implicitly_contained_type);
      llvm::Type* vp_type = get_type(_builtin->m_type);

      if (!shared_obj_type || !vp_type) {
        return nullptr;
      }

      v = m_function_builder->CreatePtrToInt(v, m_size_t);
      llvm::Value* s = llvm::ConstantInt::get(
          m_size_t, m_data_layout.getTypeAllocSize(shared_obj_type));
      v = m_function_builder->CreateAdd(v, s);
      return m_function_builder->CreateIntToPtr(v, vp_type);
    }
    default:
      WN_RELEASE_ASSERT(false, "Should not reach here");
  }
  return nullptr;
}

llvm::Value* internal::jit_compiler_context::get_member_access(
    const ast_member_access_expression* _access) {
  llvm::Value* v = get_expression(_access->m_base_expression.get());
  if (!v) {
    return nullptr;
  }

  if (_access->m_base_expression->m_type->m_classification !=
          ast_type_classification::reference &&
      _access->m_base_expression->m_type->m_classification !=
          ast_type_classification::shared_reference) {
    llvm::LoadInst* load = llvm::dyn_cast<llvm::LoadInst>(v);
    if (!load) {
      return nullptr;
    }
    v = load->getOperand(0);
    load->removeFromParent();
    delete load;
  }

  llvm::Value* gep[2] = {i32(0), i32(_access->m_member_offset)};
  v = m_function_builder->CreateLoad(
      m_function_builder->CreateInBoundsGEP(
          v, llvm::ArrayRef<llvm::Value*>(&gep[0], 2)),
      _access->m_member_name.c_str());
  return v;
}

llvm::Value* internal::jit_compiler_context::get_constant(
    const ast_constant* _const) {
  llvm::Type* t = get_type(_const->m_type);
  if (!t) {
    return nullptr;
  }

  if (_const->m_type->m_builtin == builtin_type::nullptr_type) {
    return llvm::ConstantInt::get(m_size_t, 0);
  } else if (_const->m_type->m_builtin == builtin_type::vtable_type) {
    return m_function_builder->CreateBitCast(
        m_vtables[_const->m_node_value.m_vtable],
        m_voidfn_ptr_t->getPointerTo(0));
  }

  if (t == m_int32_t) {
    return llvm::ConstantInt::get(t, _const->m_node_value.m_integer);
  } else if (t == m_uint8_t) {
    return llvm::ConstantInt::get(t, _const->m_node_value.m_char);
  } else if (t == m_float_t) {
    return llvm::ConstantFP::get(t, _const->m_node_value.m_float);
  } else if (t == m_bool_t) {
    return llvm::ConstantInt::get(t, _const->m_node_value.m_bool ? 1 : 0);
  } else if (t == m_size_t) {
    return llvm::ConstantInt::get(t, _const->m_node_value.m_integer);
  }
  WN_RELEASE_ASSERT(false, "Unimplemented constant type");
  return nullptr;
}

llvm::Value* internal::jit_compiler_context::get_id(const ast_id* _id) {
  if (_id->m_declaration) {
    auto decl = m_declarations.find(_id->m_declaration);
    if (decl == m_declarations.end()) {
      return nullptr;
    }
    return m_function_builder->CreateLoad(
        decl->second, _id->m_declaration->m_name.c_str());
  } else if (_id->m_function_parameter) {
    auto param = m_function_parameters.find(_id->m_function_parameter);
    if (param == m_function_parameters.end()) {
      return nullptr;
    }
    return m_function_builder->CreateLoad(
        param->second, _id->m_function_parameter->m_name.c_str());
  } else {
    WN_RELEASE_ASSERT(false, "Unknown id source");
  }
  return nullptr;
}

bool internal::jit_compiler_context::write_statement(
    const ast_statement* _statement) {
  switch (_statement->m_node_type) {
    case ast_node_type::ast_declaration: {
      if (!write_declaration(cast_to<ast_declaration>(_statement))) {
        return false;
      }
    } break;
    case ast_node_type::ast_scope_block: {
      if (!write_scope_block(cast_to<ast_scope_block>(_statement))) {
        return false;
      }
    } break;
    case ast_node_type::ast_evaluate_expression: {
      if (!write_evaluate_expression(
              cast_to<ast_evaluate_expression>(_statement))) {
        return false;
      }
    } break;
    case ast_node_type::ast_assignment: {
      if (!write_assignment(cast_to<ast_assignment>(_statement))) {
        return false;
      }
    } break;
    case ast_node_type::ast_loop: {
      if (!write_loop(cast_to<ast_loop>(_statement))) {
        return false;
      }
    } break;
    case ast_node_type::ast_break: {
      if (!write_break(cast_to<ast_break>(_statement))) {
        return false;
      }
    } break;
    case ast_node_type::ast_continue_instruction: {
      if (!write_continue(cast_to<ast_continue_instruction>(_statement))) {
        return false;
      }
    } break;
    case ast_node_type::ast_if_chain: {
      if (!write_if_chain(cast_to<ast_if_chain>(_statement))) {
        return false;
      }
    } break;
    case ast_node_type::ast_return_instruction:
      if (!write_return(cast_to<ast_return_instruction>(_statement))) {
        return false;
      }
      break;
    case ast_node_type::ast_builtin_statement:
      if (!write_builtin_statement(
              cast_to<ast_builtin_statement>(_statement))) {
        return false;
      }
      break;
    default:
      WN_RELEASE_ASSERT(false, "Unknown statement type");
  }
  return true;
}

bool internal::jit_compiler_context::write_assignment(
    const ast_assignment* _assign) {
  llvm::Value* lhs = get_expression(_assign->m_lhs.get());
  if (!lhs) {
    return false;
  }
  llvm::Value* rhs = get_expression(_assign->m_rhs.get());
  if (!rhs) {
    return false;
  }
  llvm::LoadInst* load = llvm::dyn_cast<llvm::LoadInst>(lhs);
  if (!load) {
    return false;
  }
  lhs = load->getOperand(0);
  m_function_builder->CreateStore(rhs, lhs);
  load->removeFromParent();
  delete load;
  return true;
}

bool internal::jit_compiler_context::write_loop(const ast_loop* _loop) {
  llvm::BasicBlock* loop_condition =
      _loop->m_pre_condition ? llvm::BasicBlock::Create(m_context,
                                   "loop_condition", m_current_function)
                             : nullptr;

  llvm::BasicBlock* loop_body =
      llvm::BasicBlock::Create(m_context, "loop_body", m_current_function);

  llvm::BasicBlock* loop_end =
      llvm::BasicBlock::Create(m_context, "loop_end", m_current_function);

  llvm::BasicBlock* increments =
      _loop->m_increment_statements
          ? llvm::BasicBlock::Create(m_context, "continue", m_current_function)
          : nullptr;

  llvm::BasicBlock* continue_target =
      increments ? increments : (loop_condition ? loop_condition : loop_body);

  llvm::BasicBlock* first_block = loop_condition ? loop_condition : loop_body;

  loop_data dat{continue_target, loop_end};
  m_loops[_loop] = dat;

  m_function_builder->CreateBr(first_block);

  if (loop_condition) {
    m_function_builder->SetInsertPoint(loop_condition);
    llvm::Value* cond = get_expression(_loop->m_pre_condition.get());

    if (!cond) {
      return false;
    }

    m_function_builder->CreateCondBr(cond, loop_body, loop_end);
  }

  m_function_builder->SetInsertPoint(loop_body);

  if (!write_scope_block(_loop->m_body.get())) {
    return false;
  }

  if (!_loop->m_body->m_returns && !_loop->m_body->m_breaks) {
    if (_loop->m_post_condition) {
      llvm::Value* cond = get_expression(_loop->m_post_condition.get());
      if (!cond) {
        return false;
      }
      m_function_builder->CreateCondBr(cond, continue_target, loop_end);
    } else {
      m_function_builder->CreateBr(continue_target);
    }
  }

  if (increments) {
    m_function_builder->SetInsertPoint(increments);
    if (!write_scope_block(_loop->m_increment_statements.get())) {
      return false;
    }
    m_function_builder->CreateBr(first_block);
  }

  m_function_builder->SetInsertPoint(loop_end);
  m_loops.erase(_loop);
  return true;
}

bool internal::jit_compiler_context::write_break(const ast_break* _break) {
  llvm::BasicBlock* target = m_loops[_break->m_break_loop].break_target;
  m_function_builder->CreateBr(target);
  return true;
}

bool internal::jit_compiler_context::write_continue(
    const ast_continue_instruction* _cont) {
  llvm::BasicBlock* target = m_loops[_cont->m_continue_loop].continue_target;
  m_function_builder->CreateBr(target);
  return true;
}

bool internal::jit_compiler_context::write_builtin_statement(
    const ast_builtin_statement* _st) {
  switch (_st->m_builtin_type) {
    case builtin_statement_type::atomic_fence:
      m_function_builder->CreateFence(llvm::AtomicOrdering::Acquire);
      return true;
    default:
      WN_RELEASE_ASSERT(false, "Unknown builtin statement type");
  }
  return false;
}

bool internal::jit_compiler_context::write_evaluate_expression(
    const ast_evaluate_expression* _expr) {
  if (!get_expression(_expr->m_expr.get())) {
    return false;
  }
  return true;
}

bool internal::jit_compiler_context::write_if_chain(
    const ast_if_chain* _chain) {
  llvm::BasicBlock* done =
      llvm::BasicBlock::Create(m_context, "if_done", m_current_function);

  uint32_t total_blocks = static_cast<uint32_t>(_chain->m_conditionals.size()) +
                          (_chain->m_else_block != nullptr);

  llvm::BasicBlock* next =
      total_blocks == 1
          ? done
          : llvm::BasicBlock::Create(m_context, "if_next", m_current_function);

  bool not_everyone_broke = !_chain->m_else_block;

  for (auto& cond : _chain->m_conditionals) {
    llvm::Value* c = get_expression(cond.m_expr.get());
    llvm::BasicBlock* this_block =
        llvm::BasicBlock::Create(m_context, "if_block", m_current_function);
    m_function_builder->CreateCondBr(c, this_block, next);
    m_function_builder->SetInsertPoint(this_block);
    if (!write_scope_block(cond.m_scope.get())) {
      return false;
    }
    if (!cond.m_scope->m_returns && !cond.m_scope->m_breaks) {
      m_function_builder->CreateBr(done);
      not_everyone_broke = true;
    }
    m_function_builder->SetInsertPoint(next);
    if (--total_blocks > 1) {
      next = llvm::BasicBlock::Create(m_context, "if_next", m_current_function);
    } else if (total_blocks == 1) {
      next = done;
    }
  }

  if (_chain->m_else_block) {
    if (!write_scope_block(_chain->m_else_block.get())) {
      return false;
    }
    if (!_chain->m_else_block->m_returns && !_chain->m_else_block->m_breaks) {
      m_function_builder->CreateBr(done);
      not_everyone_broke = true;
    }
  }

  if (not_everyone_broke) {
    m_function_builder->SetInsertPoint(done);
  } else {
    done->removeFromParent();
    delete done;
  }

  return true;
}

bool internal::jit_compiler_context::write_declaration(
    const ast_declaration* _declaration) {
  llvm::Type* t = get_type(_declaration->m_type);
  if (!t) {
    return false;
  }
  llvm::Value* decl =
      m_function_builder->CreateAlloca(t, 0, _declaration->m_name.c_str());
  m_declarations[_declaration] = decl;
  if (_declaration->m_initializer) {
    llvm::Value* v = get_expression(_declaration->m_initializer.get());
    if (!v) {
      return false;
    }
    m_function_builder->CreateStore(v, decl);
  }
  return true;
}

bool internal::jit_compiler_context::write_return(
    const ast_return_instruction* _return) {
  if (_return->m_return_expr) {
    llvm::Value* expr = get_expression(_return->m_return_expr.get());
    if (!expr) {
      return false;
    }
    m_function_builder->CreateRet(expr);
    return true;
  }
  m_function_builder->CreateRetVoid();
  return true;
}

bool internal::jit_compiler_context::write_scope_block(
    const ast_scope_block* _scope) {
  for (const auto& it : _scope->m_statements) {
    if (!write_statement(it.get())) {
      return false;
    }
  }
  return true;
}

bool internal::jit_compiler_context::write_temporaries(
    const ast_expression* _expression) {
  for (auto& decl : _expression->m_temporaries) {
    if (!write_declaration(decl.get())) {
      return false;
    }
  }
  return true;
}

bool internal::jit_compiler_context::write_temporary_cleanup(
    const ast_expression* _expression) {
  for (auto& expr : _expression->m_destroy_expressions) {
    if (!get_expression(expr.get())) {
      // WE don't actually want to DO anything with the expressions.
      // They don't feed into other expressions, they are
      // just expected to have side-effects.
      return false;
    }
  }
  return true;
}

}  // namespace scripting
}  // namespace wn