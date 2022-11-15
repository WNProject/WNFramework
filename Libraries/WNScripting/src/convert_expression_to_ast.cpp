// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNContainers/inc/WNDeque.h"
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
    case node_type::slice_expression:
      return resolve_slice_expression(cast_to<slice_expression>(_expression));
    case node_type::function_call_expression:
      return resolve_function_call(
          cast_to<function_call_expression>(_expression));
    case node_type::member_access_expression:
      return resolve_member_access_expression(
          cast_to<member_access_expression>(_expression));
    case node_type::post_unary_expression:
      return resolve_post_unary_expression(
          cast_to<post_unary_expression>(_expression));
    case node_type::short_circuit_expression:
      WN_RELEASE_ASSERT(false, "Not Implemented expression type");
    case node_type::struct_allocation_expression:
      return resolve_struct_allocation_expression(
          cast_to<struct_allocation_expression>(_expression));
    case node_type::unary_expression:
      return resolve_unary_expression(cast_to<unary_expression>(_expression));
    case node_type::builtin_unary_expression:
      return resolve_builtin_unary_expression(
          cast_to<builtin_unary_expression>(_expression));
    case node_type::resource_expression:
      return resolve_resource(cast_to<resource_expression>(_expression));
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

memory::unique_ptr<ast_expression>
parse_ast_convertor::convertor_context::resolve_resource(
    const resource_expression* _resource) {
  containers::string fn_call_name(m_allocator);
  containers::string_view data = _resource->get_string();
  core::optional<uintptr_t> user_data;
  containers::string decoded(m_allocator);

  if (!decode_string(data, &decoded)) {
    _resource->log_line(m_log, logging::log_level::error);
    m_log->log_error("Invalid resource: ", _resource->get_type());
    m_log->log_error(decoded);
    return nullptr;
  }
  const ast_type* t = m_type_manager->get_resource(
      _resource->get_type(), decoded, &fn_call_name, &user_data);
  if (t == nullptr) {
    _resource->log_line(m_log, logging::log_level::error);
    m_log->log_error("Invalid resource: ", _resource->get_type());
    m_log->log_error("                : ", _resource->get_string());
    return nullptr;
  }

  memory::unique_ptr<function_call_expression> fce =
      memory::make_unique<function_call_expression>(m_allocator, m_allocator);
  fce->set_start_location(_resource->get_start_location());

  memory::unique_ptr<id_expression> ide = memory::make_unique<id_expression>(
      m_allocator, m_allocator, fn_call_name.c_str());
  ide->copy_location_from(fce.get());
  fce->add_base_expression(core::move(ide));
  if (user_data) {
    char data_val[16];
    data_val[15] = '\0';
    uintptr_t d = user_data.value();
    size_t offset = 14;
    while (d) {
      data_val[offset] = '0' + (d % 10);
      d /= 10;
      offset -= 1;
    }
    if (offset == 14) {
      data_val[offset] = '0';
      offset -= 1;
    }
    ++offset;

    memory::unique_ptr<constant_expression> eparam =
        memory::make_unique<constant_expression>(m_allocator, m_allocator,
            type_classification::void_ptr_type, &data_val[offset]);
    eparam->copy_location_from(fce.get());
    memory::unique_ptr<function_expression> expr =
        memory::make_unique<function_expression>(
            m_allocator, core::move(eparam), false);
    fce->get_expressions().push_back(core::move(expr));
  }
  for (auto& expr : _resource->get_expressions()) {
    fce->get_expressions().push_back(expr->clone(m_allocator));
  }
  return resolve_function_call(fce.get());
}  // namespace scripting

memory::unique_ptr<ast_constant>
parse_ast_convertor::convertor_context::get_constant(
    const node* _node, const ast_type* _type, const containers::string& value) {
  memory::unique_ptr<ast_constant> c =
      memory::make_unique<ast_constant>(m_allocator, _node);
  c->m_string_value = containers::string(m_allocator, value);
  c->m_type = _type;

  if (_type == m_type_manager->integral(32, nullptr)) {
    long long val = atoll(value.c_str());
    c->m_node_value.m_integer = static_cast<int32_t>(val);
    return c;
  } else if (_type == m_type_manager->size_t_t(nullptr)) {
    unsigned long long val = ::strtoull(value.c_str(), nullptr, 0);
    c->m_node_value.m_size_t = static_cast<size_t>(val);
    return c;
  } else if (_type == m_type_manager->void_ptr_t(nullptr)) {
    unsigned long long val = ::strtoull(value.c_str(), nullptr, 0);
    c->m_node_value.m_size_t = static_cast<size_t>(val);
    return c;
  } else if (_type == m_type_manager->nullptr_t(nullptr)) {
    c->m_node_value.m_integer = static_cast<int32_t>(0);
    return c;
  } else if (_type == m_type_manager->floating(32, nullptr)) {
    double val = atof(value.c_str());
    c->m_node_value.m_float = static_cast<float>(val);
    return c;
  } else if (_type == m_type_manager->integral(8, nullptr)) {
    c->m_node_value.m_char = c->m_string_value[1];
    return c;
  } else if (_type == m_type_manager->cstr_t(nullptr)) {
    c->m_string_value.clear();
    if (!decode_string(value, &c->m_string_value)) {
      _node->log_line(m_log, logging::log_level::error);
      m_log->log_error(c->m_string_value);
    }
    return c;
  } else if (_type == m_type_manager->bool_t(nullptr)) {
    if (value == "true") {
      c->m_node_value.m_bool = true;
    } else if (value == "false") {
      c->m_node_value.m_bool = false;
    } else {
      _node->log_line(m_log, logging::log_level::error);
      m_log->log_error("Unknown boolean constant: ", value);
      return nullptr;
    }
    return c;
  }
  WN_RELEASE_ASSERT(false, "Unhandled: Custom constants");
  return nullptr;
}

memory::unique_ptr<ast_constant>
parse_ast_convertor::convertor_context::resolve_constant(
    const constant_expression* _const) {
  const ast_type* const_type = nullptr;
  switch (_const->get_index()) {
    case static_cast<uint32_t>(type_classification::int_type): {
      const_type = m_type_manager->integral(32, &m_used_types);
      break;
    }
    case static_cast<uint32_t>(type_classification::nullptr_type): {
      const_type = m_type_manager->nullptr_t(&m_used_types);
      break;
    }
    case static_cast<uint32_t>(type_classification::float_type): {
      const_type = m_type_manager->floating(32, &m_used_types);
      break;
    }
    case static_cast<uint32_t>(type_classification::char_type): {
      const_type = m_type_manager->integral(8, &m_used_types);
      break;
    }
    case static_cast<uint32_t>(type_classification::string_type): {
      const_type = m_type_manager->cstr_t(&m_used_types);
      break;
    }
    case static_cast<uint32_t>(type_classification::bool_type): {
      const_type = m_type_manager->bool_t(&m_used_types);
      break;
    }
    case static_cast<uint32_t>(type_classification::size_type): {
      const_type = m_type_manager->size_t_t(&m_used_types);
      break;
    }
    case static_cast<uint32_t>(type_classification::void_ptr_type): {
      const_type = m_type_manager->void_ptr_t(&m_used_types);
      break;
    }
    default:
      WN_RELEASE_ASSERT(false, "Unhandled: Custom constants");
  }
  return get_constant(_const, const_type, _const->get_type_text());
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

  if ((lhs->m_type->m_classification ==
              ast_type_classification::shared_reference ||
          lhs->m_type->m_classification == ast_type_classification::reference ||
          lhs->m_type->m_classification ==
              ast_type_classification::actor_type) &&
      rhs->m_type == m_type_manager->nullptr_t(&m_used_types)) {
    rhs = make_cast(core::move(rhs), lhs->m_type);
  }
  if ((rhs->m_type->m_classification ==
              ast_type_classification::shared_reference ||
          rhs->m_type->m_classification == ast_type_classification::reference ||
          rhs->m_type->m_classification ==
              ast_type_classification::actor_type) &&
      lhs->m_type == m_type_manager->nullptr_t(&m_used_types)) {
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
    case arithmetic_type::arithmetic_and:
    case arithmetic_type::arithmetic_or:
    case arithmetic_type::arithmetic_xor:
      if (bin->m_lhs->m_type->m_builtin != builtin_type::integral_type &&
          bin->m_lhs->m_type->m_builtin != builtin_type::bool_type &&
          bin->m_lhs->m_type->m_builtin != builtin_type::size_type) {
        _bin->log_line(m_log, logging::log_level::error);
        m_log->log_error("Bitwise operators must be on Int or Bool types.");
        return nullptr;
      }
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
      bin->m_type = m_type_manager->bool_t(&m_used_types);
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
    case arithmetic_type::arithmetic_and:
      bin->m_binary_type = ast_binary_type::bitwise_and;
      break;
    case arithmetic_type::arithmetic_or:
      bin->m_binary_type = ast_binary_type::bitwise_or;
      break;
    case arithmetic_type::arithmetic_xor:
      bin->m_binary_type = ast_binary_type::bitwise_xor;
      break;

    default:
      WN_RELEASE_ASSERT(false, "You should never get here");
  }
  return bin;
}

