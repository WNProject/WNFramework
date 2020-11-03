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

bool type_manager::add_builtin_functions() {
  if (m_externals_by_name.find("_ZN3wns9_allocateEPvN3wns4sizeE") ==
      m_externals_by_name.end()) {
    m_log->log_error("External function _allocate missing");
    return false;
  }
  if (m_externals_by_name.find(containers::string(
          m_allocator, "_ZN3wns5_freeEvPv")) == m_externals_by_name.end()) {
    m_log->log_error("External function _free missing");
    return false;
  }

  if (m_externals_by_name.find(containers::string(m_allocator,
          "_ZN3wns23_allocate_runtime_arrayEPvN3wns4sizeEN3wns4sizeE")) ==
      m_externals_by_name.end()) {
    m_log->log_error("External function _allocate_runtime_array missing");
    return false;
  }

  add_allocate_shared();
  add_release_shared();
  add_assign_shared();
  add_return_shared();
  add_strlen();
  return true;
}

void type_manager::add_allocate_shared() {
  auto alloc = m_externals_by_name["_ZN3wns9_allocateEPvN3wns4sizeE"];

  auto fn = memory::make_unique<ast_function>(m_allocator, nullptr);
  fn->m_is_builtin = true;
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
  cast_to_soh->m_type = get_reference_of(m_shared_object_header.get(),
      ast_type_classification::reference, nullptr);

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
      clone_ast_node(m_allocator, _shared_obj.get());

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
      clone_ast_node(m_allocator, _shared_obj.get());

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
  m_builtins.push_back(core::move(fn));
}

void type_manager::add_release_shared() {
  auto alloc = m_externals_by_name["_ZN3wns5_freeEvPv"];

  auto fn = memory::make_unique<ast_function>(m_allocator, nullptr);
  fn->m_is_builtin = true;
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
  is_null->m_lhs = clone_ast_node(m_allocator, in_param.get());
  is_null->m_rhs = core::move(const_null);
  is_null->m_type = m_bool_t.get();
  is_null->m_binary_type = ast_binary_type::neq;

  auto i_null = clone_ast_node(m_allocator, is_null.get());

  auto cast_to_vp =
      memory::make_unique<ast_cast_expression>(m_allocator, nullptr);
  cast_to_vp->m_type = m_void_ptr_t.get();
  cast_to_vp->m_base_expression = core::move(i_null->m_rhs);
  i_null->m_rhs = core::move(cast_to_vp);

  auto body_block = memory::make_unique<ast_scope_block>(m_allocator, nullptr);
  auto& body = body_block->initialized_statements(m_allocator);
  auto if_is_not_null = memory::make_unique<ast_if_block>(m_allocator, nullptr);
  if_is_not_null->m_condition = core::move(i_null);
  if_is_not_null->m_body = core::move(body_block);
  outer_body.push_back(core::move(if_is_not_null));

  auto shr_obj =
      memory::make_unique<ast_builtin_expression>(m_allocator, nullptr);
  shr_obj->m_type = get_reference_of(m_shared_object_header.get(),
      ast_type_classification::reference, nullptr);
  shr_obj->initialized_expressions(m_allocator)
      .push_back(clone_ast_node(m_allocator, in_param.get()));
  shr_obj->m_builtin_type = builtin_expression_type::pointer_to_shared;

  auto refCnt =
      memory::make_unique<ast_member_access_expression>(m_allocator, nullptr);
  refCnt->m_member_name = containers::string(m_allocator, "ref_count");
  refCnt->m_type = m_size_t.get();
  refCnt->m_base_expression = clone_ast_node(m_allocator, shr_obj.get());
  refCnt->m_member_offset = 0;

  auto dec = memory::make_unique<ast_builtin_expression>(m_allocator, nullptr);
  dec->m_type = m_size_t.get();
  dec->initialized_expressions(m_allocator).push_back(core::move(refCnt));
  dec->m_builtin_type = builtin_expression_type::atomic_dec;

  auto const_1 = memory::make_unique<ast_constant>(m_allocator, nullptr);
  const_1->m_string_value = containers::string(m_allocator, "1");
  const_1->m_node_value.m_size_t = 1;
  const_1->m_type = m_size_t.get();

  auto is_1 = memory::make_unique<ast_binary_expression>(m_allocator, nullptr);
  is_1->m_lhs = core::move(dec);
  is_1->m_rhs = core::move(const_1);
  is_1->m_type = m_bool_t.get();
  is_1->m_binary_type = ast_binary_type::eq;

  auto destroy_block =
      memory::make_unique<ast_scope_block>(m_allocator, nullptr);
  destroy_block->m_returns = false;
  auto& destroy_statements = destroy_block->initialized_statements(m_allocator);

  auto check_ref = memory::make_unique<ast_if_block>(m_allocator, nullptr);
  check_ref->m_condition = core::move(is_1);
  check_ref->m_body = core::move(destroy_block);
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
  destr->m_base_expression = clone_ast_node(m_allocator, shr_obj.get());
  destr->m_member_offset = 1;

  auto cast_to_de =
      memory::make_unique<ast_cast_expression>(m_allocator, nullptr);
  cast_to_de->m_type = m_destructor_fn_ptr_t;
  cast_to_de->m_base_expression = core::move(is_null->m_rhs);
  is_null->m_rhs = core::move(cast_to_de);

  is_null->m_lhs = clone_ast_node(m_allocator, destr.get());

  auto has_destr_block =
      memory::make_unique<ast_scope_block>(m_allocator, nullptr);
  auto& has_destr_body = has_destr_block->initialized_statements(m_allocator);
  auto if_has_destr = memory::make_unique<ast_if_block>(m_allocator, nullptr);
  if_has_destr->m_condition = core::move(is_null);
  if_has_destr->m_body = core::move(has_destr_block);
  destroy_statements.push_back(core::move(if_has_destr));

  auto dest_call =
      memory::make_unique<ast_function_call_expression>(m_allocator, nullptr);
  dest_call->initialized_parameters(m_allocator)
      .push_back(clone_ast_node(m_allocator, in_param.get()));
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
  m_builtins.push_back(core::move(fn));
}

