// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNLogging/inc/WNBufferLogger.h"
#include "WNScripting/test/inc/Common.h"
#include "WNScripting/inc/WNCTranslator.h"
#include "WNTesting/inc/WNTestHarness.h"

void flush_buffer(wn_void* v, const wn_char* bytes, wn_size_t length,
                         const std::vector<WNLogging::WNLogColorElement>&) {
  wn::containers::string* s = static_cast<wn::containers::string*>(v);
  s->append(bytes, length);
}

using buffer_logger = WNLogging::WNBufferLogger<flush_buffer>;
using log_buff = wn::containers::string;


TEST(c_translator, simple_c_translation) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::scripting::test_file_manager manager(
      &allocator, {{"file.wns", "Void main() { return; }"}});

  wn::scripting::c_translator translator(&allocator, &manager,
                                         WNLogging::get_null_logger());
  EXPECT_EQ(wn::scripting::parse_error::ok,
            translator.translate_file("file.wns"));
  EXPECT_EQ(
      "void main() {\n"
      "return;\n"
      "}\n",
      std::string(manager.get_file("file.wns.c")->data()));
}

struct source_pair {
  const char* first;
  const char* second;
};

using c_translator_direct_translation_test =
    ::testing::TestWithParam<wn::containers::dynamic_array<source_pair>>;

// The format of these tests is a dynamic_array of pairs of strings.
// This is entirely so that the test can be written as
// { "Int main() {", "wn_int32 main() {"},
// { "  return 4;",  "return 4;"},
// ...
// A \n in automatically appended to each string.
// If an empty string is encountered, no new-line is appended.
// If you want a line that is only a newline, you can insert a newline,
// a second one will not be added.
TEST_P(c_translator_direct_translation_test, translations) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::containers::string input_str(&allocator);
  wn::containers::string expected_output(&allocator);

  for(auto& a: GetParam()) {
      if(wn::memory::strlen(a.first) != 0) {
        input_str += a.first;
        if (a.first[0] != '\n') {
          input_str += '\n';
        }
      }
      if(wn::memory::strlen(a.second) != 0) {
        expected_output += a.second;
        if (a.second[0] != '\n') {
          expected_output += '\n';
        }
      }
  }
  log_buff buff(&allocator);
  buffer_logger logger(&buff);
  WNLogging::WNLog log(&logger);

  wn::scripting::test_file_manager manager(&allocator, {{"file.wns", input_str}});
  wn::scripting::c_translator translator(&allocator, &manager,
                                         &log);
  EXPECT_EQ(wn::scripting::parse_error::ok,
            translator.translate_file("file.wns")) << buff;
  EXPECT_EQ(std::string(expected_output.c_str()),
            std::string(manager.get_file("file.wns.c")->data()));
}

// clang-format off
INSTANTIATE_TEST_CASE_P(
    if_statement_tests, c_translator_direct_translation_test,
    ::testing::ValuesIn(
        wn::containers::dynamic_array<wn::containers::dynamic_array<source_pair>>({
            {
              {"Int main(Int x) {",     "wn_int32 main(wn_int32 x) {"  },
              {"  if (x == 3) {",       "if ((x == 3)) {"              },
              {"    return 7;",         "return 7;"                    },
              {"  }",                   "}"                            },
              {"  return 9;",           "return 9;"                    },
              {"}",                     "}"                            },
            },
            {
              {"Int main(Int x) {",     "wn_int32 main(wn_int32 x) {"  },
              {"  if (x == 3) {",       "if ((x == 3)) {"              },
              {"    return 7;",         "return 7;"                    },
              {"  } else {",            "} else {"                     },
              {"    return 10;",        "return 10;"                   },
              {"  }",                   "}"                            },
              {"  return 9;",           ""                             },
              {"}",                     "}"                            },
            },
            {
              {"Int main(Int x) {",     "wn_int32 main(wn_int32 x) {"  },
              {"  if (x == 4) {",       "if ((x == 4)) {"              },
              {"    if (x > 3) {",      "if ((x > 3)) {"               },
              {"      return 9;",       "return 9;"                    },
              {"    }",                 "}"                            },
              {"    return 7;",         "return 7;"                    },
              {"  } else {",            "} else {"                     },
              {"    return 10;",        "return 10;"                   },
              {"  }",                   "}"                            },
              {"  return 9;",           "}"                            },
              {"}",                     ""                             },
            },
            {
              {"Int main(Int x) {",        "wn_int32 main(wn_int32 x) {" },
              {"  if (x == 4) {",          "if ((x == 4)) {"             },
              {"    if (x > 3) {",         "if ((x > 3)) {"              },
              {"      return 9;",          "return 9;"                   },
              {"    } else if (x < 4) {",  "} else if ((x < 4)) {"       },
              {"      return 32;",         "return 32;"                  },
              {"    } else {",             "} else {"                    },
              {"      return 9;",          "return 9;"                   },
              {"    }",                    "}"                           },
              {"    return 7;",            "} else {"                    },
              {"  } else {",               "return 10;"                  },
              {"    return 10;",           "}"                           },
              {"  }",                      "}"                           },
              {"  return 9;",              ""                            },
              {"}",                        ""                            },
            }
})));
// clang-format on

