// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNFileSystem/inc/WNFactory.h"
#include "WNFileSystem/inc/WNMapping.h"
#include "WNLogging/inc/WNBufferLogger.h"
#include "WNScripting/inc/WNCTranslator.h"
#include "WNScripting/test/inc/Common.h"
#include "WNScripting/test/inc/Common.h"
#include "WNTesting/inc/WNTestHarness.h"

void flush_buffer(wn_void* v, const wn_char* bytes, wn_size_t length,
    const std::vector<WNLogging::WNLogColorElement>&) {
  wn::containers::string* s = static_cast<wn::containers::string*>(v);
  s->append(bytes, length);
}

using buffer_logger = WNLogging::WNBufferLogger<flush_buffer>;
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
  wn::memory::default_expanding_allocator<50> allocator;
  wn::scripting::type_validator validator(&allocator);
  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory().make_mapping(
          wn::file_system::mapping_type::memory_backed, &allocator);

  mapping->initialize_files({{"file.wns", "Void main() { return; }"}});

  wn::scripting::c_translator translator(
      &validator, &allocator, mapping.get(), WNLogging::get_null_logger());
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
  log_buff buff(&allocator);
  buffer_logger logger(&buff);
  WNLogging::WNLog log(&logger);

  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory().make_mapping(
          wn::file_system::mapping_type::memory_backed, &allocator);

  mapping->initialize_files({{"file.wns", input_str}});
  wn::scripting::c_translator translator(
      &validator, &allocator, mapping.get(), &log);
  EXPECT_EQ(
      wn::scripting::parse_error::ok, translator.translate_file("file.wns")) <<
    (log.Flush(), buff.c_str());
  EXPECT_EQ(std::string(expected_output.c_str()),
      get_file_data(mapping, "file.wns.c"));
}

