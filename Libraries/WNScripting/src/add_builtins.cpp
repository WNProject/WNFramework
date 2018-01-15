// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/inc/WNHashMap.h"
#include "WNContainers/inc/WNHashSet.h"
#include "WNScripting/inc/WNNodeTypes.h"
#include "WNScripting/inc/internal/ast_convertor_context.h"
#include "WNScripting/inc/parse_ast_convertor.h"

namespace wn {
namespace scripting {

namespace {
memory::unique_ptr<ast_declaration> make_builtin_declaration(
    memory::allocator* _allocator, containers::string_view _name,
    ast_type* _type) {
  auto decl = memory::make_unique<ast_declaration>(_allocator, nullptr);
  decl->m_type = _type;
  decl->m_name = _name.to_string(_allocator);
  return core::move(decl);
}
}  // namespace

void parse_ast_convertor::convertor_context::add_builtin_types() {
  auto uint32_type = memory::make_unique<ast_type>(m_allocator);
  uint32_type->m_name = containers::string(m_allocator, "Int");
  uint32_type->m_builtin = builtin_type::integral_type;
  uint32_type->m_bit_width = 32;
  uint32_type->m_is_arithmetic_type = true;
  uint32_type->m_is_comparable_type = true;
  uint32_type->calculate_mangled_name(m_allocator);
  m_integral_types[32] = core::move(uint32_type);

  auto uint8_type = memory::make_unique<ast_type>(m_allocator);
  uint8_type->m_name = containers::string(m_allocator, "Char");
  uint8_type->m_builtin = builtin_type::integral_type;
  uint8_type->m_bit_width = 8;
  uint8_type->m_is_arithmetic_type = true;
  uint8_type->m_is_comparable_type = true;
  uint8_type->calculate_mangled_name(m_allocator);
  m_integral_types[8] = core::move(uint8_type);

  auto float_type = memory::make_unique<ast_type>(m_allocator);
  float_type->m_name = containers::string(m_allocator, "Float");
  float_type->m_builtin = builtin_type::floating_point_type;
  float_type->m_bit_width = 32;
  float_type->m_is_arithmetic_type = true;
  float_type->m_is_comparable_type = true;
  float_type->calculate_mangled_name(m_allocator);
  m_float_types[32] = core::move(float_type);

  m_void_t = memory::make_unique<ast_type>(m_allocator);
  m_void_t->m_name = containers::string(m_allocator, "Void");
  m_void_t->m_builtin = builtin_type::void_type;
  m_void_t->calculate_mangled_name(m_allocator);

  m_void_ptr_t = memory::make_unique<ast_type>(m_allocator);
  m_void_ptr_t->m_name = containers::string(m_allocator, "VoidPtr");
  m_void_ptr_t->m_builtin = builtin_type::void_ptr_type;
  m_void_ptr_t->m_implicitly_contained_type = m_void_t.get();
  m_void_ptr_t->calculate_mangled_name(m_allocator);

  m_bool_t = memory::make_unique<ast_type>(m_allocator);
  m_bool_t->m_name = containers::string(m_allocator, "Bool");
  m_bool_t->m_builtin = builtin_type::bool_type;
  m_bool_t->calculate_mangled_name(m_allocator);
  m_bool_t->m_is_comparable_type = true;

  m_size_t = memory::make_unique<ast_type>(m_allocator);
  m_size_t->m_name = containers::string(m_allocator, "Size");
  m_size_t->m_builtin = builtin_type::size_type;
  m_size_t->calculate_mangled_name(m_allocator);
  m_size_t->m_is_arithmetic_type = true;
  m_size_t->m_is_comparable_type = true;

  m_nullptr_t = memory::make_unique<ast_type>(m_allocator);
  m_nullptr_t->m_name = containers::string(m_allocator, "Nullptr");
  m_nullptr_t->m_builtin = builtin_type::nullptr_type;
  m_nullptr_t->calculate_mangled_name(m_allocator);
  m_nullptr_t->m_is_comparable_type = true;

  m_function_t = memory::make_unique<ast_type>(m_allocator);
  m_function_t->m_name = containers::string(m_allocator, "Function");
  m_function_t->m_builtin = builtin_type::unresolved_function_type;
  m_function_t->calculate_mangled_name(m_allocator);

  m_vtable_t = memory::make_unique<ast_type>(m_allocator);
  m_vtable_t->m_name = containers::string(m_allocator, "__Vtable");
  m_vtable_t->m_builtin = builtin_type::vtable_type;
  m_vtable_t->calculate_mangled_name(m_allocator);

  m_destructor_fn_ptr_t =
      resolve_function_ptr_type(containers::dynamic_array<const ast_type*>(
          m_allocator, {m_void_t.get(), m_void_ptr_t.get()}));

  m_shared_object_header = memory::make_unique<ast_type>(m_allocator);
  m_shared_object_header->m_name =
      containers::string(m_allocator, "_wns_shared_object");
  m_shared_object_header->m_builtin = builtin_type::not_builtin;
  m_shared_object_header->m_classification =
      ast_type_classification::struct_type;
  m_shared_object_header->initialized_structure_members(m_allocator)
      .push_back(
          make_builtin_declaration(m_allocator, "ref_count", m_size_t.get()));
  m_shared_object_header->initialized_structure_members(m_allocator)
      .push_back(make_builtin_declaration(
          m_allocator, "destructor", m_destructor_fn_ptr_t));
  m_ordered_type_definitions.push_back(m_shared_object_header.get());
}

bool parse_ast_convertor::convertor_context::add_builtin_functions() {
  if (m_external_functions.find(containers::string(m_allocator, "_allocate")) ==
      m_external_functions.end()) {
    m_log->log_error("External function _allocate missing");
    return false;
  }
  if (m_external_functions.find(containers::string(m_allocator, "_free")) ==
      m_external_functions.end()) {
    m_log->log_error("External function _free missing");
    return false;
  }

  add_allocate_shared();
  add_release_shared();
  add_assign_shared();
  add_return_shared();
  return true;
}

void parse_ast_convertor::convertor_context::add_allocate_shared() {
  auto alloc =
      m_external_functions[containers::string(m_allocator, "_allocate")];

  auto fn = memory::make_unique<ast_function>(m_allocator, nullptr);
  fn->m_name = containers::string(m_allocator, "_wns_allocate_shared");
  fn->m_return_type = m_void_ptr_t.get();
  auto scope = memory::make_unique<ast_scope_block>(m_allocator, nullptr);
  scope->m_returns = true;
  auto& body = scope->initialized_statements(m_allocator);
  fn->m_scope = core::move(scope);

  fn->initialized_parameters(m_allocator)
      .emplace_back(ast_function::parameter(
          containers::string(m_allocator, "_size"), m_size_t.get()));
  fn->initialized_parameters(m_allocator)
      .emplace_back(ast_function::parameter(
          containers::string(m_allocator, "_destructor"),
          m_destructor_fn_ptr_t));

  auto szOf = memory::make_unique<ast_builtin_expression>(m_allocator, nullptr);
  szOf->m_type = m_size_t.get();
  szOf->initialized_extra_types(m_allocator)
      .push_back(m_shared_object_header.get());
  szOf->m_builtin_type = builtin_expression_type::size_of;

  auto in_sz = memory::make_unique<ast_id>(m_allocator, nullptr);
  in_sz->m_type = m_size_t.get();
  in_sz->m_function_parameter = &fn->m_parameters[0];

  auto size_sum =
      memory::make_unique<ast_binary_expression>(m_allocator, nullptr);
  size_sum->m_lhs = core::move(szOf);
  size_sum->m_rhs = core::move(in_sz);
  size_sum->m_type = m_size_t.get();
  size_sum->m_binary_type = ast_binary_type::add;

  auto params = containers::dynamic_array<memory::unique_ptr<ast_expression>>(
      m_allocator);
  params.push_back(core::move(size_sum));
  auto cast_to_soh =
      memory::make_unique<ast_cast_expression>(m_allocator, nullptr);
  cast_to_soh->m_base_expression =
      call_function(nullptr, alloc, core::move(params));
  cast_to_soh->m_type = get_reference_of(nullptr, m_shared_object_header.get(),
      ast_type_classification::reference);

  auto _decl = memory::make_unique<ast_declaration>(m_allocator, nullptr);
  _decl->m_name = containers::string(m_allocator, "_shared_obj");
  _decl->m_type = cast_to_soh->m_type;
  _decl->m_initializer = core::move(cast_to_soh);

  auto decl = _decl.get();
  body.push_back(core::move(_decl));

  auto _shared_obj = memory::make_unique<ast_id>(m_allocator, nullptr);
  _shared_obj->m_declaration = decl;
  _shared_obj->m_type = decl->m_type;

  auto _shared_obj_ref_count =
      memory::make_unique<ast_member_access_expression>(m_allocator, nullptr);
  _shared_obj_ref_count->m_member_name =
      containers::string(m_allocator, "ref_count");
  _shared_obj_ref_count->m_member_offset = 0;
  _shared_obj_ref_count->m_type = m_size_t.get();
  _shared_obj_ref_count->m_base_expression =
      clone_node(m_allocator, _shared_obj.get());

  auto const_1 = memory::make_unique<ast_constant>(m_allocator, nullptr);
  const_1->m_string_value = containers::string(m_allocator, "1");
  const_1->m_type = m_size_t.get();
  const_1->m_node_value.m_integer = 1;

  auto assign = memory::make_unique<ast_assignment>(m_allocator, nullptr);
  assign->m_lhs = core::move(_shared_obj_ref_count);
  assign->m_rhs = core::move(const_1);

  body.push_back(core::move(assign));

  auto in_fn_ptr = memory::make_unique<ast_id>(m_allocator, nullptr);
  in_fn_ptr->m_type = m_destructor_fn_ptr_t;
  in_fn_ptr->m_function_parameter = &fn->m_parameters[1];

  auto _shared_obj_destructor =
      memory::make_unique<ast_member_access_expression>(m_allocator, nullptr);
  _shared_obj_destructor->m_member_name =
      containers::string(m_allocator, "destructor");
  _shared_obj_destructor->m_member_offset = 1;
  _shared_obj_destructor->m_type = m_destructor_fn_ptr_t;
  _shared_obj_destructor->m_base_expression =
      clone_node(m_allocator, _shared_obj.get());

  assign = memory::make_unique<ast_assignment>(m_allocator, nullptr);
  assign->m_lhs = core::move(_shared_obj_destructor);
  assign->m_rhs = core::move(in_fn_ptr);
  body.push_back(core::move(assign));

  auto retVal =
      memory::make_unique<ast_builtin_expression>(m_allocator, nullptr);
  retVal->m_type = m_void_ptr_t.get();
  retVal->initialized_expressions(m_allocator)
      .push_back(core::move(_shared_obj));
  retVal->m_builtin_type = builtin_expression_type::shared_to_pointer;

  auto ret = memory::make_unique<ast_return_instruction>(m_allocator, nullptr);
  ret->m_return_expr = core::move(retVal);
  body.push_back(core::move(ret));

  fn->calculate_mangled_name(m_allocator);
  m_allocate_shared = fn.get();
  m_script_file->m_functions.push_back(core::move(fn));
}

void parse_ast_convertor::convertor_context::add_release_shared() {
  auto alloc = m_external_functions[containers::string(m_allocator, "_free")];

  auto fn = memory::make_unique<ast_function>(m_allocator, nullptr);
  fn->m_name = containers::string(m_allocator, "_wns_release");
  fn->m_return_type = m_void_t.get();

  auto scope = memory::make_unique<ast_scope_block>(m_allocator, nullptr);
  scope->m_returns = true;
  auto& outer_body = scope->initialized_statements(m_allocator);
  fn->m_scope = core::move(scope);

  fn->initialized_parameters(m_allocator)
      .emplace_back(ast_function::parameter(
          containers::string(m_allocator, "_ptr"), m_void_ptr_t.get()));

  auto in_param = memory::make_unique<ast_id>(m_allocator, nullptr);
  in_param->m_type = m_void_ptr_t.get();
  in_param->m_function_parameter = &fn->m_parameters[0];

  auto const_null = memory::make_unique<ast_constant>(m_allocator, nullptr);
  const_null->m_string_value = containers::string(m_allocator, "");
  const_null->m_type = m_nullptr_t.get();

  auto is_null =
      memory::make_unique<ast_binary_expression>(m_allocator, nullptr);
  is_null->m_lhs = clone_node(m_allocator, in_param.get());
  is_null->m_rhs = core::move(const_null);
  is_null->m_type = m_bool_t.get();
  is_null->m_binary_type = ast_binary_type::neq;

  auto i_null = clone_node(m_allocator, is_null.get());

  auto cast_to_vp =
      memory::make_unique<ast_cast_expression>(m_allocator, nullptr);
  cast_to_vp->m_type = m_void_ptr_t.get();
  cast_to_vp->m_base_expression = core::move(i_null->m_rhs);
  i_null->m_rhs = core::move(cast_to_vp);

  auto body_block = memory::make_unique<ast_scope_block>(m_allocator, nullptr);
  auto& body = body_block->initialized_statements(m_allocator);
  auto if_is_not_null = memory::make_unique<ast_if_chain>(m_allocator, nullptr);
  if_is_not_null->initialized_conditionals(m_allocator)
      .push_back(ast_if_chain::conditional_block(
          core::move(i_null), core::move(body_block)));
  outer_body.push_back(core::move(if_is_not_null));

  auto shr_obj =
      memory::make_unique<ast_builtin_expression>(m_allocator, nullptr);
  shr_obj->m_type = get_reference_of(nullptr, m_shared_object_header.get(),
      ast_type_classification::reference);
  shr_obj->initialized_expressions(m_allocator)
      .push_back(clone_node(m_allocator, in_param.get()));
  shr_obj->m_builtin_type = builtin_expression_type::pointer_to_shared;

  auto refCnt =
      memory::make_unique<ast_member_access_expression>(m_allocator, nullptr);
  refCnt->m_member_name = containers::string(m_allocator, "ref_count");
  refCnt->m_type = m_size_t.get();
  refCnt->m_base_expression = clone_node(m_allocator, shr_obj.get());
  refCnt->m_member_offset = 0;

  auto dec = memory::make_unique<ast_builtin_expression>(m_allocator, nullptr);
  dec->m_type = m_size_t.get();
  dec->initialized_expressions(m_allocator).push_back(core::move(refCnt));
  dec->m_builtin_type = builtin_expression_type::atomic_dec;

  auto const_1 = memory::make_unique<ast_constant>(m_allocator, nullptr);
  const_1->m_string_value = containers::string(m_allocator, "1");
  const_1->m_type = m_size_t.get();
  const_1->m_node_value.m_integer = 1;

  auto is_1 = memory::make_unique<ast_binary_expression>(m_allocator, nullptr);
  is_1->m_lhs = core::move(dec);
  is_1->m_rhs = core::move(const_1);
  is_1->m_type = m_bool_t.get();
  is_1->m_binary_type = ast_binary_type::eq;

  auto destroy_block =
      memory::make_unique<ast_scope_block>(m_allocator, nullptr);
  destroy_block->m_returns = false;
  auto& destroy_statements = destroy_block->initialized_statements(m_allocator);

  auto check_ref = memory::make_unique<ast_if_chain>(m_allocator, nullptr);
  check_ref->initialized_conditionals(m_allocator)
      .push_back(ast_if_chain::conditional_block(
          core::move(is_1), core::move(destroy_block)));
  body.push_back(core::move(check_ref));
  outer_body.push_back(
      memory::make_unique<ast_return_instruction>(m_allocator, nullptr));

  auto st = memory::make_unique<ast_builtin_statement>(m_allocator, nullptr);
  st->m_builtin_type = builtin_statement_type::atomic_fence;
  destroy_statements.push_back(core::move(st));

  auto destr =
      memory::make_unique<ast_member_access_expression>(m_allocator, nullptr);
  destr->m_member_name = containers::string(m_allocator, "destructor");
  destr->m_type = m_destructor_fn_ptr_t;
  destr->m_base_expression = clone_node(m_allocator, shr_obj.get());
  destr->m_member_offset = 1;

  auto cast_to_de =
      memory::make_unique<ast_cast_expression>(m_allocator, nullptr);
  cast_to_de->m_type = m_destructor_fn_ptr_t;
  cast_to_de->m_base_expression = core::move(is_null->m_rhs);
  is_null->m_rhs = core::move(cast_to_de);

  is_null->m_lhs = clone_node(m_allocator, destr.get());

  auto has_destr_block =
      memory::make_unique<ast_scope_block>(m_allocator, nullptr);
  auto& has_destr_body = has_destr_block->initialized_statements(m_allocator);
  auto if_has_destr = memory::make_unique<ast_if_chain>(m_allocator, nullptr);
  if_has_destr->initialized_conditionals(m_allocator)
      .push_back(ast_if_chain::conditional_block(
          core::move(is_null), core::move(has_destr_block)));
  destroy_statements.push_back(core::move(if_has_destr));

  auto dest_call =
      memory::make_unique<ast_function_call_expression>(m_allocator, nullptr);
  dest_call->initialized_parameters(m_allocator)
      .push_back(clone_node(m_allocator, in_param.get()));
  dest_call->m_function_ptr = core::move(destr);
  dest_call->m_type = m_void_t.get();

  auto dest_c =
      memory::make_unique<ast_evaluate_expression>(m_allocator, nullptr);
  dest_c->m_expr = core::move(dest_call);
  has_destr_body.push_back(core::move(dest_c));

  auto shr_to_voidp =
      memory::make_unique<ast_cast_expression>(m_allocator, nullptr);
  shr_to_voidp->m_type = m_void_ptr_t.get();
  shr_to_voidp->m_base_expression = core::move(shr_obj);

  auto params = containers::dynamic_array<memory::unique_ptr<ast_expression>>(
      m_allocator);
  params.push_back(core::move(shr_to_voidp));

  auto fn_call = call_function(nullptr, alloc, core::move(params));

  auto fn_c =
      memory::make_unique<ast_evaluate_expression>(m_allocator, nullptr);
  fn_c->m_expr = core::move(fn_call);
  destroy_statements.push_back(core::move(fn_c));
  fn->calculate_mangled_name(m_allocator);

  m_release_shared = fn.get();
  m_script_file->m_functions.push_back(core::move(fn));
}

void parse_ast_convertor::convertor_context::add_assign_shared() {
  auto fn = memory::make_unique<ast_function>(m_allocator, nullptr);
  fn->m_name = containers::string(m_allocator, "_wns_assign_shared");
  fn->m_return_type = m_void_ptr_t.get();
  auto scope = memory::make_unique<ast_scope_block>(m_allocator, nullptr);
  scope->m_returns = true;
  auto& body = scope->initialized_statements(m_allocator);
  fn->m_scope = core::move(scope);

  fn->initialized_parameters(m_allocator)
      .emplace_back(ast_function::parameter(
          containers::string(m_allocator, "_to"), m_void_ptr_t.get()));
  fn->initialized_parameters(m_allocator)
      .emplace_back(ast_function::parameter(
          containers::string(m_allocator, "_from"), m_void_ptr_t.get()));

  auto to = memory::make_unique<ast_id>(m_allocator, nullptr);
  to->m_type = m_void_ptr_t.get();
  to->m_function_parameter = &fn->m_parameters[0];

  auto from = memory::make_unique<ast_id>(m_allocator, nullptr);
  from->m_type = m_void_ptr_t.get();
  from->m_function_parameter = &fn->m_parameters[1];

  auto params = containers::dynamic_array<memory::unique_ptr<ast_expression>>(
      m_allocator);
  params.push_back(clone_node(m_allocator, to.get()));

  auto deref = call_function(nullptr, m_release_shared, params);

  auto deref_st =
      memory::make_unique<ast_evaluate_expression>(m_allocator, nullptr);
  deref_st->m_expr = core::move(deref);

  auto const_null = memory::make_unique<ast_constant>(m_allocator, nullptr);
  const_null->m_string_value = containers::string(m_allocator, "");
  const_null->m_type = m_nullptr_t.get();

  auto null_to_vp =
      memory::make_unique<ast_cast_expression>(m_allocator, nullptr);
  null_to_vp->m_type = m_void_ptr_t.get();
  null_to_vp->m_base_expression = core::move(const_null);

  auto is_null =
      memory::make_unique<ast_binary_expression>(m_allocator, nullptr);
  is_null->m_lhs = clone_node(m_allocator, from.get());
  is_null->m_rhs = core::move(null_to_vp);
  is_null->m_type = m_bool_t.get();
  is_null->m_binary_type = ast_binary_type::neq;

  auto not_null_block =
      memory::make_unique<ast_scope_block>(m_allocator, nullptr);
  auto& not_null_body = not_null_block->initialized_statements(m_allocator);
  auto if_is_not_null = memory::make_unique<ast_if_chain>(m_allocator, nullptr);
  if_is_not_null->initialized_conditionals(m_allocator)
      .push_back(ast_if_chain::conditional_block(
          core::move(is_null), core::move(not_null_block)));
  body.push_back(core::move(if_is_not_null));

  auto shr_obj =
      memory::make_unique<ast_builtin_expression>(m_allocator, nullptr);
  shr_obj->m_type = get_reference_of(nullptr, m_shared_object_header.get(),
      ast_type_classification::reference);
  shr_obj->initialized_expressions(m_allocator)
      .push_back(clone_node(m_allocator, from.get()));
  shr_obj->m_builtin_type = builtin_expression_type::pointer_to_shared;

  auto refCnt =
      memory::make_unique<ast_member_access_expression>(m_allocator, nullptr);
  refCnt->m_member_name = containers::string(m_allocator, "ref_count");
  refCnt->m_type = m_size_t.get();
  refCnt->m_base_expression = clone_node(m_allocator, shr_obj.get());
  refCnt->m_member_offset = 0;

  auto inc = memory::make_unique<ast_builtin_expression>(m_allocator, nullptr);
  inc->m_type = m_size_t.get();
  inc->initialized_expressions(m_allocator).push_back(core::move(refCnt));
  inc->m_builtin_type = builtin_expression_type::atomic_inc;

  auto inc_st =
      memory::make_unique<ast_evaluate_expression>(m_allocator, nullptr);
  inc_st->m_expr = core::move(inc);

  not_null_body.push_back(core::move(inc_st));
  body.push_back(core::move(deref_st));
  auto ret = memory::make_unique<ast_return_instruction>(m_allocator, nullptr);
  ret->m_return_expr = core::move(from);
  body.push_back(core::move(ret));
  fn->calculate_mangled_name(m_allocator);

  m_assign_shared = fn.get();
  m_script_file->m_functions.push_back(core::move(fn));
}

void parse_ast_convertor::convertor_context::add_return_shared() {
  auto fn = memory::make_unique<ast_function>(m_allocator, nullptr);
  fn->m_name = containers::string(m_allocator, "_wns_return_shared");
  fn->m_return_type = m_void_ptr_t.get();
  auto scope = memory::make_unique<ast_scope_block>(m_allocator, nullptr);
  scope->m_returns = true;
  auto& body = scope->initialized_statements(m_allocator);
  fn->m_scope = core::move(scope);

  fn->initialized_parameters(m_allocator)
      .emplace_back(ast_function::parameter(
          containers::string(m_allocator, "_val"), m_void_ptr_t.get()));

  auto val = memory::make_unique<ast_id>(m_allocator, nullptr);
  val->m_type = m_void_ptr_t.get();
  val->m_function_parameter = &fn->m_parameters[0];

  auto shr_obj =
      memory::make_unique<ast_builtin_expression>(m_allocator, nullptr);
  shr_obj->m_type = get_reference_of(nullptr, m_shared_object_header.get(),
      ast_type_classification::reference);
  shr_obj->initialized_expressions(m_allocator)
      .push_back(clone_node(m_allocator, val.get()));
  shr_obj->m_builtin_type = builtin_expression_type::pointer_to_shared;

  auto refCnt =
      memory::make_unique<ast_member_access_expression>(m_allocator, nullptr);
  refCnt->m_member_name = containers::string(m_allocator, "ref_count");
  refCnt->m_type = m_size_t.get();
  refCnt->m_base_expression = clone_node(m_allocator, shr_obj.get());
  refCnt->m_member_offset = 0;

  auto dec = memory::make_unique<ast_builtin_expression>(m_allocator, nullptr);
  dec->m_type = m_size_t.get();
  dec->initialized_expressions(m_allocator).push_back(core::move(refCnt));
  dec->m_builtin_type = builtin_expression_type::atomic_dec;

  auto inc_st =
      memory::make_unique<ast_evaluate_expression>(m_allocator, nullptr);
  inc_st->m_expr = core::move(dec);
  body.push_back(core::move(inc_st));

  auto ret = memory::make_unique<ast_return_instruction>(m_allocator, nullptr);
  ret->m_return_expr = core::move(val);
  body.push_back(core::move(ret));
  fn->calculate_mangled_name(m_allocator);
  m_return_shared = fn.get();
  m_script_file->m_functions.push_back(core::move(fn));
}

}  // namespace scripting
}  // namespace wn