// clang-format off
INSTANTIATE_TEST_CASE_P(
    declaration_tests, c_translator_direct_translation_test,
    ::testing::ValuesIn(
        wn::containers::dynamic_array<wn::containers::dynamic_array<source_pair>>({
          {
            {"Int main(Int x) {",     "wn_int32 main(wn_int32 x) {"  },
            {"  Int y = x;",          "wn_int32 y = x;"              },
            {"  Bool b = y == 4;",    "wn_bool b = (y == 4);"        },
            {"  if (b) {    ",        "if (b) {"                     },
            {"     return 3;",        "return 3;"                    },
            {"  }",                   "}"                            },
            {"  return 4;",           "return 4;"                    },
            {"}",                     "}"                            },
          }
})));
// clang-format on

// clang-format off
INSTANTIATE_TEST_CASE_P(
    assignment_tests, c_translator_direct_translation_test,
    ::testing::ValuesIn(
        wn::containers::dynamic_array<wn::containers::dynamic_array<source_pair>>({
          {
            {"Int main(Int x) {",     "wn_int32 main(wn_int32 x) {"  },
            {"  Int y = 0;",          "wn_int32 y = 0;"              },
            {"  x = y;",              "x = y;"                       },
            {"  return x;",           "return x;"                    },
            {"}",                     "}"                            },
          },
          {
            {"Bool main(Int x) {",    "wn_bool main(wn_int32 x) {"   },
            {"  Bool b = false;",     "wn_bool b = false;"           },
            {"  b = x == 4;",         "b = (x == 4);"                },
            {"  return b;",           "return b;"                    },
            {"}",                     "}"                            },
          },
          {
            {"Int main(Bool x) {",    "wn_int32 main(wn_bool x) {"   },
            {"  Int y = 4;",          "wn_int32 y = 4;"              },
            {"  if (x) {",            "if (x) {"                     },
            {"    y = 10;",           "y = 10;"                      },
            {"  } else {",            "} else {"                     },
            {"    y = 4;",            "y = 4;"                       },
            {"  }",                   "}"                            },
            {"  return y;",           "return y;"                    },
            {"}",                     "}"                            },
          },
})));
// clang-format on

using c_translator_function_params =
    ::testing::TestWithParam<std::pair<const char*, const char*>>;
TEST_P(c_translator_function_params, single_parameter) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::containers::string str(&allocator);
  str += std::get<0>(GetParam());
  str = str + " main(" + std::get<0>(GetParam()) + " x) { return x; }";

  wn::containers::string expected_str(&allocator);
  expected_str += std::get<1>(GetParam());
  expected_str = expected_str + " main(" + std::get<1>(GetParam()) +
                 " x) {\nreturn x;\n}\n";

  wn::scripting::test_file_manager manager(&allocator, {{"file.wns", str}});

  wn::scripting::c_translator translator(&allocator, &manager,
                                         WNLogging::get_null_logger());
  EXPECT_EQ(wn::scripting::parse_error::ok,
            translator.translate_file("file.wns"));
  EXPECT_EQ(std::string(expected_str.c_str()),
            std::string(manager.get_file("file.wns.c")->data()));
}

INSTANTIATE_TEST_CASE_P(
    parameter_tests, c_translator_function_params,
    ::testing::ValuesIn(std::vector<std::pair<const char*, const char*>>(
        {std::make_pair("Int", "wn_int32"),
         std::make_pair("Float", "wn_float32"),
         std::make_pair("Bool", "wn_bool"),
         std::make_pair("Char", "wn_char")})));

TEST(c_translator, multiple_c_functions) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::scripting::test_file_manager manager(&allocator,
                                           {{"file.wns",
                                             "Void main() { return; }\n"
                                             "Void foo() { return; }\n"}});

  wn::scripting::c_translator translator(&allocator, &manager,
                                         WNLogging::get_null_logger());
  EXPECT_EQ(wn::scripting::parse_error::ok,
            translator.translate_file("file.wns"));
  EXPECT_EQ(
      "void main() {\n"
      "return;\n"
      "}\n"
      "\n"
      "void foo() {\n"
      "return;\n"
      "}\n",
      std::string(manager.get_file("file.wns.c")->data()));
}

