// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/inc/WNString.h"
#include "WNScripting/inc/WNCGenerator.h"
#include "WNScripting/inc/WNScriptHelpers.h"

namespace wn {
namespace scripting {

void ast_c_translator::walk_type(const type* _type, containers::string* _str) {
  switch (_type->get_classification()) {
    case type_classification::invalid_type:
      WN_RELEASE_ASSERT_DESC(wn_false, "Cannot classify invalid types");
      break;
    case type_classification::void_type:
      *_str = std::move(containers::string(m_allocator) + "void");
      return;
    case type_classification::int_type:
      *_str = std::move(containers::string(m_allocator) + "wn_int32");
      return;
    case type_classification::float_type:
      *_str = std::move(containers::string(m_allocator) + "wn_float32");
      return;
    case type_classification::char_type:
      *_str = std::move(containers::string(m_allocator) + "wn_char");
      return;
    case type_classification::string_type:
      WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented string types");
      break;
    case type_classification::bool_type:
      *_str = std::move(containers::string(m_allocator) + "wn_bool");
      break;
    case type_classification::custom_type:
      *_str = _type->custom_type_name().to_string(m_allocator);
      break;
    case type_classification::max:
      WN_RELEASE_ASSERT_DESC(wn_false, "Type out of range");
      break;
  }
}

void ast_c_translator::walk_expression(const constant_expression* _const,
                                       containers::string* _str) {
  // TODO(awoloszyn): Validate this somewhere.
  switch (_const->get_type()->get_classification()) {
    case type_classification::int_type:
      *_str = containers::string(m_allocator) + _const->get_type_text();
      break;
    case type_classification::bool_type:
      *_str = containers::string(m_allocator) + _const->get_type_text();
      break;
    default:
      WN_RELEASE_ASSERT_DESC(wn_false,
                             "Non-integer constants not supported yet.");
  }
}

void ast_c_translator::walk_expression(const binary_expression* _binary,
                                       containers::string* _str) {
  const wn_char* m_operators[] = {
     " + ", " - ", " * ", " / ", " % ", " == ", " != ", " <= ", " >= ", " < ", " > "
  };
  static_assert(sizeof(m_operators) / sizeof(m_operators[0]) ==
                    static_cast<wn_size_t>(arithmetic_type::max),
                "New oeprator type detected");

  // TODO(awoloszyn): Validate this somewhere.
  switch (_binary->get_type()->get_classification()) {
    case type_classification::int_type:
    case type_classification::bool_type:
      *_str =
          "(" + m_generator->get_data(_binary->get_lhs()) +
          m_operators[static_cast<wn_size_t>(_binary->get_arithmetic_type())] +
          m_generator->get_data(_binary->get_rhs()) + ")";
      break;
    default:
      WN_RELEASE_ASSERT_DESC(wn_false,
                             "Non-integer binary expressions not supported yet.");
  }
}


void ast_c_translator::walk_expression(const id_expression* _id,
                                       containers::string* _str) {
  *_str = _id->get_name().to_string(m_allocator);
}

void ast_c_translator::walk_parameter(const parameter* _param,
                                      containers::string* _str) {
  *_str = containers::string(m_allocator) +
          m_generator->get_data(_param->get_type()) + " " +
          _param->get_name().to_string(m_allocator);
}

void ast_c_translator::walk_instruction(const return_instruction* _ret,
                                        containers::string* _str) {
  *_str = containers::string(m_allocator) + "return";
  if (_ret->get_expression()) {
    *_str += " " + m_generator->get_data(_ret->get_expression());
  }
  *_str += ";";
}

void ast_c_translator::walk_instruction(const declaration* _decl,
                                        containers::string* _str) {
  *_str = containers::string(m_allocator);
  *_str += m_generator->get_data(_decl->get_type()) + " " +
          _decl->get_name().to_string(m_allocator) + " = ";
  *_str += m_generator->get_data(_decl->get_expression());
  *_str += ";";
}

void ast_c_translator::walk_instruction(
    const assignment_instruction* _inst, containers::string* _str) {
  *_str = containers::string(m_allocator);
  WN_RELEASE_ASSERT_DESC(_inst->get_assignment_type() == assign_type::equal,
    "Not Implemented: Non equality assignment");
  *_str += m_generator->get_data(_inst->get_lvalue()->get_expression()) + " = " +
    m_generator->get_data(_inst->get_expression()) + ";";
}

void ast_c_translator::walk_instruction(const else_if_instruction* _i,
                                        containers::string* _str) {
  *_str = containers::string(m_allocator) + " else if (";
  *_str += m_generator->get_data(_i->get_condition());
  *_str += ") ";
  *_str += m_generator->get_data(_i->get_body());
}

void ast_c_translator::walk_instruction(const if_instruction* _i,
                                        containers::string* _str) {
  *_str = containers::string(m_allocator) + "if (";
  *_str += m_generator->get_data(_i->get_condition());
  *_str += ") ";
  *_str += m_generator->get_data(_i->get_body());
  for(auto& else_inst: _i->get_else_if_instructions()) {
    *_str += m_generator->get_data(else_inst.get());
  }
  const instruction_list* else_clause = _i->get_else();
  if (else_clause) {
    *_str += " else ";
    *_str += m_generator->get_data(else_clause);
  }
}

void ast_c_translator::walk_instruction_list(const instruction_list* l,
  containers::string* _str) {
   *_str = containers::string(m_allocator) + "{\n";
  for (auto& a : l->get_instructions()) {
    *_str += m_generator->get_data(a.get()) + "\n";
  }
  *_str += "}";
}

void ast_c_translator::walk_function(const function* _func,
                                     containers::string* _str) {
  *_str = containers::string(m_allocator) +
          m_generator->get_data(_func->get_signature()->get_type()) + " " +
          _func->get_signature()->get_name().to_string(m_allocator) + "(";
  wn_bool first_param = wn_true;
  if (_func->get_parameters()) {
    for (auto& a : _func->get_parameters()->get_parameters()) {
      if (!first_param) {
        *_str += ",";
      }
      first_param = wn_false;
      *_str += m_generator->get_data(a.get());
    }
  }
  *_str += ") ";
  *_str += m_generator->get_data(_func->get_body());
  *_str += "\n";
}

void ast_c_translator::walk_script_file(const script_file* _file) {
  wn_bool first = wn_true;
  for (auto& func : _file->get_functions()) {
    if (!first) {
      m_output_string += "\n";
    }
    first = wn_false;
    m_output_string += m_generator->get_data(func.get());
  }
}

}  // namespace scripting
}  // namespace wn