void type_manager::add_assign_shared() {
  auto fn = memory::make_unique<ast_function>(m_allocator, nullptr);
  fn->m_is_builtin = true;
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
  params.push_back(clone_ast_node(m_allocator, to.get()));

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
  is_null->m_lhs = clone_ast_node(m_allocator, from.get());
  is_null->m_rhs = core::move(null_to_vp);
  is_null->m_type = m_bool_t.get();
  is_null->m_binary_type = ast_binary_type::neq;

  auto not_null_block =
      memory::make_unique<ast_scope_block>(m_allocator, nullptr);
  auto& not_null_body = not_null_block->initialized_statements(m_allocator);
  auto if_is_not_null = memory::make_unique<ast_if_block>(m_allocator, nullptr);
  if_is_not_null->m_condition = core::move(is_null);
  if_is_not_null->m_body = core::move(not_null_block);
  body.push_back(core::move(if_is_not_null));

  auto shr_obj =
      memory::make_unique<ast_builtin_expression>(m_allocator, nullptr);
  shr_obj->m_type = get_reference_of(m_shared_object_header.get(),
      ast_type_classification::reference, nullptr);
  shr_obj->initialized_expressions(m_allocator)
      .push_back(clone_ast_node(m_allocator, from.get()));
  shr_obj->m_builtin_type = builtin_expression_type::pointer_to_shared;

  auto refCnt =
      memory::make_unique<ast_member_access_expression>(m_allocator, nullptr);
  refCnt->m_member_name = containers::string(m_allocator, "ref_count");
  refCnt->m_type = m_size_t.get();
  refCnt->m_base_expression = clone_ast_node(m_allocator, shr_obj.get());
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
  m_builtins.push_back(core::move(fn));
}

