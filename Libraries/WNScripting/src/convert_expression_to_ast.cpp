// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNScripting/inc/WNNodeTypes.h"
#include "WNScripting/inc/internal/ast_convertor_context.h"
#include "WNScripting/inc/parse_ast_convertor.h"

namespace wn {
namespace scripting {

memory::unique_ptr<ast_expression>
parse_ast_convertor::convertor_context::resolve_expression(
    const expression* _expression) {
  switch (_expression->get_node_type()) {
    case node_type::id_expression:
      return resolve_id(cast_to<id_expression>(_expression));
    case node_type::array_allocation_expression:
      return resolve_array_allocation_expression(
          cast_to<array_allocation_expression>(_expression));
    case node_type::dynamic_array_allocation_expression:
      WN_RELEASE_ASSERT(false, "Not Implemented expression type");
    case node_type::binary_expression:
      return resolve_binary(cast_to<binary_expression>(_expression));
    case node_type::cast_expression:
    case node_type::cond_expression:
      WN_RELEASE_ASSERT(false, "Not Implemented expression type");
    case node_type::constant_expression:
      return resolve_constant(cast_to<constant_expression>(_expression));
    case node_type::null_allocation_expression:
      WN_RELEASE_ASSERT(false, "Not Implemented expression type");
    case node_type::array_access_expression:
      return resolve_array_access_expression(
          cast_to<array_access_expression>(_expression));
    case node_type::function_call_expression:
      return resolve_function_call(
          cast_to<function_call_expression>(_expression));
    case node_type::member_access_expression:
      return resolve_member_access_expression(
          cast_to<member_access_expression>(_expression));
    case node_type::post_unary_expression:
    case node_type::short_circuit_expression:
      WN_RELEASE_ASSERT(false, "Not Implemented expression type");
    case node_type::struct_allocation_expression:
      return resolve_struct_allocation_expression(
          cast_to<struct_allocation_expression>(_expression));
    case node_type::unary_expression:
      WN_RELEASE_ASSERT(false, "Not Implemented expression type");
    case node_type::builtin_unary_expression:
      return resolve_builtin_unary_expression(
          cast_to<builtin_unary_expression>(_expression));
    default:
      WN_RELEASE_ASSERT(false, "Could not determine expression type");
  }
  return nullptr;
}

memory::unique_ptr<ast_expression>
parse_ast_convertor::convertor_context::resolve_id(
    const id_expression* _expression) {
  return get_id(_expression, _expression->get_name().to_string(m_allocator));
}

memory::unique_ptr<ast_constant>
parse_ast_convertor::convertor_context::resolve_constant(
    const constant_expression* _const) {
  memory::unique_ptr<ast_constant> c =
      memory::make_unique<ast_constant>(m_allocator, _const);
  c->m_string_value = containers::string(m_allocator, _const->get_type_text());

  switch (_const->get_index()) {
    case static_cast<uint32_t>(type_classification::int_type): {
      long long val = atoll(_const->get_type_text().c_str());
      c->m_type = m_type_manager->m_integral_types[32].get();
      c->m_node_value.m_integer = static_cast<int32_t>(val);
      return core::move(c);
    }
    case static_cast<uint32_t>(type_classification::nullptr_type): {
      c->m_type = m_type_manager->m_nullptr_t.get();
      c->m_node_value.m_integer = static_cast<int32_t>(0);
      return core::move(c);
    }
    case static_cast<uint32_t>(type_classification::float_type): {
      double val = atof(_const->get_type_text().c_str());
      c->m_type = m_type_manager->m_float_types[32].get();
      c->m_node_value.m_float = static_cast<float>(val);
      return core::move(c);
    }
    case static_cast<uint32_t>(type_classification::char_type): {
      c->m_type = m_type_manager->m_integral_types[8].get();
      c->m_node_value.m_char = c->m_string_value[1];
      return core::move(c);
    }
    case static_cast<uint32_t>(type_classification::string_type): {
      c->m_type = m_type_manager->m_cstr_t.get();
      return core::move(c);
    }
    case static_cast<uint32_t>(type_classification::bool_type): {
      c->m_type = m_type_manager->m_bool_t.get();
      if (_const->get_type_text() == "true") {
        c->m_node_value.m_bool = true;
      } else if (_const->get_type_text() == "false") {
        c->m_node_value.m_bool = false;
      } else {
        _const->log_line(m_log, logging::log_level::error);
        m_log->log_error("Unknown boolean constant");
        return nullptr;
      }
      return core::move(c);
    }
    default:
      WN_RELEASE_ASSERT(false, "Unhandled: Custom constants");
  }
  return nullptr;
}

void parse_ast_convertor::convertor_context::transfer_temporaries(
    ast_expression* to, ast_expression* from) {
  auto& to_temporaries = to->initialized_setup_statements(m_allocator);
  for (auto& temp : from->m_temporaries) {
    to_temporaries.push_back(core::move(temp));
  }

  auto& to_clean = to->initialized_destroy_expressions(m_allocator);
  for (auto& temp : from->m_destroy_expressions) {
    to_clean.push_back(core::move(temp));
  }
  from->m_temporaries.clear();
  from->m_destroy_expressions.clear();
}

memory::unique_ptr<ast_binary_expression>
parse_ast_convertor::convertor_context::resolve_binary(
    const binary_expression* _bin) {
  auto lhs = resolve_expression(_bin->get_lhs());
  if (!lhs) {
    return nullptr;
  }
  auto rhs = resolve_expression(_bin->get_rhs());
  if (!rhs) {
    return nullptr;
  }

  if (lhs->m_type->m_classification ==
          ast_type_classification::shared_reference &&
      rhs->m_type == m_type_manager->m_nullptr_t.get()) {
    rhs = make_cast(core::move(rhs), lhs->m_type);
  }
  if (rhs->m_type->m_classification ==
          ast_type_classification::shared_reference &&
      lhs->m_type == m_type_manager->m_nullptr_t.get()) {
    lhs = make_cast(core::move(lhs), rhs->m_type);
  }

  if (lhs->m_type != rhs->m_type) {
    _bin->log_line(m_log, logging::log_level::error);
    m_log->log_error("Types must be equal");
    return nullptr;
  }

  auto bin = memory::make_unique<ast_binary_expression>(m_allocator, _bin);
  transfer_temporaries(bin.get(), lhs.get());
  transfer_temporaries(bin.get(), rhs.get());
  bin->m_lhs = core::move(lhs);
  bin->m_rhs = core::move(rhs);

  // There are only a few cases where we have to change this.
  // So set it here, fix it later
  bin->m_type = bin->m_lhs->m_type;

  switch (_bin->get_arithmetic_type()) {
    case arithmetic_type::arithmetic_add:
    case arithmetic_type::arithmetic_sub:
    case arithmetic_type::arithmetic_mult:
    case arithmetic_type::arithmetic_div:
    case arithmetic_type::arithmetic_mod:
      if (!bin->m_lhs->m_type->m_is_arithmetic_type) {
        _bin->log_line(m_log, logging::log_level::error);
        m_log->log_error("You can only do arithmetic on arithmetic types.");
        return nullptr;
      }
      break;
    case arithmetic_type::arithmetic_less_than_or_equal:
    case arithmetic_type::arithmetic_greater_than_or_equal:
    case arithmetic_type::arithmetic_less_than:
    case arithmetic_type::arithmetic_greater_than:
      if (!bin->m_lhs->m_type->m_is_arithmetic_type) {
        _bin->log_line(m_log, logging::log_level::error);
        m_log->log_error("You can only do arithmetic on arithmetic types.");
        return nullptr;
      }
    case arithmetic_type::arithmetic_equal:
    case arithmetic_type::arithmetic_not_equal:
      bin->m_type = m_type_manager->m_bool_t.get();
      break;
    default:
      WN_RELEASE_ASSERT(false, "You should never get here");
  }

  switch (_bin->get_arithmetic_type()) {
    case arithmetic_type::arithmetic_add:
      bin->m_binary_type = ast_binary_type::add;
      break;
    case arithmetic_type::arithmetic_sub:
      bin->m_binary_type = ast_binary_type::sub;
      break;
    case arithmetic_type::arithmetic_mult:
      bin->m_binary_type = ast_binary_type::mult;
      break;
    case arithmetic_type::arithmetic_div:
      bin->m_binary_type = ast_binary_type::div;
      break;
    case arithmetic_type::arithmetic_mod:
      bin->m_binary_type = ast_binary_type::mod;
      break;
    case arithmetic_type::arithmetic_equal:
      bin->m_binary_type = ast_binary_type::eq;
      break;
    case arithmetic_type::arithmetic_not_equal:
      bin->m_binary_type = ast_binary_type::neq;
      break;
    case arithmetic_type::arithmetic_less_than_or_equal:
      bin->m_binary_type = ast_binary_type::lte;
      break;
    case arithmetic_type::arithmetic_greater_than_or_equal:
      bin->m_binary_type = ast_binary_type::gte;
      break;
    case arithmetic_type::arithmetic_less_than:
      bin->m_binary_type = ast_binary_type::lt;
      break;
    case arithmetic_type::arithmetic_greater_than:
      bin->m_binary_type = ast_binary_type::gt;
      break;
    default:
      WN_RELEASE_ASSERT(false, "You should never get here");
  }
  return bin;
}

memory::unique_ptr<ast_expression>
parse_ast_convertor::convertor_context::resolve_array_allocation_expression(
    const array_allocation_expression* _alloc) {
  auto base_expr = resolve_expression(_alloc->get_copy_initializer());
  if (!base_expr) {
    return nullptr;
  }

  containers::string temporary_name = get_next_temporary_name();

  memory::unique_ptr<ast_declaration> dec =
      memory::make_unique<ast_declaration>(m_allocator, _alloc);
  dec->m_name = containers::string(m_allocator, temporary_name);

  if (_alloc->get_array_initializers().size() != 1) {
    _alloc->log_line(m_log, logging::log_level::error);
    m_log->log_error("You must have one constant size for an array initalizer");
    return nullptr;
  }

  auto array_size =
      resolve_expression(_alloc->get_array_initializers()[0].get());

  if (array_size->m_node_type != ast_node_type::ast_constant) {
    _alloc->log_line(m_log, logging::log_level::error);
    m_log->log_error("Array size must be static");
  }

  auto* array_const = cast_to<ast_constant>(array_size.get());
  if (array_const->m_type != m_type_manager->m_integral_types[32].get()) {
    _alloc->log_line(m_log, logging::log_level::error);
    m_log->log_error("Array size must be an Integer");
  }

  const ast_type* array_base_type = resolve_type(_alloc->get_type());
  const ast_type* t =
      get_array_of(array_base_type, array_const->m_node_value.m_integer);
  dec->m_type = t;

  auto array_init =
      memory::make_unique<ast_array_allocation>(m_allocator, _alloc);
  array_init->m_initializer = core::move(base_expr);
  array_init->m_type = t;

  memory::unique_ptr<ast_id> id =
      memory::make_unique<ast_id>(m_allocator, _alloc);
  id->m_declaration = dec.get();
  id->m_type = t;

  array_init->m_initializee = clone_node(m_allocator, id.get());
  id->initialized_setup_statements(m_allocator).push_back(core::move(dec));
  id->initialized_setup_statements(m_allocator)
      .push_back(core::move(array_init));

  return id;
}

memory::unique_ptr<ast_function_call_expression>
parse_ast_convertor::convertor_context::resolve_function_call(
    const function_call_expression* _call) {
  auto base_expr = resolve_expression(_call->get_base_expression());

  if (!base_expr) {
    return nullptr;
  }

  auto function_call =
      memory::make_unique<ast_function_call_expression>(m_allocator, _call);
  transfer_temporaries(function_call.get(), base_expr.get());
  auto& params = function_call->initialized_parameters(m_allocator);
  containers::string function_name;

  const containers::deque<ast_function*>* possible_functions = nullptr;
  bool cast_first_param = false;
  switch (base_expr->m_node_type) {
    case ast_node_type::ast_id: {
      auto child_id = cast_to<ast_id>(base_expr.get());
      auto it = m_named_functions.find(child_id->name());
      if (it == m_named_functions.end()) {
        _call->log_line(m_log, logging::log_level::error);
        m_log->log_error("Cannot find function with name ", child_id->name());
        return nullptr;
      }
      possible_functions = &(it->second);
      function_name = containers::string(m_allocator, child_id->name());
      break;
    }
    case ast_node_type::ast_member_access_expression: {
      auto member_access =
          cast_to<ast_member_access_expression>(base_expr.get());
      if (member_access->m_type != m_type_manager->m_function_t.get()) {
        _call->log_line(m_log, logging::log_level::error);
        m_log->log_error(
            "Cannot find method with name ", member_access->m_member_name);
      }
      auto struct_type = member_access->m_base_expression->m_type;
      if (struct_type->m_classification ==
              ast_type_classification::shared_reference ||
          struct_type->m_classification == ast_type_classification::reference) {
        struct_type = struct_type->m_implicitly_contained_type;
      }
      if (struct_type->m_classification !=
              ast_type_classification::struct_type &&
          struct_type->m_classification !=
              ast_type_classification::extern_type) {
        _call->log_line(m_log, logging::log_level::error);
        m_log->log_error("Trying to call a member on non struct type: ",
            struct_type->m_name);
      }
      cast_first_param = true;
      possible_functions = &struct_type->m_member_functions;
      function_name =
          containers::string(m_allocator, member_access->m_member_name);
      params.push_back(core::move(member_access->m_base_expression));
      break;
    }
    default:
      _call->log_line(m_log, logging::log_level::error);
      m_log->log_error("Invalid indirection for function call");
      return nullptr;
  }

  for (auto& expr : _call->get_expressions()) {
    auto ex = resolve_expression(expr->m_expr.get());
    if (!ex) {
      return nullptr;
    }
    transfer_temporaries(function_call.get(), ex.get());
    params.push_back(core::move(ex));
  }

  uint32_t total_matches = 0;
  uint32_t match_index = 0;
  for (const auto& fn : *possible_functions) {
    if (total_matches == 0) {
      ++match_index;
    }
    if (fn->m_name != function_name) {
      continue;
    }
    bool matches = true;
    if (fn->m_parameters.size() != params.size()) {
      continue;
    }
    for (size_t i = 0; i < params.size(); ++i) {
      if (cast_first_param && i == 0) {
        continue;
      }
      if (!params[i]->m_type->can_implicitly_cast_to(
              fn->m_parameters[i].m_type)) {
        matches = false;
        break;
      }
    }
    total_matches += (matches == true);
  }
  match_index -= 1;
  if (total_matches == 0) {
    _call->log_line(m_log, logging::log_level::error);
    m_log->log_error("Could not find matching function ", function_name);
    return nullptr;
  }
  if (total_matches != 1) {
    _call->log_line(m_log, logging::log_level::error);
    m_log->log_error("Ambiguous function call ", function_name);
    return nullptr;
  }

  function_call->m_function = (*possible_functions)[match_index];
  for (size_t i = 0; i < params.size(); ++i) {
    if (i == 0 && cast_first_param) {
      params[i] = make_cast(core::move(params[i]),
          function_call->m_function->m_parameters[i].m_type);
    } else {
      params[i] = make_implicit_cast(core::move(params[i]),
          function_call->m_function->m_parameters[i].m_type);
    }
  }

  function_call->m_type = function_call->m_function->m_return_type;
  return core::move(function_call);
}

memory::unique_ptr<ast_expression>
parse_ast_convertor::convertor_context::resolve_array_access_expression(
    const array_access_expression* _access) {
  memory::unique_ptr<ast_expression> outer_expr =
      resolve_expression(_access->get_base_expression());
  memory::unique_ptr<ast_expression> inner_expr =
      resolve_expression(_access->get_access());

  if (!outer_expr || !inner_expr) {
    return nullptr;
  }

  if (inner_expr->m_type != m_type_manager->m_integral_types[32].get()) {
    _access->log_line(m_log, logging::log_level ::error);
    m_log->log_error("Array access must be an integer");
    return nullptr;
  }

  if (outer_expr->m_type->m_classification !=
          ast_type_classification::static_array &&
      outer_expr->m_type->m_builtin != builtin_type::c_string_type) {
    _access->log_line(m_log, logging::log_level ::error);
    m_log->log_error("Cannot index a non-array type");
    return nullptr;
  }

  memory::unique_ptr<ast_array_access_expression> array_access =
      memory::make_unique<ast_array_access_expression>(m_allocator, _access);
  if (outer_expr->m_type->m_builtin != builtin_type::c_string_type) {
    array_access->m_type = outer_expr->m_type->m_implicitly_contained_type;
  } else {
    array_access->m_type = m_type_manager->m_integral_types[8].get();
  }
  array_access->m_index = core::move(inner_expr);
  array_access->m_array = core::move(outer_expr);

  return core::move(array_access);
}

memory::unique_ptr<ast_expression>
parse_ast_convertor::convertor_context::resolve_struct_allocation_expression(
    const struct_allocation_expression* _alloc) {
  const ast_type* t = resolve_type(_alloc->get_type());
  if (!t) {
    return nullptr;
  }
  if (t->m_classification == ast_type_classification::shared_reference) {
    return resolve_shared_struct_allocation_expression(_alloc);
  }
  if (t->m_classification != ast_type_classification::struct_type) {
    _alloc->log_line(m_log, logging::log_level ::error);
    m_log->log_error("You must allocate a struct with a known struct type");
    return nullptr;
  }
  const ast_type* alloc_type = t->m_overloaded_construction_child;
  WN_DEBUG_ASSERT(
      alloc_type, "Have a struct type without a proper constructor");

  containers::string temporary_name = get_next_temporary_name();

  memory::unique_ptr<ast_declaration> dec =
      memory::make_unique<ast_declaration>(m_allocator, _alloc);
  dec->m_name = containers::string(m_allocator, temporary_name);
  dec->m_type = alloc_type;

  ast_declaration* decl = dec.get();

  memory::unique_ptr<ast_expression> init;
  if (alloc_type->m_constructor) {
    memory::unique_ptr<ast_function_call_expression> function_call =
        memory::make_unique<ast_function_call_expression>(m_allocator, _alloc);
    function_call->m_function = alloc_type->m_constructor;

    memory::unique_ptr<ast_id> id =
        memory::make_unique<ast_id>(m_allocator, _alloc);
    id->m_declaration = dec.get();
    id->m_type = dec->m_type;

    memory::unique_ptr<ast_cast_expression> cast =
        memory::make_unique<ast_cast_expression>(m_allocator, _alloc);
    cast->m_base_expression = core::move(id);
    cast->m_type = m_type_manager->get_reference_of(
        cast->m_base_expression->m_type, ast_type_classification::reference);
    function_call->initialized_parameters(m_allocator)
        .push_back(core::move(cast));
    function_call->m_type =
        m_type_manager->get_reference_of(t, ast_type_classification::reference);
    function_call->initialized_setup_statements(m_allocator)
        .push_back(core::move(dec));
    init = core::move(function_call);

  } else {
    memory::unique_ptr<ast_id> id =
        memory::make_unique<ast_id>(m_allocator, _alloc);
    id->m_declaration = dec.get();
    id->m_type = dec->m_type;

    init =
        make_cast(core::move(id), m_type_manager->get_reference_of(dec->m_type,
                                      ast_type_classification::reference));
    init->initialized_setup_statements(m_allocator).push_back(core::move(dec));
  }

  if (alloc_type->m_destructor) {
    memory::unique_ptr<ast_function_call_expression> function_call =
        memory::make_unique<ast_function_call_expression>(m_allocator, _alloc);
    function_call->m_function = alloc_type->m_destructor;

    memory::unique_ptr<ast_id> id =
        memory::make_unique<ast_id>(m_allocator, _alloc);
    id->m_declaration = decl;
    id->m_type = decl->m_type;

    memory::unique_ptr<ast_cast_expression> cast =
        memory::make_unique<ast_cast_expression>(m_allocator, _alloc);
    cast->m_base_expression = core::move(id);
    cast->m_type = m_type_manager->get_reference_of(
        cast->m_base_expression->m_type, ast_type_classification::reference);
    function_call->initialized_parameters(m_allocator)
        .push_back(core::move(cast));
    function_call->m_type = m_type_manager->m_void_t.get();
    init->initialized_destroy_expressions(m_allocator)
        .push_back(core::move(function_call));
  }

  return core::move(init);
}

memory::unique_ptr<ast_expression>
parse_ast_convertor::convertor_context::resolve_builtin_unary_expression(
    const builtin_unary_expression* _expr) {
  memory::unique_ptr<ast_expression> c =
      resolve_expression(_expr->root_expression());
  switch (_expr->get_unary_type()) {
    case builtin_unary_type::length: {
      if (c->m_type->m_builtin == builtin_type::c_string_type) {
        auto fncall = memory::make_unique<ast_function_call_expression>(
            m_allocator, _expr);
        fncall->m_function = m_strlen;
        fncall->initialized_parameters(m_allocator).push_back(core::move(c));
        fncall->m_type = m_strlen->m_return_type;
        return core::move(fncall);
      } else {
        if (c->m_type->m_classification !=
            ast_type_classification::static_array) {
          _expr->log_line(m_log, logging::log_level ::error);
          m_log->log_error(
              "length can only be applied to a string or an array");
          return nullptr;
        }
        auto builtin =
            memory::make_unique<ast_builtin_expression>(m_allocator, _expr);
        builtin->m_builtin_type = builtin_expression_type::array_length;
        builtin->initialized_expressions(m_allocator).push_back(core::move(c));
        builtin->m_type = m_type_manager->m_integral_types[32].get();
        return core::move(builtin);
      }
    }
    default:
      WN_RELEASE_ASSERT(false, "Unknown unary type");
  }
  return nullptr;
}

memory::unique_ptr<ast_expression> parse_ast_convertor::convertor_context::
    resolve_shared_struct_allocation_expression(
        const struct_allocation_expression* _alloc) {
  const ast_type* t = resolve_type(_alloc->get_type());
  if (!t) {
    return nullptr;
  }
  WN_DEBUG_ASSERT(
      t->m_classification == ast_type_classification::shared_reference,
      "Unhandled shared allocation with non shared type");
  const ast_type* st = t->m_implicitly_contained_type;

  if (st->m_classification != ast_type_classification::struct_type) {
    _alloc->log_line(m_log, logging::log_level ::error);
    m_log->log_error("You must allocate a struct with a known struct type");
    return nullptr;
  }
  ast_type* alloc_type = st->m_overloaded_construction_child;
  WN_DEBUG_ASSERT(alloc_type && alloc_type->m_constructor,
      "Have a struct type without a proper constructor");

  auto num_bytes =
      memory::make_unique<ast_builtin_expression>(m_allocator, _alloc);
  num_bytes->m_type = m_type_manager->m_size_t.get();
  num_bytes->initialized_extra_types(m_allocator).push_back(alloc_type);
  num_bytes->m_builtin_type = builtin_expression_type::size_of;

  memory::unique_ptr<ast_expression> dest;

  if (alloc_type->m_destructor) {
    auto d = memory::make_unique<ast_function_pointer_expression>(
        m_allocator, _alloc);
    d->m_type = alloc_type->m_destructor->m_function_pointer_type;
    d->m_function = alloc_type->m_destructor;
    dest = core::move(d);
  } else {
    auto const_null = memory::make_unique<ast_constant>(m_allocator, _alloc);
    const_null->m_string_value = containers::string(m_allocator, "");
    const_null->m_type = m_type_manager->m_nullptr_t.get();
    dest = core::move(const_null);
  }

  auto dest_as_void =
      memory::make_unique<ast_cast_expression>(m_allocator, _alloc);
  dest_as_void->m_type = m_destructor_fn_ptr_t;
  dest_as_void->m_base_expression = core::move(dest);

  memory::unique_ptr<ast_function_call_expression> allocate =
      memory::make_unique<ast_function_call_expression>(m_allocator, _alloc);
  allocate->m_function = m_allocate_shared;
  allocate->m_type = m_allocate_shared->m_return_type;
  allocate->initialized_parameters(m_allocator)
      .push_back(core::move(num_bytes));
  allocate->initialized_parameters(m_allocator)
      .push_back(core::move(dest_as_void));

  memory::unique_ptr<ast_function_call_expression> function_call =
      memory::make_unique<ast_function_call_expression>(m_allocator, _alloc);
  function_call->m_function = alloc_type->m_constructor;

  memory::unique_ptr<ast_cast_expression> cast =
      memory::make_unique<ast_cast_expression>(m_allocator, _alloc);
  cast->m_base_expression = core::move(allocate);
  cast->m_type = alloc_type->m_constructor->m_parameters[0].m_type;
  function_call->initialized_parameters(m_allocator)
      .push_back(core::move(cast));
  function_call->m_type = m_type_manager->get_reference_of(
      st, ast_type_classification::shared_reference);

  return core::move(function_call);
}

memory::unique_ptr<ast_expression>
parse_ast_convertor::convertor_context::resolve_member_access_expression(
    const member_access_expression* _expression) {
  auto base_expr = resolve_expression(_expression->get_base_expression());
  if (!base_expr) {
    return nullptr;
  }

  auto struct_type = base_expr->m_type;
  if (base_expr->m_type->m_classification ==
      ast_type_classification::reference) {
    struct_type = base_expr->m_type->m_implicitly_contained_type;
  }
  if (base_expr->m_type->m_classification ==
      ast_type_classification::shared_reference) {
    struct_type = base_expr->m_type->m_implicitly_contained_type;
  }

  auto member = memory::make_unique<ast_member_access_expression>(
      m_allocator, _expression);
  transfer_temporaries(member.get(), base_expr.get());

  member->m_base_expression = core::move(base_expr);
  member->m_member_name =
      containers::string(m_allocator, _expression->get_name());

  uint32_t child_pos = 0;
  // child_pos cannot be 0, that would not work.
  for (auto& cp : struct_type->m_structure_members) {
    if (cp->m_name == member->m_member_name) {
      break;
    }
    child_pos++;
  }
  // If we found a member with this name use it.
  if (child_pos != struct_type->m_structure_members.size()) {
    member->m_member_offset = child_pos;
    member->m_type = struct_type->m_structure_members[child_pos]->m_type;
    return core::move(member);
  }
  // Try to find a member function with this name instead.
  for (auto it : struct_type->m_member_functions) {
    if (it->m_name == member->m_member_name) {
      // At least one member function exists
      member->m_type = m_type_manager->m_function_t.get();
      return core::move(member);
    }
  }

  auto extern_it =
      struct_type->m_contained_external_types.find(member->m_member_name);
  if (extern_it != struct_type->m_contained_external_types.end()) {
    member->m_member_offset = extern_it->second.order;
    member->m_type = extern_it->second.type;
    return core::move(member);
  }

  for (auto& it : struct_type->m_external_member_functions) {
    if (it->m_name == member->m_member_name) {
      member->m_type = m_type_manager->m_function_t.get();
      return core::move(member);
    }
  }

  _expression->log_line(m_log, logging::log_level::error);
  m_log->log_error("Cannot find member");
  return nullptr;
}

}  // namespace scripting
}  // namespace wn