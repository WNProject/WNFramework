// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/inc/WNDynamicArray.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNScripting/inc/WNFactory.h"
#include "WNScripting/inc/WNJITEngine.h"
#include "WNScripting/test/inc/Common.h"
#include "WNTesting/inc/WNTestHarness.h"

TEST(jit_engine, creation) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::scripting::file_based_manager manager(&allocator);
  wn::scripting::jit_engine jit_engine(&allocator, &manager,
                                       WNLogging::get_null_logger());
}

TEST(jit_engine, basic_parsing) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::scripting::test_file_manager manager(
      &allocator, {{"file.wns", "Void main() { return; }"},
                   {"file2.wns",
                    "Void foo() { return; } \n"
                    "Void bar() { return; }"}});
  wn::scripting::jit_engine jit_engine(&allocator, &manager,
                                       WNLogging::get_null_logger());
  EXPECT_EQ(wn::scripting::parse_error::ok, jit_engine.parse_file("file.wns"));
  EXPECT_EQ(wn::scripting::parse_error::ok, jit_engine.parse_file("file2.wns"));

  wn::scripting::engine::void_func main = jit_engine.get_function("main");
  wn::scripting::engine::void_func foo = jit_engine.get_function("foo");
  wn::scripting::engine::void_func bar = jit_engine.get_function("bar");

  EXPECT_NE(wn_nullptr, main);
  EXPECT_NE(wn_nullptr, foo);
  EXPECT_NE(wn_nullptr, bar);

  // No returns so lets just see if we crash trying to call.
  (*main)();
  (*foo)();
  (*bar)();
}

// Make sure that multiple returns get coalesced
TEST(jit_engine, multiple_returns) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::scripting::test_file_manager manager(
      &allocator, {{"file.wns", "Void main() { return; return; }"}});
  wn::scripting::jit_engine jit_engine(&allocator, &manager,
                                       WNLogging::get_null_logger());
  EXPECT_EQ(wn::scripting::parse_error::ok, jit_engine.parse_file("file.wns"));

  wn::scripting::engine::void_func main = jit_engine.get_function("main");

  EXPECT_NE(wn_nullptr, main);

  // No returns so lets just see if we crash trying to call.
  (*main)();
}

TEST(jit_engine, parse_error) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::scripting::test_file_manager manager(&allocator,
                                           {{"file.wns", "Int main"}});
  wn::scripting::jit_engine jit_engine(&allocator, &manager,
                                       WNLogging::get_null_logger());
  EXPECT_EQ(wn::scripting::parse_error::parse_failed,
            jit_engine.parse_file("file.wns"));
}

struct int_test {
  const char* val;
  wn_int32 number;
};

class jit_int_params : public ::testing::TestWithParam<int_test> {};

TEST_P(jit_int_params, int_return) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::containers::string str(&allocator);
  str += "Int main() { return ";
  str += GetParam().val;
  str += "; } ";

  wn::containers::string expected(&allocator);
  wn::scripting::test_file_manager manager(&allocator, {{"file.wns", str}});

  wn::scripting::jit_engine jit_engine(&allocator, &manager,
                                       WNLogging::get_null_logger());
  EXPECT_EQ(wn::scripting::parse_error::ok, jit_engine.parse_file("file.wns"));

  wn::scripting::engine::void_func main = jit_engine.get_function("main");
  int (*new_func)() = reinterpret_cast<int (*)()>(main);
  EXPECT_EQ(GetParam().number, (*new_func)());
}

TEST_P(jit_int_params, int_passthrough) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::containers::string str(&allocator);
  str += "Int main(Int x) { return x";
  str += "; } ";

  wn::containers::string expected(&allocator);
  wn::scripting::test_file_manager manager(&allocator, {{"file.wns", str}});

  wn::scripting::jit_engine jit_engine(&allocator, &manager,
                                       WNLogging::get_null_logger());
  EXPECT_EQ(wn::scripting::parse_error::ok, jit_engine.parse_file("file.wns"));

  wn::scripting::engine::void_func main = jit_engine.get_function("main");
  int (*new_func)(int) = reinterpret_cast<int (*)(int)>(main);
  EXPECT_EQ(GetParam().number, (*new_func)(GetParam().number));
}

