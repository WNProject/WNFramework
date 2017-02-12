// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNScripting/inc/WNASTCodeGenerator.h"
#include "WNContainers/inc/WNString.h"
#include "WNExecutableTest/inc/WNTestHarness.h"
#include "WNFileSystem/inc/WNFactory.h"
#include "WNFileSystem/inc/WNMapping.h"
#include "WNLogging/inc/WNBufferLogger.h"
#include "WNScripting/test/inc/Common.h"

using ::testing::Eq;
using ::testing::Ge;

void flush_buffer(void* v, const char* bytes, size_t length,
    const wn::logging::color_element*, size_t) {
  wn::containers::string* s = static_cast<wn::containers::string*>(v);
  s->append(bytes, length);
}

using buffer_logger = wn::logging::buffer_logger<flush_buffer>;
using log_buff = wn::containers::string;

struct test_context {
  test_context()
    : mapping(wn::file_system::factory().make_mapping(
          wn::file_system::mapping_type::memory_backed, &allocator)),
      buffer(&allocator),
      logger(&buffer),
      log(&logger),
      num_warnings(0),
      num_errors(0) {}
  wn::testing::allocator allocator;
  wn::file_system::mapping_ptr mapping;
  log_buff buffer;
  buffer_logger logger;
  wn::logging::static_log<> log;
  size_t num_warnings;
  size_t num_errors;

  bool test_parse_file(const char* _file) {
    bool success =
        wn::scripting::test_parse_file(_file, mapping.get(), &allocator,
            log.log(), &num_warnings, &num_errors) != nullptr;
    log.log()->flush();
    return success;
  }
};

TEST(ast_code_generator, simplest_function) {
  test_context c;
  c.mapping->initialize_files({{"file.wns", "Void main() { return; }"}});
  EXPECT_TRUE(c.test_parse_file("file.wns"));
  EXPECT_THAT(c.num_errors, Eq(0u));
  EXPECT_THAT(c.num_warnings, Eq(0u));
}

TEST(ast_code_generator, full_file) {
  test_context c;
  c.mapping->initialize_files({{"file.wns", "Void main() { return; }"}});
  EXPECT_TRUE(c.test_parse_file("file.wns"));
  EXPECT_THAT(c.num_errors, Eq(0u));
  EXPECT_THAT(c.num_warnings, Eq(0u));
}

TEST(ast_code_generator, multiple_functions) {
  test_context c;
  c.mapping->initialize_files({{"file.wns",
      "Void main() { return; }\n"
      "Void foo() { return; }\n"}});
  EXPECT_TRUE(c.test_parse_file("file.wns"));
  EXPECT_THAT(c.num_errors, Eq(0u));
  EXPECT_THAT(c.num_warnings, Eq(0u));
}

TEST(ast_code_generator, multiple_returns) {
  test_context c;
  // TODO(awoloszyn) Once DCE becomes a pass, this should be a warning.
  c.mapping->initialize_files(
      {{"file.wns", "Void main() { return; return; }\n"}});
  EXPECT_TRUE(c.test_parse_file("file.wns"));
  EXPECT_THAT(c.num_errors, Eq(0u));
  EXPECT_THAT(c.num_warnings, Eq(1u));
}

class ast_code_generator_valid_ints
    : public ::testing::TestWithParam<const char*> {};

TEST_P(ast_code_generator_valid_ints, valid_ints) {
  test_context c;

  wn::containers::string str(&c.allocator);
  str += "Int main() { return ";
  str += GetParam();
  str += "; }\n";

  c.mapping->initialize_files({{"file.wns", str}});
  EXPECT_TRUE(c.test_parse_file("file.wns"));
  EXPECT_THAT(c.num_errors, Eq(0u));
  EXPECT_THAT(c.num_warnings, Eq(0u));
}

INSTANTIATE_TEST_CASE_P(
    valid_integers, ast_code_generator_valid_ints,
    ::testing::Values("0", "1", "2", "-1", "-32", "2147483647", "-2147483648",
        "1 + 4", "32 * 10", "-255 % 4", "-1025 / 32", "128 * 32", "32 + 4 * 10",
        "122 - 142 + 24", "65 * 65 * 23"));

class ast_code_generator_invalid_ints
    : public ::testing::TestWithParam<const char*> {};

TEST_P(ast_code_generator_invalid_ints, invalid_ints) {
  test_context c;

  wn::containers::string str(&c.allocator);
  str += "Int main() { return ";
  str += GetParam();
  str += "; }\n";

  c.mapping->initialize_files({{"file.wns", str}});
  EXPECT_FALSE(c.test_parse_file("file.wns"));
  EXPECT_THAT(c.num_errors, Eq(1u));
  EXPECT_THAT(c.num_warnings, Eq(0u));
}

