// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/inc/WNString.h"
#include "WNScripting/inc/WNNodeTypes.h"

namespace wn {
namespace scripting {

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
      containers::string);

  containers::string walk_expression(
      const wn::scripting::constant_expression* const_expr,
      wn::containers::contiguous_range<
          wn::containers::contiguous_range<containers::string>>,
      containers::string _type);

  containers::string walk_expression(
      const wn::scripting::id_expression* id_expr,
      wn::containers::contiguous_range<
          wn::containers::contiguous_range<containers::string>> expr,
      containers::string _type);

  containers::string walk_function_header(
      const wn::scripting::function* _function, containers::string _decl,
      containers::dynamic_array<
          std::pair<containers::string, containers::string>>& _parameters);

  containers::string walk_function(
      const wn::scripting::function* _function, containers::string& _header,
      containers::dynamic_array<containers::string>& _body);

  containers::string walk_declaration(
      const wn::scripting::declaration* _declaration,
      containers::string& _type);

  containers::string walk_parameter_instantiation(
      const wn::scripting::parameter*,  //_parameter,
      const containers::string&,        // function_header
      const std::pair<containers::string, containers::string>& parameter_name,
      wn_size_t  // parameter_number
      );

  containers::string walk_parameter_name(
      const wn::scripting::parameter* _parameter,
      const containers::string& _type);

  containers::string walk_function_name(
      const wn::scripting::parameter* _parameter,
      const containers::string& _type);

  containers::string walk_type(const wn::scripting::type* _type);

  containers::string walk_return_instruction(
      const wn::scripting::return_instruction* _return_instruction,
      containers::string _expression);

  containers::string walk_return_instruction(
      const wn::scripting::return_instruction* _return_instruction);

  containers::string walk_script_file(
      const wn::scripting::script_file* _file,
      const wn::containers::contiguous_range<containers::string>& _functions,
      const wn::containers::contiguous_range<containers::string>& _includes,
      const wn::containers::contiguous_range<containers::string>& _structs);

 private:
  memory::allocator* m_allocator;
  containers::string m_output_string;
};

}  // namespace scripting
}  // namespace wn
