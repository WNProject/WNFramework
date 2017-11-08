// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNExecutableTest/inc/WNTestHarness.h"
#include "WNFileSystem/inc/WNFactory.h"
#include "WNFileSystem/inc/WNMapping.h"
#include "WNLogging/inc/WNBufferLogger.h"
#include "WNLogging/inc/WNLog.h"
#include "WNScripting/inc/WNCTranslator.h"
#include "WNScripting/test/inc/Common.h"

void flush_buffer(void* v, const char* bytes, size_t length,
    const wn::logging::color_element*, size_t) {
  wn::containers::string* s = static_cast<wn::containers::string*>(v);
  s->append(bytes, length);
}

using buffer_logger = wn::logging::buffer_logger<flush_buffer>;
using log_buff = wn::containers::string;

std::string get_file_data(wn::file_system::mapping_ptr& mapping,
    wn::containers::string_view file_name) {
  wn::file_system::result res;

  wn::file_system::file_ptr pt = mapping->open_file(file_name, res);
  if (!pt) {
    return "";
  }
  return std::string(pt->typed_data<char>(), pt->size());
}

TEST(c_translator, simple_c_translation) {
  wn::testing::allocator allocator;
  wn::scripting::type_validator validator(&allocator);
  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory(&allocator)
          .make_mapping(
              &allocator, wn::file_system::mapping_type::memory_backed);

  mapping->initialize_files({{"file.wns", "Void main() { return; }"}});

  wn::scripting::c_translator translator(&allocator, &validator, mapping.get(),
      mapping.get(), wn::logging::get_null_logger());
  EXPECT_EQ(
      wn::scripting::parse_error::ok, translator.translate_file("file.wns"));

  EXPECT_EQ(
      "void _Z3wns4mainEv() {\n"
      "return;\n"
      "}\n",
      get_file_data(mapping, "file.wns.c"));
}

struct source_pair {
  const char* first;
  const char* second;
};

using c_translator_direct_translation_test =
    ::testing::TestWithParam<std::vector<source_pair>>;

// The format of these tests is a vector of pairs of strings.
// This is entirely so that the test can be written as
// { "Int main() {", "int32_t main() {"},
// { "  return 4;",  "return 4;"},
// ...
// A \n in automatically appended to each string.
// If an empty string is encountered, no new-line is appended.
// If you want a line that is only a newline, you can insert a newline,
// a second one will not be added.
TEST_P(c_translator_direct_translation_test, translations) {
  wn::testing::allocator allocator;
  wn::scripting::type_validator validator(&allocator);
  wn::containers::string input_str(&allocator);
  wn::containers::string expected_output(&allocator);

  for (auto& a : GetParam()) {
    if (wn::memory::strlen(a.first) != 0) {
      input_str += a.first;
      if (a.first[0] != '\n') {
        input_str += '\n';
      }
    }
    if (wn::memory::strlen(a.second) != 0) {
      expected_output += a.second;
      if (a.second[0] != '\n') {
        expected_output += '\n';
      }
    }
  }
  log_buff buff(&allocator, "");
  buffer_logger logger(&buff);
  wn::logging::static_log<> log(&logger);

  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory(&allocator)
          .make_mapping(
              &allocator, wn::file_system::mapping_type::memory_backed);

  mapping->initialize_files({{"file.wns", input_str}});
  wn::scripting::c_translator translator(
      &allocator, &validator, mapping.get(), mapping.get(), log.log());
  EXPECT_EQ(wn::scripting::parse_error::ok,
      translator.translate_file_with_error("file.wns", true))
      << (log.log()->flush(), buff.c_str()) << "\n";
  EXPECT_EQ(std::string(expected_output.c_str()),
      get_file_data(mapping, "file.wns.c"))
      << get_file_data(mapping, "file.wns.c");
}

using c_translator_function_params =
    ::testing::TestWithParam<std::tuple<const char*, const char*, const char*>>;
