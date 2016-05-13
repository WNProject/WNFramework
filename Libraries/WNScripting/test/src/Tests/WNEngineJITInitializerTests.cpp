// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/inc/WNDynamicArray.h"
#include "WNFileSystem/inc/WNFactory.h"
#include "WNFileSystem/inc/WNMapping.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNScripting/inc/WNFactory.h"
#include "WNScripting/inc/WNJITEngine.h"
#include "WNScripting/test/inc/Common.h"
#include "WNTesting/inc/WNTestHarness.h"

TEST(jit_engine, creation) {
  wn::testing::allocator allocator;
  wn::scripting::type_validator validator(&allocator);
  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory().make_mapping(
          wn::file_system::mapping_type::memory_backed, &allocator);

  wn::scripting::jit_engine jit_engine(
      &validator, &allocator, mapping.get(), WNLogging::get_null_logger());
}

TEST(jit_engine, basic_parsing) {
  wn::testing::allocator allocator;
  wn::scripting::type_validator validator(&allocator);

  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory().make_mapping(
          wn::file_system::mapping_type::memory_backed, &allocator);

  mapping->initialize_files(
      {{"file.wns", "Void main() { return; }"}, {"file2.wns",
                                                    "Void foo() { return; } \n"
                                                    "Void bar() { return; }"}});
  wn::scripting::jit_engine jit_engine(
      &validator, &allocator, mapping.get(), WNLogging::get_null_logger());
  EXPECT_EQ(wn::scripting::parse_error::ok, jit_engine.parse_file("file.wns"));
  EXPECT_EQ(wn::scripting::parse_error::ok, jit_engine.parse_file("file2.wns"));

  wn::scripting::engine::void_func main;
  EXPECT_TRUE(jit_engine.get_function_pointer("main", main));
  wn::scripting::engine::void_func foo;
  EXPECT_TRUE(jit_engine.get_function_pointer("foo", foo));
  wn::scripting::engine::void_func bar;
  EXPECT_TRUE(jit_engine.get_function_pointer("bar", bar));

  ASSERT_NE(nullptr, main);
  ASSERT_NE(nullptr, foo);
  ASSERT_NE(nullptr, bar);

  // No returns so lets just see if we crash trying to call.
  (*main)();
  (*foo)();
  (*bar)();
}

// Make sure that multiple returns get coalesced
TEST(jit_engine, multiple_returns) {
  wn::testing::allocator allocator;
  wn::scripting::type_validator validator(&allocator);

  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory().make_mapping(
          wn::file_system::mapping_type::memory_backed, &allocator);

  mapping->initialize_files({{"file.wns", "Void main() { return; return; }"}});
  wn::scripting::jit_engine jit_engine(
      &validator, &allocator, mapping.get(), WNLogging::get_null_logger());
  EXPECT_EQ(wn::scripting::parse_error::ok, jit_engine.parse_file("file.wns"));

  wn::scripting::engine::void_func main;
  ASSERT_TRUE(jit_engine.get_function_pointer("main", main));

  EXPECT_NE(nullptr, main);

  // No returns so lets just see if we crash trying to call.
  (*main)();
}

TEST(jit_engine, parse_error) {
  wn::testing::allocator allocator;
  wn::scripting::type_validator validator(&allocator);

  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory().make_mapping(
          wn::file_system::mapping_type::memory_backed, &allocator);

  mapping->initialize_files({{"file.wns", "Int main"}});
  wn::scripting::jit_engine jit_engine(
      &validator, &allocator, mapping.get(), WNLogging::get_null_logger());
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
      wn::file_system::factory().make_mapping(
          wn::file_system::mapping_type::memory_backed, &allocator);

  mapping->initialize_files({{"file.wns", str}});

  wn::scripting::jit_engine jit_engine(
      &validator, &allocator, mapping.get(), WNLogging::get_null_logger());
  EXPECT_EQ(wn::scripting::parse_error::ok, jit_engine.parse_file("file.wns"));

  int32_t (*new_func)();
  ASSERT_TRUE(jit_engine.get_function_pointer("main", new_func));
  EXPECT_EQ(GetParam().number, (*new_func)());
}