void type_manager::add_return_shared() {
  auto fn = memory::make_unique<ast_function>(m_allocator, nullptr);
  fn->m_is_builtin = true;
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

  auto const_null = memory::make_unique<ast_constant>(m_allocator, nullptr);
  const_null->m_string_value = containers::string(m_allocator, "");
  const_null->m_type = m_nullptr_t.get();

  auto null_to_vp =
      memory::make_unique<ast_cast_expression>(m_allocator, nullptr);
  null_to_vp->m_type = m_void_ptr_t.get();
  null_to_vp->m_base_expression = core::move(const_null);

  auto is_null =
      memory::make_unique<ast_binary_expression>(m_allocator, nullptr);
  is_null->m_lhs = clone_ast_node(m_allocator, val.get());
  is_null->m_rhs = core::move(null_to_vp);
  is_null->m_type = m_bool_t.get();
  is_null->m_binary_type = ast_binary_type::neq;

  auto not_null_block =
      memory::make_unique<ast_scope_block>(m_allocator, nullptr);
  auto& not_null_body = not_null_block->initialized_statements(m_allocator);
  auto if_is_not_null = memory::make_unique<ast_if_block>(m_allocator, nullptr);
  if_is_not_null->m_condition = core::move(is_null);
  if_is_not_null->m_body = core::move(not_null_block);
  body.push_back(core::move(if_is_not_null));

  auto shr_obj =
      memory::make_unique<ast_builtin_expression>(m_allocator, nullptr);
  shr_obj->m_type = get_reference_of(m_shared_object_header.get(),
      ast_type_classification::reference, nullptr);
  shr_obj->initialized_expressions(m_allocator)
      .push_back(clone_ast_node(m_allocator, val.get()));
  shr_obj->m_builtin_type = builtin_expression_type::pointer_to_shared;

  auto refCnt =
      memory::make_unique<ast_member_access_expression>(m_allocator, nullptr);
  refCnt->m_member_name = containers::string(m_allocator, "ref_count");
  refCnt->m_type = m_size_t.get();
  refCnt->m_base_expression = clone_ast_node(m_allocator, shr_obj.get());
  refCnt->m_member_offset = 0;

  auto dec = memory::make_unique<ast_builtin_expression>(m_allocator, nullptr);
  dec->m_type = m_size_t.get();
  dec->initialized_expressions(m_allocator).push_back(core::move(refCnt));
  dec->m_builtin_type = builtin_expression_type::atomic_dec;

  auto inc_st =
      memory::make_unique<ast_evaluate_expression>(m_allocator, nullptr);
  inc_st->m_expr = core::move(dec);
  not_null_body.push_back(core::move(inc_st));

  auto ret = memory::make_unique<ast_return_instruction>(m_allocator, nullptr);
  ret->m_return_expr = core::move(val);
  body.push_back(core::move(ret));
  fn->calculate_mangled_name(m_allocator);
  m_return_shared = fn.get();
  m_builtins.push_back(core::move(fn));
}