TEST_P(c_translator_function_params, single_parameter) {
  wn::testing::allocator allocator;
  wn::scripting::type_validator validator(&allocator);
  wn::containers::string str(&allocator);
  str += std::get<0>(GetParam());
  str = str + " main(" + std::get<0>(GetParam()) + " x) { return x; }";

  wn::containers::string expected_str(&allocator);
  expected_str += std::get<1>(GetParam());
  expected_str = expected_str + " _Z3wns4mainE" + std::get<2>(GetParam()) +
                 std::get<2>(GetParam()) + "(" + std::get<1>(GetParam()) +
                 " x) {\nreturn x;\n}\n";

  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory(&allocator)
          .make_mapping(
              &allocator, wn::file_system::mapping_type::memory_backed);

  mapping->initialize_files({{"file.wns", str}});

  wn::scripting::c_translator translator(&allocator, &validator, mapping.get(),
      mapping.get(), wn::logging::get_null_logger());
  EXPECT_EQ(
      wn::scripting::parse_error::ok, translator.translate_file("file.wns"));
  EXPECT_EQ(
      std::string(expected_str.c_str()), get_file_data(mapping, "file.wns.c"));
}

INSTANTIATE_TEST_CASE_P(parameter_tests, c_translator_function_params,
    ::testing::ValuesIn(
        std::vector<std::tuple<const char*, const char*, const char*>>(
            {std::make_tuple("Int", "int32_t", "l"),
                std::make_tuple("Float", "float", "f"),
                std::make_tuple("Bool", "bool", "b"),
                std::make_tuple("Char", "char", "c")})));

TEST(c_translator, multiple_c_functions) {
  wn::testing::allocator allocator;
  wn::scripting::type_validator validator(&allocator);
  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory(&allocator)
          .make_mapping(
              &allocator, wn::file_system::mapping_type::memory_backed);

  mapping->initialize_files({{"file.wns",
      "Void main() { return; }\n"
      "Void foo() { return; }\n"}});

  wn::scripting::c_translator translator(&allocator, &validator, mapping.get(),
      mapping.get(), wn::logging::get_null_logger());
  EXPECT_EQ(
      wn::scripting::parse_error::ok, translator.translate_file("file.wns"));
  EXPECT_EQ(
      "void _Z3wns4mainEv() {\n"
      "return;\n"
      "}\n"
      "\n"
      "void _Z3wns3fooEv() {\n"
      "return;\n"
      "}\n",
      get_file_data(mapping, "file.wns.c"));
}

// Make sure multiple returns get combined into just 1.
TEST(c_translator, multiple_returns) {
  wn::testing::allocator allocator;
  wn::scripting::type_validator validator(&allocator);

  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory(&allocator)
          .make_mapping(
              &allocator, wn::file_system::mapping_type::memory_backed);

  mapping->initialize_files({{"file.wns", "Void main() { return; return; }"}});

  wn::scripting::c_translator translator(&allocator, &validator, mapping.get(),
      mapping.get(), wn::logging::get_null_logger());
  EXPECT_EQ(
      wn::scripting::parse_error::ok, translator.translate_file("file.wns"));
  EXPECT_EQ(
      "void _Z3wns4mainEv() {\n"
      "return;\n"
      "}\n",
      get_file_data(mapping, "file.wns.c"));
}

class c_int_params : public ::testing::TestWithParam<const char*> {};

TEST_P(c_int_params, int_return) {
  wn::testing::allocator allocator;
  wn::scripting::type_validator validator(&allocator);
  wn::containers::string str(&allocator);
  str += "Int main() { return ";
  str += GetParam();
  str += "; } ";

  wn::containers::string expected(&allocator);
  expected +=
      "int32_t _Z3wns4mainEl() {\n"
      "return ";
  expected += GetParam();
  expected += ";\n}\n";
  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory(&allocator)
          .make_mapping(
              &allocator, wn::file_system::mapping_type::memory_backed);

  mapping->initialize_files({{"file.wns", str}});

  wn::scripting::c_translator translator(&allocator, &validator, mapping.get(),
      mapping.get(), wn::logging::get_null_logger());
  EXPECT_EQ(
      wn::scripting::parse_error::ok, translator.translate_file("file.wns"));
  EXPECT_EQ(
      std::string(expected.c_str()), get_file_data(mapping, "file.wns.c"));
}