TEST_P(jit_int_params, int_passthrough) {
  wn::testing::allocator allocator;
  wn::scripting::type_validator validator(&allocator);
  wn::containers::string str(&allocator);
  str += "Int main(Int x) { return x; }";

  wn::containers::string expected(&allocator);
  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory().make_mapping(
          wn::file_system::mapping_type::memory_backed, &allocator);

  mapping->initialize_files({{"file.wns", str}});

  wn::scripting::jit_engine jit_engine(
      &validator, &allocator, mapping.get(), WNLogging::get_null_logger());
  EXPECT_EQ(wn::scripting::parse_error::ok, jit_engine.parse_file("file.wns"));

  int32_t (*new_func)(int32_t);
  ASSERT_TRUE(jit_engine.get_function_pointer("main", new_func));
  EXPECT_EQ(GetParam().number, (*new_func)(GetParam().number));
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
      wn::file_system::factory().make_mapping(
          wn::file_system::mapping_type::memory_backed, &allocator);

  mapping->initialize_files({{"file.wns", str}});

  wn::scripting::jit_engine jit_engine(
      &validator, &allocator, mapping.get(), WNLogging::get_null_logger());
  EXPECT_EQ(wn::scripting::parse_error::ok, jit_engine.parse_file("file.wns"));

  int32_t (*new_func)();
  ASSERT_TRUE(jit_engine.get_function_pointer("main", new_func));
  EXPECT_EQ(GetParam().expected_return, (*new_func)());
}

INSTANTIATE_TEST_CASE_P(int_arithmetic_tests, jit_binary_arithmetic,
    ::testing::ValuesIn(std::vector<int_binary_test>(
        {{"1 + 2", 3}, {"2 * -3", -6}, {"10 % 4", 2}, {"-32 + 9", -23},
            {"16 / 4", 4}, {"16 / 5", 3}, {"16 / 4 + 3", 7},
            {"32 * 6 - 7", 185}, {"32 * (6 - 7)", -32}, {"191 + 10 * -3", 161},
            {"100 + 396 * -1", -296}})));

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
      wn::file_system::factory().make_mapping(
          wn::file_system::mapping_type::memory_backed, &allocator);

  mapping->initialize_files({{"file.wns", str}});

  wn::scripting::jit_engine jit_engine(
      &validator, &allocator, mapping.get(), WNLogging::get_null_logger());
  EXPECT_EQ(wn::scripting::parse_error::ok, jit_engine.parse_file("file.wns"));

  bool (*new_func)(bool);
  ASSERT_TRUE(jit_engine.get_function_pointer("main", new_func));

  EXPECT_EQ(GetParam().expected_return, (*new_func)(GetParam().input));
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
  std::vector<std::pair<std::pair<uint32_t, uint32_t>, uint32_t>> cases;
};
using two_params_tests = ::testing::TestWithParam<two_params_test>;