INSTANTIATE_TEST_CASE_P(invalid_integers, ast_code_generator_invalid_ints,
    ::testing::Values("2147483648", "-2147483649", "11111111111"));

using ast_code_generator_valid_bools = ::testing::TestWithParam<const char*>;

TEST_P(ast_code_generator_valid_bools, valid_bools) {
  test_context c;

  wn::containers::string str(&c.allocator);
  str += "Bool main() { return ";
  str += GetParam();
  str += "; }\n";

  c.mapping->initialize_files({{"file.wns", str}});
  EXPECT_TRUE(c.test_parse_file("file.wns")) << c.buffer;
  EXPECT_THAT(c.num_errors, Eq(0u));
  EXPECT_THAT(c.num_warnings, Eq(0u));
}

INSTANTIATE_TEST_CASE_P(
    valid_booleans, ast_code_generator_valid_bools,
    ::testing::Values("true", "false", "true != false", "true != true",
        "false != true", "(1 != 2) == true", "1 != 2", "1 == 2", "1 >= 4",
        "1 < 7", "1 <= 32", "(1 + 3) <= 47", "(1 >= 4) != (1 < 32)"));

using ast_code_generator_invalid_bools = ::testing::TestWithParam<const char*>;

TEST_P(ast_code_generator_invalid_bools, invalid_bools) {
  test_context c;

  wn::containers::string str(&c.allocator);
  str += "Bool main() { return ";
  str += GetParam();
  str += "; }\n";

  c.mapping->initialize_files({{"file.wns", str}});
  EXPECT_FALSE(c.test_parse_file("file.wns"));
  EXPECT_THAT(c.num_errors, Eq(1u));
  EXPECT_THAT(c.num_warnings, Eq(0u));
}

INSTANTIATE_TEST_CASE_P(invalid_bools, ast_code_generator_invalid_bools,
    ::testing::Values("true + false", "true - false", "true * true",
                            "true * false", "true % false",
                            "(1 != 2) + (1 == 2)", "1 != 2 - 1 == 2"));

using ast_code_generator_valid_declarations =
    ::testing::TestWithParam<const char*>;
TEST_P(ast_code_generator_valid_declarations, valid_declarations) {
  test_context c;

  wn::containers::string str(&c.allocator);
  str += "Bool main(Int q) { ";
  str += GetParam();
  str += "return x; }\n";

  c.mapping->initialize_files({{"file.wns", str}});
  EXPECT_TRUE(c.test_parse_file("file.wns"));
  EXPECT_THAT(c.num_errors, Eq(0u));
  EXPECT_THAT(c.num_warnings, Eq(0u));
}
// clang-format off
INSTANTIATE_TEST_CASE_P(valid_declarations, ast_code_generator_valid_declarations,
    ::testing::Values(
      "Bool x = false;",
      "Int y = 4; Bool x = y == 4;",
      "Int f = q; Bool x = f == 3;",
      "Int z = 10; Int y = z; Bool x = y == z;"));
// clang-format on

using ast_code_generator_invalid_declarations =
    ::testing::TestWithParam<const char*>;
TEST_P(ast_code_generator_invalid_declarations, invalid_declarations) {
  test_context c;

  wn::containers::string str(&c.allocator);
  str += "Bool main(Int q) { ";
  str += GetParam();
  str += "return x; }\n";

  c.mapping->initialize_files({{"file.wns", str}});
  EXPECT_FALSE(c.test_parse_file("file.wns"));
  EXPECT_THAT(c.num_errors, Eq(1u));
  EXPECT_THAT(c.num_warnings, Eq(0u));
}

INSTANTIATE_TEST_CASE_P(invalid_declarations,
    ast_code_generator_invalid_declarations,
    ::testing::Values("Bool x = 4;", "Int y = q;"));

using ast_code_generator_valid_code = ::testing::TestWithParam<const char*>;
using ast_code_generator_invalid_code = ::testing::TestWithParam<const char*>;

TEST_P(ast_code_generator_valid_code, compiles_cleanly) {
  test_context c;

  wn::containers::string str(GetParam(), &c.allocator);
  c.mapping->initialize_files({{"file.wns", str}});
  EXPECT_TRUE(c.test_parse_file("file.wns")) << c.buffer;
  EXPECT_THAT(c.num_errors, Eq(0u));
  EXPECT_THAT(c.num_warnings, Eq(0u));
}

