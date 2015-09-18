// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNContainers/inc/WNString.h"
#include "WNLogging/inc/WNBufferLogger.h"
#include "WNScripting/test/inc/Common.h"
#include "WNScripting/inc/WNASTWalker.h"

struct ExpressionTestStruct {
  ExpressionTestStruct()
      : walked_functions(0),
        walked_function_headers(0),
        walked_expressions(0),
        walked_null_expressions(0),
        walked_declarations(0),
        walked_return_instructions(0),
        walked_script_files(0),
        walked_parameters(0),
        walked_constants(0) {}

  void pre_walk_expression(const wn::scripting::expression*) {}
  void* walk_expression(
      const wn::scripting::expression*,
      wn::containers::contiguous_range<wn::containers::contiguous_range<void*>>,
      void* _type) {
    walked_expressions += 1;
    return reinterpret_cast<void*>(walked_expressions);
  }
  void* walk_expression(
      const wn::scripting::null_allocation_expression*,
      wn::containers::contiguous_range<wn::containers::contiguous_range<void*>>,
      void* _type) {
    walked_null_expressions += 1;
    return reinterpret_cast<void*>(walked_null_expressions);
  }

  void* walk_expression(
      const wn::scripting::constant_expression*,
      wn::containers::contiguous_range<wn::containers::contiguous_range<void*>>,
      void* _type) {
    walked_constants += 1;
    return reinterpret_cast<void*>(walked_constants);
  }

  void pre_walk_function(const wn::scripting::function* _function) {}

  void* walk_function(const wn::scripting::function* _function,
                      void* _function_header,
                      containers::dynamic_array<void*> _body) {
    walked_functions += 1;
    return reinterpret_cast<void*>(walked_functions);
  }
  void pre_walk_function_header(const wn::scripting::function* _function) {}

  void* walk_function_header(const wn::scripting::function* _function,
                             void* _decl,
                             containers::dynamic_array<void*> _parameters) {
    walked_function_headers += 1;
    return reinterpret_cast<void*>(walked_function_headers);
  }

  void pre_walk_declaration(const wn::scripting::declaration* _declaration) {}

  void* walk_declaration(const wn::scripting::declaration* _declaration,
                         void* _type) {
    walked_declarations += 1;
    return reinterpret_cast<void*>(walked_declarations);
  }

  void pre_walk_parameter(const wn::scripting::parameter* _parameter) {}

  void* walk_parameter(const wn::scripting::parameter* _parameter,
                       void* _type) {
    walked_parameters += 1;
    return reinterpret_cast<void*>(walked_parameters);
  }

  void* walk_type(const wn::scripting::type* _type) {
    walked_types += 1;
    return reinterpret_cast<void*>(walked_types);
  }

  void pre_walk_type(const wn::scripting::type* _type) {}

  void pre_walk_return_instruction(
      const wn::scripting::return_instruction* _return_instruction) {}

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

  void pre_walk_script_file(const wn::scripting::script_file* _file) {}

  void* walk_script_file(const wn::scripting::script_file* _file,
                         const containers::contiguous_range<void*>& _functions,
                         const containers::contiguous_range<void*>& _includes,
                         const containers::contiguous_range<void*>& _structs) {
    walked_script_files += 1;
    return reinterpret_cast<void*>(walked_script_files);
  }

  wn_size_t walked_expressions;
  wn_size_t walked_null_expressions;
  wn_size_t walked_functions;
  wn_size_t walked_function_headers;
  wn_size_t walked_declarations;
  wn_size_t walked_types;
  wn_size_t walked_return_instructions;
  wn_size_t walked_script_files;
  wn_size_t walked_parameters;
  wn_size_t walked_constants;
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
  EXPECT_EQ(1, s.walked_parameters);
  EXPECT_EQ(1, s.walked_return_instructions);
  EXPECT_EQ(0, s.walked_expressions);
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
  EXPECT_EQ(1, s.walked_script_files);
  EXPECT_EQ(0, s.walked_declarations);
  EXPECT_EQ(1, s.walked_parameters);
  EXPECT_EQ(1, s.walked_return_instructions);
  EXPECT_EQ(0, s.walked_expressions);
}

TEST(ast_walker, multiple_functions) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::scripting::test_file_manager manager(&allocator,
                                           {{"file.wns",
                                             "Void main() { return; }\n"
                                             "Void foo() { return; }\n"}});

  wn::scripting::type_validator validator(&allocator);
  ExpressionTestStruct s;
  wn::scripting::ast_walker<ExpressionTestStruct, TestTraits> walker(
      &s, WNLogging::get_null_logger(), &validator, &allocator);

  auto a = test_parse_file("file.wns", &manager, &allocator);
  walker.walk_script_file(a.get());
  EXPECT_EQ(1, s.walked_script_files);
  EXPECT_EQ(2, s.walked_parameters);
  EXPECT_EQ(2, s.walked_return_instructions);
  EXPECT_EQ(0, s.walked_expressions);
}