INSTANTIATE_TEST_CASE_P(int_tests, c_int_params,
    ::testing::Values("0", "-1", "-32", "-4096", "2147483647", "-2147483648"));

struct arithmetic_operations {
  const char* source;
  const char* dest;
};

using c_arith_params = ::testing::TestWithParam<arithmetic_operations>;

TEST_P(c_arith_params, binary_arithmetic) {
  wn::testing::allocator allocator;
  wn::scripting::type_validator validator(&allocator);
  wn::containers::string str(&allocator);
  str += "Int main() { return ";
  str += GetParam().source;
  str += "; } ";

  wn::containers::string expected(&allocator);
  expected +=
      "int32_t _Z3wns4mainEl() {\n"
      "return ";
  expected += GetParam().dest;
  expected += ";\n}\n";
  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory(&allocator)
          .make_mapping(
              &allocator, wn::file_system::mapping_type::memory_backed);

  mapping->initialize_files({{"file.wns", str}});

  wn::scripting::c_translator translator(&allocator, &validator, mapping.get(),
      mapping.get(), wn::logging::get_null_logger());
  EXPECT_EQ(
      wn::scripting::parse_error::ok, translator.translate_file("file.wns"));
  EXPECT_EQ(
      std::string(expected.c_str()), get_file_data(mapping, "file.wns.c"));
}

INSTANTIATE_TEST_CASE_P(int_int_tests, c_arith_params,
    ::testing::ValuesIn(
        std::vector<arithmetic_operations>({{"1 + 2", "(1 + 2)"},
            {"10 - 20", "(10 - 20)"}, {"-32 * 0", "(-32 * 0)"},
            {"-32 % 22", "(-32 % 22)"}, {"-32 + 4 * 10", "(-32 + (4 * 10))"},
            {"27 / 4 + 8 * 3", "((27 / 4) + (8 * 3))"}})));

using c_bool_params = ::testing::TestWithParam<arithmetic_operations>;

TEST_P(c_bool_params, boolean_arithmetic) {
  wn::testing::allocator allocator;
  wn::scripting::type_validator validator(&allocator);
  wn::containers::string str(&allocator);
  str += "Bool wn_main(Bool b) { return ";
  str += GetParam().source;
  str += "; } ";

  wn::containers::string expected(&allocator);
  expected +=
      "bool _Z3wns7wn_mainEbb(bool b) {\n"
      "return ";
  expected += GetParam().dest;
  expected += ";\n}\n";
  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory(&allocator)
          .make_mapping(
              &allocator, wn::file_system::mapping_type::memory_backed);

  mapping->initialize_files({{"file.wns", str}});

  wn::scripting::c_translator translator(&allocator, &validator, mapping.get(),
      mapping.get(), wn::logging::get_null_logger());
  EXPECT_EQ(
      wn::scripting::parse_error::ok, translator.translate_file("file.wns"));
  EXPECT_EQ(
      std::string(expected.c_str()), get_file_data(mapping, "file.wns.c"));
}

INSTANTIATE_TEST_CASE_P(int_int_tests, c_bool_params,
    ::testing::ValuesIn(std::vector<arithmetic_operations>(
        {{"true", "true"}, {"true == true", "(true == true)"},
            {"false", "false"}, {"2 == 3", "(2 == 3)"},
            {"3 == 4 != b", "((3 == 4) != b)"}, {"b == true", "(b == true)"},
            {"b == b", "(b == b)"}, {"1 >= 3", "(1 >= 3)"},
            {"1 < 1", "(1 < 1)"}, {"1 > 1", "(1 > 1)"}, {"1 >= 1", "(1 >= 1)"},
            {"1 <= 1", "(1 <= 1)"}, {"1 > (3 + 2)", "(1 > (3 + 2))"},
            {"(1 < 2) == (4 > 10)", "((1 < 2) == (4 > 10))"},
            {"(1 <= 2) == (b == false)", "((1 <= 2) == (b == false))"}})));