void type_manager::add_strlen() {
  auto fn = memory::make_unique<ast_function>(m_allocator, nullptr);
  fn->m_is_builtin = true;
  fn->m_name = containers::string(m_allocator, "_wns_strlen");
  fn->m_return_type = m_integral_types[32].get();
  auto scope = memory::make_unique<ast_scope_block>(m_allocator, nullptr);
  scope->m_returns = true;
  auto& body = scope->initialized_statements(m_allocator);
  fn->m_scope = core::move(scope);

  fn->initialized_parameters(m_allocator)
      .emplace_back(ast_function::parameter(
          containers::string(m_allocator, "_val"), m_cstr_t.get()));

  auto val = memory::make_unique<ast_id>(m_allocator, nullptr);
  val->m_type = m_cstr_t.get();
  val->m_function_parameter = &fn->m_parameters[0];

  containers::string temp(m_allocator, "_size");
  memory::unique_ptr<ast_loop> m_loop =
      memory::make_unique<ast_loop>(m_allocator, nullptr);
  m_loop->m_body = memory::make_unique<ast_scope_block>(m_allocator, nullptr);

  memory::unique_ptr<ast_declaration> decl =
      memory::make_unique<ast_declaration>(m_allocator, nullptr);
  decl->m_name = temp;
  decl->m_type = m_integral_types[32].get();

  auto zero = memory::make_unique<ast_constant>(m_allocator, nullptr);
  zero->m_type = m_integral_types[32].get();
  zero->m_node_value.m_integer = 0;
  zero->m_string_value = containers::string(m_allocator, "0");

  decl->m_initializer = core::move(zero);

  auto sizer = memory::make_unique<ast_id>(m_allocator, nullptr);
  sizer->m_type = m_integral_types[32].get();
  sizer->m_declaration = decl.get();
  body.push_back(core::move(decl));

  auto const_one = memory::make_unique<ast_constant>(m_allocator, nullptr);
  const_one->m_type = m_integral_types[32].get();
  const_one->m_node_value.m_integer = 1;
  const_one->m_string_value = containers::string(m_allocator, "1");

  auto add = memory::make_unique<ast_binary_expression>(m_allocator, nullptr);
  add->m_binary_type = ast_binary_type::add;
  add->m_lhs = clone_ast_node(m_allocator, sizer.get());
  add->m_rhs = core::move(const_one);
  add->m_type = m_integral_types[32].get();

  auto assign = memory::make_unique<ast_assignment>(m_allocator, nullptr);
  assign->m_lhs = clone_ast_node(m_allocator, sizer.get());
  assign->m_rhs = core::move(add);

  m_loop->m_body->initialized_statements(m_allocator)
      .push_back(core::move(assign));

  auto byte =
      memory::make_unique<ast_array_access_expression>(m_allocator, nullptr);
  byte->m_type = m_integral_types[8].get();
  byte->m_array = core::move(val);
  byte->m_index = clone_ast_node(m_allocator, sizer.get());

  auto const_zero_char =
      memory::make_unique<ast_constant>(m_allocator, nullptr);
  const_zero_char->m_type = m_integral_types[8].get();
  const_zero_char->m_node_value.m_char = 0;
  const_zero_char->m_string_value = containers::string(m_allocator, "0");

  auto exit = memory::make_unique<ast_binary_expression>(m_allocator, nullptr);
  exit->m_type = m_bool_t.get();
  exit->m_lhs = core::move(const_zero_char);
  exit->m_rhs = core::move(byte);
  exit->m_binary_type = ast_binary_type::neq;
  m_loop->m_pre_condition = core::move(exit);

  body.push_back(core::move(m_loop));

  auto ret = memory::make_unique<ast_return_instruction>(m_allocator, nullptr);
  ret->m_return_expr = core::move(sizer);
  body.push_back(core::move(ret));
  fn->calculate_mangled_name(m_allocator);
  m_strlen = fn.get();
  m_builtins.push_back(core::move(fn));
}

memory::unique_ptr<ast_function_call_expression> type_manager::call_function(
    const node* _base_node, const ast_function* _func,
    containers::contiguous_range<memory::unique_ptr<ast_expression>>
        _expressions) {
  memory::unique_ptr<ast_function_call_expression> function_call =
      memory::make_unique<ast_function_call_expression>(
          m_allocator, _base_node);
  function_call->m_function = _func;

  WN_DEBUG_ASSERT(_expressions.size() == _func->m_parameters.size(),
      "Cannot create a function with an incorrect number of parameters");

  int i = 0;
  (void)i;

  for (auto& p : _expressions) {
    WN_DEBUG_ASSERT(_func->m_parameters[i++].m_type == p->m_type,
        "Trying to create function call with invalid type");

    function_call->initialized_parameters(m_allocator).push_back(core::move(p));
  }

  function_call->m_type = _func->m_return_type;

  return function_call;
}

}  // namespace scripting
}  // namespace wn