memory::unique_ptr<ast_unary_expression>
parse_ast_convertor::convertor_context::resolve_unary_expression(
    const unary_expression* _unary) {
  auto base = resolve_expression(_unary->get_root_expression());
  if (!base) {
    return nullptr;
  }

  memory::unique_ptr<ast_unary_expression> expr =
      memory::make_unique<ast_unary_expression>(m_allocator, _unary);

  expr->m_base_expression = core::move(base);
  expr->m_type = expr->m_base_expression->m_type;
  auto ut = _unary->get_unary_type();
  switch (ut) {
    case unary_type::inversion:
      if (expr->m_type->m_builtin != builtin_type::bool_type &&
          expr->m_type->m_builtin != builtin_type::integral_type) {
        _unary->log_line(m_log, logging::log_level::error);
        m_log->log_error("You can only invert Int and Bool types");
        return nullptr;
      }
      expr->m_unary_type = ast_unary_type::invert;
      break;
    case unary_type::negation:
      if (expr->m_type->m_builtin != builtin_type::integral_type) {
        _unary->log_line(m_log, logging::log_level::error);
        m_log->log_error("You can only negate Int");
        return nullptr;
      }
      expr->m_unary_type = ast_unary_type::negate;
      break;
    case unary_type::pre_decrement:
      if (expr->m_type->m_builtin != builtin_type::integral_type) {
        _unary->log_line(m_log, logging::log_level::error);
        m_log->log_error("You can only decrement Int");
        return nullptr;
      }
      expr->m_unary_type = ast_unary_type::pre_decrement;
      break;
    case unary_type::pre_increment:
      if (expr->m_type->m_builtin != builtin_type::integral_type) {
        _unary->log_line(m_log, logging::log_level::error);
        m_log->log_error("You can only increment Int");
        return nullptr;
      }
      expr->m_unary_type = ast_unary_type::pre_increment;
      break;
    default:
      WN_RELEASE_ASSERT(false, "Unknown expression type");
  }

  if (ut == unary_type::pre_decrement || ut == unary_type::pre_increment) {
    if (expr->m_base_expression->m_node_type !=
            ast_node_type::ast_member_access_expression &&
        expr->m_base_expression->m_node_type != ast_node_type::ast_id &&
        expr->m_base_expression->m_node_type !=
            ast_node_type::ast_array_access_expression) {
      _unary->log_line(m_log, logging::log_level::error);
      m_log->log_error("Operand must be an lvalue");
      return nullptr;
    }
  }

  return expr;
}

memory::unique_ptr<ast_unary_expression>
parse_ast_convertor::convertor_context::resolve_post_unary_expression(
    const post_unary_expression* _unary) {
  auto base = resolve_expression(_unary->get_base_expression());
  if (!base) {
    return nullptr;
  }

  memory::unique_ptr<ast_unary_expression> expr =
      memory::make_unique<ast_unary_expression>(m_allocator, _unary);

  expr->m_base_expression = core::move(base);
  expr->m_type = expr->m_base_expression->m_type;
  auto ut = _unary->get_post_unary_type();
  switch (ut) {
    case post_unary_type::post_increment:
      if (expr->m_type->m_builtin != builtin_type::integral_type) {
        _unary->log_line(m_log, logging::log_level::error);
        m_log->log_error("You can only increment Int");
        return nullptr;
      }
      expr->m_unary_type = ast_unary_type::post_increment;
      break;
    case post_unary_type::post_decrement:
      if (expr->m_type->m_builtin != builtin_type::integral_type) {
        _unary->log_line(m_log, logging::log_level::error);
        m_log->log_error("You can only decrement Int");
        return nullptr;
      }
      expr->m_unary_type = ast_unary_type::post_decrement;
      break;
    default:
      WN_RELEASE_ASSERT(false, "Unknown expression type");
  }

  if (expr->m_base_expression->m_node_type !=
          ast_node_type::ast_member_access_expression &&
      expr->m_base_expression->m_node_type != ast_node_type::ast_id &&
      expr->m_base_expression->m_node_type !=
          ast_node_type::ast_array_access_expression) {
    _unary->log_line(m_log, logging::log_level::error);
    m_log->log_error("Operand must be an lvalue");
    return nullptr;
  }

  return expr;
}

