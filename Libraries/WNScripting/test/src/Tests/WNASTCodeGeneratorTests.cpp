// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/inc/WNString.h"
#include "WNLogging/inc/WNBufferLogger.h"
#include "WNScripting/test/inc/Common.h"
#include "WNScripting/inc/WNASTCodeGenerator.h"
#include "WNTesting/inc/WNTestHarness.h"
using ::testing::Eq;


void flush_buffer(wn_void* v, const wn_char* bytes, wn_size_t length,
                         const std::vector<WNLogging::WNLogColorElement>&) {
  wn::containers::string* s = static_cast<wn::containers::string*>(v);
  s->append(bytes, length);
}

using buffer_logger = WNLogging::WNBufferLogger<flush_buffer>;
using log_buff = wn::containers::string;

struct test_context {
  test_context()
      : manager(&allocator),
        buffer(&allocator),
        logger(&buffer),
        log(&logger),
        num_warnings(0),
        num_errors(0) {}
  wn::memory::default_expanding_allocator<50> allocator;
  wn::scripting::test_file_manager manager;
  log_buff buffer;
  buffer_logger logger;
  WNLogging::WNLog log;
  wn_size_t num_warnings;
  wn_size_t num_errors;

  bool test_parse_file(const wn_char* _file) {
    bool success = wn::scripting::test_parse_file(_file, &manager, &allocator,
                                                  &log, &num_warnings,
                                                  &num_errors) != wn_nullptr;
    log.Flush();
    return success;
  }
};

TEST(ast_code_generator, simplest_function) {
  test_context c;
  c.manager.add_files({{"file.wns", "Void main() { return; }"}});
  EXPECT_TRUE(c.test_parse_file("file.wns"));
  EXPECT_THAT(c.num_errors, Eq(0u));
  EXPECT_THAT(c.num_warnings, Eq(0u));
}

TEST(ast_code_generator, full_file) {
  test_context c;
  c.manager.add_files({{"file.wns", "Void main() { return; }"}});
  EXPECT_TRUE(c.test_parse_file("file.wns"));
  EXPECT_THAT(c.num_errors, Eq(0u));
  EXPECT_THAT(c.num_warnings, Eq(0u));
}

TEST(ast_code_generator, multiple_functions) {
  test_context c;
  c.manager.add_files({{"file.wns",
                        "Void main() { return; }\n"
                        "Void foo() { return; }\n"}});
  EXPECT_TRUE(c.test_parse_file("file.wns"));
  EXPECT_THAT(c.num_errors, Eq(0u));
  EXPECT_THAT(c.num_warnings, Eq(0u));
}

TEST(ast_code_generator, multiple_returns) {
  test_context c;
  // TODO(awoloszyn) Once DCE becomes a pass, this should be a warning.
  c.manager.add_files({{"file.wns", "Void main() { return; return; }\n"}});
  EXPECT_TRUE(c.test_parse_file("file.wns"));
  EXPECT_THAT(c.num_errors, Eq(0u));
  EXPECT_THAT(c.num_warnings, Eq(1u));

}

class ast_code_generator_valid_ints : public ::testing::TestWithParam<const char*> {};

TEST_P(ast_code_generator_valid_ints, valid_ints) {
  test_context c;

  wn::containers::string str(&c.allocator);
  str += "Int main() { return ";
  str += GetParam();
  str += "; }\n";

  c.manager.add_files({{"file.wns", str}});
  EXPECT_TRUE(c.test_parse_file("file.wns"));
  EXPECT_THAT(c.num_errors, Eq(0u));
  EXPECT_THAT(c.num_warnings, Eq(0u));
}

INSTANTIATE_TEST_CASE_P(valid_integers, ast_code_generator_valid_ints,
                        ::testing::Values("0", "1", "2", "-1", "-32",
                                          "2147483647", "-2147483648",
                          "1 + 4", "32 * 10", "-255 % 4", "-1025 / 32",
                          "128 * 32", "32 + 4 * 10", "122 - 142 + 24",
                          "65 * 65 * 23"));

class ast_code_generator_invalid_ints : public ::testing::TestWithParam<const char*> {};

TEST_P(ast_code_generator_invalid_ints, invalid_ints) {
  test_context c;

  wn::containers::string str(&c.allocator);
  str += "Int main() { return ";
  str += GetParam();
  str += "; }\n";

  c.manager.add_files({{"file.wns", str}});
  EXPECT_FALSE(c.test_parse_file("file.wns"));
  EXPECT_THAT(c.num_errors, Eq(1u));
  EXPECT_THAT(c.num_warnings, Eq(0u));
}

INSTANTIATE_TEST_CASE_P(invalid_integers, ast_code_generator_invalid_ints,
                        ::testing::Values("2147483648", "-2147483649",
                                          "11111111111"));

using ast_code_generator_valid_bools = ::testing::TestWithParam<const char*>;

TEST_P(ast_code_generator_valid_bools, valid_bools) {
  test_context c;

  wn::containers::string str(&c.allocator);
  str += "Bool main() { return ";
  str += GetParam();
  str += "; }\n";

  c.manager.add_files({{"file.wns", str}});
  EXPECT_TRUE(c.test_parse_file("file.wns"));
  EXPECT_THAT(c.num_errors, Eq(0u));
  EXPECT_THAT(c.num_warnings, Eq(0u));
}

INSTANTIATE_TEST_CASE_P(valid_booleans, ast_code_generator_valid_bools,
                        ::testing::Values("true", "false", "true != false",
                                          "true != true", "false != true",
                                          "(1 != 2) == true", "1 != 2", "1 == 2"));

using ast_code_generator_invalid_bools = ::testing::TestWithParam<const char*>;

TEST_P(ast_code_generator_invalid_bools, invalid_bools) {
  test_context c;

  wn::containers::string str(&c.allocator);
  str += "Bool main() { return ";
  str += GetParam();
  str += "; }\n";

  c.manager.add_files({{"file.wns", str}});
  EXPECT_FALSE(c.test_parse_file("file.wns"));
  EXPECT_THAT(c.num_errors, Eq(1u));
  EXPECT_THAT(c.num_warnings, Eq(0u));
}

INSTANTIATE_TEST_CASE_P(invalid_bools, ast_code_generator_invalid_bools,
                        ::testing::Values("true + false", "true - false",
                          "true * true", "true * false", "true % false",
                          "(1 != 2) + (1 == 2)", "1 != 2 - 1 == 2"));
