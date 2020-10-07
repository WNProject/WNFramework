// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNScripting/inc/c_compiler.h"

#include "WNContainers/inc/WNHashMap.h"
#include "WNContainers/inc/WNHashSet.h"
#include "WNScripting/inc/WNNodeTypes.h"
#include "WNScripting/inc/internal/c_preamble.h"
#include "WNScripting/inc/parse_ast_convertor.h"

namespace wn {
namespace scripting {
c_compiler::c_compiler(memory::allocator* _allocator)
  : m_output(_allocator),
    m_loop_temporaries(_allocator),
    m_types(_allocator),
    m_vtable_functions(_allocator),
    m_allocator(_allocator) {}

bool c_compiler::compile(const ast_script_file* _file) {
  if (!write_preamble()) {
    return false;
  }

  m_output +=
      "////////////////////////////////////////"
      "////////////////////////////////////////\n";

  m_output += "//                           ";
  m_output += "FORWARD DECLARED TYPES";
  m_output += "                           //\n";
  m_output +=
      "////////////////////////////////////////"
      "////////////////////////////////////////\n";

  for (auto& t : _file->m_initialization_order) {
    if (!forward_declare_type(t)) {
      return false;
    }
  }

  m_output += "\n";
  m_output +=
      "////////////////////////////////////////"
      "////////////////////////////////////////\n";

  m_output += "//                               ";
  m_output += "DECLARED TYPES";
  m_output += "                               //\n";
  m_output +=
      "////////////////////////////////////////"
      "////////////////////////////////////////\n";

  for (auto& t : _file->m_initialization_order) {
    if (!declare_type(t)) {
      return false;
    }
  }

  m_output += "\n";
  m_output +=
      "////////////////////////////////////////"
      "////////////////////////////////////////\n";

  m_output += "//                         ";
  m_output += "FORWARD DECLARED FUNCTIONS";
  m_output += "                         //\n";
  m_output +=
      "////////////////////////////////////////"
      "////////////////////////////////////////\n";
  if (_file->m_used_builtins.size() > 0) {
    m_output += "// BUILTIN FUNCTIONS //\n";

    for (auto& function : _file->m_used_builtins) {
      if (!forward_declare_function(function)) {
        return false;
      }
    }
  }

  if (_file->m_used_externals.size() > 0) {
    m_output += "// EXTERNAL FUNCTIONS //\n";
    for (auto& function : _file->m_used_externals) {
      if (!forward_declare_function(function)) {
        return false;
      }
    }
  }

  m_output += "// DEFINED FUNCTIONS //\n";
  for (auto& function : _file->m_functions) {
    if (!forward_declare_function(function.get())) {
      return false;
    }
  }
  m_output += "\n\n";

  m_output +=
      "////////////////////////////////////////"
      "////////////////////////////////////////\n";

  m_output += "//                         ";
  m_output += "         VTABLES          ";
  m_output += "                         //\n";
  m_output +=
      "////////////////////////////////////////"
      "////////////////////////////////////////\n";

  for (auto& type : _file->m_initialization_order) {
    if (type->m_vtable) {
      if (!write_vtable(type)) {
        return false;
      }
    }
  }
  m_output += "\n\n";

  if (_file->m_used_builtins.size() > 0) {
    m_output +=
        "////////////////////////////////////////"
        "////////////////////////////////////////\n";

    m_output += "//                         ";
    m_output += "    BUILTIN FUNCTIONS     ";
    m_output += "                         //\n";
    m_output +=
        "////////////////////////////////////////"
        "////////////////////////////////////////\n";

    for (auto& function : _file->m_used_builtins) {
      if (!write_function(function)) {
        return false;
      }
    }
  }

  m_output +=
      "////////////////////////////////////////"
      "////////////////////////////////////////\n";

  m_output += "//                         ";
  m_output += "        FUNCTIONS         ";
  m_output += "                         //\n";
  m_output +=
      "////////////////////////////////////////"
      "////////////////////////////////////////\n";

  for (auto& function : _file->m_functions) {
    if (function->m_scope) {
      if (!write_function(function.get())) {
        return false;
      }
      m_output += "\n";
    }
  }
  return true;
}

bool c_compiler::forward_declare_function_pointer(const ast_type* _type) {
  align_line();
  m_output += "typedef ";
  if (!write_type(_type->m_contained_types[0])) {
    return false;
  }
  m_output += "(*";
  m_output += _type->m_name;
  m_output += ")(";
  bool first = true;
  for (size_t i = 1; i < _type->m_contained_types.size(); ++i) {
    if (!first) {
      m_output += ", ";
    }
    first = false;
    write_type(_type->m_contained_types[i]);
  }
  m_output += ");\n";
  return true;
}

bool c_compiler::forward_declare_shared_reference(const ast_type* _type) {
  align_line();
  m_output += "typedef ";
  write_type(_type->m_implicitly_contained_type);
  m_output += "* ";
  m_output += _type->m_name;
  m_output += ";\n";
  return true;
}

bool c_compiler::forward_declare_type(const ast_type* _type) {
  if (_type->m_classification == ast_type_classification::primitive ||
      _type->m_classification == ast_type_classification::reference ||
      _type->m_classification == ast_type_classification::static_array) {
    return true;
  }
  if (_type->m_classification == ast_type_classification::function_pointer) {
    return forward_declare_function_pointer(_type);
  }
  if (_type->m_classification == ast_type_classification::shared_reference) {
    return forward_declare_shared_reference(_type);
  }

  align_line();
  m_output += "struct __";
  if (!write_type(_type)) {
    return false;
  }
  m_output += ";\n";

  align_line();
  m_output += "typedef struct __";
  if (!write_type(_type)) {
    return false;
  }
  m_output += " ";
  if (!write_type(_type)) {
    return false;
  }
  m_output += ";\n\n";
  return true;
}

bool c_compiler::declare_struct(const ast_type* _type) {
  align_line();
  m_output += "typedef struct __";
  if (!write_type(_type)) {
    return false;
  }
  m_output += " {\n";
  m_scope_depth += 1;
  for (auto& decl : _type->m_structure_members) {
    if (!write_declaration(decl.get())) {
      return false;
    }
    m_output += "\n";
  }
  m_scope_depth -= 1;
  align_line();
  m_output += "} ";
  if (!write_type(_type)) {
    return false;
  }
  m_output += ";\n\n";
  return true;
}

bool c_compiler::declare_type(const ast_type* _type) {
  if (_type->m_classification == ast_type_classification::primitive ||
      _type->m_classification == ast_type_classification::reference ||
      _type->m_classification == ast_type_classification::function_pointer ||
      _type->m_classification == ast_type_classification::shared_reference) {
    return true;
  }
  switch (_type->m_classification) {
    case ast_type_classification::primitive:
    case ast_type_classification::extern_type:
      return true;
    case ast_type_classification::struct_type:
      return declare_struct(_type);
    case ast_type_classification::runtime_array: {
      align_line();
      containers::string name;
      containers::string size_str;
      decode_type(_type->m_implicitly_contained_type);
      name = containers::string(m_allocator, "_runtime_array_") +
             m_types[_type->m_implicitly_contained_type];
      m_output += "typedef struct __";
      m_output += name;
      m_output += " {\n";
      m_scope_depth += 1;
      align_line();
      m_output += "uint32_t _size;\n";
      align_line();
      write_type(_type->m_implicitly_contained_type);
      m_output += " _val[];\n";
      m_scope_depth -= 1;
      align_line();
      m_output += "} _";
      m_output += name;
      m_output += ";\n";
      align_line();
      m_output += "typedef _" + name + "* " + name + ";\n\n";
      return true;
    } break;
    case ast_type_classification::weak_reference: {
      WN_RELEASE_ASSERT(false, "Unimplemented type");
    } break;
    case ast_type_classification::static_array: {
      align_line();
      containers::string name;
      containers::string size_str;
      decode_type(_type->m_implicitly_contained_type);
      if (_type->m_static_array_size == 0) {
        name = containers::string(m_allocator, "_array0_") +
               m_types[_type->m_implicitly_contained_type];
      } else {
        char buff[11];
        memory::writeuint32(buff, _type->m_static_array_size, 11);

        name = containers::string(m_allocator, "_array") + buff + "_" +
               m_types[_type->m_implicitly_contained_type];
        size_str = containers::string(m_allocator, buff);
      }
      m_output += "typedef struct _";
      m_output += name;
      m_output += " {\n";
      m_scope_depth += 1;
      align_line();
      m_output += "uint32_t _size;\n";
      align_line();
      write_type(_type->m_implicitly_contained_type);
      m_output += " _val[";
      m_output += size_str;
      m_output += "];\n";
      m_scope_depth -= 1;
      align_line();
      m_output += "} ";
      m_output += name;
      m_output += ";\n\n";
      return true;
    } break;
    case ast_type_classification::slice_type: {
      align_line();
      m_output += "typedef struct _";
      m_output += _type->m_name;
      m_output += " {\n";
      m_scope_depth += 1;
      align_line();
      write_type(_type->m_implicitly_contained_type);
      m_output += "* _begin;\n";
      char buff[11];
      for (uint32_t i = 0; i < _type->m_slice_dimensions; ++i) {
        memory::writeuint32(buff, i, 11);
        align_line();
        m_output += "size_t _size";
        m_output += buff;
        m_output += ";\n";
        if (i < _type->m_slice_dimensions - 1) {
          align_line();
          m_output += "ssize_t _stride";
          m_output += buff;
          m_output += ";\n";
        }
      }
      m_scope_depth -= 1;
      align_line();
      m_output += "} ";
      m_output += _type->m_name;
      m_output += ";\n\n";
    }
      return true;
    default:
      WN_RELEASE_ASSERT(false, "Should not get here");
  }
  return false;
}

bool c_compiler::forward_declare_function(const ast_function* _function) {
  if (!write_function_signature(_function)) {
    return false;
  }
  m_output += ";\n";
  return true;
}

bool c_compiler::write_function_signature(const ast_function* _function) {
  if (_function->m_is_builtin) {
    m_output += "inline ";
  }

  if (_function->m_return_type->m_pass_by_reference) {
    m_output += "void";
  } else {
    if (!write_type(_function->m_return_type)) {
      return false;
    }
  }

  m_output += " ";
  m_output += _function->m_mangled_name;
  m_output += "(";
  bool first = true;
  for (auto& param : _function->m_parameters) {
    if (!first) {
      m_output += ", ";
    } else {
      first = false;
    }
    if (!write_type(param.m_type)) {
      return false;
    }
    if (param.m_type->m_pass_by_reference) {
      m_output += "*";
    }
    m_output += " ";
    m_output += param.m_name;
  }
  m_output += ")";
  return true;
}

bool c_compiler::write_vtable(const ast_type* _type) {
  align_line();
  const ast_vtable* vt = _type->m_vtable;
  m_output += "const _vtable_function ";
  m_output += vt->m_name;
  m_output += "{\n";
  m_scope_depth += 1;
  for (auto fn : vt->m_functions) {
    align_line();
    m_output += "(_vtable_function)&";
    m_output += fn->m_mangled_name;
    m_output += "\n";
  }
  m_scope_depth -= 1;
  align_line();
  m_output += "};\n\n";

  containers::dynamic_array<containers::string> vtable_functions(m_allocator);
  uint32_t num = 0;
  for (auto fn : vt->m_functions) {
    char buff[11];
    memory::writeuint32(buff, num, 11);

    align_line();

    containers::string function_name(m_allocator, "_call_");
    function_name += _type->m_name;
    function_name += "_v";
    function_name += buff;
    function_name += "_";

    function_name += fn->m_name;

    m_output += "__wns_inline__\n";
    align_line();
    write_type(fn->m_return_type);
    m_output += " ";
    m_output += function_name;
    vtable_functions.push_back(core::move(function_name));

    m_output += "(";
    bool first = true;
    for (auto& param : fn->m_parameters) {
      if (!first) {
        m_output += ", ";
      }
      first = false;
      write_type(param.m_type);
      m_output += " ";
      m_output += param.m_name;
    }

    m_output += ") {\n";
    m_scope_depth += 1;
    align_line();

    m_output += "return ";
    m_output += "((";
    write_type(fn->m_return_type);
    m_output += "(*)(";
    first = true;
    for (auto& param : fn->m_parameters) {
      if (!first) {
        m_output += ", ";
      }
      first = false;
      write_type(param.m_type);
    }
    m_output += ")";
    m_output += fn->m_parameters[0].m_name;
    m_output += "->__vtable[";

    m_output += buff;
    m_output += "])(";
    first = true;
    for (auto& param : fn->m_parameters) {
      if (!first) {
        m_output += ", ";
      }
      first = false;
      m_output += param.m_name;
    }
    m_output += ");\n";

    m_scope_depth -= 1;
    align_line();
    m_output += "}\n\n";
    num += 1;
  }

  m_vtable_functions[_type->m_vtable] = core::move(vtable_functions);

  return true;
}

bool c_compiler::write_function(const ast_function* _function) {
  if (!_function->m_scope) {
    return true;
  }

  if (!write_function_signature(_function)) {
    return false;
  }
  m_output += " ";
  if (!write_scope_block(_function->m_scope.get())) {
    return false;
  }
  m_output += "\n";
  return true;
}

bool c_compiler::write_type(const ast_type* _type) {
  auto it = m_types.find(_type);
  if (it == m_types.end()) {
    if (!decode_type(_type)) {
      return false;
    }
    it = m_types.find(_type);
  }
  m_output += it->second;
  return true;
}

void c_compiler::align_line() {
  for (size_t i = 0; i < m_scope_depth; ++i) {
    m_output += "  ";
  }
}

containers::string c_compiler::get_temporary() {
  char count[11] = {
      0,
  };
  memory::writeuint32(count, m_temporary_number++, 10);
  return containers::string(m_allocator, "__c_temp") + count;
}

bool c_compiler::write_scope_block(
    const ast_scope_block* _block, bool begin_line) {
  if (begin_line) {
    align_line();
  }
  m_output += "{\n";
  m_scope_depth += 1;
  for (auto& statement : _block->m_statements) {
    if (!write_statement(statement.get())) {
      return false;
    }
  }
  m_scope_depth -= 1;
  align_line();
  m_output += "}";
  return true;
}

bool c_compiler::write_statement(const ast_statement* _statement) {
  bool skip_newline = false;
  switch (_statement->m_node_type) {
    case ast_node_type::ast_declaration: {
      if (cast_to<ast_declaration>(_statement)->m_indirected_on_this) {
        skip_newline = true;
      }
      if (!write_declaration(cast_to<ast_declaration>(_statement))) {
        return false;
      }
    } break;
    case ast_node_type::ast_scope_block: {
      if (!write_scope_block(cast_to<ast_scope_block>(_statement), true)) {
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
    case ast_node_type::ast_if_block: {
      if (!write_if_block(cast_to<ast_if_block>(_statement))) {
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

    case ast_node_type::ast_array_destruction: {
      return write_array_destruction(
          cast_to<ast_array_destruction>(_statement));
    }
    default:
      WN_RELEASE_ASSERT(false, "Unknown statement type");
  }
  if (!skip_newline) {
    m_output += "\n";
  }
  return true;
}

bool c_compiler::write_declaration(const ast_declaration* _declaration) {
  if (!write_temporaries(_declaration->m_initializer.get())) {
    return false;
  }
  if (_declaration->m_indirected_on_this) {
    return true;
  }

  align_line();
  if (!write_type(_declaration->m_type)) {
    return false;
  }

  m_output += " ";
  m_output += _declaration->m_name;

  if (_declaration->m_initializer) {
    m_output += " = ";
    if (!write_expression(_declaration->m_initializer.get())) {
      return false;
    }
  }
  m_output += ";";
  if (!write_cleanups(_declaration->m_initializer.get())) {
    return false;
  }
  return true;
}

bool c_compiler::write_temporaries(const ast_expression* _expr) {
  if (!_expr) {
    return true;
  }
  for (auto& t : _expr->m_temporaries) {
    if (!write_statement(t.get())) {
      return false;
    }
  }
  return true;
}

bool c_compiler::write_cleanups(const ast_expression* _expr) {
  if (!_expr) {
    return true;
  }
  for (auto& e : _expr->m_destroy_expressions) {
    if (!write_statement(e.get())) {
      return false;
    }
  }
  return true;
}

bool c_compiler::write_evaluate_expression(
    const ast_evaluate_expression* _expression) {
  if (!write_temporaries(_expression->m_expr.get())) {
    return false;
  }
  align_line();
  if (!write_expression(_expression->m_expr.get())) {
    return false;
  }
  m_output += ";";
  if (!write_cleanups(_expression->m_expr.get())) {
    return false;
  }
  return true;
}

bool c_compiler::write_if_block(const ast_if_block* _expression) {
  // The is is the "normal" if chain state.
  // Translate this directly into "if/elseif/else"

  align_line();
  m_output += "if (";
  if (!write_expression(_expression->m_condition.get())) {
    return false;
  }
  m_output += ") ";
  if (!write_scope_block(_expression->m_body.get())) {
    return false;
  }

  return true;
}

bool c_compiler::write_id(const ast_id* _expression) {
  if (_expression->m_declaration) {
    if (_expression->m_declaration->m_indirected_on_this) {
      m_output += "_this->";
    }
    m_output += _expression->m_declaration->m_name;
  } else if (_expression->m_function_parameter) {
    if (_expression->m_function_parameter->m_type->m_pass_by_reference) {
      m_output += "(*";
      m_output += _expression->m_function_parameter->m_name;
      m_output += ")";
    } else {
      m_output += _expression->m_function_parameter->m_name;
    }
  } else {
    WN_RELEASE_ASSERT(false, "Unknown id source");
  }
  return true;
}

bool c_compiler::write_constant_expression(const ast_constant* _const) {
  if (_const->m_type->m_builtin == builtin_type::nullptr_type) {
    m_output += "NULL";
    return true;
  }
  if (_const->m_type->m_builtin == builtin_type::c_string_type) {
    m_output += '"';
    // Time to output a bunch of octal.
    // We do this so we can handle arbitrary unicode / escape sequences etc.
    const char* octalChars = "01234567\n";
    for (size_t i = 1; i < _const->m_string_value.size() - 1; ++i) {
      char c = _const->m_string_value[i];
      m_output += '\\';
      m_output += octalChars[(c >> 6) & 0x3];
      m_output += octalChars[(c >> 3) & 0x7];
      m_output += octalChars[c & 0x7];
    }
    m_output += '"';
  } else if (_const->m_type->m_builtin == builtin_type::integral_type &&
             _const->m_type->m_bit_width == 8) {
    m_output += "(uint8_t)(";
    m_output += _const->m_string_value;
    m_output += ")";
  } else {
    m_output += _const->m_string_value;
  }
  if (_const->m_type->m_builtin == builtin_type::floating_point_type &&
      _const->m_type->m_bit_width == 32) {
    m_output += "f";
  }
  return true;
}

bool c_compiler::write_while_loop(const ast_loop* _loop) {
  align_line();
  containers::string m_increment_temp;
  if (_loop->m_increment_statements) {
    m_increment_temp = get_temporary();
    m_loop_temporaries[_loop] =
        containers::string(m_allocator, m_increment_temp);
    m_output += "bool ";
    m_output += m_increment_temp;
    m_output += " = false; // Should we break from this loop\n";
    align_line();
  }

  m_output += "while (";
  if (_loop->m_pre_condition) {
    if (!write_expression(_loop->m_pre_condition.get())) {
      return false;
    }
  } else {
    m_output += "true";
  }
  m_output += ") {\n";

  m_scope_depth += 1;
  align_line();
  auto precondition = _loop->m_condition_scope_block.get();
  if (precondition) {
    write_scope_block(precondition);
  }
  if (_loop->m_increment_statements) {
    m_output += "{\n";
    m_scope_depth += 1;
    align_line();
    m_output += "do ";
  }

  if (!write_scope_block(_loop->m_body.get())) {
    return false;
  }
  if (_loop->m_increment_statements) {
    m_output += " while (false);\n";
    align_line();
    m_output += "if (";
    m_output += m_increment_temp;
    m_output += ") { break; }\n";
    align_line();
    write_scope_block(_loop->m_increment_statements.get());
  }
  m_output += "\n";

  m_scope_depth -= 1;
  align_line();
  m_output += "}";

  if (_loop->m_increment_statements) {
    m_loop_temporaries.erase(_loop);
  }
  return true;
}

bool c_compiler::write_do_loop(const ast_loop* _loop) {
  auto post_condition = _loop->m_post_condition.get();
  align_line();
  m_output += "do ";

  if (post_condition->m_temporaries.empty() &&
      post_condition->m_destroy_expressions.empty()) {
    write_scope_block(_loop->m_body.get());
    m_output += " while (";
    if (!write_expression(post_condition)) {
      return false;
    }
    m_output += ");\n";
  } else {
    m_output += "{\n";
    m_scope_depth += 1;
    for (auto& statement : _loop->m_body->m_statements) {
      if (!write_statement(statement.get())) {
        return false;
      }
    }
    align_line();
    m_output += "{\n";
    m_scope_depth += 1;
    if (!write_temporaries(post_condition)) {
      return false;
    }
    containers::string temp = get_temporary();
    align_line();
    m_output += "bool ";
    m_output += temp;
    m_output += " = ";
    if (!write_expression(post_condition)) {
      return false;
    }
    m_output += ";\n";
    if (!write_cleanups(post_condition)) {
      return false;
    }
    align_line();
    m_output += "if (!";
    m_output += temp;
    m_output += ") { break; }\n";
    m_scope_depth -= 1;
    align_line();
    m_output += "}\n";
    m_scope_depth -= 1;
    align_line();
    m_output += "} while (true);\n";
  }
  return true;
}

bool c_compiler::write_loop(const ast_loop* _loop) {
  WN_DEBUG_ASSERT(!_loop->m_post_condition || !_loop->m_increment_statements,
      "Invalid to have a post condition, and any pre-loop modifiers");

  if (_loop->m_post_condition) {
    return write_do_loop(_loop);
  } else {
    return write_while_loop(_loop);
  }
}

bool c_compiler::write_break(const ast_break* _break) {
  const ast_loop* l = _break->m_break_loop;
  align_line();
  auto it = m_loop_temporaries.find(l);
  if (it == m_loop_temporaries.end()) {
    m_output += "break;";
    return true;
  }
  m_output += it->second;
  m_output += " = true; break;";
  return true;
}

bool c_compiler::write_continue(const ast_continue_instruction* _cont) {
  const ast_loop* l = _cont->m_continue_loop;
  align_line();
  auto it = m_loop_temporaries.find(l);
  if (it == m_loop_temporaries.end()) {
    m_output += "continue;";
    return true;
  }
  m_output += it->second;
  m_output += " = false; break;";
  return true;
}

bool c_compiler::write_assignment(const ast_assignment* _assign) {
  if (!write_temporaries(_assign->m_lhs.get())) {
    return false;
  }
  if (!write_temporaries(_assign->m_rhs.get())) {
    return false;
  }
  align_line();
  if (!write_expression(_assign->m_lhs.get())) {
    return false;
  }

  m_output += " = ";
  if (!write_expression(_assign->m_rhs.get())) {
    return false;
  }
  m_output += ";";
  if (!write_cleanups(_assign->m_lhs.get())) {
    return false;
  }
  if (!write_cleanups(_assign->m_rhs.get())) {
    return false;
  }
  return true;
}

bool c_compiler::write_binary_expression(const ast_binary_expression* _bin) {
  m_output += "(";
  if (!write_expression(_bin->m_lhs.get())) {
    return false;
  }
  switch (_bin->m_binary_type) {
    case ast_binary_type::add:
      m_output += " + ";
      break;
    case ast_binary_type::sub:
      m_output += " - ";
      break;
    case ast_binary_type::div:
      m_output += " / ";
      break;
    case ast_binary_type::mult:
      m_output += " * ";
      break;
    case ast_binary_type::mod:
      m_output += " %% ";
      break;
    case ast_binary_type::lt:
      m_output += " < ";
      break;
    case ast_binary_type::gt:
      m_output += " > ";
      break;
    case ast_binary_type::lte:
      m_output += " <= ";
      break;
    case ast_binary_type::gte:
      m_output += " >= ";
      break;
    case ast_binary_type::eq:
      m_output += " == ";
      break;
    case ast_binary_type::neq:
      m_output += " != ";
      break;
    case ast_binary_type::bitwise_and:
      m_output += " & ";
      break;
    case ast_binary_type::bitwise_or:
      m_output += " | ";
      break;
    case ast_binary_type::bitwise_xor:
      m_output += " ^ ";
      break;
  }
  if (!write_expression(_bin->m_rhs.get())) {
    return false;
  }
  m_output += ")";
  return true;
}

bool c_compiler::write_unary_expression(const ast_unary_expression* _unary) {
  switch (_unary->m_unary_type) {
    case ast_unary_type::invert:
      m_output += "!(";
      if (!write_expression(_unary->m_base_expression.get())) {
        return false;
      }
      m_output += ")";
      break;
    case ast_unary_type::negate:
      m_output += "-(";
      if (!write_expression(_unary->m_base_expression.get())) {
        return false;
      }
      m_output += ")";
      break;
    case ast_unary_type::post_increment:
      m_output += "(";
      if (!write_expression(_unary->m_base_expression.get())) {
        return false;
      }
      m_output += ")++";
      break;
    case ast_unary_type::post_decrement:
      m_output += "(";
      if (!write_expression(_unary->m_base_expression.get())) {
        return false;
      }
      m_output += ")--";
      break;
    case ast_unary_type::pre_increment:
      m_output += "++(";
      if (!write_expression(_unary->m_base_expression.get())) {
        return false;
      }
      m_output += ")";
      break;
    case ast_unary_type::pre_decrement:
      m_output += "--(";
      if (!write_expression(_unary->m_base_expression.get())) {
        return false;
      }
      m_output += ")";
      break;
  }
  return true;
}

bool c_compiler::write_function_pointer_expression(
    const ast_function_pointer_expression* _expr) {
  m_output += "&";
  m_output += _expr->m_function->m_mangled_name;
  return true;
}

bool c_compiler::write_array_destruction(const ast_array_destruction* _call) {
  align_line();
  containers::string op(m_allocator, ".");
  if (_call->m_target->m_type->m_classification ==
          ast_type_classification::runtime_array ||
      _call->m_target->m_type->m_static_array_size == 0) {
    op = containers::string(m_allocator, "->");
  }

  auto temp_name = get_temporary();
  m_output += "for (uint32_t ";
  m_output += temp_name;
  m_output += " = 0; ";
  m_output += temp_name;
  m_output += " < ";
  write_expression(_call->m_target.get());
  m_output += op + "_size; ++";
  m_output += temp_name;
  m_output += ") {\n";

  align_line();
  m_output += "  ";
  m_output += _call->m_destructor->m_mangled_name;
  m_output += "(";
  if (_call->m_shared) {
    m_output += "(void*)(";
  } else {
    m_output += "&(";
  }
  write_expression(_call->m_target.get());
  m_output += op + "_val[";

  m_output += temp_name;
  m_output += "]));\n";
  align_line();
  m_output += "}\n";
  return true;
}

bool c_compiler::write_call_function_pointer(
    const ast_function_call_expression* _call) {
  m_output += "(";
  if (!write_expression(_call->m_function_ptr.get())) {
    return false;
  }
  m_output += ")";
  return true;
}

bool c_compiler::write_call_function(
    const ast_function_call_expression* _call) {
  if (_call->m_function->m_is_virtual || _call->m_function->m_is_override) {
    // If this is a virtual function, then the first parameter is the "this"
    // pointer. Therefore we find the vtable from that, and call the
    // appropriate wrapper function.

    const ast_vtable* vtable =
        _call->m_parameters[0]->m_type->m_implicitly_contained_type->m_vtable;
    m_output += m_vtable_functions[vtable][_call->m_function->m_virtual_index];
  } else {
    m_output += _call->m_function->m_mangled_name;
  }

  return true;
}
bool c_compiler::write_function_call(
    const ast_function_call_expression* _call) {
  WN_RELEASE_ASSERT(_call->m_function || _call->m_function_ptr,
      "Expected function or function pointer");
  if (_call->m_function) {
    write_call_function(_call);
  } else if (_call->m_function_ptr) {
    write_call_function_pointer(_call);
  }
  m_output += "(";
  bool first = true;
  for (auto& param : _call->m_parameters) {
    if (!first) {
      m_output += ", ";
    } else {
      first = false;
    }
    if (param->m_type->m_pass_by_reference) {
      m_output += "&(";
      if (!write_expression(param.get())) {
        return false;
      }
      m_output += ")";
    } else {
      if (!write_expression(param.get())) {
        return false;
      }
    }
  }
  m_output += ")";
  return true;
}

bool c_compiler::write_member_access_expression(
    const ast_member_access_expression* _expression) {
  m_output += "(";
  if (!write_expression(_expression->m_base_expression.get())) {
    return false;
  }

  if (_expression->m_base_expression->m_type->m_classification ==
          ast_type_classification::reference ||
      _expression->m_base_expression->m_type->m_classification ==
          ast_type_classification::shared_reference) {
    m_output += "->";
  } else {
    m_output += ".";
  }

  m_output += _expression->m_member_name;
  m_output += ")";
  return true;
}

bool c_compiler::write_array_access_expression(
    const ast_array_access_expression* _expression) {
  if (_expression->m_array->m_type->m_classification ==
      ast_type_classification::slice_type) {
    if (_expression->m_array->m_type->m_slice_dimensions == 1) {
      if (!write_expression(_expression->m_array.get())) {
        return false;
      }
      m_output += "._begin[";
      if (!write_expression(_expression->m_index.get())) {
        return false;
      }
      m_output += "]";
      return true;
    } else {
      m_output += "((";
      if (!write_type(_expression->m_type)) {
        return false;
      }
      m_output += ")";
      m_output += "{";
      m_output += "(";
      if (!write_type(_expression->m_type->m_implicitly_contained_type)) {
        return false;
      }
      m_output += "*)((uint8_t*)(";
      if (!write_expression(_expression->m_array.get())) {
        return false;
      }
      m_output += "._begin) + (";
      if (!write_expression(_expression->m_array.get())) {
        return false;
      }
      m_output += "._stride0 * ";
      if (!write_expression(_expression->m_index.get())) {
        return false;
      }
      m_output += ")), ";

      if (!write_expression(_expression->m_array.get())) {
        return false;
      }
      m_output += "._size1";

      char buff[11];
      for (uint32_t i = 1;
           i < _expression->m_array->m_type->m_slice_dimensions - 1; ++i) {
        m_output += ", ";
        memory::writeuint32(buff, i, 11);
        if (!write_expression(_expression->m_array.get())) {
          return false;
        }
        m_output += "._stride";
        m_output += buff;
        memory::writeuint32(buff, i + 1, 11);
        m_output += ", ";
        if (!write_expression(_expression->m_array.get())) {
          return false;
        }
        m_output += "._size";
        m_output += buff;
      }

      m_output += "})";
      return true;
    }
  }

  if (!write_expression(_expression->m_array.get())) {
    return false;
  }

  if (_expression->m_array->m_type->m_builtin == builtin_type::c_string_type) {
    m_output += "[";
  } else {
    if (_expression->m_array->m_type->m_static_array_size == 0) {
      m_output += "->_val[";
    } else {
      m_output += "._val[";
    }
  }

  if (!write_expression(_expression->m_index.get())) {
    return false;
  }
  m_output += "]";
  return true;
}

bool c_compiler::write_slice_expression(
    const ast_slice_expression* _expression) {
  m_output += "((";
  if (!write_type(_expression->m_type)) {
    return false;
  }
  m_output += ")";
  m_output += "{";
  if (_expression->m_array->m_type->m_classification ==
      ast_type_classification::slice_type) {
    m_output += "(";
    if (!write_type(_expression->m_type->m_implicitly_contained_type)) {
      return false;
    }
    m_output += "*)(";
    if (!write_expression(_expression->m_array.get())) {
      return false;
    }
    m_output += "._begin + (";
    if (_expression->m_array->m_type->m_slice_dimensions > 1) {
      if (!write_expression(_expression->m_array.get())) {
        return false;
      }
      m_output += "._stride0 * ";
    }
    if (!write_expression(_expression->m_index_0.get())) {
      return false;
    }
    m_output += ")), ";
    if (!write_expression(_expression->m_index_1.get())) {
      return false;
    }
    m_output += "-";
    if (!write_expression(_expression->m_index_0.get())) {
      return false;
    }

    char buff[11];
    for (uint32_t i = 0;
         i < _expression->m_array->m_type->m_slice_dimensions - 1; ++i) {
      m_output += ", ";
      memory::writeuint32(buff, i, 11);
      if (!write_expression(_expression->m_array.get())) {
        return false;
      }
      m_output += "._stride";
      m_output += buff;
      memory::writeuint32(buff, i + 1, 11);
      m_output += ", ";
      if (!write_expression(_expression->m_array.get())) {
        return false;
      }
      m_output += "._size";
      m_output += buff;
    }
  } else if (_expression->m_array->m_type->m_builtin ==
             builtin_type::c_string_type) {
    if (!write_expression(_expression->m_array.get())) {
      return false;
    }
    m_output += " + ";
    if (!write_expression(_expression->m_index_0.get())) {
      return false;
    }
    m_output += ",";
    if (!write_expression(_expression->m_index_1.get())) {
      return false;
    }
    m_output += "-";
    if (!write_expression(_expression->m_index_0.get())) {
      return false;
    }
  } else {
    // This is an array of some type
    if (!write_expression(_expression->m_array.get())) {
      return false;
    }
    if (_expression->m_array->m_type->m_static_array_size == 0) {
      m_output += "->_val";
    } else {
      m_output += "._val";
    }

    m_output += " + ";
    if (!write_expression(_expression->m_index_0.get())) {
      return false;
    }
    m_output += ",";
    if (!write_expression(_expression->m_index_1.get())) {
      return false;
    }
    m_output += "-";
    if (!write_expression(_expression->m_index_0.get())) {
      return false;
    }
  }
  m_output += "})";
  return true;
}

bool c_compiler::write_cast_expression(const ast_cast_expression* _expression) {
  if (_expression->m_type->m_classification ==
          ast_type_classification::reference &&
      _expression->m_base_expression->m_type->m_classification ==
          ast_type_classification::struct_type) {
    if (_expression->m_type->m_implicitly_contained_type ==
        _expression->m_base_expression->m_type) {
      m_output += "(&";
      if (!write_expression(_expression->m_base_expression.get())) {
        return false;
      }
      m_output += ")";
      return true;
    }
  }

  if (_expression->m_type->m_classification ==
          ast_type_classification::static_array &&
      _expression->m_base_expression->m_type->m_classification ==
          ast_type_classification::static_array) {
    if (_expression->m_type->m_static_array_size == 0 &&
        _expression->m_base_expression->m_type->m_static_array_size != 0) {
      m_output += "((";
      if (!write_type(_expression->m_type)) {
        return false;
      }
      m_output += ")&";
      if (!write_expression(_expression->m_base_expression.get())) {
        return false;
      }
      m_output += ")";
      return true;
    }
  }

  if (_expression->m_type->m_classification ==
          ast_type_classification::struct_type &&
      _expression->m_base_expression->m_type->m_classification ==
          ast_type_classification::reference) {
    if (_expression->m_base_expression->m_type->m_implicitly_contained_type ==
        _expression->m_type) {
      m_output += "(*";
      if (!write_expression(_expression->m_base_expression.get())) {
        return false;
      }
      m_output += ")";
      return true;
    }
  }

  if (_expression->m_type->m_classification ==
          ast_type_classification::slice_type &&
      _expression->m_base_expression->m_type->m_builtin ==
          builtin_type::nullptr_type) {
    m_output += "(";
    if (!write_type(_expression->m_type)) {
      return false;
    }
    m_output += ")";
    m_output += "{";
    if (!write_expression(_expression->m_base_expression.get())) {
      return false;
    }
    m_output += "}";
    return true;
  }

  bool is_bitcast = _expression->m_type->m_classification ==
                        ast_type_classification::reference &&
                    _expression->m_base_expression->m_type->m_classification ==
                        ast_type_classification::reference;
  is_bitcast |= _expression->m_type->m_builtin == builtin_type::void_ptr_type &&
                _expression->m_base_expression->m_type->m_classification ==
                    ast_type_classification::reference;
  is_bitcast |= _expression->m_type->m_builtin == builtin_type::void_ptr_type &&
                _expression->m_base_expression->m_type->m_classification ==
                    ast_type_classification::shared_reference;
  is_bitcast |= _expression->m_type->m_classification ==
                    ast_type_classification::shared_reference &&
                _expression->m_base_expression->m_type->m_classification ==
                    ast_type_classification::shared_reference;
  is_bitcast |= _expression->m_type->m_classification ==
                    ast_type_classification::reference &&
                _expression->m_base_expression->m_type->m_classification ==
                    ast_type_classification::shared_reference;
  is_bitcast |= _expression->m_type->m_classification ==
                    ast_type_classification::shared_reference &&
                _expression->m_base_expression->m_type->m_classification ==
                    ast_type_classification::reference;
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
                _expression->m_base_expression->m_type->m_builtin ==
                    builtin_type::nullptr_type;
  is_bitcast |= _expression->m_type->m_classification ==
                    ast_type_classification::function_pointer &&
                _expression->m_base_expression->m_type->m_classification ==
                    ast_type_classification::function_pointer;
  is_bitcast |= _expression->m_type->m_classification ==
                    ast_type_classification::reference &&
                _expression->m_base_expression->m_type->m_builtin ==
                    builtin_type::nullptr_type;
  is_bitcast |= _expression->m_type->m_classification ==
                    ast_type_classification::shared_reference &&
                _expression->m_base_expression->m_type->m_builtin ==
                    builtin_type::nullptr_type;
  is_bitcast |= _expression->m_type->m_builtin == builtin_type::void_ptr_type &&
                _expression->m_base_expression->m_type->m_builtin ==
                    builtin_type::nullptr_type;
  is_bitcast |= _expression->m_type->m_builtin == builtin_type::void_ptr_type &&
                _expression->m_base_expression->m_type->m_classification ==
                    ast_type_classification::shared_reference;
  is_bitcast |= _expression->m_type->m_classification ==
                    ast_type_classification::runtime_array &&
                _expression->m_base_expression->m_type->m_builtin ==
                    builtin_type::void_ptr_type;
  is_bitcast |= _expression->m_base_expression->m_type->m_classification ==
                    ast_type_classification::runtime_array &&
                _expression->m_type->m_builtin == builtin_type::void_ptr_type;
  is_bitcast |= _expression->m_base_expression->m_type->m_builtin ==
                    builtin_type::integral_type &&
                _expression->m_type->m_builtin == builtin_type::size_type;
  is_bitcast |= _expression->m_type->m_classification ==
                    ast_type_classification::static_array &&
                _expression->m_type->m_static_array_size == 0 &&
                _expression->m_base_expression->m_type->m_classification ==
                    ast_type_classification::runtime_array;

  if (is_bitcast) {
    m_output += "((";
    write_type(_expression->m_type);
    m_output += ")(";
    if (!write_expression(_expression->m_base_expression.get())) {
      return false;
    }
    m_output += "))";
    return true;
  }

  WN_RELEASE_ASSERT(false, "Unknown cast");
  return false;
}

bool c_compiler::write_builtin_statement(
    const ast_builtin_statement* _statement) {
  align_line();
  switch (_statement->m_builtin_type) {
    case builtin_statement_type::atomic_fence:
      m_output += "_wns_fence();";
      return true;
      break;
    case builtin_statement_type::internal_set_array_length:
      if (!write_expression(_statement->m_expressions[0].get())) {
        return false;
      }
      if (_statement->m_expressions[0]->m_type->m_static_array_size == 0) {
        m_output += "->_size";
      } else {
        m_output += "._size";
      }
      m_output += " = ";
      if (!write_expression(_statement->m_expressions[1].get())) {
        return false;
      }
      m_output += ";";
      return true;
    case builtin_statement_type::break_if_not:
      m_output += "if (!";
      if (!write_expression(_statement->m_expressions[0].get())) {
        return false;
      }
      m_output += ") break;";
      return true;
    default:
      WN_RELEASE_ASSERT(false, "Unknown statement type");
  }
  return false;
}

bool c_compiler::write_builtin_expression(
    const ast_builtin_expression* _expression) {
  switch (_expression->m_builtin_type) {
    case builtin_expression_type::size_of:
      m_output += "sizeof(";
      if (!write_type(_expression->m_extra_types[0])) {
        return false;
      }
      m_output += ")";
      return true;
    case builtin_expression_type::align_of:
      WN_RELEASE_ASSERT(false, "Unhandled: alignof");
    case builtin_expression_type::pointer_to_shared:
      m_output += "(((_wns_shared_object*)(";
      if (!write_expression(_expression->m_expressions[0].get())) {
        return false;
      }
      m_output += ")) - 1)";
      return true;
    case builtin_expression_type::shared_to_pointer:
      m_output += "(void*)(";
      if (!write_expression(_expression->m_expressions[0].get())) {
        return false;
      }
      m_output += " + 1)";
      return true;
    case builtin_expression_type::atomic_inc:
      m_output += "_wns_atomic_inc(";
      if (!write_expression(_expression->m_expressions[0].get())) {
        return false;
      }
      m_output += ")";
      return true;
    case builtin_expression_type::atomic_dec:
      m_output += "_wns_atomic_dec(";
      if (!write_expression(_expression->m_expressions[0].get())) {
        return false;
      }
      m_output += ")";
      return true;
    case builtin_expression_type::array_length:
      if (!write_expression(_expression->m_expressions[0].get())) {
        return false;
      }
      if (_expression->m_expressions[0]->m_type->m_classification ==
          ast_type_classification::slice_type) {
        m_output += "._size0";
      } else {
        if (_expression->m_expressions[0]->m_type->m_static_array_size == 0) {
          m_output += "->_size";
        } else {
          m_output += "._size";
        }
      }
      return true;
    default:
      WN_RELEASE_ASSERT(false, "You should not get here");
  }
  WN_RELEASE_ASSERT(false, "Not implemented builtin type");
  return false;
}

bool c_compiler::write_expression(const ast_expression* _expression) {
  switch (_expression->m_node_type) {
    case ast_node_type::ast_binary_expression: {
      return write_binary_expression(
          cast_to<ast_binary_expression>(_expression));
    } break;
    case ast_node_type::ast_constant: {
      return write_constant_expression(cast_to<ast_constant>(_expression));
    } break;
    case ast_node_type::ast_id:
      return write_id(cast_to<ast_id>(_expression));
    case ast_node_type::ast_array_access_expression: {
      return write_array_access_expression(
          cast_to<ast_array_access_expression>(_expression));
    } break;
    case ast_node_type::ast_slice_expression: {
      return write_slice_expression(cast_to<ast_slice_expression>(_expression));
    } break;
    case ast_node_type::ast_member_access_expression: {
      return write_member_access_expression(
          cast_to<ast_member_access_expression>(_expression));
    } break;
    case ast_node_type::ast_unary_expression: {
      return write_unary_expression(cast_to<ast_unary_expression>(_expression));
    } break;
    case ast_node_type::ast_cast_expression: {
      return write_cast_expression(cast_to<ast_cast_expression>(_expression));
    } break;
    case ast_node_type::ast_builtin_expression: {
      return write_builtin_expression(
          cast_to<ast_builtin_expression>(_expression));
    } break;
    case ast_node_type::ast_function_call_expression: {
      return write_function_call(
          cast_to<ast_function_call_expression>(_expression));
    } break;
    case ast_node_type::ast_function_pointer_expression: {
      return write_function_pointer_expression(
          cast_to<ast_function_pointer_expression>(_expression));
    }
    default:
      WN_RELEASE_ASSERT(false, "You should not get here");
  }
  WN_RELEASE_ASSERT(false, "Unimplemented expression type");
  return false;
}

bool c_compiler::write_return(const ast_return_instruction* _return) {
  align_line();
  if (_return->m_return_expr != nullptr) {
    WN_RELEASE_ASSERT(_return->m_return_expr->m_temporaries.empty(),
        "Return instruction temporaries should have been handled already");

    m_output += "return ";
    if (!write_expression(_return->m_return_expr.get())) {
      return false;
    }
    m_output += ";";
  } else {
    m_output += "return;";
  }
  return true;
}

bool c_compiler::decode_type(const ast_type* _type) {
  switch (_type->m_classification) {
    case ast_type_classification::primitive:
      switch (_type->m_builtin) {
        case builtin_type::bool_type:
          m_types[_type] = containers::string(m_allocator, "bool");
          return true;
        case builtin_type::integral_type:
          switch (_type->m_bit_width) {
            case 8:
              m_types[_type] = containers::string(m_allocator, "uint8_t");
              return true;
            case 32:
              m_types[_type] = containers::string(m_allocator, "int32_t");
              return true;
            default:
              WN_RELEASE_ASSERT(false, "Unimplemented bit width");
              return false;
          }
        case builtin_type::floating_point_type:
          switch (_type->m_bit_width) {
            case 32:
              m_types[_type] = containers::string(m_allocator, "float");
              return true;
            default:
              WN_RELEASE_ASSERT(false, "Unimplemented bit width");
              return false;
          }
        case builtin_type::size_type:
          m_types[_type] = containers::string(m_allocator, "size_t");
          return true;
        case builtin_type::void_type:
          m_types[_type] = containers::string(m_allocator, "void");
          return true;
        case builtin_type::void_ptr_type:
          m_types[_type] = containers::string(m_allocator, "void*");
          return true;
        case builtin_type::c_string_type:
          m_types[_type] = containers::string(m_allocator, "const char*");
          return true;
        case builtin_type::nullptr_type:
          m_types[_type] = containers::string(m_allocator, "");
          return true;
        case builtin_type::vtable_type:
          m_types[_type] = containers::string(m_allocator, "_vtable");
          return true;
        case builtin_type::not_builtin:
          WN_RELEASE_ASSERT(false, "You shouldn't get here");
          return false;
        default:
          WN_RELEASE_ASSERT(false, "Unimplemented builtin type");
          return false;
      }
    case ast_type_classification::function_pointer:
      m_types[_type] = containers::string(m_allocator, _type->m_name);
      return true;
    case ast_type_classification::static_array:
      if (!decode_type(_type->m_implicitly_contained_type)) {
        return false;
      }
      if (_type->m_static_array_size == 0) {
        m_types[_type] = containers::string(m_allocator, "_array0_") +
                         m_types[_type->m_implicitly_contained_type] + "*";

      } else {
        char buff[11];
        memory::writeuint32(buff, _type->m_static_array_size, 11);

        m_types[_type] = containers::string(m_allocator, "_array") + buff +
                         "_" + m_types[_type->m_implicitly_contained_type];
      }
      return true;
    case ast_type_classification::runtime_array:
      if (!decode_type(_type->m_implicitly_contained_type)) {
        return false;
      }
      m_types[_type] = containers::string(m_allocator, "_runtime_array_") +
                       m_types[_type->m_implicitly_contained_type];
      return true;
    case ast_type_classification::reference:
      if (!decode_type(_type->m_implicitly_contained_type)) {
        return false;
      }
      m_types[_type] = m_types[_type->m_implicitly_contained_type] + "*";
      return true;
    case ast_type_classification::shared_reference:
      if (!decode_type(_type->m_implicitly_contained_type)) {
        return false;
      }
      m_types[_type] = containers::string(m_allocator, "_shared_") +
                       m_types[_type->m_implicitly_contained_type];
      return true;
    case ast_type_classification::weak_reference:
      if (!decode_type(_type->m_implicitly_contained_type)) {
        return false;
      }
      m_types[_type] = containers::string(m_allocator, "_weak_") +
                       m_types[_type->m_implicitly_contained_type];
      return true;
    case ast_type_classification::struct_type:
      m_types[_type] = _type->m_name;
      return true;
    case ast_type_classification::extern_type:
      m_types[_type] = containers::string(m_allocator, _type->m_name);
      return true;
    case ast_type_classification::slice_type:
      m_types[_type] = _type->m_name;
      return true;
    default:
      WN_RELEASE_ASSERT(false, "We shouldn't get here");
      return false;
  }
}

bool c_compiler::write_preamble() {
  m_output += internal::c_premable;
  return true;
}

}  // namespace scripting
}  // namespace wn