wn_void log_flush_cb(wn_void* _dat, const wn_char* _str, wn_size_t _length,
                     const std::vector<WNLogging::WNLogColorElement>&) {
  wn::containers::string* str = static_cast<wn::containers::string*>(_dat);
  str->append(_str, _str + _length);
}

TEST(ast_walker, multiple_returns) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::scripting::test_file_manager manager(
      &allocator, {{"file.wns", "Void main() { return; return; }\n"}});

  wn::scripting::type_validator validator(&allocator);
  wn::containers::string log_string(&allocator);
  WNLogging::WNBufferLogger<log_flush_cb> logger(&log_string);
  WNLogging::WNLog log(&logger);
  log.SetLogLevel(WNLogging::eLogMax);
  ExpressionTestStruct s;
  wn::scripting::ast_walker<ExpressionTestStruct, TestTraits> walker(
      &s, &log, &validator, &allocator);
  auto a = test_parse_file("file.wns", &manager, &allocator, &log);
  walker.walk_script_file(a.get());
  log.Flush();
  EXPECT_EQ(1, s.walked_script_files);
  EXPECT_EQ(1, s.walked_parameters);
  EXPECT_EQ(1, s.walked_return_instructions);
  EXPECT_EQ(1, s.walked_return_instructions);
  EXPECT_EQ(0, s.walked_expressions);
  EXPECT_EQ(
      "Warning:Instruction after return statement\n"
      "Warning:Line: 1\n"
      "Void main() { return; return; }\n",
      log_string);
}

class ast_walker_valid_ints : public ::testing::TestWithParam<const char*> {};

TEST_P(ast_walker_valid_ints, valid_ints) {
  wn::memory::default_expanding_allocator<50> allocator;
  containers::string str(&allocator);
  str += "Int main() { return ";
  str += GetParam();
  str += "; }\n";

  wn::scripting::test_file_manager manager(&allocator, {{"file.wns", str}});

  wn::scripting::type_validator validator(&allocator);
  wn::containers::string log_string(&allocator);
  WNLogging::WNBufferLogger<log_flush_cb> logger(&log_string);
  WNLogging::WNLog log(&logger);
  log.SetLogLevel(WNLogging::eLogMax);
  ExpressionTestStruct s;
  wn::scripting::ast_walker<ExpressionTestStruct, TestTraits> walker(
      &s, &log, &validator, &allocator);
  auto a = test_parse_file("file.wns", &manager, &allocator, &log);
  walker.walk_script_file(a.get());
  log.Flush();
  EXPECT_EQ("", std::string(log_string.c_str()));

  EXPECT_EQ(1, s.walked_script_files);
  EXPECT_EQ(1, s.walked_script_files);
  EXPECT_EQ(1, s.walked_parameters);
  EXPECT_EQ(1, s.walked_return_instructions);
  EXPECT_EQ(0, s.walked_expressions);
  EXPECT_EQ(1, s.walked_constants);
}

INSTANTIATE_TEST_CASE_P(valid_integers, ast_walker_valid_ints,
                        ::testing::Values("0", "1", "2", "-1", "-32",
                                          "2147483647", "-2147483648"));

class ast_walker_invalid_ints : public ::testing::TestWithParam<const char*> {};

TEST_P(ast_walker_invalid_ints, invalid_ints) {
  wn::memory::default_expanding_allocator<50> allocator;
  containers::string str(&allocator);
  str += "Int main() { return ";
  str += GetParam();
  str += "; }\n";

  wn::scripting::test_file_manager manager(&allocator, {{"file.wns", str}});

  wn::scripting::type_validator validator(&allocator);
  wn::containers::string log_string(&allocator);
  WNLogging::WNBufferLogger<log_flush_cb> logger(&log_string);
  WNLogging::WNLog log(&logger);
  log.SetLogLevel(WNLogging::eLogMax);
  ExpressionTestStruct s;
  wn::scripting::ast_walker<ExpressionTestStruct, TestTraits> walker(
      &s, &log, &validator, &allocator);
  auto a = test_parse_file("file.wns", &manager, &allocator, &log);
  walker.walk_script_file(a.get());
  log.Flush();

  std::string expected("Error:Invalid constant: \"");
  expected += GetParam();
  expected += "\"\n";
  expected += "Error:Line: 1\n";
  expected += "Int main() { return ";
  expected += GetParam();
  expected += "; }\n";

  EXPECT_EQ(expected, std::string(log_string.c_str()));
}

INSTANTIATE_TEST_CASE_P(invalid_integers, ast_walker_invalid_ints,
                        ::testing::Values("2147483648", "-2147483649",
                                          "11111111111"));