// clang-format off
INSTANTIATE_TEST_CASE_P(
    if_statement_tests, c_translator_direct_translation_test,
    ::testing::ValuesIn(
        wn::containers::dynamic_array<wn::containers::dynamic_array<source_pair>>({
            {
              {"Int main(Int x) {",     "wn_int32 _Z3wns4mainEll(wn_int32 x) {"  },
              {"  if (x == 3) {",       "if ((x == 3)) {"              },
              {"    return 7;",         "return 7;"                    },
              {"  }",                   "}"                            },
              {"  return 9;",           "return 9;"                    },
              {"}",                     "}"                            },
            },
            {
              {"Int main(Int x) {",     "wn_int32 _Z3wns4mainEll(wn_int32 x) {"  },
              {"  if (x == 3) {",       "if ((x == 3)) {"              },
              {"    return 7;",         "return 7;"                    },
              {"  } else {",            "} else {"                     },
              {"    return 10;",        "return 10;"                   },
              {"  }",                   "}"                            },
              {"  return 9;",           ""                             },
              {"}",                     "}"                            },
            },
            {
              {"Int main(Int x) {",     "wn_int32 _Z3wns4mainEll(wn_int32 x) {"  },
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
              {"Int main(Int x) {",        "wn_int32 _Z3wns4mainEll(wn_int32 x) {" },
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
            {"Int main(Int x) {",     "wn_int32 _Z3wns4mainEll(wn_int32 x) {"  },
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
            {"Int main(Int x) {",     "wn_int32 _Z3wns4mainEll(wn_int32 x) {"  },
            {"  Int y = 0;",          "wn_int32 y = 0;"              },
            {"  x = y;",              "x = y;"                       },
            {"  return x;",           "return x;"                    },
            {"}",                     "}"                            },
          },
          {
            {"Bool main(Int x) {",    "wn_bool _Z3wns4mainEbl(wn_int32 x) {"   },
            {"  Bool b = false;",     "wn_bool b = false;"           },
            {"  b = x == 4;",         "b = (x == 4);"                },
            {"  return b;",           "return b;"                    },
            {"}",                     "}"                            },
          },
          {
            {"Int main(Bool x) {",    "wn_int32 _Z3wns4mainElb(wn_bool x) {"   },
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

// clang-format off
INSTANTIATE_TEST_CASE_P(
    struct_definition_tests, c_translator_direct_translation_test,
    ::testing::ValuesIn(
        wn::containers::dynamic_array<wn::containers::dynamic_array<source_pair>>({
          {
            {"struct Foo {",          "typedef struct {"             },
            {"  Int x = 0;",          "  wn_int32 x;"                },
            {"}",                     "} Foo;"                       },
            {"",                      "\n"                           }
          },
          {
            {"struct Bar {",          "typedef struct {"             },
            {"  Int x = 0;",          "  wn_int32 x;"                },
            {"  Int y = 0;",          "  wn_int32 y;"                },
            {"  Float z = 0;",        "  wn_float32 z;"              },
            {"}",                     "} Bar;"                       },
            {"",                      "\n"                           }
          },
          {
            {"struct Foo {",          "typedef struct {"             },
            {"  Int x = 0;",          "  wn_int32 x;"                },
            {"  Int y = 0;",          "  wn_int32 y;"                },
            {"  Float z = 0;",        "  wn_float32 z;"              },
            {"}",                     "} Foo;"                       },
            {"",                      "\n"                           },
            {"struct Bar {",          "typedef struct {"             },
            {"  Int x = 0;",          "  wn_int32 x;"                },
            {"  Int y = 0;",          "  wn_int32 y;"                },
            {"  Float z = 0;",        "  wn_float32 z;"              },
            {"}",                     "} Bar;"                       },
            {"",                      "\n"                           }
          },
          {
            {"struct Foo {",          "typedef struct {"             },
            {"  Int x = 0;",          "  wn_int32 x;"                },
            {"  Int y = 0;",          "  wn_int32 y;"                },
            {"  Float z = 0;",        "  wn_float32 z;"              },
            {"}",                     "} Foo;"                       },
            {"",                      "\n"                           },
            {"struct Bar {",          "typedef struct {"             },
            {"  Int x = 0;",          "  wn_int32 x;"                },
            {"  Int y = 0;",          "  wn_int32 y;"                },
            {"  Float z = 0;",        "  wn_float32 z;"              },
            {"}",                     "} Bar;"                       },
            {"",                      "\n"                           },
            {"Int main(Int x) {",     "wn_int32 _Z3wns4mainEll(wn_int32 x) {"  },
            {"  if (x == 3) {",       "if ((x == 3)) {"              },
            {"    return 7;",         "return 7;"                    },
            {"  }",                   "}"                            },
            {"  return 9;",           "return 9;"                    },
            {"}",                     "}"                            }
          },
})));
// clang-format on

// clang-format off
INSTANTIATE_TEST_CASE_P(
    struct_explicit_declaration, c_translator_direct_translation_test,
    ::testing::ValuesIn(
        wn::containers::dynamic_array<wn::containers::dynamic_array<source_pair>>({
          {
            {"struct Foo {",          "typedef struct {"             },
            {"  Int x = 0;",          "  wn_int32 x;"                },
            {"}",                     "} Foo;"                       },
            {"",                      "\n"                           },
            {"Int main(Int x) {",     "wn_int32 _Z3wns4mainEll(wn_int32 x) {"   },
            {" Foo f = Foo();",       "Foo __wns_temp0;"             },
            {"",                      "Foo* f = &__wns_temp0;"       },
            {" return 4;",            "return 4;"                    },
            {"}",                     "}"                            }
          },
})));
// clang-format on

// clang-format off
INSTANTIATE_TEST_CASE_P(
    struct_usage, c_translator_direct_translation_test,
    ::testing::ValuesIn(
        wn::containers::dynamic_array<wn::containers::dynamic_array<source_pair>>({
          {
            {"struct Foo {",          "typedef struct {"             },
            {"  Int x = 0;",          "  wn_int32 x;"                },
            {"}",                     "} Foo;"                       },
            {"",                      "\n"                           },
            {"Int main(Int x) {",     "wn_int32 _Z3wns4mainEll(wn_int32 x) {"   },
            {" Foo f = Foo();",       "Foo __wns_temp0;"             },
            {"",                      "Foo* f = &__wns_temp0;"       },
            {" f.x = x;",             "f->x = x;"                    },
            {" return f.x;",          "return f->x;"                 },
            {"}",                     "}"                            }
          },
          {
            {"struct Foo {",          "typedef struct {"             },
            {"  Int x = 0;",          "  wn_int32 x;"                },
            {"  Int y = 0;",          "  wn_int32 y;"                },
            {"}",                     "} Foo;"                       },
            {"",                      "\n"                           },
            {"Int main(Int x) {",     "wn_int32 _Z3wns4mainEll(wn_int32 x) {"   },
            {" Foo f = Foo();",       "Foo __wns_temp0;"             },
            {"",                      "Foo* f = &__wns_temp0;"       },
            {" f.x = x;",             "f->x = x;"                    },
            {" f.y = 2 * x;",         "f->y = (2 * x);"              },
            {" return f.x + f.y;",    "return (f->x + f->y);"        },
            {"}",                     "}"                            }
          },
})));
// clang-format on

using c_translator_function_params =
    ::testing::TestWithParam<std::tuple<const char*, const char*, const char*>>;
TEST_P(c_translator_function_params, single_parameter) {
  wn::memory::default_expanding_allocator<50> allocator;
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
      wn::file_system::factory().make_mapping(
          wn::file_system::mapping_type::memory_backed, &allocator);

  mapping->initialize_files({{"file.wns", str}});

  wn::scripting::c_translator translator(
      &validator, &allocator, mapping.get(), WNLogging::get_null_logger());
  EXPECT_EQ(
      wn::scripting::parse_error::ok, translator.translate_file("file.wns"));
  EXPECT_EQ(
      std::string(expected_str.c_str()), get_file_data(mapping, "file.wns.c"));
}

INSTANTIATE_TEST_CASE_P(
    parameter_tests, c_translator_function_params,
    ::testing::ValuesIn(
        std::vector<std::tuple<const char*, const char*, const char*>>(
            {std::make_tuple("Int", "wn_int32", "l"),
                std::make_tuple("Float", "wn_float32", "f"),
                std::make_tuple("Bool", "wn_bool", "b"),
                std::make_tuple("Char", "wn_char", "c")})));

TEST(c_translator, multiple_c_functions) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::scripting::type_validator validator(&allocator);
  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory().make_mapping(
          wn::file_system::mapping_type::memory_backed, &allocator);

  mapping->initialize_files({{"file.wns",
      "Void main() { return; }\n"
      "Void foo() { return; }\n"}});

  wn::scripting::c_translator translator(
      &validator, &allocator, mapping.get(), WNLogging::get_null_logger());
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
  wn::memory::default_expanding_allocator<50> allocator;
  wn::scripting::type_validator validator(&allocator);

  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory().make_mapping(
          wn::file_system::mapping_type::memory_backed, &allocator);

  mapping->initialize_files({{"file.wns", "Void main() { return; return; }"}});

  wn::scripting::c_translator translator(
      &validator, &allocator, mapping.get(), WNLogging::get_null_logger());
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
  wn::memory::default_expanding_allocator<50> allocator;
  wn::scripting::type_validator validator(&allocator);
  wn::containers::string str(&allocator);
  str += "Int main() { return ";
  str += GetParam();
  str += "; } ";

  wn::containers::string expected(&allocator);
  expected +=
      "wn_int32 _Z3wns4mainEl() {\n"
      "return ";
  expected += GetParam();
  expected += ";\n}\n";
  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory().make_mapping(
          wn::file_system::mapping_type::memory_backed, &allocator);

  mapping->initialize_files({{"file.wns", str}});

  wn::scripting::c_translator translator(
      &validator, &allocator, mapping.get(), WNLogging::get_null_logger());
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
  wn::memory::default_expanding_allocator<50> allocator;
  wn::scripting::type_validator validator(&allocator);
  wn::containers::string str(&allocator);
  str += "Int main() { return ";
  str += GetParam().source;
  str += "; } ";

  wn::containers::string expected(&allocator);
  expected +=
      "wn_int32 _Z3wns4mainEl() {\n"
      "return ";
  expected += GetParam().dest;
  expected += ";\n}\n";
  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory().make_mapping(
          wn::file_system::mapping_type::memory_backed, &allocator);

  mapping->initialize_files({{"file.wns", str}});

  wn::scripting::c_translator translator(
      &validator, &allocator, mapping.get(), WNLogging::get_null_logger());
  EXPECT_EQ(
      wn::scripting::parse_error::ok, translator.translate_file("file.wns"));
  EXPECT_EQ(
      std::string(expected.c_str()), get_file_data(mapping, "file.wns.c"));
}

INSTANTIATE_TEST_CASE_P(int_int_tests, c_arith_params,
    ::testing::ValuesIn(wn::containers::dynamic_array<arithmetic_operations>(
        {{"1 + 2", "(1 + 2)"}, {"10 - 20", "(10 - 20)"},
            {"-32 * 0", "(-32 * 0)"}, {"-32 % 22", "(-32 % 22)"},
            {"-32 + 4 * 10", "(-32 + (4 * 10))"},
            {"27 / 4 + 8 * 3", "((27 / 4) + (8 * 3))"}})));

using c_bool_params = ::testing::TestWithParam<arithmetic_operations>;

TEST_P(c_bool_params, boolean_arithmetic) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::scripting::type_validator validator(&allocator);
  wn::containers::string str(&allocator);
  str += "Bool wn_main(Bool b) { return ";
  str += GetParam().source;
  str += "; } ";

  wn::containers::string expected(&allocator);
  expected +=
      "wn_bool _Z3wns7wn_mainEbb(wn_bool b) {\n"
      "return ";
  expected += GetParam().dest;
  expected += ";\n}\n";
  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory().make_mapping(
          wn::file_system::mapping_type::memory_backed, &allocator);

  mapping->initialize_files({{"file.wns", str}});

  wn::scripting::c_translator translator(
      &validator, &allocator, mapping.get(), WNLogging::get_null_logger());
  EXPECT_EQ(
      wn::scripting::parse_error::ok, translator.translate_file("file.wns"));
  EXPECT_EQ(
      std::string(expected.c_str()), get_file_data(mapping, "file.wns.c"));
}

INSTANTIATE_TEST_CASE_P(int_int_tests, c_bool_params,
    ::testing::ValuesIn(wn::containers::dynamic_array<arithmetic_operations>(
        {{"true", "true"}, {"true == true", "(true == true)"},
            {"false", "false"}, {"2 == 3", "(2 == 3)"},
            {"3 == 4 != b", "((3 == 4) != b)"}, {"b == true", "(b == true)"},
            {"b == b", "(b == b)"}, {"1 >= 3", "(1 >= 3)"},
            {"1 < 1", "(1 < 1)"}, {"1 > 1", "(1 > 1)"}, {"1 >= 1", "(1 >= 1)"},
            {"1 <= 1", "(1 <= 1)"}, {"1 > (3 + 2)", "(1 > (3 + 2))"},
            {"(1 < 2) == (4 > 10)", "((1 < 2) == (4 > 10))"},
            {"(1 <= 2) == (b == false)", "((1 <= 2) == (b == false))"}})));