TEST_P(ast_code_generator_invalid_code, generates_error) {
  test_context c;

  wn::containers::string str(GetParam(), &c.allocator);
  c.mapping->initialize_files({{"file.wns", str}});
  EXPECT_FALSE(c.test_parse_file("file.wns"));
  EXPECT_THAT(c.num_errors, Ge(1u));
  EXPECT_THAT(c.num_warnings, Eq(0u));
}

INSTANTIATE_TEST_CASE_P(
    assignment_tests, ast_code_generator_valid_code,
    ::testing::Values("Int main(Int x) { Int y = 0; y = x; return y; }",
        "Bool main(Int x) { Bool b = false; b = x == 4; return b; }",
        "Int main(Bool x) { Int y = 0; if (x) { y = 3; } return y; }",
        "Bool main(Int x) { Bool b = false; if (x == 3) { b = true; } return "
        "b; }"));

INSTANTIATE_TEST_CASE_P(
    assignment_tests, ast_code_generator_invalid_code,
    ::testing::Values("Int main(Int x) { y = x; return y; }",
        "Bool main(Int x) { Bool b = false; b = x; return b; }",
        "Int main(Int x) { Int x = 0; x = false; return x; }"));

TEST(name_mangling, basic_types) {
  wn::testing::allocator allocator;
  wn::scripting::type_validator validator(&allocator);

  EXPECT_EQ("_Z3wns3FooEll",
      validator.get_mangled_name(
          "Foo",
          static_cast<uint32_t>(wn::scripting::type_classification::int_type),
          wn::containers::dynamic_array<uint32_t>(
              &allocator, {static_cast<uint32_t>(
                              wn::scripting::type_classification::int_type)})));
  EXPECT_EQ("_Z3wns3BarEvf",
      validator.get_mangled_name(
          "Bar",
          static_cast<uint32_t>(wn::scripting::type_classification::void_type),
          wn::containers::dynamic_array<uint32_t>(&allocator,
              {static_cast<uint32_t>(
                  wn::scripting::type_classification::float_type)})));
  EXPECT_EQ("_Z3wns7TestingEbc",
      validator.get_mangled_name(
          "Testing",
          static_cast<uint32_t>(wn::scripting::type_classification::bool_type),
          wn::containers::dynamic_array<uint32_t>(&allocator,
              {static_cast<uint32_t>(
                  wn::scripting::type_classification::char_type)})));
  EXPECT_EQ("_Z3wns3FooEcb",
      validator.get_mangled_name(
          "Foo",
          static_cast<uint32_t>(wn::scripting::type_classification::char_type),
          wn::containers::dynamic_array<uint32_t>(&allocator,
              {static_cast<uint32_t>(
                  wn::scripting::type_classification::bool_type)})));

  EXPECT_EQ("_Z3wns14ReallyLongNameEvlb",
      validator.get_mangled_name(
          "ReallyLongName",
          static_cast<uint32_t>(wn::scripting::type_classification::void_type),
          wn::containers::dynamic_array<uint32_t>(&allocator,
              {static_cast<uint32_t>(
                   wn::scripting::type_classification::int_type),
                  static_cast<uint32_t>(
                      wn::scripting::type_classification::bool_type)})));
  EXPECT_EQ("_Z3wns1AEvfl",
      validator.get_mangled_name(
          "A",
          static_cast<uint32_t>(wn::scripting::type_classification::void_type),
          wn::containers::dynamic_array<uint32_t>(&allocator,
              {static_cast<uint32_t>(
                   wn::scripting::type_classification::float_type),
                  static_cast<uint32_t>(
                      wn::scripting::type_classification::int_type)})));
  EXPECT_EQ("_Z3wns22MultipleParameterTypesEfcb",
      validator.get_mangled_name(
          "MultipleParameterTypes",
          static_cast<uint32_t>(wn::scripting::type_classification::float_type),
          wn::containers::dynamic_array<uint32_t>(&allocator,
              {static_cast<uint32_t>(
                   wn::scripting::type_classification::char_type),
                  static_cast<uint32_t>(
                      wn::scripting::type_classification::bool_type)})));
  EXPECT_EQ("_Z3wns3FooEbbb",
      validator.get_mangled_name(
          "Foo",
          static_cast<uint32_t>(wn::scripting::type_classification::bool_type),
          wn::containers::dynamic_array<uint32_t>(&allocator,
              {static_cast<uint32_t>(
                   wn::scripting::type_classification::bool_type),
                  static_cast<uint32_t>(
                      wn::scripting::type_classification::bool_type)})));
}
