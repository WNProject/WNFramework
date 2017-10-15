// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/inc/WNDynamicArray.h"
#include "WNExecutableTest/inc/WNTestHarness.h"
#include "WNFileSystem/inc/WNFactory.h"
#include "WNFileSystem/inc/WNMapping.h"
#include "WNLogging/inc/WNBufferLogger.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNScripting/inc/WNFactory.h"
#include "WNScripting/inc/WNJITEngine.h"
#include "WNScripting/test/inc/Common.h"

void flush_buffer(void* v, const char* bytes, size_t length,
    const wn::logging::color_element*, size_t) {
  wn::containers::string* s = static_cast<wn::containers::string*>(v);
  s->append(bytes, length);
}

using buffer_logger = wn::logging::buffer_logger<flush_buffer>;
using log_buff = wn::containers::string;

TEST(jit_engine, creation) {
  wn::testing::allocator allocator;
  wn::scripting::type_validator validator(&allocator);
  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory(&allocator, wn::testing::k_system_data)
          .make_mapping(
              &allocator, wn::file_system::mapping_type::memory_backed);

  wn::scripting::jit_engine jit_engine(
      &allocator, &validator, mapping.get(), wn::logging::get_null_logger());
}

TEST(jit_engine, basic_parsing) {
  wn::testing::allocator allocator;
  wn::scripting::type_validator validator(&allocator);

  log_buff buff(&allocator);
  buffer_logger logger(&buff);
  wn::logging::static_log<> log(&logger);

  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory(&allocator, wn::testing::k_system_data)
          .make_mapping(
              &allocator, wn::file_system::mapping_type::memory_backed);

  mapping->initialize_files(
      {{"file.wns", "Void main() { return; }"}, {"file2.wns",
                                                    "Void foo() { return; } \n"
                                                    "Void bar() { return; }"}});
  wn::scripting::jit_engine jit_engine(
      &allocator, &validator, mapping.get(), log.log());
  EXPECT_EQ(wn::scripting::parse_error::ok, jit_engine.parse_file("file.wns"))
      << (log.log()->flush(), buff.c_str());
  EXPECT_EQ(wn::scripting::parse_error::ok, jit_engine.parse_file("file2.wns"))
      << (log.log()->flush(), buff.c_str());

  wn::scripting::engine::void_func main;
  ASSERT_TRUE(jit_engine.get_function("main", main));
  wn::scripting::engine::void_func foo;
  ASSERT_TRUE(jit_engine.get_function("foo", foo));
  wn::scripting::engine::void_func bar;
  ASSERT_TRUE(jit_engine.get_function("bar", bar));

  // No returns so lets just see if we crash trying to call.
  jit_engine.invoke(main);
  jit_engine.invoke(foo);
  jit_engine.invoke(bar);
}

// Make sure that multiple returns get coalesced
TEST(jit_engine, multiple_returns) {
  wn::testing::allocator allocator;
  wn::scripting::type_validator validator(&allocator);

  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory(&allocator, wn::testing::k_system_data)
          .make_mapping(
              &allocator, wn::file_system::mapping_type::memory_backed);

  mapping->initialize_files({{"file.wns", "Void main() { return; return; }"}});
  wn::scripting::jit_engine jit_engine(
      &allocator, &validator, mapping.get(), wn::logging::get_null_logger());
  EXPECT_EQ(wn::scripting::parse_error::ok, jit_engine.parse_file("file.wns"));

  wn::scripting::engine::void_func main;
  ASSERT_TRUE(jit_engine.get_function("main", main));

  // No returns so lets just see if we crash trying to call.
  jit_engine.invoke(main);
}

TEST(jit_engine, parse_error) {
  wn::testing::allocator allocator;
  wn::scripting::type_validator validator(&allocator);

  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory(&allocator, wn::testing::k_system_data)
          .make_mapping(
              &allocator, wn::file_system::mapping_type::memory_backed);

  mapping->initialize_files({{"file.wns", "Int main"}});
  wn::scripting::jit_engine jit_engine(
      &allocator, &validator, mapping.get(), wn::logging::get_null_logger());
  EXPECT_EQ(wn::scripting::parse_error::parse_failed,
      jit_engine.parse_file("file.wns"));
}

