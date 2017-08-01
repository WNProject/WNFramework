// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNScripting/inc/WNCGenerator.h"
#include "WNContainers/inc/WNString.h"
#include "WNMemory/inc/WNStringUtility.h"
#include "WNScripting/inc/WNScriptHelpers.h"

namespace wn {
namespace scripting {
namespace {

void initialize_data(memory::allocator* _allocator,
    core::pair<containers::string, containers::string>* expr_dat) {
  expr_dat->first = containers::string(_allocator);
  expr_dat->second = containers::string(_allocator);
}

}  // anonymous namespace

void ast_c_translator::walk_type(
    const array_type* _array_type, containers::string* _str) {
  *_str = containers::string(m_allocator);
  uint32_t num_bases = 1;
  const array_type* t = _array_type;
  const type* basest_type = t->get_subtype();
  while (t->get_subtype()->get_node_type() == node_type::array_type) {
    basest_type = t->get_subtype();
    t = cast_to<array_type>(t->get_subtype());
    num_bases += 1;
  }

  containers::string& base_type_string = m_generator->get_data(basest_type);

  *_str += "struct { ";
  uint32_t current_index = 0;
  uint32_t total_size = 1;

  char scratch[12] = {0};

  for (uint32_t i = 0; i < num_bases; ++i) {
    memory::writeuint32(scratch, current_index++, sizeof(scratch) - 1);
    *_str += "uint32_t size";
    *_str += scratch;
    *_str += "; ";
  }
  memory::memset(scratch, 0, sizeof(scratch));
  memory::writeuint32(scratch, total_size, sizeof(scratch) - 1);
  *_str += base_type_string + " _val[]; }";

  switch (_array_type->get_reference_type()) {
    case reference_type::self:
    case reference_type::unique:
    case reference_type::shared:
    case reference_type::construction:
      _str->append("*");
      break;
    case reference_type::raw:
    default:
      break;
  }
}

void ast_c_translator::walk_type(
    const concretized_array_type* _array_type, containers::string* _str) {
  WN_RELEASE_ASSERT_DESC(_array_type->get_index() != 0, "Invalid type id");

  containers::string& sub_type =
      m_generator->get_data(_array_type->get_subtype());
  *_str = containers::string(m_allocator);
  *_str += "struct { ";
  uint32_t current_index = 0;
  uint32_t total_size = 1;

  char scratch[12] = {0};

  for (uint32_t size : _array_type->get_sizes()) {
    total_size *= size;
    memory::writeuint32(scratch, current_index++, sizeof(scratch) - 1);
    *_str += "uint32_t size";
    *_str += scratch;
    *_str += "; ";
  }
  memory::memset(scratch, 0, sizeof(scratch));
  memory::writeuint32(scratch, total_size, sizeof(scratch) - 1);
  *_str += sub_type + " _val[";
  if (_array_type->get_reference_type() == reference_type::raw ||
      _array_type->get_reference_type() == reference_type::self ||
      _array_type->get_reference_type() == reference_type::construction) {
    *_str += scratch;
  }
  *_str += "];";

  if (_array_type->get_reference_type() == reference_type::raw ||
      _array_type->get_reference_type() == reference_type::self ||
      _array_type->get_reference_type() == reference_type::construction) {
    if (_array_type->get_subtype()->get_reference_type() ==
        reference_type::unique) {
      *_str += " ";
      *_str += sub_type.substr(0, sub_type.size() - 1);
      *_str += " _c_val[";
      *_str += scratch;
      *_str += "];";
    }
  }

  *_str += "}";
  switch (_array_type->get_reference_type()) {
    case reference_type::self:
    case reference_type::unique:
    case reference_type::shared:
    case reference_type::construction:
      _str->append("*");
      break;
    case reference_type::raw:
    default:
      break;
  }
}

void ast_c_translator::walk_type(const type* _type, containers::string* _str) {
  WN_RELEASE_ASSERT_DESC(_type->get_index() != 0, "Invalid type id");

  switch (_type->get_index()) {
    case static_cast<uint32_t>(type_classification::invalid_type):
      WN_RELEASE_ASSERT_DESC(false, "Cannot classify invalid types");
      break;
    case static_cast<uint32_t>(type_classification::void_type):
      *_str = core::move(containers::string(m_allocator) + "void");
      return;
    case static_cast<uint32_t>(type_classification::int_type):
      *_str = core::move(containers::string(m_allocator) + "int32_t");
      return;
    case static_cast<uint32_t>(type_classification::float_type):
      *_str = core::move(containers::string(m_allocator) + "float");
      return;
    case static_cast<uint32_t>(type_classification::char_type):
      *_str = core::move(containers::string(m_allocator) + "char");
      return;
    case static_cast<uint32_t>(type_classification::string_type):
      WN_RELEASE_ASSERT_DESC(false, "Unimplemented string types");
      break;
    case static_cast<uint32_t>(type_classification::bool_type):
      *_str = core::move(containers::string(m_allocator) + "bool");
      break;
    case static_cast<uint32_t>(type_classification::size_type):
      *_str = core::move(containers::string(m_allocator) + "size_t");
      break;
    case static_cast<uint32_t>(type_classification::void_ptr_type):
      *_str = core::move(containers::string(m_allocator) + "void*");
      break;
    case static_cast<uint32_t>(type_classification::function_ptr_type):
      *_str = core::move(containers::string(m_allocator) + "void(*)()");
      break;
    default: {
      const containers::string_view view =
          m_validator->get_type_name(_type->get_index());
      *_str = containers::string(m_allocator, view.data(), view.size());
      switch (_type->get_reference_type()) {
        case reference_type::self:
        case reference_type::unique:
        case reference_type::shared:
        case reference_type::construction:
          _str->append("*");
          break;
        default:
          break;
      }
    } break;
  }
}

void ast_c_translator::walk_expression(const constant_expression* _const,
    core::pair<containers::string, containers::string>* _str) {
  initialize_data(m_allocator, _str);

  switch (_const->get_type()->get_index()) {
    case static_cast<uint32_t>(type_classification::int_type):
      _str->second.append(_const->get_type_text());
      break;
    case static_cast<uint32_t>(type_classification::bool_type):
      _str->second.append(_const->get_type_text());
      break;
    case static_cast<uint32_t>(type_classification::float_type):
      _str->second.append(_const->get_type_text());
      _str->second.append("f");
      break;
    case static_cast<uint32_t>(type_classification::void_ptr_type):
      _str->second.append("NULL");
      break;
    default:
      WN_RELEASE_ASSERT_DESC(false, "Non-integer constants not supported yet.");
  }
}

void ast_c_translator::walk_expression(const short_circuit_expression* _ss,
    core::pair<containers::string, containers::string>* _str) {
  initialize_data(m_allocator, _str);
  const char* m_operators[] = {" && ", " || "};
  static_assert(sizeof(m_operators) / sizeof(m_operators[0]) ==
                    static_cast<size_t>(short_circuit_type::max),
      "New oeprator type detected");
  const auto& lhs = m_generator->get_data(_ss->get_lhs());
  const auto& rhs = m_generator->get_data(_ss->get_rhs());
  _str->first.append(lhs.first);
  _str->first.append(rhs.first);
  switch (_ss->get_type()->get_index()) {
    case static_cast<uint32_t>(type_classification::int_type):
    case static_cast<uint32_t>(type_classification::bool_type):
      _str->second.append("(");
      _str->second.append(lhs.second);
      _str->second.append(m_operators[static_cast<size_t>(_ss->get_ss_type())]);
      _str->second.append(rhs.second);
      _str->second.append(")");
      break;
    default:
      WN_RELEASE_ASSERT_DESC(
          false, "Non-integer short_circuit expressions not supported yet.");
  }
}

void ast_c_translator::walk_expression(const binary_expression* _binary,
    core::pair<containers::string, containers::string>* _str) {
  initialize_data(m_allocator, _str);
  const char* m_operators[] = {" + ", " - ", " * ", " / ", " % ", " == ",
      " != ", " <= ", " >= ", " < ", " > "};
  static_assert(sizeof(m_operators) / sizeof(m_operators[0]) ==
                    static_cast<size_t>(arithmetic_type::max),
      "New oeprator type detected");
  const auto& lhs = m_generator->get_data(_binary->get_lhs());
  const auto& rhs = m_generator->get_data(_binary->get_rhs());
  _str->first.append(lhs.first);
  _str->first.append(rhs.first);

  switch (_binary->get_type()->get_index()) {
    case static_cast<uint32_t>(type_classification::int_type):
    case static_cast<uint32_t>(type_classification::bool_type):
      _str->second.append("(");
      _str->second.append(lhs.second);
      _str->second.append(
          m_operators[static_cast<size_t>(_binary->get_arithmetic_type())]);
      _str->second.append(rhs.second);
      _str->second.append(")");
      break;
    default:
      WN_RELEASE_ASSERT_DESC(
          false, "Non-integer binary expressions not supported yet.");
  }
}

void ast_c_translator::walk_expression(const id_expression* _id,
    core::pair<containers::string, containers::string>* _str) {
  initialize_data(m_allocator, _str);
  _str->second = _id->get_name().to_string(m_allocator);
}

void ast_c_translator::walk_expression(const function_pointer_expression* _ptr,
    core::pair<containers::string, containers::string>* _str) {
  initialize_data(m_allocator, _str);
  _str->second = containers::string(m_allocator, "&")
                     .append(_ptr->get_source()->get_mangled_name());
}

void ast_c_translator::walk_expression(const array_access_expression* _access,
    core::pair<containers::string, containers::string>* _str) {
  initialize_data(m_allocator, _str);
  const auto& root_expr = m_generator->get_data(_access->get_base_expression());
  const auto& index_expr = m_generator->get_data(_access->get_access());

  _str->first.append(root_expr.first);
  _str->first.append(index_expr.first);

  _str->second.append(root_expr.second);
  switch (_access->get_base_expression()->get_type()->get_reference_type()) {
    case reference_type::unique:
    case reference_type::self:
    case reference_type::construction:
      if (_access->is_construction()) {
        containers::string s = _str->second;
        _str->second = containers::string(m_allocator, "&");
        _str->second.append(s);
        _str->second.append("->_c");
      } else {
        _str->second.append("->");
      }
      break;
    case reference_type::raw:
      _str->second.append(".");
      break;
  }
  _str->second.append("_val[");
  _str->second.append(index_expr.second);
  _str->second.append("]");
}

void ast_c_translator::walk_expression(const sizeof_expression* _sizeof,
    core::pair<containers::string, containers::string>* _str) {
  initialize_data(m_allocator, _str);
  const auto& root_expr = m_generator->get_data(_sizeof->get_sized_type());

  _str->second.append("sizeof(");
  _str->second.append(root_expr);
  _str->second.append(")");
}

void ast_c_translator::walk_expression(const array_allocation_expression*,
    core::pair<containers::string, containers::string>* _str) {
  initialize_data(m_allocator, _str);

  _str->second.append("{}");
}

void ast_c_translator::walk_expression(const member_access_expression* _access,
    core::pair<containers::string, containers::string>* _str) {
  initialize_data(m_allocator, _str);

  const auto& root_expr = m_generator->get_data(_access->get_base_expression());

  _str->first.append(root_expr.first);
  _str->second.append(root_expr.second);

  _str->second.append("->");
  _str->second.append(_access->get_name().data(), _access->get_name().length());
}

void ast_c_translator::walk_expression(const struct_allocation_expression*,
    core::pair<containers::string, containers::string>* _str) {
  initialize_data(m_allocator, _str);
  // There is actually NOTHING to do here.
}

void ast_c_translator::walk_expression(const cast_expression* _cast,
    core::pair<containers::string, containers::string>* _str) {
  initialize_data(m_allocator, _str);
  const auto& dat = m_generator->get_data(_cast->get_expression());
  _str->first.append(dat.first);

  bool needs_ref = false;

  if (_cast->get_type()->get_reference_type() == reference_type::unique &&
      _cast->get_expression()->get_type()->get_reference_type() ==
          reference_type::raw) {
    needs_ref = true;
  }
  if (_cast->get_type()->get_reference_type() == reference_type::self &&
      _cast->get_expression()->get_type()->get_reference_type() ==
          reference_type::raw) {
    needs_ref = true;
  }

  if (_cast->get_type()->get_reference_type() == reference_type::construction &&
      _cast->get_expression()->get_type()->get_reference_type() ==
          reference_type::raw) {
    needs_ref = true;
  }

  if (_cast->get_type()->get_type_value() -
          static_cast<uint32_t>(_cast->get_type()->get_reference_type()) ==
      _cast->get_expression()->get_type()->get_type_value() -
          static_cast<uint32_t>(
              _cast->get_expression()->get_type()->get_reference_type())) {
    if (needs_ref) {
      _str->second.append("&");
    }
    // If the types are identical, EXCEPT for the reference type, then
    // we don't have to do anything, it's all the same in C, for now.
    _str->second.append(dat.second);
    return;
  }

  if (_cast->get_expression()->get_type()->get_type_value() ==
          static_cast<uint32_t>(type_classification::void_ptr_type) &&
      _cast->get_type()->get_reference_type() == reference_type::self) {
    const auto& type_dat = m_generator->get_data(_cast->get_type());
    _str->second.append("(").append(type_dat).append(")").append(dat.second);
    return;
  }

  if (_cast->get_expression()->get_type()->get_type_value() ==
          static_cast<uint32_t>(type_classification::void_ptr_type) &&
      _cast->get_type()->get_reference_type() == reference_type::construction) {
    const auto& type_dat = m_generator->get_data(_cast->get_type());
    _str->second.append("(").append(type_dat).append(")").append(dat.second);
    return;
  }

  if (_cast->get_expression()->get_type()->get_type_value() ==
          static_cast<uint32_t>(type_classification::void_ptr_type) &&
      _cast->get_type()->get_reference_type() == reference_type::shared) {
    const auto& type_dat = m_generator->get_data(_cast->get_type());
    _str->second.append("(").append(type_dat).append(")").append(dat.second);
    return;
  }

  if (_cast->get_expression()->get_type()->get_reference_type() ==
          reference_type::shared &&
      _cast->get_type()->get_type_value() ==
          static_cast<uint32_t>(type_classification::void_ptr_type)) {
    _str->second.append("(void*)").append(dat.second);
    return;
  }

  if (_cast->get_expression()->get_type()->get_subtype() != nullptr &&
      *_cast->get_type()->get_subtype() ==
          *_cast->get_expression()->get_type()->get_subtype()) {
    // This is a cast operation between 2 array types with the same subtype
    if (_cast->get_expression()->get_type()->get_node_type() ==
            node_type::concretized_array_type &&
        _cast->get_type()->get_node_type() ==
            node_type::concretized_array_type) {
      _str->second.append(needs_ref ? "&" : "").append(dat.second);
    } else {
      const auto& type_dat = m_generator->get_data(_cast->get_type());
      _str->second.append("(")
          .append(type_dat)
          .append(")")
          .append(needs_ref ? "&" : "")
          .append(dat.second);
    }
    return;
  }

  WN_RELEASE_ASSERT_DESC(false, "Not implemented: other types of casts");
}

void ast_c_translator::walk_expression(const function_call_expression* _call,
    core::pair<containers::string, containers::string>* _str) {
  initialize_data(m_allocator, _str);

  _str->second.append(_call->callee()->get_mangled_name());
  _str->second.append("(");
  const char* comma = "";
  for (auto& expr : _call->get_expressions()) {
    const auto& dat = m_generator->get_data(expr->m_expr.get());
    _str->first.append(dat.first);
    _str->second.append(comma);
    comma = ", ";
    _str->second.append(dat.second);
  }
  _str->second.append(")");
}

void ast_c_translator::walk_parameter(
    const parameter* _param, containers::string* _str) {
  *_str = containers::string(m_allocator) +
          m_generator->get_data(_param->get_type()) + " " +
          _param->get_name().to_string(m_allocator);
}

void ast_c_translator::walk_instruction(const do_instruction* _do,
    core::pair<containers::string, containers::string>* _str) {
  initialize_data(m_allocator, _str);

  const auto& expr = m_generator->get_data(_do->get_condition());
  const auto& body = m_generator->get_data(_do->get_body());

  _str->second.append(expr.first).append("do \n");
  _str->second.append(body.first).append(body.second);
  _str->second.append(" while(");
  _str->second.append(expr.second);
  _str->second.append(");");
}

void ast_c_translator::walk_instruction(const set_array_length* _arr_len,
    core::pair<containers::string, containers::string>* _str) {
  initialize_data(m_allocator, _str);
  const auto& lvalue =
      m_generator->get_data(_arr_len->get_lvalue()->get_expression());
  const auto& expr = m_generator->get_data(_arr_len->get_expression());

  _str->second.append(lvalue.first);
  _str->second.append(expr.first);
  _str->second.append(lvalue.second);
  switch (_arr_len->get_lvalue()
              ->get_expression()
              ->get_type()
              ->get_reference_type()) {
    case reference_type::unique:
    case reference_type::self:
    case reference_type::construction:
      _str->second.append("->");
      break;
    case reference_type::raw:
      _str->second.append(".");
      break;
  }
  _str->second.append("size0");
  _str->second.append(" = ");
  _str->second.append(expr.second);
  _str->second.append(";");
}

void ast_c_translator::walk_instruction(const break_instruction*,
    core::pair<containers::string, containers::string>* _str) {
  initialize_data(m_allocator, _str);
  _str->second.append("break;");
}

void ast_c_translator::walk_instruction(const continue_instruction*,
    core::pair<containers::string, containers::string>* _str) {
  initialize_data(m_allocator, _str);
  _str->second.append("continue;");
}

void ast_c_translator::walk_instruction(const expression_instruction* _expr,
    core::pair<containers::string, containers::string>* _str) {
  initialize_data(m_allocator, _str);
  const auto& expr = m_generator->get_data(_expr->get_expression());
  _str->second.append(expr.first);
  _str->second.append(expr.second);
  _str->second.append(";");
}

void ast_c_translator::walk_instruction(const return_instruction* _ret,
    core::pair<containers::string, containers::string>* _str) {
  initialize_data(m_allocator, _str);
  if (_ret->get_expression()) {
    const auto& expr = m_generator->get_data(_ret->get_expression());
    _str->second.append(expr.first);
    _str->second.append("return ");
    _str->second.append(expr.second);
    _str->second.append(";");
  } else {
    _str->second.append("return;");
  }
}

void ast_c_translator::walk_instruction(const declaration* _decl,
    core::pair<containers::string, containers::string>* _str) {
  initialize_data(m_allocator, _str);
  const auto& expr = m_generator->get_data(_decl->get_expression());

  _str->second.append(expr.first);

  _str->second += m_generator->get_data(_decl->get_type());
  _str->second += " " + _decl->get_name().to_string(m_allocator);
  if (_decl->get_expression()->get_node_type() !=
      node_type::struct_allocation_expression) {
    _str->second += " = ";
  }
  _str->second += expr.second;
  _str->second += ";";
}

void ast_c_translator::walk_instruction(const assignment_instruction* _inst,
    core::pair<containers::string, containers::string>* _str) {
  initialize_data(m_allocator, _str);
  WN_RELEASE_ASSERT_DESC(_inst->get_assignment_type() == assign_type::equal,
      "Not Implemented: Non equality assignment");
  const auto& expr = m_generator->get_data(_inst->get_expression());
  const auto& lvalue =
      m_generator->get_data(_inst->get_lvalue()->get_expression());
  _str->second += expr.first;
  _str->second += lvalue.first;
  _str->second += lvalue.second + " = " + expr.second + ";";
}

void ast_c_translator::walk_instruction(const else_if_instruction* _i,
    core::pair<containers::string, containers::string>* _str) {
  initialize_data(m_allocator, _str);
  const auto& cond = m_generator->get_data(_i->get_condition());
  _str->first += cond.first;
  _str->second += " else if (";
  _str->second += cond.second;
  _str->second += ") ";
  auto& dat = m_generator->get_data(_i->get_body());
  _str->second += dat.first;
  _str->second += dat.second;
}

void ast_c_translator::walk_instruction(const if_instruction* _i,
    core::pair<containers::string, containers::string>* _str) {
  initialize_data(m_allocator, _str);
  const auto& condition = m_generator->get_data(_i->get_condition());
  auto& dat = m_generator->get_data(_i->get_body());

  _str->first.append(dat.first);
  _str->first.append(condition.first);
  _str->second += "if (";
  _str->second += condition.second;
  _str->second += ") ";
  _str->second += dat.second;
  for (auto& else_inst : _i->get_else_if_instructions()) {
    const auto& else_data = m_generator->get_data(else_inst.get());
    _str->first.append(else_data.first);
    _str->second.append(else_data.second);
  }

  const instruction_list* else_clause = _i->get_else();
  if (else_clause) {
    auto& else_dat = m_generator->get_data(else_clause);
    _str->first += else_dat.first;
    _str->second += " else ";
    _str->second += else_dat.second;
  }
}

void ast_c_translator::walk_instruction_list(const instruction_list* l,
    core::pair<containers::string, containers::string>* _str) {
  initialize_data(m_allocator, _str);
  _str->second += containers::string(m_allocator) + "{\n";
  for (auto& a : l->get_instructions()) {
    const auto& dat = m_generator->get_data(a.get());
    _str->second += dat.first;
    _str->second += dat.second + "\n";
  }
  _str->second += "}";
}

void ast_c_translator::walk_function(
    const function* _func, containers::string* _str) {
  *_str = containers::string(m_allocator) +
          m_generator->get_data(_func->get_signature()->get_type()) + " " +
          _func->get_mangled_name() + "(";
  bool first_param = true;
  if (_func->get_parameters()) {
    for (auto& a : _func->get_parameters()->get_parameters()) {
      if (!first_param) {
        *_str += ", ";
      }
      first_param = false;
      *_str += m_generator->get_data(a.get());
    }
  }
  *_str += ") ";

  const auto& else_data = m_generator->get_data(_func->get_body());
  *_str += else_data.first;
  *_str += else_data.second;
  *_str += "\n";
}

void ast_c_translator::walk_struct_definition(
    const struct_definition* _definition, containers::string* _str) {
  *_str = containers::string(m_allocator) + "typedef struct {\n";
  for (const auto& a : _definition->get_struct_members()) {
    *_str += "  " + m_generator->get_data(a->get_type()) + " ";
    _str->append(a->get_name().data(), a->get_name().size());
    _str->append(";\n");
  }
  *_str += "} ";
  _str->append(_definition->get_name().data(), _definition->get_name().size());
  *_str += ";\n\n";
}

void ast_c_translator::walk_script_file(const script_file* _file) {
  bool first = true;
  for (auto& strt : _file->get_structs()) {
    m_output_string += m_generator->get_data(strt.get());
  }
  for (auto& func : _file->get_functions()) {
    if (!first) {
      m_output_string += "\n";
    }
    first = false;
    m_output_string += m_generator->get_data(func.get());
  }
}

}  // namespace scripting
}  // namespace wn