INSTANTIATE_TEST_CASE_P(
    int_tests, jit_int_params,
    ::testing::Values(int_test({"0", 0}), int_test({"-1", -1}),
                      int_test({"-32", -32}), int_test({"-4096", -4096}),
                      int_test({"2147483647", WN_INT32_MAX}),
                      int_test({"-2147483648", (WN_INT32_MIN)})));

struct int_binary_test {
  const char* code;
  wn_int32 expected_return;
};

using jit_binary_arithmetic = ::testing::TestWithParam<int_binary_test>;

TEST_P(jit_binary_arithmetic, simple_operations) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::containers::string str(&allocator);
  str += "Int main() { return ";
  str += GetParam().code;
  str += "; } ";

  wn::containers::string expected(&allocator);
  wn::scripting::test_file_manager manager(&allocator, {{"file.wns", str}});

  wn::scripting::jit_engine jit_engine(&allocator, &manager,
                                       WNLogging::get_null_logger());
  EXPECT_EQ(wn::scripting::parse_error::ok, jit_engine.parse_file("file.wns"));

  wn::scripting::engine::void_func main = jit_engine.get_function("main");
  int (*new_func)() = reinterpret_cast<int (*)()>(main);
  EXPECT_EQ(GetParam().expected_return, (*new_func)());
}

INSTANTIATE_TEST_CASE_P(
    int_arithmetic_tests, jit_binary_arithmetic,
    ::testing::ValuesIn(
        wn::containers::dynamic_array<int_binary_test>({{"1 + 2", 3},
                                                        {"2 * -3", -6},
                                                        {"10 % 4", 2},
                                                        {"-32 + 9", -23},
                                                        {"16 / 4", 4},
                                                        {"16 / 5", 3},
                                                        {"16 / 4 + 3", 7},
                                                        {"32 * 6 - 7", 185},
                                                        {"32 * (6 - 7)", -32},
                                                        {"191 + 10 * -3", 161},
                                                        {"100 + 396 * -1", -296}})));


struct boolean_test {
  const char* code;
  bool input;
  bool expected_return;
};

using bool_arithmetic_tests = ::testing::TestWithParam<boolean_test>;

TEST_P(bool_arithmetic_tests, boolean_arithmetic) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::containers::string str(&allocator);
  str += "Bool main(Bool b) { return ";
  str += GetParam().code;
  str += "; } ";

  wn::scripting::test_file_manager manager(&allocator, {{"file.wns", str}});

  wn::scripting::jit_engine jit_engine(&allocator, &manager,
                                       WNLogging::get_null_logger());
  EXPECT_EQ(wn::scripting::parse_error::ok, jit_engine.parse_file("file.wns"));

  wn::scripting::engine::void_func main = jit_engine.get_function("main");
  wn_bool (*new_func)(wn_bool) = reinterpret_cast<wn_bool (*)(wn_bool)>(main);
  EXPECT_EQ(GetParam().expected_return, (*new_func)(GetParam().input));
}

INSTANTIATE_TEST_CASE_P(
    bool_tests, bool_arithmetic_tests,
    ::testing::ValuesIn(wn::containers::dynamic_array<boolean_test>(
        {{"true", true, true},
         {"true == true", false, true},
         {"false", true, false},
         {"2 == 3", true, false},
         {"3 == 4 != b", false, false},
         {"3 == 4 != b", true, true},
         {"b == true", true, true},
         {"b == true", false, false},
         {"b == b", false, true},
         {"b == b", false, true},
         {"b", false, false},
         {"b", true, true},
         {"1 >= 3", false, false},
         {"1 < 1", false, false},
         {"1 > 1", false, false},
         {"1 >= 1", false, true},
         {"1 <= 1", false, true},
         {"1 > (3 + 2)", false, false},
         {"(1 < 2) == (4 > 10)", false, false},
         {"(1 <= 2) == (b == false)", false, true},
         {"(1 <= 2) == (b == false)", true, false}})));

//TODO(awoloszyn) Arithmetic comparisons