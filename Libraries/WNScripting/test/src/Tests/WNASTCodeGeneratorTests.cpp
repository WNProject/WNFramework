// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/inc/WNString.h"
#include "WNLogging/inc/WNBufferLogger.h"
#include "WNScripting/test/inc/Common.h"
#include "WNScripting/inc/WNASTCodeGenerator.h"
#include "WNTesting/inc/WNTestHarness.h"

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
        walked_headers(0),
        walked_constants(0),
        walked_parameter_instatiations(0) {}

  void* walk_expression(
      const wn::scripting::expression*,
      wn::containers::contiguous_range<wn::containers::contiguous_range<void*>>,
      void*) {
    walked_expressions += 1;
    return reinterpret_cast<void*>(walked_expressions);
  }
  void* walk_expression(
      const wn::scripting::null_allocation_expression*,
      wn::containers::contiguous_range<wn::containers::contiguous_range<void*>>,
      void*) {
    walked_null_expressions += 1;
    return reinterpret_cast<void*>(walked_null_expressions);
  }

  void* walk_expression(
      const wn::scripting::constant_expression*,
      wn::containers::contiguous_range<wn::containers::contiguous_range<void*>>,
      void*) {
    walked_constants += 1;
    return reinterpret_cast<void*>(walked_constants);
  }

  void* walk_function(const wn::scripting::function*,
                      void*,
                      wn::containers::dynamic_array<void*>) {
    walked_functions += 1;
    return reinterpret_cast<void*>(walked_functions);
  }

  void* walk_function_header(
      const wn::scripting::function*, void*,
      wn::containers::dynamic_array<std::pair<void*, void*>>) {
    walked_function_headers += 1;
    return reinterpret_cast<void*>(walked_function_headers);
  }

  void* walk_declaration(const wn::scripting::declaration*,
                         void*) {
    walked_declarations += 1;
    return reinterpret_cast<void*>(walked_declarations);
  }

  void* walk_parameter_name(const wn::scripting::parameter*,
                       void*) {
    walked_parameters += 1;
    return reinterpret_cast<void*>(walked_parameters);
  }

  void* walk_parameter_instantiation(const wn::scripting::parameter*,
    void*, std::pair<void*, void*>&, wn_size_t) {
    walked_parameter_instatiations += 1;
    return reinterpret_cast<void*>(walked_parameters);
  }

  void* walk_function_name(const wn::scripting::parameter*,
                       void*) {
    walked_headers += 1;
    return reinterpret_cast<void*>(walked_headers);
  }

  void* walk_type(const wn::scripting::type*) {
    walked_types += 1;
    return reinterpret_cast<void*>(walked_types);
  }

  void* walk_return_instruction(
      const wn::scripting::return_instruction*,
      void*) {
    walked_return_instructions += 1;
    return reinterpret_cast<void*>(walked_return_instructions);
  }

  void* walk_return_instruction(
      const wn::scripting::return_instruction*) {
    walked_return_instructions += 1;
    return reinterpret_cast<void*>(walked_return_instructions);
  }

  void* walk_script_file(const wn::scripting::script_file*,
                         const wn::containers::contiguous_range<void*>&,
                         const wn::containers::contiguous_range<void*>&,
                         const wn::containers::contiguous_range<void*>&) {
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
  wn_size_t walked_headers;
  wn_size_t walked_parameters;
  wn_size_t walked_constants;
  wn_size_t walked_parameter_instatiations;
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
  typedef void* function_type_name;
  typedef void* script_file_type;
  typedef void* struct_definition_type;
  typedef void* type_type;
  typedef void* include_type;
};

TEST(ast_code_generator, simplest_function) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::scripting::test_file_manager manager(
      &allocator, {{"file.wns", "Void main() { return; }"}});
}

TEST(ast_code_generator, full_file) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::scripting::test_file_manager manager(
      &allocator, {{"file.wns", "Void main() { return; }"}});
}

TEST(ast_code_generator, multiple_functions) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::scripting::test_file_manager manager(&allocator,
                                           {{"file.wns",
                                             "Void main() { return; }\n"
                                             "Void foo() { return; }\n"}});
}

wn_void log_flush_cb(wn_void* _dat, const wn_char* _str, wn_size_t _length,
                     const std::vector<WNLogging::WNLogColorElement>&) {
  wn::containers::string* str = static_cast<wn::containers::string*>(_dat);
  str->append(_str, _str + _length);
}

TEST(ast_code_generator, multiple_returns) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::scripting::test_file_manager manager(
      &allocator, {{"file.wns", "Void main() { return; return; }\n"}});
}

class ast_code_generator_valid_ints : public ::testing::TestWithParam<const char*> {};

TEST_P(ast_code_generator_valid_ints, valid_ints) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::containers::string str(&allocator);
  str += "Int main() { return ";
  str += GetParam();
  str += "; }\n";

  wn::scripting::test_file_manager manager(&allocator, {{"file.wns", str}});
}

INSTANTIATE_TEST_CASE_P(valid_integers, ast_code_generator_valid_ints,
                        ::testing::Values("0", "1", "2", "-1", "-32",
                                          "2147483647", "-2147483648"));

class ast_code_generator_invalid_ints : public ::testing::TestWithParam<const char*> {};

TEST_P(ast_code_generator_invalid_ints, invalid_ints) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::containers::string str(&allocator);
  str += "Int main() { return ";
  str += GetParam();
  str += "; }\n";

  wn::scripting::test_file_manager manager(&allocator, {{"file.wns", str}});
}

INSTANTIATE_TEST_CASE_P(invalid_integers, ast_code_generator_invalid_ints,
                        ::testing::Values("2147483648", "-2147483649",
                                          "11111111111"));