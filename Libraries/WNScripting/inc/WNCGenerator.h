// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/inc/WNString.h"
#include "WNScripting/inc/WNASTCodeGenerator.h"
#include "WNScripting/inc/WNNodeTypes.h"

namespace wn {
namespace scripting {
class ast_c_translator;
struct ast_c_traits {
  using instruction_list_data = containers::string;
  using instruction_data = containers::string;
  using expression_data = containers::string;
  using parameter_data = containers::string;
  using function_data = containers::string;
  using type_data = containers::string;
  using code_gen = ast_c_translator;
};

// Implements the functionality required by the
// ASTCodeGenerator. After walking the tree
// get_output_string() will return a file written in
// the C language, that is equivalent to the AST.
class ast_c_translator {
public:
  ast_c_translator(memory::allocator* _allocator,
                   ast_code_generator<ast_c_traits>* _generator)
      : m_allocator(_allocator),
        m_output_string(_allocator),
        m_generator(_generator) {}
  const containers::string& get_output() { return m_output_string; }
  void walk_expression(const expression*,
                       containers::string*) {}
  void walk_expression(const constant_expression* _const,
                       containers::string* _str);
  void walk_expression(const id_expression* _const,
                       containers::string* _str);
  void walk_expression(const binary_expression* _binary,
                       containers::string* _str);
  void walk_instruction_list(const instruction_list* _list,
                             containers::string* _str);
  void walk_type(const type* _type, containers::string* _str);
  void walk_instruction(const instruction*,
                        containers::string*) {}
  void walk_instruction(const return_instruction* _inst,
                        containers::string* _str);
  void walk_instruction(const declaration* _inst,
                        containers::string* _str);
  void walk_instruction(const assignment_instruction* _inst,
                        containers::string* _str);
  void walk_instruction(const if_instruction* _inst,
                        containers::string* _str);
  void walk_instruction(const else_if_instruction* _inst,
                        containers::string* _str);
  void walk_parameter(const parameter* _param, containers::string* _str);
  void walk_function(const function* _func, containers::string* _str);
  void walk_script_file(const script_file* _file);

 private:
  memory::allocator* m_allocator;
  containers::string m_output_string;
  ast_code_generator<ast_c_traits>* m_generator;
};

}  // namespace scripting
}  // namespace wn
