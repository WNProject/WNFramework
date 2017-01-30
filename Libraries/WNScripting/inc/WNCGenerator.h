// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/inc/WNString.h"
#include "WNCore/inc/WNPair.h"
#include "WNScripting/inc/WNASTCodeGenerator.h"
#include "WNScripting/inc/WNNodeTypes.h"

namespace wn {
namespace scripting {
class ast_c_translator;
class type_validator;
struct ast_c_traits {
  // the first element is a list of temporaries
  // to be hoisted out of the expression.
  using instruction_data =
      core::pair<containers::string, containers::string>;
  // The first element is a list of any temporaries,
  // that must be hoisted out of the expression.
  using expression_data =
      core::pair<containers::string, containers::string>;
  using parameter_data = containers::string;
  using function_data = containers::string;
  using type_data = containers::string;
  using struct_definition_data = containers::string;
  using code_gen = ast_c_translator;
};

// Implements the functionality required by the
// ASTCodeGenerator. After walking the tree
// get_output_string() will return a file written in
// the C language, that is equivalent to the AST.
class ast_c_translator {
public:
  ast_c_translator(memory::allocator* _allocator,
      ast_code_generator<ast_c_traits>* _generator, type_validator* _validator)
    : m_temporaries(0),
      m_allocator(_allocator),
      m_output_string(_allocator),
      m_generator(_generator),
      m_validator(_validator) {
    memory::memory_set(m_last_temporary, 0, sizeof(m_last_temporary));
  }

  const containers::string& get_output() {
    return m_output_string;
  }

  void walk_expression(const expression*,
      core::pair<containers::string, containers::string>*) {
    WN_RELEASE_ASSERT_DESC(false, "Not implemented expression type");
  }
  void walk_expression(const sizeof_expression* _sizeof,
      core::pair<containers::string, containers::string>* _str);
  void walk_expression(const constant_expression* _const,
      core::pair<containers::string, containers::string>* _str);
  void walk_expression(const id_expression* _const,
      core::pair<containers::string, containers::string>* _str);
  void walk_expression(const function_pointer_expression* _ptr,
      core::pair<containers::string, containers::string>* _str);
  void walk_expression(const binary_expression* _binary,
      core::pair<containers::string, containers::string>* _str);
  void walk_expression(const short_circuit_expression* _binary,
      core::pair<containers::string, containers::string>* _str);
  void walk_expression(const struct_allocation_expression* _alloc,
      core::pair<containers::string, containers::string>* _str);
  void walk_expression(const member_access_expression* _access,
      core::pair<containers::string, containers::string>* _str);
  void walk_expression(const cast_expression* _cast,
      core::pair<containers::string, containers::string>* _str);
  void walk_expression(const function_call_expression* _call,
      core::pair<containers::string, containers::string>* _str);

  void walk_instruction_list(const instruction_list* _list,
      core::pair<containers::string, containers::string>* _str);
  void walk_type(const type* _type, containers::string* _str);
  void walk_instruction(const instruction*,
      core::pair<containers::string, containers::string>*) {
    WN_RELEASE_ASSERT_DESC(false, "Unimplemented instruction:");
  }
  void walk_instruction(const expression_instruction*,
      core::pair<containers::string, containers::string>*);
  void walk_instruction(const do_instruction*,
      core::pair<containers::string, containers::string>*);
  void walk_instruction(const break_instruction* _break,
      core::pair<containers::string, containers::string>* _str);
  void walk_instruction(const continue_instruction* _continue,
      core::pair<containers::string, containers::string>* _str);
  void walk_instruction(const return_instruction* _inst,
      core::pair<containers::string, containers::string>* _str);
  void walk_instruction(const declaration* _inst,
      core::pair<containers::string, containers::string>* _str);
  void walk_instruction(const assignment_instruction* _inst,
      core::pair<containers::string, containers::string>* _str);
  void walk_instruction(const if_instruction* _inst,
      core::pair<containers::string, containers::string>* _str);
  void walk_instruction(const else_if_instruction* _inst,
      core::pair<containers::string, containers::string>* _str);
  void walk_parameter(const parameter* _param, containers::string* _str);
  void walk_function(const function* _func, containers::string* _str);
  void walk_struct_definition(
      const struct_definition* _definition, containers::string* _str);
  void walk_script_file(const script_file* _file);
  void pre_walk_script_file(const script_file*) {}

private:
  uint32_t m_temporaries;
  char m_last_temporary[11];
  memory::allocator* m_allocator;
  containers::string m_output_string;
  ast_code_generator<ast_c_traits>* m_generator;
  type_validator* m_validator;
};

}  // namespace scripting
}  // namespace wn
