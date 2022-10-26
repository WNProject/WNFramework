// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_SCRIPTING_C_COMPILER_H__
#define __WN_SCRIPTING_C_COMPILER_H__

#include "WNContainers/inc/WNHashMap.h"
#include "WNContainers/inc/WNHashSet.h"
#include "WNScripting/inc/internal/ast_convertor_context.h"
#include "WNScripting/inc/parse_ast_convertor.h"

namespace wn {
namespace scripting {
class c_compiler {
public:
  c_compiler(memory::allocator* _allocator);

  const containers::string& get_output() const {
    return m_output;
  }

  bool compile(const ast_script_file* _file);

private:
  bool forward_declare_function(const ast_function* _function);
  bool write_function(const ast_function* _function);
  bool write_function_signature(const ast_function* _function);
  bool write_preamble();

  // VTables
  bool write_vtable(const ast_type* _type);

  // Expressions
  bool write_expression(const ast_expression* _expression);
  bool write_id(const ast_id* _id);
  bool write_constant_expression(const ast_constant* _expression);
  bool write_binary_expression(const ast_binary_expression* _expression);
  bool write_unary_expression(const ast_unary_expression* _expression);
  bool write_function_call(const ast_function_call_expression* _expression);
  bool write_function_pointer_expression(
      const ast_function_pointer_expression* _expression);
  bool write_call_function_pointer(
      const ast_function_call_expression* _expression);
  bool write_call_function(const ast_function_call_expression* _expression);
  bool write_member_access_expression(
      const ast_member_access_expression* _expression);
  bool write_cast_expression(const ast_cast_expression* _expression);
  bool write_array_destruction(const ast_array_destruction* _dest);
  bool write_array_access_expression(
      const ast_array_access_expression* _access);
  bool write_slice_expression(const ast_slice_expression* _slice);

  // Statements
  bool write_statement(const ast_statement* _statement);
  bool write_declaration(const ast_declaration* _declaration);
  bool write_scope_block(
      const ast_scope_block* _block, bool begin_line = false);
  bool write_return(const ast_return_instruction* _return);
  bool write_assignment(const ast_assignment* _assignment);
  bool write_if_block(const ast_if_block* _if);
  bool write_builtin_statement(const ast_builtin_statement* _statement);
  bool write_evaluate_expression(const ast_evaluate_expression* _expr);
  bool write_builtin_expression(const ast_builtin_expression* _expr);
  bool write_loop(const ast_loop* _loop);
  bool write_while_loop(const ast_loop* _loop);
  bool write_do_loop(const ast_loop* _loop);
  bool write_break(const ast_break* _loop);
  bool write_continue(const ast_continue_instruction* _loop);

  // Types
  bool write_type(const ast_type* _type);
  bool decode_type(const ast_type* _type);
  bool forward_declare_type(const ast_type* _type);
  bool forward_declare_function_pointer(const ast_type* _type);
  bool forward_declare_shared_reference(const ast_type* _type);
  bool declare_type(const ast_type* _type);
  bool declare_struct(const ast_type* _type);

  // Helpers
  void align_line();
  bool write_temporaries(const ast_expression* _expr);
  bool write_cleanups(const ast_expression* _expr);
  containers::string get_temporary();

  containers::string m_output;
  uint32_t m_scope_depth = 0;
  uint32_t m_temporary_number = 0;

  containers::hash_map<const ast_loop*, containers::string> m_loop_temporaries;
  containers::hash_map<const ast_type*, containers::string> m_types;
  containers::hash_map<const ast_vtable*,
      containers::dynamic_array<containers::string>>
      m_vtable_functions;
  memory::allocator* m_allocator;
  const ast_function* m_current_function = nullptr;
};
}  // namespace scripting
}  // namespace wn

#endif  //__WN_SCRIPTING_C_COMPILER_H__