// Make sure multiple returns get combined into just 1.
TEST(c_translator, multiple_returns) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::scripting::test_file_manager manager(
      &allocator, {{"file.wns", "Void main() { return; return; }"}});

  wn::scripting::c_translator translator(&allocator, &manager,
                                         WNLogging::get_null_logger());
  EXPECT_EQ(wn::scripting::parse_error::ok,
            translator.translate_file("file.wns"));
  EXPECT_EQ(
      "void main() {\n"
      "return;\n"
      "}\n",
      std::string(manager.get_file("file.wns.c")->data()));
}

class c_int_params : public ::testing::TestWithParam<const char*> {};

TEST_P(c_int_params, int_return) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::containers::string str(&allocator);
  str += "Int main() { return ";
  str += GetParam();
  str += "; } ";

  wn::containers::string expected(&allocator);
  expected +=
      "wn_int32 main() {\n"
      "return ";
  expected += GetParam();
  expected += ";\n}\n";
  wn::scripting::test_file_manager manager(&allocator, {{"file.wns", str}});

  wn::scripting::c_translator translator(&allocator, &manager,
                                         WNLogging::get_null_logger());
  EXPECT_EQ(wn::scripting::parse_error::ok,
            translator.translate_file("file.wns"));
  EXPECT_EQ(std::string(expected.c_str()),
            std::string(manager.get_file("file.wns.c")->data()));
}

INSTANTIATE_TEST_CASE_P(int_tests, c_int_params,
                        ::testing::Values("0", "-1", "-32", "-4096",
                                          "2147483647", "-2147483648"));

struct arithmetic_operations {
  const char* source;
  const char* dest;
};

using c_arith_params = ::testing::TestWithParam<arithmetic_operations>;

TEST_P(c_arith_params, binary_arithmetic) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::containers::string str(&allocator);
  str += "Int main() { return ";
  str += GetParam().source;
  str += "; } ";

  wn::containers::string expected(&allocator);
  expected +=
      "wn_int32 main() {\n"
      "return ";
  expected += GetParam().dest;
  expected += ";\n}\n";
  wn::scripting::test_file_manager manager(&allocator, {{"file.wns", str}});

  wn::scripting::c_translator translator(&allocator, &manager,
                                         WNLogging::get_null_logger());
  EXPECT_EQ(wn::scripting::parse_error::ok,
            translator.translate_file("file.wns"));
  EXPECT_EQ(std::string(expected.c_str()),
            std::string(manager.get_file("file.wns.c")->data()));
}

INSTANTIATE_TEST_CASE_P(
    int_int_tests, c_arith_params,
    ::testing::ValuesIn(
        wn::containers::dynamic_array<arithmetic_operations>(
            {{"1 + 2", "(1 + 2)"},
             {"10 - 20", "(10 - 20)"},
             {"-32 * 0", "(-32 * 0)"},
             {"-32 % 22", "(-32 % 22)"},
             {"-32 + 4 * 10", "(-32 + (4 * 10))"},
             {"27 / 4 + 8 * 3", "((27 / 4) + (8 * 3))"}})));

using c_bool_params = ::testing::TestWithParam<arithmetic_operations>;

TEST_P(c_bool_params, boolean_arithmetic) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::containers::string str(&allocator);
  str += "Bool wn_main(Bool b) { return ";
  str += GetParam().source;
  str += "; } ";

  wn::containers::string expected(&allocator);
  expected +=
      "wn_bool wn_main(wn_bool b) {\n"
      "return ";
  expected += GetParam().dest;
  expected += ";\n}\n";
  wn::scripting::test_file_manager manager(&allocator, {{"file.wns", str}});

  wn::scripting::c_translator translator(&allocator, &manager,
                                         WNLogging::get_null_logger());
  EXPECT_EQ(wn::scripting::parse_error::ok,
            translator.translate_file("file.wns"));
  EXPECT_EQ(std::string(expected.c_str()),
            std::string(manager.get_file("file.wns.c")->data()));
}

INSTANTIATE_TEST_CASE_P(
    int_int_tests, c_bool_params,
    ::testing::ValuesIn(wn::containers::dynamic_array<arithmetic_operations>(
        {{"true", "true"},
         {"true == true", "(true == true)"},
         {"false", "false"},
         {"2 == 3", "(2 == 3)"},
         {"3 == 4 != b", "((3 == 4) != b)"},
         {"b == true", "(b == true)"},
         {"b == b", "(b == b)"},
         {"1 >= 3", "(1 >= 3)"},
         {"1 < 1", "(1 < 1)"},
         {"1 > 1", "(1 > 1)"},
         {"1 >= 1", "(1 >= 1)"},
         {"1 <= 1", "(1 <= 1)"},
         {"1 > (3 + 2)", "(1 > (3 + 2))"},
         {"(1 < 2) == (4 > 10)", "((1 < 2) == (4 > 10))"},
         {"(1 <= 2) == (b == false)", "((1 <= 2) == (b == false))"}})));

//TODO(awoloszyn) Arithmetic comparisons