TEST_P(two_params_tests, int_in_out_tests) {
  wn::testing::allocator allocator;
  wn::scripting::type_validator validator(&allocator);
  wn::containers::string str(GetParam().code, &allocator);
  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory().make_mapping(
          wn::file_system::mapping_type::memory_backed, &allocator);

  mapping->initialize_files({{"file.wns", str}});

  wn::scripting::jit_engine jit_engine(
      &validator, &allocator, mapping.get(), WNLogging::get_null_logger());
  EXPECT_EQ(wn::scripting::parse_error::ok, jit_engine.parse_file("file.wns"));

  int32_t (*new_func)(int32_t, int32_t);
  ASSERT_TRUE(jit_engine.get_function_pointer("main", new_func));
  for (auto& test_case : GetParam().cases) {
    EXPECT_EQ(test_case.second,
        (*new_func)(test_case.first.first, test_case.first.second));
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

struct integer_test {
  const char* code;
  std::vector<std::pair<int32_t, int32_t>> cases;
};

using integer_tests = ::testing::TestWithParam<integer_test>;

TEST_P(integer_tests, int_in_out_tests) {
  wn::testing::allocator allocator;
  wn::scripting::type_validator validator(&allocator);
  wn::containers::string str(GetParam().code, &allocator);
  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory().make_mapping(
          wn::file_system::mapping_type::memory_backed, &allocator);

  mapping->initialize_files({{"file.wns", str}});

  wn::scripting::jit_engine jit_engine(
      &validator, &allocator, mapping.get(), WNLogging::get_null_logger());
  EXPECT_EQ(wn::scripting::parse_error::ok, jit_engine.parse_file("file.wns"));

  int32_t (*new_func)(int32_t);
  ASSERT_TRUE(jit_engine.get_function_pointer("main", new_func));
  for (auto& test_case : GetParam().cases) {
    EXPECT_EQ(test_case.second, (*new_func)(test_case.first));
  }
}

// clang-format off
INSTANTIATE_TEST_CASE_P(
    comparison_tests, integer_tests,
    ::testing::ValuesIn(std::vector<integer_test>({
      {"Int main(Int x) { if (x > 3) { return 2; } return 1;}",
          {{0, 1}, {-1, 1}, {2, 1}, {3, 1}, {4, 2}, {50, 2}}},
         {"Int main(Int x) { if (x > 3) { return 2; } else { return 3; } "
          "return 4;}",
          {{0, 3}, {-1, 3}, {2, 3}, {3, 3}, {4, 2}, {50, 2}}},
         {"Int main(Int x) { if (x > 3) { return 2; } else if (x < 2) { "
          "return 1; } return 3;}",
          {{0, 1}, {-1, 1}, {2, 3}, {3, 3}, {4, 2}, {50, 2}}},
         {"Int main(Int x) { if (x > 3) { return 2; } else if (x < 2) { "
          "} return 3;}",
          {{0, 3}, {-1, 3}, {2, 3}, {3, 3}, {4, 2}, {50, 2}}},
         {"Int main(Int x) { if (x > 3) { } else if (x < 2) { return 4;"
          "} return 3;}",
          {{0, 4}, {-1, 4}, {2, 3}, {3, 3}, {4, 3}, {50, 3}}}
   })));

INSTANTIATE_TEST_CASE_P(
    declaration_tests, integer_tests,
    ::testing::ValuesIn(std::vector<integer_test>({
      {"Int main(Int x) { Int y = x; return y; }",
          {{0, 0}, {-1, -1}, {2, 2}, {3, 3}, {4, 4}, {50, 50}}},
      {"Int main(Int x) { Bool b = x == 3; if (b) { return 3; }"
            "return 4; }",
          {{0, 4}, {-1, 4}, {2, 4}, {3, 3}, {4, 4}, {50, 4}}},
      {"Int main(Int x) { Int y = x; Int z = y; return z + y; }",
          {{0, 0}, {-1, -2}, {2, 4}, {3, 6}, {4, 8}, {50, 100}}},
   })));

INSTANTIATE_TEST_CASE_P(
    assignment_tests, integer_tests,
    ::testing::ValuesIn(std::vector<integer_test>({
      {"Int main(Int x) { Int y = 0; y = x; return y; }",
        {{0, 0}, {-1, -1}, {2, 2}, {3, 3}, {4, 4}, {50, 50}}},
      {"Int main(Int x) { Bool b = false; b = x == 3; if (b) { return 3; }"
            "return 4; }",
        {{0, 4}, {-1, 4}, {2, 4}, {3, 3}, {4, 4}, {50, 4}}},
      {"Int main(Int x) { Int y = 0; if (x > 3) { y = x; } else "
            "{ y = 4; } return y; }",
        {{0, 4}, {-1, 4}, {2, 4}, {3, 4}, {8, 8}, {100, 100}}},
   })));

INSTANTIATE_TEST_CASE_P(
    struct_access_tests, integer_tests,
    ::testing::ValuesIn(std::vector<integer_test>({
      {"struct Foo { Int x = 0; } Int main(Int x) { Foo f = Foo(); f.x = x; return f.x; }",
        {{0, 0}, {-1, -1}, {2, 2}, {3, 3}, {4, 4}, {50, 50}}},
      {"struct Foo { Int x = 0; Int y = 0; } Int main(Int x) { Foo f = Foo(); f.y = x; return f.y; }",
        {{0, 0}, {-1, -1}, {2, 2}, {3, 3}, {4, 4}, {50, 50}}},
      {"struct Foo {"
       "  Int x = 0; Int y = 0;"
       "}"
       "Int main(Int x) { "
       "  Foo f = Foo();"
       "  f.x = x;"
       "  f.y = x * 2;"
       "  return f.x + f.y;"
      "}", {{0, 0}, {-1, -3}, {2, 6}, {50, 150}}},
  })));

INSTANTIATE_TEST_CASE_P(
    simple_function_call_tests, integer_tests,
    ::testing::ValuesIn(std::vector<integer_test>({
      {"Int foo() { return 4; }"
       "Int main(Int x) { return x + foo(); }",
        {{0, 4}, {-1, 3}, {2, 6}, {3, 7}, {4, 8}, {50, 54}}},
      {"Int foo(Int x) { return 4 + x; }"
       "Int main(Int x) { return foo(x); }",
        {{0, 4}, {-1, 3}, {2, 6}, {3, 7}, {4, 8}, {50, 54}}},
      {"Int foo(Int x, Int y) { return x + y; }"
       "Int main(Int x) { return foo(x, x); }",
        {{0, 0}, {-1, -2}, {2, 4}, {3, 6}, {4, 8}, {50, 100}}},

})));

INSTANTIATE_TEST_CASE_P(
    constructor_tests, integer_tests,
    ::testing::ValuesIn(std::vector<integer_test>({
      {"struct Foo { Int x = 0; } Int main(Int x) { Foo f = Foo(); return x + f.x; }",
        {{0, 0}, {-1, -1}, {2, 2}, {3, 3}, {4, 4}, {50, 50}}},
      {"struct Foo { Int x = 10; } Int main(Int x) { Foo f = Foo(); return x + f.x; }",
        {{0, 10}, {-1, 9}, {2, 12}, {3, 13}, {4, 14}, {50, 60}}},
      {"struct Foo { Int x = 10; Int y = 4; } Int main(Int x) { Foo f = Foo(); return f.y + x + f.x; }",
        {{0, 14}, {-1, 13}, {2, 16}, {3, 17}, {4, 18}, {50, 64}}},
})));

INSTANTIATE_TEST_CASE_P(
    scope_tests, integer_tests,
    ::testing::ValuesIn(std::vector<integer_test>({
      {"Int main(Int x) { Int y = x; { return 5; } return y;}",
        {{0, 5}, {-1, 5}}},
      {"Int main(Int x) { Int y = x; { return y; } return 3;}",
        {{0, 0}, {-1, -1}, {2, 2}}},
      {"Int main(Int x) { Int y = x; { y = 3; { return y; } } return 3;}",
        {{0, 3}, {-1, 3}, {2, 3}}},
})));


INSTANTIATE_TEST_CASE_P(
    temporary_struct_test, integer_tests,
    ::testing::ValuesIn(std::vector<integer_test>({
      {"struct Foo { Int x = 4; } Int main(Int x) { return Foo().x + x; }",
        {{0, 4}, {-1, 3}}},
})));

INSTANTIATE_TEST_CASE_P(
    struct_cleanup_tests, integer_tests,
    ::testing::ValuesIn(std::vector<integer_test>({
      {"struct Foo { Int x = 4; }"
      "Int main(Int x) { Int r = 3; { Foo f = Foo(); r = f.x + x; } return r; }",
        {{0, 4}, {-1, 3}}},
})));

INSTANTIATE_TEST_CASE_P(
    empty_expression_test, integer_tests,
    ::testing::ValuesIn(std::vector<integer_test>({
      {"Int main(Int x) { x + 4; x - 3; return x; }",
      {{0, 0}, {-1, -1}, {10, 10}}},
})));

// clang-format on