struct int_test {
  const char* val;
  int32_t number;
};

class jit_int_params : public ::testing::TestWithParam<int_test> {};

TEST_P(jit_int_params, int_return) {
  wn::testing::allocator allocator;
  wn::containers::string str(&allocator);
  wn::scripting::type_validator validator(&allocator);
  str += "Int main() { return ";
  str += GetParam().val;
  str += "; } ";

  wn::containers::string expected(&allocator);
  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory(&allocator, wn::testing::k_system_data)
          .make_mapping(
              &allocator, wn::file_system::mapping_type::memory_backed);

  mapping->initialize_files({{"file.wns", str}});

  wn::scripting::jit_engine jit_engine(
      &allocator, &validator, mapping.get(), wn::logging::get_null_logger());
  EXPECT_EQ(wn::scripting::parse_error::ok, jit_engine.parse_file("file.wns"));

  wn::scripting::script_function<int32_t> new_func;
  ASSERT_TRUE(jit_engine.get_function("main", new_func));
  EXPECT_EQ(GetParam().number, jit_engine.invoke(new_func));
}

TEST_P(jit_int_params, int_passthrough) {
  wn::testing::allocator allocator;
  wn::scripting::type_validator validator(&allocator);
  wn::containers::string str(&allocator);
  str += "Int main(Int x) { return x; }";

  wn::containers::string expected(&allocator);
  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory(&allocator, wn::testing::k_system_data)
          .make_mapping(
              &allocator, wn::file_system::mapping_type::memory_backed);

  mapping->initialize_files({{"file.wns", str}});

  wn::scripting::jit_engine jit_engine(
      &allocator, &validator, mapping.get(), wn::logging::get_null_logger());
  EXPECT_EQ(wn::scripting::parse_error::ok, jit_engine.parse_file("file.wns"));

  wn::scripting::script_function<int32_t, int32_t> new_func;
  ASSERT_TRUE(jit_engine.get_function("main", new_func));
  EXPECT_EQ(GetParam().number, jit_engine.invoke(new_func, GetParam().number));
}

INSTANTIATE_TEST_CASE_P(int_tests, jit_int_params,
    ::testing::Values(int_test({"0", 0}), int_test({"-1", -1}),
        int_test({"-32", -32}), int_test({"-4096", -4096}),
        int_test({"2147483647", INT32_MAX}),
        int_test({"-2147483648", (INT32_MIN)})));

struct int_binary_test {
  const char* code;
  int32_t expected_return;
};

using jit_binary_arithmetic = ::testing::TestWithParam<int_binary_test>;

TEST_P(jit_binary_arithmetic, simple_operations) {
  wn::testing::allocator allocator;
  wn::containers::string str(&allocator);
  wn::scripting::type_validator validator(&allocator);
  str += "Int main() { return ";
  str += GetParam().code;
  str += "; } ";

  wn::containers::string expected(&allocator);

  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory(&allocator, wn::testing::k_system_data)
          .make_mapping(
              &allocator, wn::file_system::mapping_type::memory_backed);

  mapping->initialize_files({{"file.wns", str}});

  wn::scripting::jit_engine jit_engine(
      &allocator, &validator, mapping.get(), wn::logging::get_null_logger());
  EXPECT_EQ(wn::scripting::parse_error::ok, jit_engine.parse_file("file.wns"));

  wn::scripting::script_function<int32_t> new_func;
  ASSERT_TRUE(jit_engine.get_function("main", new_func));
  EXPECT_EQ(GetParam().expected_return, jit_engine.invoke(new_func));
}