memory::unique_ptr<ast_expression>
parse_ast_convertor::convertor_context::resolve_array_allocation_expression(
    const array_allocation_expression* _alloc) {
  if (_alloc->get_array_initializers().size() != 1 &&
      !_alloc->get_inline_initializers()) {
    _alloc->log_line(m_log, logging::log_level::error);
    m_log->log_error("You must have a size for an array initializer");
    return nullptr;
  }

  bool is_runtime = _alloc->is_runtime();

  const ast_type* t = nullptr;
  memory::unique_ptr<ast_expression> array_size;
  if (_alloc->get_inline_initializers()) {
    auto const_size = memory::make_unique<ast_constant>(m_allocator, nullptr);
    const_size->m_type = m_type_manager->integral(32, &m_used_types);
    const_size->m_node_value.m_integer = static_cast<int32_t>(
        _alloc->get_inline_initializers()->get_expressions().size());

    char buff[11] = {
        0,
    };
    memory::writeuint32(buff,
        static_cast<uint32_t>(
            _alloc->get_inline_initializers()->get_expressions().size()),
        11);
    const_size->m_string_value = containers::string(m_allocator, buff);
    array_size = core::move(const_size);
  } else {
    array_size = resolve_expression(_alloc->get_array_initializers()[0].get());
  }

  const ast_type* array_base_type = resolve_type(_alloc->get_type());

  if (!is_runtime && array_size->m_node_type != ast_node_type::ast_constant) {
    _alloc->log_line(m_log, logging::log_level::error);
    m_log->log_error("Array size must be static");
  }

  if (is_runtime) {
    t = get_runtime_array_of(array_base_type);
  } else {
    auto* array_const = cast_to<ast_constant>(array_size.get());
    if (array_const->m_type != m_type_manager->integral(32, &m_used_types)) {
      _alloc->log_line(m_log, logging::log_level::error);
      m_log->log_error("Array size must be an Integer");
    }
    t = get_array_of(array_base_type, array_const->m_node_value.m_integer);
  }

  memory::unique_ptr<ast_expression> assigned_declaration =
      core::move(m_assigned_declaration);
  memory::unique_ptr<ast_expression> assign_back = nullptr;

  memory::unique_ptr<ast_declaration> temp_declaration;

  bool cleanup_after_statement = assigned_declaration == nullptr;
  bool make_temporary =
      !assigned_declaration || assigned_declaration->m_type != t;

  if (make_temporary) {
    // Make a temporary here and fill it.
    temp_declaration =
        make_temp_declaration(_alloc, get_next_temporary_name(), t);
    memory::make_unique<ast_declaration>(m_allocator, _alloc);
    temp_declaration->m_is_scope_bound = true;

    memory::unique_ptr<ast_id> id =
        memory::make_unique<ast_id>(m_allocator, _alloc);
    id->m_declaration = temp_declaration.get();
    id->m_type = t;

    m_current_statements->push_back(core::move(temp_declaration));
    assign_back = core::move(assigned_declaration);
    assigned_declaration = core::move(id);
  }

  if (is_runtime) {
    auto allocate_runtime =
        memory::make_unique<ast_function_call_expression>(m_allocator, _alloc);
    allocate_runtime->m_function = m_external_functions[containers::string(
        m_allocator, "_allocate_runtime_array")];

    auto num_bytes =
        memory::make_unique<ast_builtin_expression>(m_allocator, _alloc);
    num_bytes->m_type = m_type_manager->size_t_t(&m_used_types);
    num_bytes->initialized_extra_types(m_allocator)
        .push_back(t->m_implicitly_contained_type);
    num_bytes->m_builtin_type = builtin_expression_type::size_of;

    allocate_runtime->initialized_parameters(m_allocator)
        .push_back(core::move(num_bytes));
    allocate_runtime->initialized_parameters(m_allocator)
        .push_back(make_cast(clone_ast_node(m_allocator, array_size.get()),
            m_type_manager->size_t_t(&m_used_types)));
    allocate_runtime->m_type = m_type_manager->void_ptr_t(&m_used_types);

    auto runtime_assign =
        memory::make_unique<ast_assignment>(m_allocator, _alloc);
    runtime_assign->m_lhs =
        clone_ast_node(m_allocator, assigned_declaration.get());
    runtime_assign->m_rhs =
        make_cast(core::move(allocate_runtime), runtime_assign->m_lhs->m_type);
    m_current_statements->push_back(core::move(runtime_assign));
  }

  // array.length = <size>
  auto assign_size =
      memory::make_unique<ast_builtin_statement>(m_allocator, _alloc);
  assign_size->m_builtin_type =
      builtin_statement_type::internal_set_array_length;
  assign_size->initialized_expressions(m_allocator)
      .push_back(clone_ast_node(m_allocator, assigned_declaration.get()));
  assign_size->initialized_expressions(m_allocator)
      .push_back(clone_ast_node(m_allocator, array_size.get()));
  m_current_statements->push_back(core::move(assign_size));

  if (!_alloc->get_inline_initializers() &&
      t->m_implicitly_contained_type->m_classification !=
          ast_type_classification::reference) {
    // array.length
    auto set_size =
        memory::make_unique<ast_builtin_expression>(m_allocator, _alloc);
    set_size->m_builtin_type = builtin_expression_type::array_length;
    set_size->initialized_expressions(m_allocator)
        .push_back(clone_ast_node(m_allocator, assigned_declaration.get()));

    auto zero = memory::make_unique<ast_constant>(m_allocator, _alloc);
    zero->m_type = m_type_manager->integral(32, &m_used_types);
    zero->m_node_value.m_integer = 0;
    zero->m_string_value = containers::string(m_allocator, "0");

    // int i = 0;
    memory::unique_ptr<ast_declaration> counter = make_temp_declaration(_alloc,
        get_next_temporary_name(), m_type_manager->integral(32, &m_used_types));
    memory::make_unique<ast_declaration>(m_allocator, _alloc);
    counter->m_initializer = clone_ast_node(m_allocator, zero.get());

    auto current_size = memory::make_unique<ast_id>(m_allocator, _alloc);
    current_size->m_type = m_type_manager->integral(32, &m_used_types);
    current_size->m_declaration = counter.get();
    m_current_statements->push_back(core::move(counter));

    // i != array.length
    auto not_done =
        memory::make_unique<ast_binary_expression>(m_allocator, _alloc);
    not_done->m_type = m_type_manager->bool_t(&m_used_types);
    not_done->m_lhs = clone_ast_node(m_allocator, current_size.get());
    not_done->m_rhs = clone_ast_node(m_allocator, set_size.get());
    not_done->m_binary_type = ast_binary_type::lt;

    memory::unique_ptr<ast_loop> m_loop =
        memory::make_unique<ast_loop>(m_allocator, _alloc);
    m_loop->m_pre_condition = core::move(not_done);

    m_loop->m_body = memory::make_unique<ast_scope_block>(m_allocator, _alloc);
    push_scope(m_loop->m_body.get());

    auto target =
        memory::make_unique<ast_array_access_expression>(m_allocator, nullptr);
    target->m_type = t->m_implicitly_contained_type;
    target->m_array = clone_ast_node(m_allocator, assigned_declaration.get());
    target->m_index = clone_ast_node(m_allocator, current_size.get());
    if (t->m_implicitly_contained_type->m_classification ==
            ast_type_classification::struct_type ||
        t->m_implicitly_contained_type->m_classification ==
            ast_type_classification::shared_reference ||
        t->m_implicitly_contained_type->m_classification ==
            ast_type_classification::actor_type) {
      m_assigned_declaration = core::move(target);
      m_declaration_succeeded = false;
      resolve_expression(_alloc->get_copy_initializer());
      if (!m_declaration_succeeded) {
        return nullptr;
      }
    } else {
      auto set_value = memory::make_unique<ast_assignment>(m_allocator, _alloc);
      set_value->m_lhs = core::move(target);
      auto expr = resolve_expression(_alloc->get_copy_initializer());
      if (!expr) {
        return nullptr;
      }
      set_value->m_rhs = core::move(expr);
      m_loop->m_body->initialized_statements(m_allocator)
          .push_back(core::move(set_value));
      for (auto& clean : m_temporary_cleanup) {
        m_loop->m_body->initialized_statements(m_allocator)
            .push_back(core::move(clean));
      }
      m_temporary_cleanup.clear();
    }

    {
      auto const_one = memory::make_unique<ast_constant>(m_allocator, _alloc);
      const_one->m_type = m_type_manager->integral(32, &m_used_types);
      const_one->m_node_value.m_integer = 1;
      const_one->m_string_value = containers::string(m_allocator, "1");

      // i = i + 1;
      auto add =
          memory::make_unique<ast_binary_expression>(m_allocator, _alloc);
      add->m_binary_type = ast_binary_type::add;
      add->m_lhs = clone_ast_node(m_allocator, current_size.get());
      add->m_rhs = core::move(const_one);
      add->m_type = m_type_manager->integral(32, &m_used_types);

      auto increment_assign =
          memory::make_unique<ast_assignment>(m_allocator, _alloc);
      increment_assign->m_lhs = clone_ast_node(m_allocator, current_size.get());
      increment_assign->m_rhs = core::move(add);

      m_loop->m_body->initialized_statements(m_allocator)
          .push_back(core::move(increment_assign));
    }
    pop_scope();
    m_current_statements->push_back(core::move(m_loop));
  } else if (!_alloc->get_inline_initializers() &&
             t->m_implicitly_contained_type->m_classification ==
                 ast_type_classification::reference) {
    // We are going to create this one as a set of invidual values, and assign
    // them into an array.
    if (is_runtime) {
      _alloc->log_line(m_log, logging::log_level::error);
      m_log->log_error(
          "Cannot create a runtime array of classes, only a static array");
      return nullptr;
    }

    for (size_t i = 0; i < t->m_static_array_size; ++i) {
      char buff[11] = {
          0,
      };
      memory::writeuint32(buff, static_cast<uint32_t>(i), 11);

      auto const_i = memory::make_unique<ast_constant>(m_allocator, _alloc);
      const_i->m_type = m_type_manager->integral(32, &m_used_types);
      const_i->m_node_value.m_integer = static_cast<int32_t>(i);
      const_i->m_string_value = containers::string(m_allocator, buff);

      auto num = memory::make_unique<ast_constant>(m_allocator, _alloc);
      num->m_type = m_type_manager->integral(32, &m_used_types);
      num->m_node_value.m_integer = static_cast<int32_t>(i);
      num->m_string_value = containers::string(m_allocator, buff);

      auto target = memory::make_unique<ast_array_access_expression>(
          m_allocator, nullptr);
      target->m_type = t->m_implicitly_contained_type;
      target->m_array = clone_ast_node(m_allocator, assigned_declaration.get());
      target->m_index = core::move(num);

      auto t_dcl = make_temp_declaration(
          _alloc, get_next_temporary_name(), t->m_implicitly_contained_type);
      t_dcl->m_is_scope_bound = true;
      auto id = id_to(_alloc, t_dcl.get());
      m_current_statements->push_back(core::move(t_dcl));

      m_assigned_declaration = clone_ast_node(m_allocator, id.get());
      m_declaration_succeeded = false;
      resolve_expression(_alloc->get_copy_initializer());
      if (!m_declaration_succeeded) {
        return nullptr;
      }

      auto set_value = memory::make_unique<ast_assignment>(m_allocator, _alloc);
      set_value->m_lhs = core::move(target);
      set_value->m_rhs = make_cast(core::move(id), set_value->m_lhs->m_type);
      m_current_statements->push_back(core::move(set_value));
      for (auto& clean : m_temporary_cleanup) {
        m_current_statements->push_back(core::move(clean));
      }
      m_temporary_cleanup.clear();
    }
  } else if (t->m_implicitly_contained_type->m_classification ==
             ast_type_classification::reference) {
    const arg_list* initializers = _alloc->get_inline_initializers();
    size_t i = 0;
    for (const auto& expr : initializers->get_expressions()) {
      char buff[11] = {
          0,
      };
      memory::writeuint32(buff, static_cast<uint32_t>(i), 11);

      auto num = memory::make_unique<ast_constant>(m_allocator, _alloc);
      num->m_type = m_type_manager->integral(32, &m_used_types);
      num->m_node_value.m_integer = static_cast<int32_t>(i);
      num->m_string_value = containers::string(m_allocator, buff);
      ++i;

      auto target = memory::make_unique<ast_array_access_expression>(
          m_allocator, nullptr);
      target->m_type = t->m_implicitly_contained_type;
      target->m_array = clone_ast_node(m_allocator, assigned_declaration.get());
      target->m_index = core::move(num);

      auto t_dcl = make_temp_declaration(
          _alloc, get_next_temporary_name(), t->m_implicitly_contained_type);
      t_dcl->m_is_scope_bound = true;
      auto id = id_to(_alloc, t_dcl.get());
      m_current_statements->push_back(core::move(t_dcl));

      m_assigned_declaration = clone_ast_node(m_allocator, id.get());
      m_declaration_succeeded = false;
      resolve_expression(expr->m_expr.get());
      if (!m_declaration_succeeded) {
        return nullptr;
      }

      auto set_value = memory::make_unique<ast_assignment>(m_allocator, _alloc);
      set_value->m_lhs = core::move(target);
      set_value->m_rhs = make_cast(core::move(id), set_value->m_lhs->m_type);
      m_current_statements->push_back(core::move(set_value));
      for (auto& clean : m_temporary_cleanup) {
        m_current_statements->push_back(core::move(clean));
      }
      m_temporary_cleanup.clear();
    }
  } else {
    const arg_list* initializers = _alloc->get_inline_initializers();
    size_t i = 0;
    for (const auto& expr : initializers->get_expressions()) {
      char buff[11] = {
          0,
      };
      memory::writeuint32(buff, static_cast<uint32_t>(i), 11);

      auto num = memory::make_unique<ast_constant>(m_allocator, _alloc);
      num->m_type = m_type_manager->integral(32, &m_used_types);
      num->m_node_value.m_integer = static_cast<int32_t>(i);
      num->m_string_value = containers::string(m_allocator, buff);
      ++i;

      auto target = memory::make_unique<ast_array_access_expression>(
          m_allocator, nullptr);
      target->m_type = t->m_implicitly_contained_type;
      target->m_array = clone_ast_node(m_allocator, assigned_declaration.get());
      target->m_index = core::move(num);

      if (t->m_implicitly_contained_type->m_classification ==
              ast_type_classification::struct_type ||
          t->m_implicitly_contained_type->m_classification ==
              ast_type_classification::shared_reference ||
          t->m_implicitly_contained_type->m_classification ==
              ast_type_classification::actor_type) {
        m_assigned_declaration = core::move(target);
        m_declaration_succeeded = false;
        resolve_expression(expr->m_expr.get());
        if (!m_declaration_succeeded) {
          return nullptr;
        }
        for (auto& clean : m_temporary_cleanup) {
          m_current_statements->push_back(core::move(clean));
        }
        m_temporary_cleanup.clear();

      } else {
        auto set_value =
            memory::make_unique<ast_assignment>(m_allocator, _alloc);
        set_value->m_lhs = core::move(target);
        auto ee = resolve_expression(expr->m_expr.get());
        if (!ee) {
          return nullptr;
        }
        set_value->m_rhs = make_cast(core::move(ee), set_value->m_lhs->m_type);
        m_current_statements->push_back(core::move(set_value));
        for (auto& clean : m_temporary_cleanup) {
          m_current_statements->push_back(core::move(clean));
        }
        m_temporary_cleanup.clear();
      }
    }
  }

  if (t->m_implicitly_contained_type->m_destructor) {
    auto dest = memory::make_unique<ast_array_destruction>(m_allocator, _alloc);
    dest->m_destructor = t->m_implicitly_contained_type->m_destructor;
    dest->m_shared = false;
    dest->m_target = clone_ast_node(m_allocator, assigned_declaration.get());

    if (cleanup_after_statement) {
      m_temporary_cleanup.push_back(core::move(dest));
    } else {
      m_nested_scopes.back()
          ->initialized_cleanup(m_allocator)
          .push_front(core::move(dest));
    }
  } else if (t->m_implicitly_contained_type->m_classification ==
             ast_type_classification::shared_reference) {
    auto dest = memory::make_unique<ast_array_destruction>(m_allocator, _alloc);
    dest->m_destructor = m_type_manager->release_shared(&m_used_builtins);
    dest->m_shared = true;
    dest->m_target = clone_ast_node(m_allocator, assigned_declaration.get());

    if (cleanup_after_statement) {
      m_temporary_cleanup.push_back(core::move(dest));
    } else {
      m_nested_scopes.back()
          ->initialized_cleanup(m_allocator)
          .push_front(core::move(dest));
    }
  } else if (t->m_implicitly_contained_type->m_classification ==
             ast_type_classification::actor_type) {
    auto dest = memory::make_unique<ast_array_destruction>(m_allocator, _alloc);
    dest->m_destructor = m_type_manager->release_actor(&m_used_builtins);
    dest->m_shared = true;
    dest->m_target = clone_ast_node(m_allocator, assigned_declaration.get());

    if (cleanup_after_statement) {
      m_temporary_cleanup.push_back(core::move(dest));
    } else {
      m_nested_scopes.back()
          ->initialized_cleanup(m_allocator)
          .push_front(core::move(dest));
    }
  }

  if (is_runtime) {
    auto free_runtime =
        memory::make_unique<ast_function_call_expression>(m_allocator, _alloc);
    free_runtime->m_function =
        m_external_functions[containers::string(m_allocator, "_free")];
    free_runtime->initialized_parameters(m_allocator)
        .push_back(
            make_cast(clone_ast_node(m_allocator, assigned_declaration.get()),
                m_type_manager->void_ptr_t(&m_used_types)));
    free_runtime->m_type = free_runtime->m_function->m_return_type;
    auto expression_statement =
        memory::make_unique<ast_evaluate_expression>(m_allocator, _alloc);
    expression_statement->m_expr = core::move(free_runtime);

    if (cleanup_after_statement) {
      m_temporary_cleanup.push_back(core::move(expression_statement));
    } else {
      m_nested_scopes.back()
          ->initialized_cleanup(m_allocator)
          .push_front(core::move(expression_statement));
    }
  }

  if (assign_back) {
    auto back_assign = memory::make_unique<ast_assignment>(m_allocator, _alloc);
    back_assign->m_lhs = core::move(assign_back);
    back_assign->m_rhs =
        make_cast(core::move(assigned_declaration), back_assign->m_lhs->m_type);
    m_current_statements->push_back(core::move(back_assign));
  }

  if (!cleanup_after_statement) {
    m_declaration_succeeded = true;
  }

  return assigned_declaration;
}

