// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/test/inc/Common.h"
#include "WNScripting/inc/WNASTWalker.h"

struct ExpressionTestStruct {
  ExpressionTestStruct()
      : pre_walked_functions(0),
        walked_functions(0),
        pre_walked_function_headers(0),
        walked_function_headers(0),
        pre_walked_expressions(0),
        walked_expressions(0),
        walked_null_expressions(0),
        pre_walked_declarations(0),
        walked_declarations(0),
        pre_walked_return_instructions(0),
        walked_return_instructions(0),
        pre_walked_script_files(0),
        walked_script_files(0),
        pre_walked_parameters(0),
        walked_parameters(0){}
  void pre_walk_expression(
    const wn::scripting::expression*) {
    pre_walked_expressions += 1;
  }
  void* walk_expression(
      const wn::scripting::expression*,
      wn::containers::contiguous_range<wn::containers::contiguous_range<void*>>,
      wn_uint32) {
    walked_expressions += 1;
    return reinterpret_cast<void*>(walked_expressions);
  }
  void* walk_expression(
      const wn::scripting::null_allocation_expression*,
      wn::containers::contiguous_range<wn::containers::contiguous_range<void*>>,
      wn_uint32) {
    walked_null_expressions += 1;
    return reinterpret_cast<void*>(walked_null_expressions);
  }

  void pre_walk_function(const wn::scripting::function* _function) {
    pre_walked_functions += 1;
  }

  void* walk_function(const wn::scripting::function* _function,
                      void* _function_header,
                      containers::dynamic_array<void*> _body) {
    walked_functions += 1;
    return reinterpret_cast<void*>(walked_functions);
  }
  void pre_walk_function_header(
      const wn::scripting::function* _function) {
    pre_walked_function_headers += 1;
  }

  void* walk_function_header(const wn::scripting::function* _function, void* _decl,
                      containers::dynamic_array<void*> _parameters) {
    walked_function_headers += 1;
    return reinterpret_cast<void*>(walked_function_headers);
  }

  void pre_walk_declaration(const wn::scripting::declaration* _declaration) {
    pre_walked_declarations += 1;
  }

  void* walk_declaration(const wn::scripting::declaration* _declaration,
                         void* _type) {
    walked_declarations += 1;
    return reinterpret_cast<void*>(walked_declarations);
  }

  void pre_walk_parameter(const wn::scripting::parameter* _parameter) {
    pre_walked_parameters += 1;
  }

  void* walk_parameter(const wn::scripting::parameter* _parameter,
                         void* _type) {
    walked_parameters += 1;
    return reinterpret_cast<void*>(walked_parameters);
  }

  void* walk_type(const wn::scripting::type* _type) {
    walked_types += 1;
    return reinterpret_cast<void*>(walked_types);
  }

  void pre_walk_type(const wn::scripting::type* _type) {
    pre_walked_types += 1;
  }

  void pre_walk_return_instruction(
      const wn::scripting::return_instruction* _return_instruction) {
    pre_walked_return_instructions += 1;
  }

  void* walk_return_instruction(
      const wn::scripting::return_instruction* _return_instruction,
      void* _expression) {
    walked_return_instructions += 1;
    return reinterpret_cast<void*>(walked_return_instructions);
  }

  void* walk_return_instruction(
      const wn::scripting::return_instruction* _return_instruction) {
    walked_return_instructions += 1;
    return reinterpret_cast<void*>(walked_return_instructions);
  }

  void pre_walk_script_file(
    const wn::scripting::script_file* _file) {
    pre_walked_script_files += 1;
  }

  void* walk_script_file(const wn::scripting::script_file* _file,
    const containers::contiguous_range<void*>& _functions,
    const containers::contiguous_range<void*>& _includes,
    const containers::contiguous_range<void*>& _structs) {
    walked_script_files += 1;
    return reinterpret_cast<void*>(walked_script_files);
  }

  wn_size_t pre_walked_expressions;
  wn_size_t walked_expressions;
  wn_size_t walked_null_expressions;

  wn_size_t pre_walked_functions;
  wn_size_t walked_functions;


  wn_size_t pre_walked_function_headers;
  wn_size_t walked_function_headers;

  wn_size_t pre_walked_declarations;
  wn_size_t walked_declarations;

  wn_size_t pre_walked_types;
  wn_size_t walked_types;

  wn_size_t pre_walked_return_instructions;
  wn_size_t walked_return_instructions;

  wn_size_t pre_walked_script_files;
  wn_size_t walked_script_files;

  wn_size_t pre_walked_parameters;
  wn_size_t walked_parameters;
};

struct TestTraits {
  typedef void* expression_type;
  typedef void* function_type;
  typedef void* function_header_type;
  typedef void* assignment_instruction_type;
  typedef void* parameter_type;
  typedef void* instruction_type;
  typedef void* return_instruction_type;
  typedef void* lvalue_type;
  typedef void* script_file_type;
  typedef void* struct_definition_type;
  typedef void* type_type;
  typedef void* include_type;
};

TEST(ast_walker, simplest_function) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::scripting::test_file_manager manager(
      &allocator, {{"file.wns", "Void main() { return; }"}});

  wn::scripting::type_validator validator(&allocator);
  ExpressionTestStruct s;
  wn::scripting::ast_walker<ExpressionTestStruct, TestTraits> walker(
      &s, WNLogging::get_null_logger(), &validator, &allocator);

  auto a = test_parse_file("file.wns", &manager, &allocator);
  ASSERT_EQ(1, a->get_functions().size());
  walker.walk_function(a->get_functions()[0].get());
  EXPECT_EQ(1, a->get_functions().size());
  EXPECT_EQ(1, s.pre_walked_parameters);
  EXPECT_EQ(1, s.pre_walked_return_instructions);
  EXPECT_EQ(0, s.pre_walked_expressions);
  walker.walk_function(a->get_functions()[0].get());
}

TEST(ast_walker, full_file) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::scripting::test_file_manager manager(
      &allocator, {{"file.wns", "Void main() { return; }"}});

  wn::scripting::type_validator validator(&allocator);
  ExpressionTestStruct s;
  wn::scripting::ast_walker<ExpressionTestStruct, TestTraits> walker(
      &s, WNLogging::get_null_logger(), &validator, &allocator);

  auto a = test_parse_file("file.wns", &manager, &allocator);
  walker.walk_script_file(a.get());
  EXPECT_EQ(1, s.pre_walked_script_files);
  EXPECT_EQ(1, s.walked_script_files);
  EXPECT_EQ(0, s.pre_walked_declarations);
  EXPECT_EQ(1, s.pre_walked_parameters);
  EXPECT_EQ(1, s.pre_walked_return_instructions);
  EXPECT_EQ(0, s.pre_walked_expressions);
}

TEST(ast_walker, multiple_functions) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::scripting::test_file_manager manager(
      &allocator, {{"file.wns", "Void main() { return; }\n"
    "Void foo() { return; }\n"}});

  wn::scripting::type_validator validator(&allocator);
  ExpressionTestStruct s;
  wn::scripting::ast_walker<ExpressionTestStruct, TestTraits> walker(
      &s, WNLogging::get_null_logger(), &validator, &allocator);

  auto a = test_parse_file("file.wns", &manager, &allocator);
  walker.walk_script_file(a.get());
  EXPECT_EQ(1, s.pre_walked_script_files);
  EXPECT_EQ(1, s.walked_script_files);
  EXPECT_EQ(2, s.pre_walked_parameters);
  EXPECT_EQ(2, s.pre_walked_return_instructions);
  EXPECT_EQ(0, s.pre_walked_expressions);
}
