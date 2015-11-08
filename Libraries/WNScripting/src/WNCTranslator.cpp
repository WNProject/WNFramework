// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/inc/WNString.h"
#include "WNScripting/inc/WNASTWalker.h"
#include "WNScripting/inc/WNCTranslator.h"
#include "WNScripting/inc/WNScriptHelpers.h"
#include "WNScripting/inc/WNTypeValidator.h"
#include "WNLogging/inc/WNLog.h"

namespace {
struct ast_c_translator_traits {
  typedef containers::string expression_type;
  typedef containers::string function_header_type;
  typedef containers::string function_type;
  typedef containers::string function_type_name;
  typedef containers::string assignment_instruction_type;
  typedef containers::string parameter_type;
  typedef containers::string instruction_type;
  typedef containers::string return_instruction_type;
  typedef containers::string lvalue_type;
  typedef containers::string script_file_type;
  typedef containers::string struct_definition_type;
  typedef containers::string type_type;
  typedef containers::string include_type;
};

struct ast_c_translator {
  ast_c_translator(memory::allocator* _allocator) : m_allocator(_allocator) {}
  const containers::string& get_output() { return m_output_string; }

  containers::string walk_expression(
      const wn::scripting::expression*,
      wn::containers::contiguous_range<
          wn::containers::contiguous_range<containers::string>>,
      containers::string) {
    WN_RELEASE_ASSERT_DESC(wn_false, "Not Implemented");
    return "";
  }

  containers::string walk_expression(
      const wn::scripting::constant_expression* const_expr,
      wn::containers::contiguous_range<
          wn::containers::contiguous_range<containers::string>>,
      containers::string _type) {
    switch(const_expr->get_classification()) {
    case wn::scripting::type_classification::int_type:
      return const_expr->get_type_text();
      break;
    default:
      WN_RELEASE_ASSERT_DESC(wn_false, "Not Implemented");
    }
    return "";
  }

  containers::string walk_expression(
      const wn::scripting::id_expression* id_expr,
      wn::containers::contiguous_range<
          wn::containers::contiguous_range<containers::string>> expr,
      containers::string _type) {
    return expr[0][0];
  }

  containers::string walk_function_header(
      const wn::scripting::function* _function, containers::string _decl,
      containers::dynamic_array<
          std::pair<containers::string, containers::string>>& _parameters) {
    containers::string ret_string(m_allocator);
    ret_string += _decl + "(";
    bool first = true;
    for (const auto& a : _parameters) {
      if (!first) {
        ret_string += ", ";
      }
      first = false;
      ret_string += std::get<1>(a);
      ret_string += " ";
      ret_string += std::get<0>(a);
    }
    ret_string += ")";
    return ret_string;
  }

  containers::string walk_function(
      const wn::scripting::function* _function, containers::string& _header,
      containers::dynamic_array<containers::string>& _body) {
    containers::string ret_string(m_allocator);
    ret_string += _header + " {";
    for (const auto& a : _body) {
      ret_string += "\n";
      ret_string += a;
    }
    ret_string += "\n}\n";
    return ret_string;
  }

  containers::string walk_declaration(
      const wn::scripting::declaration* _declaration,
      containers::string& _type) {
    WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented");
    return "";
  }

  containers::string walk_parameter_instantiation(
      const wn::scripting::parameter*,  //_parameter,
      const containers::string&,        // function_header
      const std::pair<containers::string, containers::string>& parameter_name,
      wn_size_t  // parameter_number
      ) {
    return std::get<0>(parameter_name);
  }

  containers::string walk_parameter_name(const wn::scripting::parameter* _parameter,
                                    const containers::string& _type) {
    return _parameter->get_name().to_string(m_allocator);
  }

  containers::string walk_function_name(const wn::scripting::parameter* _parameter,
                                 const containers::string& _type) {
    return _type + " " + _parameter->get_name().to_string(m_allocator);
  }

  containers::string walk_type(const wn::scripting::type* _type) {
    switch (_type->get_classification()) {
      case wn::scripting::type_classification::invalid_type:
        WN_RELEASE_ASSERT_DESC(wn_false, "Cannot classify invalid types");
        break;
      case wn::scripting::type_classification::void_type:
        return "void";
        break;
      case wn::scripting::type_classification::int_type:
        return "wn_int32";
        break;
      case wn::scripting::type_classification::float_type:
        return "wn_float32";
        break;
      case wn::scripting::type_classification::char_type:
        return "wn_char";
        break;
      case wn::scripting::type_classification::string_type:
        WN_RELEASE_ASSERT_DESC(wn_false, "Unimplemented string types");
        break;
      case wn::scripting::type_classification::bool_type:
        return "wn_bool";
        break;
      case wn::scripting::type_classification::custom_type:
        return _type->custom_type_name().to_string(m_allocator);
        break;
      case wn::scripting::type_classification::max:
        WN_RELEASE_ASSERT_DESC(wn_false, "Type out of range");
        break;
    }
    return "";
  }

  containers::string walk_return_instruction(
      const wn::scripting::return_instruction* _return_instruction,
      containers::string _expression) {
    return "return " + _expression + ";";
  }

  containers::string walk_return_instruction(
      const wn::scripting::return_instruction* _return_instruction) {
    return "return;";
  }

  containers::string walk_script_file(
    const wn::scripting::script_file* _file,
    const wn::containers::contiguous_range<containers::string>& _functions,
    const wn::containers::contiguous_range<containers::string>& _includes,
    const wn::containers::contiguous_range<containers::string>& _structs
    ) {
    containers::string output_string(m_allocator);
    bool first = true;
    for(auto& function: _functions) {
      if (!first) {
        output_string += "\n";
      }
      first = false;
      output_string += function;
    }
    return output_string;
  }

 private:
  memory::allocator* m_allocator;
  containers::string m_output_string;
};
}  // anonymous

namespace wn {
namespace scripting {

c_translator::c_translator(memory::allocator* _allocator,
                           file_manager* _manager, WNLogging::WNLog* _log)
    : m_allocator(_allocator),
      m_file_manager(_manager),
      m_compilation_log(_log) {}

parse_error c_translator::translate_file(
    const char* file) {
  memory::allocated_ptr<file_buffer> buff = m_file_manager->get_file(file);

  if (!buff) {
    return parse_error::does_not_exist;
  }

  memory::allocated_ptr<script_file> parsed_file = parse_script(
      m_allocator, file, containers::string_view(buff->data(), buff->size()),
      m_compilation_log);

  if (parsed_file == wn_nullptr) {
    return wn::scripting::parse_error::parse_failed;
  }
  type_validator validator(m_allocator);
  ast_c_translator translator(m_allocator);
  wn::scripting::ast_walker<ast_c_translator, ast_c_translator_traits> walker(
      &translator, WNLogging::get_null_logger(), &validator, m_allocator);

  containers::string output_string = walker.walk_script_file(parsed_file.get());

  containers::string output_filename(file);
  output_filename.append(".c");

  m_file_manager->write_file(output_filename.c_str(), output_string);

  return parse_error::ok;
}

}  // namespace scripting
}  // namespace wn