struct p_functions {
  virtual size_t size() = 0;
  virtual const ast_function* get(size_t i) = 0;
};

template <typename T>
struct p_functions_t : p_functions {
  const containers::deque<T*>* deq;
  size_t size() override {
    return deq->size();
  }
  const ast_function* get(size_t i) override {
    return (*deq)[i];
  }
};

memory::unique_ptr<ast_expression>
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
  p_functions_t<ast_function> m_non_const_functions;
  p_functions_t<const ast_function> m_const_functions;

  p_functions* possible_functions = nullptr;
  bool cast_first_param = false;
  bool synchronized_callee = false;
  bool is_this = false;
  switch (base_expr->m_node_type) {
    case ast_node_type::ast_id: {
      auto child_id = cast_to<ast_id>(base_expr.get());
      auto it = m_named_functions.find(child_id->name());
      if (it == m_named_functions.end()) {
        _call->log_line(m_log, logging::log_level::error);
        m_log->log_error("Cannot find function with name ", child_id->name());
        return nullptr;
      }
      m_const_functions.deq = &(it->second);
      possible_functions = &m_const_functions;
      function_name = containers::string(m_allocator, child_id->name());
      break;
    }
    case ast_node_type::ast_member_access_expression: {
      auto member_access =
          cast_to<ast_member_access_expression>(base_expr.get());
      if (member_access->m_type != m_type_manager->function_t(&m_used_types)) {
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
              ast_type_classification::extern_type &&
          struct_type->m_classification !=
              ast_type_classification::actor_type) {
        _call->log_line(m_log, logging::log_level::error);
        m_log->log_error("Trying to call a member on non struct type: ",
            struct_type->m_name);
      }
      is_this = member_access->m_is_this;
      synchronized_callee = struct_type->is_synchronized();
      cast_first_param = true;
      m_non_const_functions.deq = &struct_type->m_member_functions;
      possible_functions = &m_non_const_functions;
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

  for (size_t fi = 0; fi < possible_functions->size(); ++fi) {
    const auto fn = possible_functions->get(fi);
    if (total_matches == 0) {
      ++match_index;
    }
    if (fn->m_name != function_name) {
      continue;
    }
    size_t num_params = fn->m_parameters.size();
    if (fn->m_return_type->m_pass_by_reference) {
      num_params -= 1;
    }
    bool matches = true;
    if (num_params != params.size()) {
      continue;
    }
    for (size_t i = 0; i < params.size(); ++i) {
      if (cast_first_param && i == 0) {
        continue;
      }
      if (params[i]->m_type == fn->m_parameters[i].m_type) {
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

  function_call->m_function = possible_functions->get(match_index);
  if (is_this) {
    if (m_current_function->m_is_synchronized &&
        !function_call->m_function->m_is_synchronized &&
        !function_call->m_function->m_action_function) {
      _call->log_line(m_log, logging::log_level::error);
      m_log->log_error("Cannot call '", function_name,
          "' from a @synchronized function as it is neither an @action or "
          "@synchronized");
      return nullptr;
    }
  } else if (synchronized_callee &&
             !function_call->m_function->m_is_synchronized &&
             !function_call->m_function->m_action_function) {
    _call->log_line(m_log, logging::log_level::error);
    m_log->log_error("Cannot call '", function_name,
        "' as it is neither an @action or @synchronized");
    return nullptr;
  }

  if (function_call->m_function->m_is_external) {
    m_used_externals.insert(function_call->m_function);
  }
  for (size_t i = 0; i < params.size(); ++i) {
    if (i == 0 && cast_first_param) {
      params[i] = make_cast(core::move(params[i]),
          function_call->m_function->m_parameters[i].m_type);
    } else {
      if (function_call->m_function->m_parameters[i]
              .m_type->m_pass_by_reference) {
        containers::string temporary_name = get_next_temporary_name();

        memory::unique_ptr<ast_declaration> dec =
            make_temp_declaration(_call, get_next_temporary_name(),
                function_call->m_function->m_parameters[i].m_type);

        ast_declaration* decl = dec.get();
        decl->m_initializer = core::move(core::move(params[i]));

        memory::unique_ptr<ast_id> id =
            memory::make_unique<ast_id>(m_allocator, _call);
        id->m_declaration = dec.get();
        id->m_type = dec->m_type;
        params[i] = core::move(id);
        function_call->initialized_setup_statements(m_allocator)
            .push_back(core::move(dec));
      } else {
        params[i] = make_implicit_cast(core::move(params[i]),
            function_call->m_function->m_parameters[i].m_type);
      }
    }
  }
  function_call->m_type = function_call->m_function->m_return_type;

  if (function_call->m_function->m_return_type->m_pass_by_reference) {
    containers::string temporary_name = get_next_temporary_name();
    memory::unique_ptr<ast_declaration> dec = make_temp_declaration(_call,
        get_next_temporary_name(), function_call->m_function->m_return_type);
    memory::unique_ptr<ast_id> id =
        memory::make_unique<ast_id>(m_allocator, _call);
    id->m_declaration = dec.get();
    id->m_type = dec->m_type;

    memory::unique_ptr<ast_id> ret_id = clone_ast_node(m_allocator, id.get());

    params.push_back(core::move(id));

    function_call->initialized_setup_statements(m_allocator)
        .push_back(core::move(dec));

    transfer_temporaries(ret_id.get(), function_call.get());

    auto expr =
        memory::make_unique<ast_evaluate_expression>(m_allocator, _call);

    expr->m_expr = core::move(function_call);
    ret_id->initialized_setup_statements(m_allocator)
        .push_back(core::move(expr));
    return ret_id;
  }

  if (function_call->m_function->m_return_type->m_classification ==
          ast_type_classification::shared_reference ||
      function_call->m_function->m_return_type->m_classification ==
          ast_type_classification::actor_type) {
    const ast_type* t = function_call->m_function->m_return_type;
    auto td = make_temp_declaration(_call, get_next_temporary_name(), t);
    auto fc = function_call.get();

    td->m_initializer = core::move(function_call);
    auto id = id_to(_call, td.get());
    m_current_statements->push_back(core::move(td));

    memory::unique_ptr<ast_function_call_expression> destructor_call =
        memory::make_unique<ast_function_call_expression>(m_allocator, _call);
    destructor_call->m_function =
        fc->m_function->m_return_type->m_classification ==
                ast_type_classification::shared_reference
            ? m_type_manager->release_shared(&m_used_builtins)
            : m_type_manager->release_actor(&m_used_builtins);
    memory::unique_ptr<ast_cast_expression> cat_to_void_ptr =
        memory::make_unique<ast_cast_expression>(m_allocator, _call);
    cat_to_void_ptr->m_base_expression = clone_ast_node(m_allocator, id.get());
    cat_to_void_ptr->m_type = m_type_manager->void_ptr_t(&m_used_types);

    destructor_call->initialized_parameters(m_allocator)
        .push_back(core::move(cat_to_void_ptr));
    destructor_call->m_type = m_type_manager->void_t(&m_used_types);

    auto expression_statement =
        memory::make_unique<ast_evaluate_expression>(m_allocator, _call);
    expression_statement->m_expr = core::move(destructor_call);

    m_nested_scopes.back()
        ->initialized_cleanup(m_allocator)
        .push_back(core::move(expression_statement));

    return id;
  }

  if ((!is_this && function_call->m_function->m_action_function &&
          !m_current_function->m_is_action_caller) ||
      (is_this && function_call->m_function->m_action_function &&
          !m_current_function->m_action_function)) {
    // We are invoking an action on a different actor,
    // then instead of
    // foo->bar(a, b, c);
    // we instead call
    // __call_action_bar(0, foo, a, b, c);
    function_call->m_function =
        function_call->m_function->m_action_call_function;
    function_call->m_type = function_call->m_function->m_return_type;

    auto const_zero = memory::make_unique<ast_constant>(m_allocator, nullptr);
    const_zero->m_type = m_type_manager->integral(32, &m_used_types);
    const_zero->m_node_value.m_integer = 0;
    const_zero->m_string_value = containers::string(m_allocator, "0");

    function_call->m_parameters.push_front(core::move(const_zero));
  }

  return function_call;
}

memory::unique_ptr<ast_expression>
parse_ast_convertor::convertor_context::resolve_slice_expression(
    const slice_expression* _slice) {
  memory::unique_ptr<ast_expression> outer_expr =
      resolve_expression(_slice->get_base_expression());

  if (outer_expr->m_type->m_classification ==
      ast_type_classification::slice_type) {
    containers::string temporary_name = get_next_temporary_name();
    memory::unique_ptr<ast_declaration> dec =
        memory::make_unique<ast_declaration>(m_allocator, _slice);
    dec->m_name = containers::string(m_allocator, temporary_name);
    dec->m_type = outer_expr->m_type;

    memory::unique_ptr<ast_id> id =
        memory::make_unique<ast_id>(m_allocator, _slice);
    id->m_declaration = dec.get();
    id->m_type = dec->m_type;
    transfer_temporaries(id.get(), outer_expr.get());
    dec->m_initializer = core::move(outer_expr);
    id->initialized_setup_statements(m_allocator).push_back(core::move(dec));
    outer_expr = core::move(id);
  }

  memory::unique_ptr<ast_slice_expression> slice =
      memory::make_unique<ast_slice_expression>(m_allocator, _slice);
  transfer_temporaries(slice.get(), outer_expr.get());
  if (_slice->get_index0()) {
    auto index0 = resolve_expression(_slice->get_index0());
    if (index0->m_type != m_type_manager->integral(32, &m_used_types)) {
      _slice->log_line(m_log, logging::log_level ::error);
      m_log->log_error("Array slice must be an integer");
      return nullptr;
    }
    transfer_temporaries(slice.get(), index0.get());

    memory::unique_ptr<ast_declaration> dec = make_temp_declaration(
        _slice, get_next_temporary_name(), index0->m_type);

    dec->m_initializer = core::move(index0);

    memory::unique_ptr<ast_id> id =
        memory::make_unique<ast_id>(m_allocator, _slice);
    id->m_declaration = dec.get();
    id->m_type = dec->m_type;
    slice->initialized_setup_statements(m_allocator).push_back(core::move(dec));
    slice->m_index_0 = core::move(id);
  }

  if (_slice->get_index1()) {
    auto index1 = resolve_expression(_slice->get_index1());
    if (!index1) {
      return nullptr;
    }
    if (index1->m_type != m_type_manager->integral(32, &m_used_types)) {
      _slice->log_line(m_log, logging::log_level ::error);
      m_log->log_error("Array slice must be an integer");
      return nullptr;
    }
    transfer_temporaries(slice.get(), index1.get());
    slice->m_index_1 = core::move(index1);
  }

  if (outer_expr->m_type->m_classification !=
          ast_type_classification::static_array &&
      outer_expr->m_type->m_classification !=
          ast_type_classification::runtime_array &&
      outer_expr->m_type->m_classification !=
          ast_type_classification::slice_type &&
      outer_expr->m_type->m_builtin != builtin_type::c_string_type) {
    _slice->log_line(m_log, logging::log_level ::error);
    m_log->log_error("Cannot slice into a ", outer_expr->m_type->m_name);
    return nullptr;
  }

  if (outer_expr->m_type->m_classification ==
      ast_type_classification::slice_type) {
    slice->m_type = outer_expr->m_type;
  } else if (outer_expr->m_type->m_builtin == builtin_type::c_string_type) {
    slice->m_type = get_slice_of(m_type_manager->integral(8, &m_used_types), 1);
  } else {
    slice->m_type =
        get_slice_of(outer_expr->m_type->m_implicitly_contained_type, 1);
  }
  slice->m_array = core::move(outer_expr);
  return slice;
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

  if (inner_expr->m_type != m_type_manager->integral(32, &m_used_types)) {
    _access->log_line(m_log, logging::log_level ::error);
    m_log->log_error("Array access must be an integer");
    return nullptr;
  }

  if (outer_expr->m_type->m_classification !=
          ast_type_classification::static_array &&
      outer_expr->m_type->m_classification !=
          ast_type_classification::runtime_array &&
      outer_expr->m_type->m_classification !=
          ast_type_classification::slice_type &&
      outer_expr->m_type->m_builtin != builtin_type::c_string_type) {
    _access->log_line(m_log, logging::log_level ::error);
    m_log->log_error("Cannot index a ", outer_expr->m_type->m_name);
    return nullptr;
  }

  memory::unique_ptr<ast_array_access_expression> array_access =
      memory::make_unique<ast_array_access_expression>(m_allocator, _access);
  if (outer_expr->m_type->m_classification ==
      ast_type_classification::slice_type) {
    if (outer_expr->m_type->m_slice_dimensions == 1) {
      array_access->m_type = outer_expr->m_type->m_implicitly_contained_type;
    } else {
      memory::unique_ptr<ast_declaration> dec = make_temp_declaration(
          _access, get_next_temporary_name(), outer_expr->m_type);

      memory::unique_ptr<ast_id> id =
          memory::make_unique<ast_id>(m_allocator, _access);
      id->m_declaration = dec.get();
      id->m_type = dec->m_type;
      transfer_temporaries(id.get(), outer_expr.get());
      array_access->m_type =
          get_slice_of(outer_expr->m_type->m_implicitly_contained_type,
              outer_expr->m_type->m_slice_dimensions - 1);

      dec->m_initializer = core::move(outer_expr);
      outer_expr = core::move(id);
      outer_expr->initialized_setup_statements(m_allocator)
          .push_back(core::move(dec));
    }
  } else if (outer_expr->m_type->m_builtin != builtin_type::c_string_type) {
    array_access->m_type = outer_expr->m_type->m_implicitly_contained_type;
  } else {
    array_access->m_type = m_type_manager->integral(8, &m_used_types);
  }
  transfer_temporaries(array_access.get(), inner_expr.get());
  transfer_temporaries(array_access.get(), outer_expr.get());
  array_access->m_index = core::move(inner_expr);
  array_access->m_array = core::move(outer_expr);

  return array_access;
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
  if (t->m_classification == ast_type_classification::actor_type) {
    return resolve_actor_allocation_expression(_alloc);
  }
  if (t->m_classification != ast_type_classification::struct_type) {
    _alloc->log_line(m_log, logging::log_level ::error);
    m_log->log_error("You must allocate a struct with a known struct type");
    return nullptr;
  }
  const ast_type* alloc_type = t->m_overloaded_construction_child;
  WN_DEBUG_ASSERT(
      alloc_type, "Have a struct type without a proper constructor");

  memory::unique_ptr<ast_declaration> temp_decl;
  ast_declaration* decl = nullptr;
  bool cleanup_after_statement = false;
  bool insert_function = false;
  bool is_inserted_into_array = false;

  memory::unique_ptr<ast_expression> declared_target;
  // We have to get this out of the high-level variable BEFORE we
  // assign.
  memory::unique_ptr<ast_expression> assigned_declaration =
      core::move(m_assigned_declaration);

  if (assigned_declaration) {
    is_inserted_into_array = assigned_declaration->m_node_type ==
                             ast_node_type::ast_array_access_expression;
  }

  // Instead of creating a new temporary struct, we are trying to fill an
  // existing declaration;
  if (assigned_declaration && assigned_declaration->m_type->m_classification ==
                                  ast_type_classification::struct_type) {
    declared_target = core::move(assigned_declaration);
    insert_function = true;
  } else {
    cleanup_after_statement = true;
    temp_decl =
        make_temp_declaration(_alloc, get_next_temporary_name(), alloc_type);
    if (assigned_declaration) {
      cleanup_after_statement = false;
      temp_decl->m_is_scope_bound = true;
    }

    decl = temp_decl.get();

    memory::unique_ptr<ast_id> id =
        memory::make_unique<ast_id>(m_allocator, _alloc);
    id->m_declaration = decl;
    id->m_type = decl->m_type;

    declared_target = core::move(id);
  }

  memory::unique_ptr<ast_expression> init;
  if (alloc_type->m_constructor) {
    use_function(alloc_type->m_constructor);
    memory::unique_ptr<ast_function_call_expression> function_call =
        memory::make_unique<ast_function_call_expression>(m_allocator, _alloc);
    function_call->m_function = alloc_type->m_constructor;

    memory::unique_ptr<ast_cast_expression> cast =
        memory::make_unique<ast_cast_expression>(m_allocator, _alloc);
    cast->m_base_expression =
        clone_ast_node(m_allocator, declared_target.get());
    cast->m_type =
        m_type_manager->get_reference_of(cast->m_base_expression->m_type,
            ast_type_classification::reference, &m_used_types);
    if (!cast->m_base_expression->m_type->m_struct_is_class) {
      // If this is a struct, we may be using this with a differnet constructor.
      function_call->initialized_parameters(m_allocator)
          .push_back(make_cast(core::move(cast),
              m_type_manager->get_reference_of(alloc_type,
                  ast_type_classification::reference, &m_used_types)));
    } else {
      function_call->initialized_parameters(m_allocator)
          .push_back(core::move(cast));
    }

    if (_alloc->get_args()) {
      const ast_function* constructor = alloc_type->m_constructor;
      if (alloc_type->m_constructor->m_parameters.size() !=
          _alloc->get_args()->get_expressions().size() + 1) {
        _alloc->log_line(m_log, logging::log_level::error);
        m_log->log_error("Constructor expects different arguments");
        return nullptr;
      }
      size_t i = 0;
      for (auto& expr : _alloc->get_args()->get_expressions()) {
        // Handily this lets us skip the first element.
        i++;

        auto ex = resolve_expression(expr->m_expr.get());
        if (!ex) {
          return nullptr;
        }
        transfer_temporaries(function_call.get(), ex.get());

        if (ex->m_type != constructor->m_parameters[i].m_type) {
          if (!ex->m_type->can_implicitly_cast_to(
                  constructor->m_parameters[i].m_type)) {
            _alloc->log_line(m_log, logging::log_level::error);
            m_log->log_error("Cannot convert arugment in constructor");
            return nullptr;
          }
          ex = make_cast(core::move(ex), constructor->m_parameters[i].m_type);
        }

        function_call->initialized_parameters(m_allocator)
            .push_back(core::move(ex));
      }
    } else {
      if (alloc_type->m_constructor->m_parameters.size() > 1) {
        _alloc->log_line(m_log, logging::log_level::error);
        m_log->log_error("Expect ",
            alloc_type->m_constructor->m_parameters.size() - 1,
            " parameters to the constructor");
        return nullptr;
      }
    }

    function_call->m_type = alloc_type->m_constructor->m_return_type;

    m_type_manager->get_reference_of(
        t, ast_type_classification::reference, &m_used_types);

    if (temp_decl) {
      m_current_statements->push_back(core::move(temp_decl));
    }
    init = make_cast(core::move(function_call),
        m_type_manager->get_reference_of(declared_target->m_type,
            ast_type_classification::reference, &m_used_types));
  } else {
    memory::unique_ptr<ast_id> id =
        memory::make_unique<ast_id>(m_allocator, _alloc);
    id->m_declaration = decl;
    id->m_type = decl->m_type;

    init = make_cast(
        core::move(id), m_type_manager->get_reference_of(decl->m_type,
                            ast_type_classification::reference, &m_used_types));
    if (temp_decl) {
      m_current_statements->push_back(core::move(temp_decl));
    }
  }

  // Array destruction is handled at the array level, not the individual element
  // level.
  if (alloc_type->m_destructor && !is_inserted_into_array) {
    memory::unique_ptr<ast_function_call_expression> function_call =
        memory::make_unique<ast_function_call_expression>(m_allocator, _alloc);
    function_call->m_function = alloc_type->m_destructor;

    memory::unique_ptr<ast_cast_expression> cast =
        memory::make_unique<ast_cast_expression>(m_allocator, _alloc);
    cast->m_base_expression =
        clone_ast_node(m_allocator, declared_target.get());
    cast->m_type =
        m_type_manager->get_reference_of(cast->m_base_expression->m_type,
            ast_type_classification::reference, &m_used_types);
    function_call->initialized_parameters(m_allocator)
        .push_back(core::move(cast));
    function_call->m_type = m_type_manager->void_t(&m_used_types);
    auto expression_statement =
        memory::make_unique<ast_evaluate_expression>(m_allocator, _alloc);
    expression_statement->m_expr = core::move(function_call);

    if (cleanup_after_statement) {
      m_temporary_cleanup.push_back(core::move(expression_statement));
    } else {
      m_nested_scopes.back()
          ->initialized_cleanup(m_allocator)
          .push_front(core::move(expression_statement));
    }
  }

  // Instead of returning init, we really want to actually just assign
  // here
  if (assigned_declaration) {
    m_declaration_succeeded = true;
    auto assign = memory::make_unique<ast_assignment>(m_allocator, _alloc);
    assign->m_lhs = core::move(assigned_declaration);
    assign->m_rhs = make_cast(core::move(init), assign->m_lhs->m_type);
    m_current_statements->push_back(core::move(assign));
    return nullptr;
  }
  if (insert_function) {
    m_declaration_succeeded = true;
    auto expression_statement =
        memory::make_unique<ast_evaluate_expression>(m_allocator, _alloc);
    expression_statement->m_expr = core::move(init);
    m_current_statements->push_back(core::move(expression_statement));
    return nullptr;
  }

  return init;
}

memory::unique_ptr<ast_expression>
parse_ast_convertor::convertor_context::resolve_builtin_unary_expression(
    const builtin_unary_expression* _expr) {
  memory::unique_ptr<ast_expression> c =
      resolve_expression(_expr->root_expression());
  if (!c) {
    return nullptr;
  }
  switch (_expr->get_unary_type()) {
    case builtin_unary_type::length: {
      if (c->m_type->m_builtin == builtin_type::c_string_type) {
        auto fncall = memory::make_unique<ast_function_call_expression>(
            m_allocator, _expr);
        fncall->m_function = m_type_manager->strlen(&m_used_builtins);
        fncall->initialized_parameters(m_allocator).push_back(core::move(c));
        fncall->m_type = fncall->m_function->m_return_type;
        return fncall;
      } else {
        if (c->m_type->m_classification !=
                ast_type_classification::static_array &&
            c->m_type->m_classification !=
                ast_type_classification::slice_type &&
            c->m_type->m_classification !=
                ast_type_classification::runtime_array) {
          _expr->log_line(m_log, logging::log_level ::error);
          m_log->log_error("length can not applied to ", c->m_type->m_name);
          return nullptr;
        }
        auto builtin =
            memory::make_unique<ast_builtin_expression>(m_allocator, _expr);
        builtin->m_builtin_type = builtin_expression_type::array_length;
        builtin->initialized_expressions(m_allocator).push_back(core::move(c));
        builtin->m_type = m_type_manager->integral(32, &m_used_types);
        return builtin;
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

  memory::unique_ptr<ast_declaration> temp_decl;
  memory::unique_ptr<ast_expression> declared_target;
  // We have to get this out of the high-level variable BEFORE we
  // assign.
  memory::unique_ptr<ast_expression> assigned_declaration =
      core::move(m_assigned_declaration);
  bool cleanup_after_statement = false;
  bool mark_success = false;
  bool is_inserted_into_array = false;
  bool is_synchronized_init = false;
  if (!assigned_declaration) {
    cleanup_after_statement = true;
    temp_decl = make_temp_declaration(_alloc, get_next_temporary_name(), t);

    memory::unique_ptr<ast_id> id =
        memory::make_unique<ast_id>(m_allocator, _alloc);
    id->m_declaration = temp_decl.get();
    id->m_type = temp_decl->m_type;

    declared_target = core::move(id);
  } else {
    is_inserted_into_array = assigned_declaration->m_node_type ==
                             ast_node_type::ast_array_access_expression;
    if (assigned_declaration->m_node_type == ast_node_type::ast_id) {
      is_synchronized_init = cast_to<ast_id>(assigned_declaration.get())
                                 ->m_declaration->m_is_synchronized;
    }

    declared_target = core::move(assigned_declaration);
    mark_success = true;
  }

  WN_DEBUG_ASSERT(alloc_type && alloc_type->m_constructor,
      "Have a struct type without a proper constructor");

  auto num_bytes =
      memory::make_unique<ast_builtin_expression>(m_allocator, _alloc);
  num_bytes->m_type = m_type_manager->size_t_t(&m_used_types);
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
    const_null->m_type = m_type_manager->nullptr_t(&m_used_types);
    dest = core::move(const_null);
  }

  auto dest_as_void =
      memory::make_unique<ast_cast_expression>(m_allocator, _alloc);
  dest_as_void->m_type = m_type_manager->destructor_fn_ptr(&m_used_types);
  dest_as_void->m_base_expression = core::move(dest);
  memory::unique_ptr<ast_function_call_expression> allocate =
      memory::make_unique<ast_function_call_expression>(m_allocator, _alloc);
  allocate->m_function = m_type_manager->allocate_shared(&m_used_builtins);
  allocate->m_type = allocate->m_function->m_return_type;
  allocate->initialized_parameters(m_allocator)
      .push_back(core::move(num_bytes));
  allocate->initialized_parameters(m_allocator)
      .push_back(core::move(dest_as_void));

  memory::unique_ptr<ast_function_call_expression> function_call =
      memory::make_unique<ast_function_call_expression>(m_allocator, _alloc);
  function_call->m_function = alloc_type->m_constructor;
  use_function(alloc_type->m_constructor);

  memory::unique_ptr<ast_cast_expression> cast =
      memory::make_unique<ast_cast_expression>(m_allocator, _alloc);
  cast->m_base_expression = core::move(allocate);
  cast->m_type = alloc_type->m_constructor->m_parameters[0].m_type;
  function_call->initialized_parameters(m_allocator)
      .push_back(core::move(cast));

  if (_alloc->get_args()) {
    const ast_function* constructor = alloc_type->m_constructor;
    if (alloc_type->m_constructor->m_parameters.size() !=
        _alloc->get_args()->get_expressions().size() + 1) {
      _alloc->log_line(m_log, logging::log_level::error);
      m_log->log_error("Constructor expects different arguments");
      return nullptr;
    }
    size_t i = 0;
    for (auto& expr : _alloc->get_args()->get_expressions()) {
      // Handily this lets us skip the first element.
      i++;

      auto ex = resolve_expression(expr->m_expr.get());
      if (!ex) {
        return nullptr;
      }
      transfer_temporaries(function_call.get(), ex.get());

      if (ex->m_type != constructor->m_parameters[i].m_type) {
        if (!ex->m_type->can_implicitly_cast_to(
                constructor->m_parameters[i].m_type)) {
          _alloc->log_line(m_log, logging::log_level::error);
          m_log->log_error("Cannot convert arugment in constructor");
          return nullptr;
        }
        ex = make_cast(core::move(ex), constructor->m_parameters[i].m_type);
      }

      function_call->initialized_parameters(m_allocator)
          .push_back(core::move(ex));
    }
  }

  function_call->m_type = m_type_manager->get_reference_of(
      st, ast_type_classification::reference, &m_used_types);

  if (temp_decl) {
    m_current_statements->push_back(core::move(temp_decl));
  }

  const ast_type* to_t = declared_target->m_type;
  auto assign = memory::make_unique<ast_assignment>(m_allocator, _alloc);
  assign->m_lhs = clone_ast_node(m_allocator, declared_target.get());
  assign->m_rhs = make_cast(core::move(function_call), to_t);
  m_current_statements->push_back(core::move(assign));

  // Handle the cleanup of this ref
  if (!is_inserted_into_array && !is_synchronized_init) {
    memory::unique_ptr<ast_function_call_expression> destructor_call =
        memory::make_unique<ast_function_call_expression>(m_allocator, _alloc);
    destructor_call->m_function =
        m_type_manager->release_shared(&m_used_builtins);
    memory::unique_ptr<ast_cast_expression> cat_to_void_ptr =
        memory::make_unique<ast_cast_expression>(m_allocator, _alloc);
    cat_to_void_ptr->m_base_expression =
        clone_ast_node(m_allocator, declared_target.get());
    cat_to_void_ptr->m_type = m_type_manager->void_ptr_t(&m_used_types);

    destructor_call->initialized_parameters(m_allocator)
        .push_back(core::move(cat_to_void_ptr));
    destructor_call->m_type = m_type_manager->void_t(&m_used_types);

    auto expression_statement =
        memory::make_unique<ast_evaluate_expression>(m_allocator, _alloc);
    expression_statement->m_expr = core::move(destructor_call);

    if (cleanup_after_statement) {
      m_temporary_cleanup.push_back(core::move(expression_statement));
    } else {
      m_nested_scopes.back()
          ->initialized_cleanup(m_allocator)
          .push_front(core::move(expression_statement));
    }
  }
  if (mark_success) {
    m_declaration_succeeded = true;
  }

  return declared_target;
}

memory::unique_ptr<ast_expression>
parse_ast_convertor::convertor_context::resolve_actor_allocation_expression(
    const struct_allocation_expression* _alloc) {
  const ast_type* t = resolve_type(_alloc->get_type());
  if (!t) {
    return nullptr;
  }
  WN_DEBUG_ASSERT(t->m_classification == ast_type_classification::actor_type,
      "Unhandled shared allocation with non shared type");
  const ast_type* st = t;

  ast_type* alloc_type = st->m_overloaded_construction_child;

  memory::unique_ptr<ast_declaration> temp_decl;
  memory::unique_ptr<ast_expression> declared_target;
  // We have to get this out of the high-level variable BEFORE we
  // assign.
  memory::unique_ptr<ast_expression> assigned_declaration =
      core::move(m_assigned_declaration);
  bool cleanup_after_statement = false;
  bool mark_success = false;
  bool is_inserted_into_array = false;
  bool is_synchronized_init = false;
  if (!assigned_declaration) {
    cleanup_after_statement = true;
    temp_decl = make_temp_declaration(_alloc, get_next_temporary_name(), t);

    memory::unique_ptr<ast_id> id =
        memory::make_unique<ast_id>(m_allocator, _alloc);
    id->m_declaration = temp_decl.get();
    id->m_type = temp_decl->m_type;

    declared_target = core::move(id);
  } else {
    is_inserted_into_array = assigned_declaration->m_node_type ==
                             ast_node_type::ast_array_access_expression;
    if (assigned_declaration->m_node_type == ast_node_type::ast_id) {
      is_synchronized_init = cast_to<ast_id>(assigned_declaration.get())
                                 ->m_declaration->m_is_synchronized;
    }

    declared_target = core::move(assigned_declaration);
    mark_success = true;
  }

  WN_DEBUG_ASSERT(alloc_type && alloc_type->m_constructor,
      "Have a struct type without a proper constructor");

  auto num_bytes =
      memory::make_unique<ast_builtin_expression>(m_allocator, _alloc);
  num_bytes->m_type = m_type_manager->size_t_t(&m_used_types);
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
    const_null->m_type = m_type_manager->nullptr_t(&m_used_types);
    dest = core::move(const_null);
  }

  memory::unique_ptr<ast_expression> update;
  if (alloc_type->m_actor_update) {
    auto d = memory::make_unique<ast_function_pointer_expression>(
        m_allocator, _alloc);
    d->m_type = alloc_type->m_actor_update->m_function_pointer_type;
    d->m_function = alloc_type->m_actor_update;
    update = core::move(d);
  } else {
    auto const_null = memory::make_unique<ast_constant>(m_allocator, _alloc);
    const_null->m_string_value = containers::string(m_allocator, "");
    const_null->m_type = m_type_manager->nullptr_t(&m_used_types);
    update = core::move(const_null);
  }

  auto dest_as_void =
      memory::make_unique<ast_cast_expression>(m_allocator, _alloc);
  dest_as_void->m_type = m_type_manager->destructor_fn_ptr(&m_used_types);
  dest_as_void->m_base_expression = core::move(dest);

  auto update_as_void =
      memory::make_unique<ast_cast_expression>(m_allocator, _alloc);
  update_as_void->m_type = m_type_manager->destructor_fn_ptr(&m_used_types);
  update_as_void->m_base_expression = core::move(update);

  memory::unique_ptr<ast_function_call_expression> allocate =
      memory::make_unique<ast_function_call_expression>(m_allocator, _alloc);
  allocate->m_function = m_type_manager->allocate_actor(&m_used_builtins);
  allocate->m_type = allocate->m_function->m_return_type;
  allocate->initialized_parameters(m_allocator)
      .push_back(core::move(num_bytes));
  allocate->initialized_parameters(m_allocator)
      .push_back(core::move(dest_as_void));
  allocate->initialized_parameters(m_allocator)
      .push_back(core::move(update_as_void));

  memory::unique_ptr<ast_function_call_expression> function_call =
      memory::make_unique<ast_function_call_expression>(m_allocator, _alloc);
  function_call->m_function = alloc_type->m_constructor;
  use_function(alloc_type->m_constructor);

  memory::unique_ptr<ast_cast_expression> cast =
      memory::make_unique<ast_cast_expression>(m_allocator, _alloc);
  cast->m_base_expression = core::move(allocate);
  cast->m_type = alloc_type->m_constructor->m_parameters[0].m_type;
  function_call->initialized_parameters(m_allocator)
      .push_back(core::move(cast));

  if (_alloc->get_args()) {
    const ast_function* constructor = alloc_type->m_constructor;
    if (alloc_type->m_constructor->m_parameters.size() !=
        _alloc->get_args()->get_expressions().size() + 1) {
      _alloc->log_line(m_log, logging::log_level::error);
      m_log->log_error("Constructor expects different arguments");
      return nullptr;
    }
    size_t i = 0;
    for (auto& expr : _alloc->get_args()->get_expressions()) {
      // Handily this lets us skip the first element.
      i++;

      auto ex = resolve_expression(expr->m_expr.get());
      if (!ex) {
        return nullptr;
      }
      transfer_temporaries(function_call.get(), ex.get());

      if (ex->m_type != constructor->m_parameters[i].m_type) {
        if (!ex->m_type->can_implicitly_cast_to(
                constructor->m_parameters[i].m_type)) {
          _alloc->log_line(m_log, logging::log_level::error);
          m_log->log_error("Cannot convert arugment in constructor");
          return nullptr;
        }
        ex = make_cast(core::move(ex), constructor->m_parameters[i].m_type);
      }

      function_call->initialized_parameters(m_allocator)
          .push_back(core::move(ex));
    }
  }

  function_call->m_type = st;

  if (temp_decl) {
    m_current_statements->push_back(core::move(temp_decl));
  }

  const ast_type* to_t = declared_target->m_type;
  auto assign = memory::make_unique<ast_assignment>(m_allocator, _alloc);
  assign->m_lhs = clone_ast_node(m_allocator, declared_target.get());
  assign->m_rhs = make_cast(core::move(function_call), to_t);
  m_current_statements->push_back(core::move(assign));

  // Handle the cleanup of this ref
  if (!is_inserted_into_array && !is_synchronized_init) {
    memory::unique_ptr<ast_function_call_expression> destructor_call =
        memory::make_unique<ast_function_call_expression>(m_allocator, _alloc);
    destructor_call->m_function =
        m_type_manager->release_actor(&m_used_builtins);
    memory::unique_ptr<ast_cast_expression> cat_to_void_ptr =
        memory::make_unique<ast_cast_expression>(m_allocator, _alloc);
    cat_to_void_ptr->m_base_expression =
        clone_ast_node(m_allocator, declared_target.get());
    cat_to_void_ptr->m_type = m_type_manager->void_ptr_t(&m_used_types);

    destructor_call->initialized_parameters(m_allocator)
        .push_back(core::move(cat_to_void_ptr));
    destructor_call->m_type = m_type_manager->void_t(&m_used_types);

    auto expression_statement =
        memory::make_unique<ast_evaluate_expression>(m_allocator, _alloc);
    expression_statement->m_expr = core::move(destructor_call);

    if (cleanup_after_statement) {
      m_temporary_cleanup.push_back(core::move(expression_statement));
    } else {
      m_nested_scopes.back()
          ->initialized_cleanup(m_allocator)
          .push_front(core::move(expression_statement));
    }
  }
  if (mark_success) {
    m_declaration_succeeded = true;
  }

  return declared_target;
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
      if (struct_type->m_is_synchronized) {
        cp->log_line(m_log, logging::log_level::error);
        m_log->log_error("Cannot access member: '", member->m_member_name,
            "' in Actor as it is not synchronized");
        return nullptr;
      }

      break;
    }
    child_pos++;
  }
  // If we found a member with this name use it.
  if (child_pos != struct_type->m_structure_members.size()) {
    member->m_member_offset = child_pos;
    member->m_type = struct_type->m_structure_members[child_pos]->m_type;
    return member;
  }

  child_pos = 0;
  for (auto& sp : struct_type->m_synchronized_declarations) {
    if (sp->m_name == member->m_member_name) {
      break;
    }
    child_pos++;
  }
  // If we found a synchronized member with this name use it.
  if (child_pos != struct_type->m_synchronized_declarations.size()) {
    member->m_member_offset = child_pos;
    member->m_type =
        struct_type->m_synchronized_declarations[child_pos]->m_type;
    member->m_is_synchronized = true;
    member->m_is_synchronized_function = true;
    return member;
  }

  // Try to find a member function with this name instead.
  for (auto it : struct_type->m_member_functions) {
    if (it->m_name == member->m_member_name) {
      // At least one member function exists
      member->m_type = m_type_manager->function_t(&m_used_types);
      return member;
    }
  }

  auto extern_it =
      struct_type->m_contained_external_types.find(member->m_member_name);
  if (extern_it != struct_type->m_contained_external_types.end()) {
    member->m_member_offset = extern_it->second.order;
    member->m_type = extern_it->second.type;
    return member;
  }

  for (auto& it : struct_type->m_external_member_functions) {
    if (it->m_name == member->m_member_name) {
      member->m_type = m_type_manager->function_t(&m_used_types);
      return member;
    }
  }

  _expression->log_line(m_log, logging::log_level::error);
  m_log->log_error("Cannot find member");
  return nullptr;
}

}  // namespace scripting
}  // namespace wn