INSTANTIATE_TEST_CASE_P(int_arithmetic_tests, jit_binary_arithmetic,
    ::testing::ValuesIn(
        std::vector<int_binary_test>({{"1 + 2", 3}, {"2 * -3", -6},
            {"10 % 4", 2}, {"-32 + 9", -23}, {"16 / 4", 4}, {"16 / 5", 3},
            {"16 / 4 + 3", 7}, {"32 * 6 - 7", 185}, {"32 * (6 - 7)", -32},
            {"191 + 10 * -3", 161}, {"100 + 396 * -1", -296}})));

struct boolean_test {
  const char* code;
  bool input;
  bool expected_return;
};

using bool_arithmetic_tests = ::testing::TestWithParam<boolean_test>;

TEST_P(bool_arithmetic_tests, boolean_arithmetic) {
  wn::testing::allocator allocator;
  wn::scripting::type_validator validator(&allocator);
  wn::containers::string str(&allocator);
  str += "Bool main(Bool b) { return ";
  str += GetParam().code;
  str += "; } ";
  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory(&allocator, wn::testing::k_system_data)
          .make_mapping(
              &allocator, wn::file_system::mapping_type::memory_backed);

  mapping->initialize_files({{"file.wns", str}});

  wn::scripting::jit_engine jit_engine(
      &allocator, &validator, mapping.get(), wn::logging::get_null_logger());
  EXPECT_EQ(wn::scripting::parse_error::ok, jit_engine.parse_file("file.wns"));

  wn::scripting::script_function<bool, bool> new_func;
  ASSERT_TRUE(jit_engine.get_function("main", new_func));

  EXPECT_EQ(GetParam().expected_return,
      jit_engine.invoke(new_func, GetParam().input));
}

INSTANTIATE_TEST_CASE_P(bool_tests, bool_arithmetic_tests,
    ::testing::ValuesIn(std::vector<boolean_test>({{"true", true, true},
        {"true == true", false, true}, {"false", true, false},
        {"2 == 3", true, false}, {"3 == 4 != b", false, false},
        {"3 == 4 != b", true, true}, {"b == true", true, true},
        {"b == true", false, false}, {"b == b", false, true},
        {"b == b", false, true}, {"b", false, false}, {"b", true, true},
        {"1 >= 3", false, false}, {"1 < 1", false, false},
        {"1 > 1", false, false}, {"1 >= 1", false, true},
        {"1 <= 1", false, true}, {"1 > (3 + 2)", false, false},
        {"(1 < 2) == (4 > 10)", false, false},
        {"(1 <= 2) == (b == false)", false, true},
        {"(1 <= 2) == (b == false)", true, false}})));

struct two_params_test {
  const char* code;
  std::vector<std::pair<std::pair<int32_t, int32_t>, int32_t>> cases;
};
using two_params_tests = ::testing::TestWithParam<two_params_test>;

TEST_P(two_params_tests, int_in_out_tests) {
  wn::testing::allocator allocator;
  wn::scripting::type_validator validator(&allocator);
  wn::containers::string str(&allocator, GetParam().code);
  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory(&allocator, wn::testing::k_system_data)
          .make_mapping(
              &allocator, wn::file_system::mapping_type::memory_backed);

  mapping->initialize_files({{"file.wns", str}});

  wn::scripting::jit_engine jit_engine(
      &allocator, &validator, mapping.get(), wn::logging::get_null_logger());
  EXPECT_EQ(wn::scripting::parse_error::ok, jit_engine.parse_file("file.wns"));

  wn::scripting::script_function<int32_t, int32_t, int32_t> new_func;
  ASSERT_TRUE(jit_engine.get_function("main", new_func));
  for (auto& test_case : GetParam().cases) {
    EXPECT_EQ(
        test_case.second, jit_engine.invoke(new_func, test_case.first.first,
                              test_case.first.second));
  }
}

// clang-format off
INSTANTIATE_TEST_CASE_P(
    simple_tests, two_params_tests,
    ::testing::ValuesIn(std::vector<two_params_test>({
      {"Int main(Int x, Int y) {  return x + y; }",
      {{{0, 4}, 4},{{1, 4}, 5},{{32, -10}, 22}}}
   })));
// clang-format on
