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

// clang-format off
INSTANTIATE_TEST_CASE_P(
    do_loop_tests, c_translator_direct_translation_test,
    ::testing::ValuesIn(
        std::vector<std::vector<source_pair>>({
          {
            {"struct Foo{ Int x = 4; }", "typedef struct {"},
            {"",                         "  int32_t x;"},
            {"",                         "} Foo;"},
            {"",                         "\n"},
            {"",                         "Foo* _Z3wns14_construct_FooENR3FooENUC3FooE(Foo* _this) {"},
            {"",                         "_this->x = 4;"},
            {"",                         "return _this;"},
            {"",                         "}"},
            {"",                         "\n"},
            {"",                         "void _Z3wns13_destruct_FooEvNUC3FooE(Foo* _this) {"},
            {"",                         "return;"},
            {"",                         "}"},
            {"",                         "\n"},
            {"Int main(Int x) {",        "int32_t _Z3wns4mainEll(int32_t x) {"},
            {"Int y = 1;",               "int32_t y = 1;"},
            {"do {",                     "do "},
            {"",                         "{"},
            {"  y = y + y;",             "y = (y + y);"},
            {"",                         "bool __wns_if_temp0 = false;"},
            {"",                         "{"},
            {"",                         "Foo __wns_temp_expression0;"},
            {"",                         "__wns_if_temp0 = ((_Z3wns14_construct_FooENR3FooENUC3FooE(&__wns_temp_expression0)->x > 0) == false);"},
            {"",                         "_Z3wns13_destruct_FooEvNUC3FooE(&__wns_temp_expression0);"},
            {"",                         "}"},
            {"",                         "if (__wns_if_temp0) {"},
            {"",                         "{"},
            {"",                         "break;"},
            {"",                         "}"},
            {"",                         "}"},
            {"} while (Foo().x > 0);",   "} while(true);"},
            {"return y;",                "return y;"},
            {"}",                        "}"},
          },
          {
            {"struct Foo{ Int x = 4; }", "typedef struct {"},
            {"",                         "  int32_t x;"},
            {"",                         "} Foo;"},
            {"",                         "\n"},
            {"",                         "Foo* _Z3wns14_construct_FooENR3FooENUC3FooE(Foo* _this) {"},
            {"",                         "_this->x = 4;"},
            {"",                         "return _this;"},
            {"",                         "}"},
            {"",                         "\n"},
            {"",                         "void _Z3wns13_destruct_FooEvNUC3FooE(Foo* _this) {"},
            {"",                         "return;"},
            {"",                         "}"},
            {"",                         "\n"},
            {"Int main(Int x) {",        "int32_t _Z3wns4mainEll(int32_t x) {"},
            {"Int y = 1;",               "int32_t y = 1;"},
            {"do {",                     "do "},
            {"",                         "{"},
            {"  y = y + y;",             "y = (y + y);"},
            {"  Foo f = Foo();",         "Foo __wns_temp_expression0;"},
            {"",                         "Foo* f = _Z3wns14_construct_FooENR3FooENUC3FooE(&__wns_temp_expression0);"},
            {"",                         "bool __wns_if_temp0 = false;"},
            {"",                         "{"},
            {"",                         "__wns_if_temp0 = ((f->x > 0) == false);"},
            {"",                         "}"},
            {"",                         "if (__wns_if_temp0) {"},
            {"",                         "{"},
            {"",                         "_Z3wns13_destruct_FooEvNUC3FooE(&__wns_temp_expression0);"},
            {"",                         "break;"},
            {"",                         "}"},
            {"",                         "}"},
            {"",                         "_Z3wns13_destruct_FooEvNUC3FooE(&__wns_temp_expression0);"},
            {"} while (f.x > 0);",       "} while(true);"},
            {"return y;",                "return y;"},
            {"}",                        "}"},
          },

          {
            {"struct Foo{ Int x = 4; }",        "typedef struct {"},
            {"",                                "  int32_t x;"},
            {"",                                "} Foo;"},
            {"",                                "\n"},
            {"",                                "Foo* _Z3wns14_construct_FooENR3FooENUC3FooE(Foo* _this) {"},
            {"",                                "_this->x = 4;"},
            {"",                                "return _this;"},
            {"",                                "}"},
            {"",                                "\n"},
            {"",                                "void _Z3wns13_destruct_FooEvNUC3FooE(Foo* _this) {"},
            {"",                                "return;"},
            {"",                                "}"},
            {"",                                "\n"},
            {"Int main(Int x) {",               "int32_t _Z3wns4mainEll(int32_t x) {"},
            {"Int y = 1;",                      "int32_t y = 1;"},
            {"do {",                            "do "},
            {"",                                "{"},
            {"  y = y + y;",                    "y = (y + y);"},
            {"  shared Foo f = shared Foo();",  "Foo* f = (Foo*)_Z3wns14_assign_sharedEvpvpvp((void*)_Z3wns14_construct_FooENR3FooENUC3FooE((Foo*)_Z3wns16_allocate_sharedEvpsfp(sizeof(Foo), &_Z3wns13_destruct_FooEvNUC3FooE)), NULL);"},
            {"",                                "bool __wns_if_temp0 = false;"},
            {"",                                "{"},
            {"",                                "__wns_if_temp0 = ((f->x > 0) == false);"},
            {"",                                "}"},
            {"",                                "if (__wns_if_temp0) {"},
            {"",                                "{"},
            {"",                                "_Z3wns13_deref_sharedEvvp((void*)f);"},
            {"",                                "break;"},
            {"",                                "}"},
            {"",                                "}"},
            {"",                                "_Z3wns13_deref_sharedEvvp((void*)f);"},
            {"} while (f.x > 0);",              "} while(true);"},
            {"return y;",                       "return y;"},
            {"}",                               "}"},
          },
          {
            {"struct Foo{ Int x = 4; }",        "typedef struct {"},
            {"",                                "  int32_t x;"},
            {"",                                "} Foo;"},
            {"",                                "\n"},
            {"",                                "Foo* _Z3wns14_construct_FooENR3FooENUC3FooE(Foo* _this) {"},
            {"",                                "_this->x = 4;"},
            {"",                                "return _this;"},
            {"",                                "}"},
            {"",                                "\n"},
            {"",                                "void _Z3wns13_destruct_FooEvNUC3FooE(Foo* _this) {"},
            {"",                                "return;"},
            {"",                                "}"},
            {"",                                "\n"},
            {"Int main(Int x) {",               "int32_t _Z3wns4mainEll(int32_t x) {"},
            {"Int y = 1;",                      "int32_t y = 1;"},
            {"shared Foo p = shared Foo();",    "Foo* p = (Foo*)_Z3wns14_assign_sharedEvpvpvp((void*)_Z3wns14_construct_FooENR3FooENUC3FooE((Foo*)_Z3wns16_allocate_sharedEvpsfp(sizeof(Foo), &_Z3wns13_destruct_FooEvNUC3FooE)), NULL);"},
            {"do {",                            "do "},
            {"",                                "{"},
            {"  y = y + y;",                    "y = (y + y);"},
            {"  shared Foo f = shared Foo();",  "Foo* f = (Foo*)_Z3wns14_assign_sharedEvpvpvp((void*)_Z3wns14_construct_FooENR3FooENUC3FooE((Foo*)_Z3wns16_allocate_sharedEvpsfp(sizeof(Foo), &_Z3wns13_destruct_FooEvNUC3FooE)), NULL);"},
            {"",                                "bool __wns_if_temp0 = false;"},
            {"",                                "{"},
            {"",                                "__wns_if_temp0 = ((f->x > 0) == false);"},
            {"",                                "}"},
            {"",                                "if (__wns_if_temp0) {"},
            {"",                                "{"},
            {"",                                "_Z3wns13_deref_sharedEvvp((void*)f);"},
            {"",                                "break;"},
            {"",                                "}"},
            {"",                                "}"},
            {"",                                "_Z3wns13_deref_sharedEvvp((void*)f);"},
            {"} while (f.x > 0);",              "} while(true);"},
            {"",                                "{"},
            {"",                                "int32_t __wns_ret_temp0 = y;"},
            {"",                                "_Z3wns13_deref_sharedEvvp((void*)p);"},
            {"return y;",                       "return __wns_ret_temp0;"},
            {"}",                               "}"},
            {"",                                "}"},
          }
})));
// clang-format on

// clang-format off
INSTANTIATE_TEST_CASE_P(
    break_tests, c_translator_direct_translation_test,
    ::testing::ValuesIn(
        std::vector<std::vector<source_pair>>({
          {
            {"Int main(Int x) {",     "int32_t _Z3wns4mainEll(int32_t x) {"  },
            {"  Int y = x;",          "int32_t y = x;"                },
            {"  do {",                "do "                           },
            {"",                      "{"                             },
            {"y = y - 1;",            "y = (y - 1);"                  },
            {"",                      "bool __wns_if_temp0 = false;"  },
            {"",                      "{"                             },
            {"",                      "__wns_if_temp0 = (y < 2);"     },
            {"",                      "}",                            },
            {"",                      "bool __wns_if_temp1 = false;"  },
            {"",                      "{"                             },
            {"",                      "__wns_if_temp1 = __wns_if_temp0;"},
            {"",                      "}"                             },
            {"if (y < 2) { ",         "if (__wns_if_temp1) {"         },
            {"",                      "{"                             },
            {"break;",                "break;"                        },
            {"",                      "}"                             },
            {"}",                     "}"                             },
            {"",                      "bool __wns_if_temp2 = false;"  },
            {"",                      "{"                             },
            {"",                      "__wns_if_temp2 = (true == false);"},
            {"",                      "}"                             },
            {"",                      "if (__wns_if_temp2) {"         },
            {"",                      "{"                             },
            {"",                      "break;"                        },
            {"",                      "}"                             },
            {"",                      "}"                             },
            {"} while(true);",        "} while(true);"                },
            {"return y;",             "return y;"                     },
            {"}",                     "}"                             }
          }
})));
// clang-format on

// clang-format off
INSTANTIATE_TEST_CASE_P(
    continue_tests, c_translator_direct_translation_test,
    ::testing::ValuesIn(
        std::vector<std::vector<source_pair>>({
          {
            {"Int main(Int x) {",     "int32_t _Z3wns4mainEll(int32_t x) {"  },
            {"  Int y = x;",          "int32_t y = x;"                },
            {"  do {",                "do "                           },
            {"",                      "{"                             },
            {"y = y - 1;",            "y = (y - 1);"                  },
            {"",                      "bool __wns_if_temp0 = false;"  },
            {"",                      "{"                             },
            {"",                      "__wns_if_temp0 = (y > 2);"     },
            {"",                      "}",                            },
            {"if (y > 2) { ",         "if (__wns_if_temp0) {"         },
            {"",                      "{"                             },
            {"continue;",                "continue;"                  },
            {"}",                     "}"                             },
            {"",                      "}"                             },
            {"return y;",             "return y;"                     },
            {"} while(true);",        "} while(true);"                },
            {"}",                     "}"                             }
          }
})));
// clang-format on

// clang-format off
INSTANTIATE_TEST_CASE_P(
    declaration_tests, c_translator_direct_translation_test,
    ::testing::ValuesIn(
        std::vector<std::vector<source_pair>>({
          {
            {"Int main(Int x) {",     "int32_t _Z3wns4mainEll(int32_t x) {"  },
            {"  Int y = x;",          "int32_t y = x;"              },
            {"  Bool b = y == 4;",    "bool b = (y == 4);"          },
            {"",                      "bool __wns_if_temp0 = false;"},
            {"",                      "{"                           },
            {"",                      "__wns_if_temp0 = b;"         },
            {"",                      "}"                           },
            {"  if (b) {    ",        "if (__wns_if_temp0) {"       },
            {"     return 3;",        "return 3;"                   },
            {"  }",                   "}"                           },
            {"  return 4;",           "return 4;"                   },
            {"}",                     "}"                           },
          }
})));
// clang-format on

// clang-format off
INSTANTIATE_TEST_CASE_P(
    scope_tests, c_translator_direct_translation_test,
    ::testing::ValuesIn(
        std::vector<std::vector<source_pair>>({
          {
            {"Int main(Int x) {",     "int32_t _Z3wns4mainEll(int32_t x) {"  },
            {"  Int y = x;",          "int32_t y = x;"               },
            {"  {",                   "{"                            },
            {"    y = 4;",            "y = 4;"                       },
            {"  }",                   "}"                            },
            {"}",                     "}"                            },
          },
          {
            {"Int main(Int x) {",     "int32_t _Z3wns4mainEll(int32_t x) {"  },
            {"  Int y = x;",          "int32_t y = x;"               },
            {"  {",                   "{"                            },
            {"    {",                 "{"                            },
            {"      y = 4;",          "y = 4;"                       },
            {"    }",                 "}"                            },
            {"    return 3; ",        "return 3;"                    },
            {"  }",                   "}"                            },
            {"return 4; ",            ""                             },
            {"}",                     "}"                            },
          },
          {
            {"Int main(Int x) {",     "int32_t _Z3wns4mainEll(int32_t x) {"  },
            {"  Int y = x;",          "int32_t y = x;"               },
            {"  {",                   "{"                            },
            {"     return 3; ",       "return 3;"                    },
            {"  }",                   "}"                            },
            {"return 4; ",            ""                             },
            {"}",                     "}"                            },
          }
})));
// clang-format on

// clang-format off
INSTANTIATE_TEST_CASE_P(
    assignment_tests, c_translator_direct_translation_test,
    ::testing::ValuesIn(
        std::vector<std::vector<source_pair>>({
          {
            {"Int main(Int x) {",     "int32_t _Z3wns4mainEll(int32_t x) {"  },
            {"  Int y = 0;",          "int32_t y = 0;"              },
            {"  x = y;",              "x = y;"                       },
            {"  return x;",           "return x;"                    },
            {"}",                     "}"                            },
          },
          {
            {"Bool main(Int x) {",    "bool _Z3wns4mainEbl(int32_t x) {"   },
            {"  Bool b = false;",     "bool b = false;"           },
            {"  b = x == 4;",         "b = (x == 4);"                },
            {"  return b;",           "return b;"                    },
            {"}",                     "}"                            },
          },
          {
            {"Int main(Bool x) {",    "int32_t _Z3wns4mainElb(bool x) {"   },
            {"  Int y = 4;",          "int32_t y = 4;"               },
            {"",                      "bool __wns_if_temp0 = false;" },
            {"",                      "{"                            },
            {"",                      "__wns_if_temp0 = x;"          },
            {"",                      "}"                            },
            {"  if (x) {",            "if (__wns_if_temp0) {"        },
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
    struct_with_array_test, c_translator_direct_translation_test,
    ::testing::ValuesIn(
        std::vector<std::vector<source_pair>>({
          {
            {"struct Foo {",           "typedef struct {"             },
            {"  Int[] x = Int[10](3);","  struct { uint32_t size0; int32_t _val[];}* x;"},
            {"",                       "  struct { uint32_t size0; int32_t _val[10];} __x;"},
            {"}",                      "} Foo;"                       },
            {"",                       "\n"                           },
            {"",                       "Foo* _Z3wns14_construct_FooENR3FooENUC3FooE(Foo* _this) {"   },
            {"",                       "_this->x = _Z3wns17_construct_array1E(&_this->__x);"},
            {"",                       "return _this;"                },
            {"",                       "}"                            },
            {"",                       "\n"                           },
            {"",                       "void _Z3wns13_destruct_FooEvNUC3FooE(Foo* _this) {"   },
            {"",                       "return;"                      },
            {"",                       "}"                            },
            {"",                       "\n"                            },
            {"",                       "struct { uint32_t size0; int32_t _val[10];}* _Z3wns17_construct_array1E(struct { uint32_t size0; int32_t _val[10];}* _this) {"},
            {"",                       "_this->size0 = 10;"           },
            {"",                       "int32_t __wns_if_temp0 = 10;" },
            {"",                       "do "                           },
            {"",                       "{"                            },
            {"",                       "__wns_if_temp0 = (__wns_if_temp0 - 1);"},
            {"",                       "_this->_val[__wns_if_temp0] = 3;"},
            {"",                       "if ((__wns_if_temp0 == 0)) {" },
            {"",                       "{"                            },
            {"",                       "break;"                       },
            {"",                       "}"                            },
            {"",                       "}"                            },
            {"",                       "} while(true);"               },
            {"",                       "return _this;"                },
            {"",                       "}"                            }
          },
          {
            {"struct Foo {",           "typedef struct {"             },
            {"  Int[] x = Int[10](3);","  struct { uint32_t size0; int32_t _val[];}* x;"},
            {"",                       "  struct { uint32_t size0; int32_t _val[10];} __x;"},
            {"}",                      "} Foo;"                       },
            {"",                       "\n"                           },
            {"",                       "Foo* _Z3wns14_construct_FooENR3FooENUC3FooE(Foo* _this) {"   },
            {"",                       "_this->x = _Z3wns17_construct_array1E(&_this->__x);"},
            {"",                       "return _this;"                },
            {"",                       "}"                            },
            {"",                       "\n"                           },
            {"",                       "void _Z3wns13_destruct_FooEvNUC3FooE(Foo* _this) {"   },
            {"",                       "return;"                      },
            {"",                       "}"                            },
            {"",                       "\n"                            },
            {"",                       "struct { uint32_t size0; int32_t _val[10];}* _Z3wns17_construct_array1E(struct { uint32_t size0; int32_t _val[10];}* _this) {"},
            {"",                       "_this->size0 = 10;"           },
            {"",                       "int32_t __wns_if_temp0 = 10;" },
            {"",                       "do "                           },
            {"",                       "{"                            },
            {"",                       "__wns_if_temp0 = (__wns_if_temp0 - 1);"},
            {"",                       "_this->_val[__wns_if_temp0] = 3;"},
            {"",                       "if ((__wns_if_temp0 == 0)) {" },
            {"",                       "{"                            },
            {"",                       "break;"                       },
            {"",                       "}"                            },
            {"",                       "}"                            },
            {"",                       "} while(true);"               },
            {"",                       "return _this;"                },
            {"",                       "}"                            },
            {"",                       "\n"                           },
            {"Int main(Int x) {",      "int32_t _Z3wns4mainEll(int32_t x) {"},
            {"",                       "Foo __wns_temp_expression0;"},
            {"  Foo f = Foo();",       "Foo* f = _Z3wns14_construct_FooENR3FooENUC3FooE(&__wns_temp_expression0);"},
            {"",                       "{"},
            {"",                       "int32_t __wns_ret_temp0 = f->x->_val[x];"},
            {"",                       "_Z3wns13_destruct_FooEvNUC3FooE(&__wns_temp_expression0);"},
            {"  return f.x[x];",       "return __wns_ret_temp0;"},
            {"",                       "}"},
            {"}",                      "}"},
          },
          {
            {"",                                                    "struct { uint32_t size0; int32_t _val[10];}* _Z3wns17_construct_array1E(struct { uint32_t size0; int32_t _val[10];}* _this) {"},
            {"",                                                    "_this->size0 = 10;"          },
            {"",                                                    "int32_t __wns_if_temp0 = 10;"},
            {"",                                                    "do "                          },
            {"",                                                    "{"                           },
            {"",                                                    "__wns_if_temp0 = (__wns_if_temp0 - 1);"},
            {"",                                                    "_this->_val[__wns_if_temp0] = 0;"},
            {"",                                                    "if ((__wns_if_temp0 == 0)) {"},
            {"",                                                    "{"                           },
            {"",                                                    "break;"                      },
            {"",                                                    "}"                           },
            {"",                                                    "}"                           },
            {"",                                                    "} while(true);"              },
            {"",                                                    "return _this;"               },
            {"",                                                    "}"                           },
            {"",                                                    "\n"                          },
            {"Int main(Int x) {",                                   "int32_t _Z3wns4mainEll(int32_t x) {"},
            {"",                                                    "struct { uint32_t size0; int32_t _val[10];} __wns_temp_expression0 = {};"},
            {"",                                                    "struct { uint32_t size0; int32_t _val[];}* y = _Z3wns17_construct_array1E(&__wns_temp_expression0);"},
            {"",                                                    "{"                            },
            {"  Int[] y = Int[10](0);",                             "int32_t __wns_ret_temp0 = y->_val[x];"},
            {"  return y[x];",                                      "return __wns_ret_temp0;"      },
            {"",                                                    "}"                            },
            {"}",                                                   "}"                            },
          },
          {
            {"struct A {",             "typedef struct {"             },
            {"}",                      "} A;"                         },
            {"",                       "\n"                           },
            {"struct Foo {",           "typedef struct {"             },
            {"  A[] x = A[10](A());",  "  struct { uint32_t size0; A* _val[];}* x;"},
            {"",                       "  struct { uint32_t size0; A* _val[10]; A _c_val[10];} __x;"},
            {"}",                      "} Foo;"                       },
            {"",                       "\n"                           },
            {"",                       "Foo* _Z3wns14_construct_FooENR3FooENUC3FooE(Foo* _this) {"   },
            {"",                       "_this->x = (struct { uint32_t size0; A* _val[]; }*)_Z3wns17_construct_array1E((struct { uint32_t size0; A* _val[10]; A _c_val[10];}*)&_this->__x);"},
            {"",                       "return _this;"                },
            {"",                       "}"                            },
            {"",                       "\n"                           },
            {"",                       "void _Z3wns13_destruct_FooEvNUC3FooE(Foo* _this) {"   },
            {"",                       "_Z3wns16_destruct_array1Ev((struct { uint32_t size0; A* _val[10]; A _c_val[10];}*)_this->x);"               },
            {"",                       "return;"                      },
            {"",                       "}"                            },
            {"",                       "\n"                            },
            {"",                       "void _Z3wns16_destruct_array1Ev(struct { uint32_t size0; A* _val[10]; A _c_val[10];}* _this) {"},
            {"",                       "_this->size0 = 10;"           },
            {"",                       "int32_t __wns_if_temp0 = 10;" },
            {"",                       "do "                           },
            {"",                       "{"                            },
            {"",                       "if ((__wns_if_temp0 == 0)) {" },
            {"",                       "{"                            },
            {"",                       "break;"                       },
            {"",                       "}"                            },
            {"",                       "}"                            },
            {"",                       "__wns_if_temp0 = (__wns_if_temp0 - 1);"},
            {"",                       "_Z3wns11_destruct_AEvNUC1AE(_this->_val[__wns_if_temp0]);"},
            {"",                       "} while(true);"               },
            {"",                       "return;"                      },
            {"",                       "}"                            },
            {"",                       "\n"                           },
            {"",                       "struct { uint32_t size0; A* _val[10]; A _c_val[10];}* _Z3wns17_construct_array1E(struct { uint32_t size0; A* _val[10]; A _c_val[10];}* _this) {"},
            {"",                       "_this->size0 = 10;"           },
            {"",                       "int32_t __wns_if_temp0 = 10;" },
            {"",                       "do "                           },
            {"",                       "{"                            },
            {"",                       "if ((__wns_if_temp0 == 0)) {" },
            {"",                       "{"                            },
            {"",                       "break;"                       },
            {"",                       "}"                            },
            {"",                       "}"                            },
            {"",                       "__wns_if_temp0 = (__wns_if_temp0 - 1);"},
            {"",                       "_this->_val[__wns_if_temp0] = _Z3wns12_construct_AENR1AENUC1AE(&_this->_c_val[__wns_if_temp0]);"},
            {"",                       "} while(true);"               },
            {"",                       "return _this;"                },
            {"",                       "}"                            },
            {"",                       "\n"                           },
            {"",                       "A* _Z3wns12_construct_AENR1AENUC1AE(A* _this) {"},
            {"",                       "return _this;"},
            {"",                       "}"},
            {"",                       "\n"},
            {"",                       "void _Z3wns11_destruct_AEvNUC1AE(A* _this) {"},
            {"",                       "return;"},
            {"",                       "}"},
          },
})));
// clang-format on

// clang-format off
INSTANTIATE_TEST_CASE_P(
    array_test, c_translator_direct_translation_test,
    ::testing::ValuesIn(
        std::vector<std::vector<source_pair>>({
          {
            {"",                                 "struct { uint32_t size0; int32_t _val[10];}* _Z3wns17_construct_array1E(struct { uint32_t size0; int32_t _val[10];}* _this) {"},
            {"",                                 "_this->size0 = 10;"},
            {"",                                 "int32_t __wns_if_temp0 = 10;"},
            {"",                                 "do "},
            {"",                                 "{"},
            {"",                                 "__wns_if_temp0 = (__wns_if_temp0 - 1);"},
            {"",                                 "_this->_val[__wns_if_temp0] = 0;"},
            {"",                                 "if ((__wns_if_temp0 == 0)) {"},
            {"",                                 "{"},
            {"",                                 "break;"},
            {"",                                 "}"},
            {"",                                 "}"},
            {"",                                 "} while(true);"},
            {"",                                 "return _this;" },
            {"",                                 "}"},
            {"",                                 "\n"},
            {"Int foo(Int[] x) { return x[3]; } ","int32_t _Z3wns3fooEl(struct { uint32_t size0; int32_t _val[]; }* x) {"},
            {"",                                 "return x->_val[3];"},
            {"",                                 "}"},
            {"",                                 "\n"},
            {"Int main(Int q) {",                "int32_t _Z3wns4mainEll(int32_t q) {"},
            {"",                                 "struct { uint32_t size0; int32_t _val[10];} __wns_temp_expression0 = {};"},
            {"",                                 "struct { uint32_t size0; int32_t _val[];}* x = _Z3wns17_construct_array1E(&__wns_temp_expression0);"},
            {"",                                 "{"},
            {"  Int[] x = Int[10](0);",          "int32_t __wns_ret_temp0 = _Z3wns3fooEl(x);"},
            {"  return foo(x);",                 "return __wns_ret_temp0;"},
            {"",                                 "}"},
            {"}",                                "}"},
          },
          {
            {"struct A {",             "typedef struct {"             },
            {"  Int x = 4;",           "  int32_t x;"                 },
            {"}",                      "} A;"                         },
            {"",                       "\n"                           },
            {"",                       "void _Z3wns16_destruct_array1Ev(struct { uint32_t size0; A* _val[10]; A _c_val[10];}* _this) {"},
            {"",                       "_this->size0 = 10;"           },
            {"",                       "int32_t __wns_if_temp0 = 10;" },
            {"",                       "do "                           },
            {"",                       "{"                            },
            {"",                       "if ((__wns_if_temp0 == 0)) {" },
            {"",                       "{"                            },
            {"",                       "break;"                       },
            {"",                       "}"                            },
            {"",                       "}"                            },
            {"",                       "__wns_if_temp0 = (__wns_if_temp0 - 1);"},
            {"",                       "_Z3wns11_destruct_AEvNUC1AE(_this->_val[__wns_if_temp0]);"},
            {"",                       "} while(true);"               },
            {"",                       "return;"                      },
            {"",                       "}"                            },
            {"",                       "\n"                           },
            {"",                       "struct { uint32_t size0; A* _val[10]; A _c_val[10];}* _Z3wns17_construct_array1E(struct { uint32_t size0; A* _val[10]; A _c_val[10];}* _this) {"},
            {"",                       "_this->size0 = 10;"},
            {"",                       "int32_t __wns_if_temp0 = 10;"},
            {"",                       "do "},
            {"",                       "{"},
            {"",                       "if ((__wns_if_temp0 == 0)) {"},
            {"",                       "{"},
            {"",                       "break;"},
            {"",                       "}"},
            {"",                       "}"},
            {"",                       "__wns_if_temp0 = (__wns_if_temp0 - 1);"},
            {"",                       "_this->_val[__wns_if_temp0] = _Z3wns12_construct_AENR1AENUC1AE(&_this->_c_val[__wns_if_temp0]);"},
            {"",                       "} while(true);"},
            {"",                       "return _this;" },
            {"",                       "}"},
            {"",                       "\n"},
            {"",                       "A* _Z3wns12_construct_AENR1AENUC1AE(A* _this) {"},
            {"",                       "_this->x = 4;"},
            {"",                       "return _this;"},
            {"",                       "}"},
            {"",                       "\n"},
            {"",                       "void _Z3wns11_destruct_AEvNUC1AE(A* _this) {"},
            {"",                       "return;"},
            {"",                       "}"},
            {"",                       "\n"},
            {"Int foo(A[] x) { return x[3].x; } ", "int32_t _Z3wns3fooEl(struct { uint32_t size0; A* _val[]; }* x) {"},
            {"",                                 "return x->_val[3]->x;" },
            {"",                                 "}"},
            {"",                                 "\n"},
            {"Int main(Int q) {",                "int32_t _Z3wns4mainEll(int32_t q) {"},
            {"",                                 "struct { uint32_t size0; A* _val[10]; A _c_val[10];} __wns_temp_expression0 = {};"},
            {"",                                 "struct { uint32_t size0; A* _val[];}* x = _Z3wns17_construct_array1E(&__wns_temp_expression0);"},
            {"",                                 "{"},
            {"  A[] x = A[10](A());",            "int32_t __wns_ret_temp0 = _Z3wns3fooEl(x);"},
			{"",                                 "_Z3wns16_destruct_array1Ev(x);"},
            {"  return foo(x);",                 "return __wns_ret_temp0;"},
            {"",                                 "}"},
            {"}",                                "}"},
          },
        }
)));
// clang-format on

// clang-format off
INSTANTIATE_TEST_CASE_P(
  inherited_structs, c_translator_direct_translation_test,
  ::testing::ValuesIn(
    std::vector<std::vector<source_pair>>({
      {
        { "class A {}",             "typedef struct {" },
        { "",                       "} A;"             },
        { "",                       "\n"               },
        { "class B : A { Int b = 4; }", "typedef struct {" },
        { "",                       "  int32_t b;"       },
        { "",                       "} B;"             },
        { "",                       "\n" },
        { "class Foo {",           "typedef struct {"  },
        { "  A a = B();",          "  A* a;"             },
        { "",                      "  B __a;"            },
        { "}",                     "} Foo;"            },
        { "",                      "\n"                },
        { "",                      "Foo* _Z3wns14_construct_FooENR3FooENUC3FooE(Foo* _this) {"},
        { "",                      "_this->a = (A*)(_Z3wns12_construct_BENR1BENUC1BE((B*)&(_this->__a)));" },
        { "",                      "return _this;" },
        { "",                      "}" },
        { "",                      "\n" },
        { "",                      "void _Z3wns13_destruct_FooEvNUC3FooE(Foo* _this) {" },
        { "",                      "_Z3wns11_destruct_BEvNUC1BE((B*)&(_this->__a));" },
        { "",                      "return;" },
        { "",                      "}" },
        { "",                      "\n" },
        { "",                      "B* _Z3wns12_construct_BENR1BENUC1BE(B* _this) {" },
        { "",                      "_this->b = 4;" },
        { "",                      "return _this;" },
        { "",                      "}" },
        { "",                      "\n" },
        { "",                      "void _Z3wns11_destruct_BEvNUC1BE(B* _this) {" },
        { "",                      "return;" },
        { "",                      "}" },
        { "",                      "\n" },
        { "",                      "A* _Z3wns12_construct_AENR1AENUC1AE(A* _this) {" },
        { "",                      "return _this;" },
        { "",                      "}" },
        { "",                      "\n" },
        { "",                      "void _Z3wns11_destruct_AEvNUC1AE(A* _this) {" },
        { "",                      "return;" },
        { "",                      "}" },
      },
      {
        { "class A {}",             "typedef struct {" },
        { "",                       "} A;" },
        { "",                       "\n" },
        { "class B : A { Int b = 4; }", "typedef struct {" },
        { "",                       "  int32_t b;" },
        { "",                       "} B;" },
        { "",                       "\n" },
        { "class Foo {",           "typedef struct {" },
        { "  A a = A();",          "  A* a;" },
        { "",                      "  A __a;" },
        { "}",                     "} Foo;" },
        { "",                      "\n" },
        { "class Bar : Foo {",     "typedef struct {" },
        { "  .a = B();",           "  A* a;" },
        { "",                      "  B __a;" },
        { "}",                     "} Bar;" },
        { "",                      "\n" },
        { "",                      "Bar* _Z3wns14_construct_BarENR3BarENUC3BarE(Bar* _this) {" },
        { "",                      "_this->a = (A*)(_Z3wns12_construct_BENR1BENUC1BE((B*)&(_this->__a)));" },
        { "",                      "return _this;" },
        { "",                      "}" },
        { "",                      "\n" },
        { "",                      "void _Z3wns13_destruct_BarEvNUC3BarE(Bar* _this) {" },
        { "",                      "_Z3wns11_destruct_BEvNUC1BE((B*)&(_this->__a));" },
        { "",                      "return;" },
        { "",                      "}" },
        { "",                      "\n" },
        { "",                      "Foo* _Z3wns14_construct_FooENR3FooENUC3FooE(Foo* _this) {" },
        { "",                      "_this->a = _Z3wns12_construct_AENR1AENUC1AE(&_this->__a);" },
        { "",                      "return _this;" },
        { "",                      "}" },
        { "",                      "\n" },
        { "",                      "void _Z3wns13_destruct_FooEvNUC3FooE(Foo* _this) {" },
        { "",                      "_Z3wns11_destruct_AEvNUC1AE(&_this->__a);" },
        { "",                      "return;" },
        { "",                      "}" },
        { "",                      "\n" },
        { "",                      "B* _Z3wns12_construct_BENR1BENUC1BE(B* _this) {" },
        { "",                      "_this->b = 4;" },
        { "",                      "return _this;" },
        { "",                      "}" },
        { "",                      "\n" },
        { "",                      "void _Z3wns11_destruct_BEvNUC1BE(B* _this) {" },
        { "",                      "return;" },
        { "",                      "}" },
        { "",                      "\n" },
        { "",                      "A* _Z3wns12_construct_AENR1AENUC1AE(A* _this) {" },
        { "",                      "return _this;" },
        { "",                      "}" },
        { "",                      "\n" },
        { "",                      "void _Z3wns11_destruct_AEvNUC1AE(A* _this) {" },
        { "",                      "return;" },
        { "",                      "}" },
      },
      {
        { "class Foo{ Int x = 4; }", "typedef struct {" },
        { "",                         "  int32_t x;" },
        { "",                         "} Foo;" },
        { "",                         "\n" },
        { "class Bar : Foo { .x = 7; }",     "typedef struct {" },
        { "",                         "  int32_t x;" },
        { "",                         "} Bar;" },
        { "",                         "\n" },
        { "",                         "Bar* _Z3wns14_construct_BarENR3BarENUC3BarE(Bar* _this) {" },
        { "",                         "_this->x = 7;" },
        { "",                         "return _this;" },
        { "",                         "}" },
        { "",                         "\n" },
        { "",                         "void _Z3wns13_destruct_BarEvNUC3BarE(Bar* _this) {" },
        { "",                         "return;" },
        { "",                         "}" },
        { "",                         "\n" },
        { "",                         "Foo* _Z3wns14_construct_FooENR3FooENUC3FooE(Foo* _this) {" },
        { "",                         "_this->x = 4;" },
        { "",                         "return _this;" },
        { "",                         "}" },
        { "",                         "\n" },
        { "",                         "void _Z3wns13_destruct_FooEvNUC3FooE(Foo* _this) {" },
        { "",                         "return;" },
        { "",                         "}" },
      },
      {
        { "class Foo { Int x = 4; Int y = x;}", "typedef struct {" },
        { "",                         "  int32_t x;" },
        { "",                         "  int32_t y;" },
        { "",                         "} Foo;" },
        { "",                         "\n" },
        { "class Bar : Foo { .x = 9; }",     "typedef struct {" },
        { "",                         "  int32_t x;" },
        { "",                         "  int32_t y;" },
        { "",                         "} Bar;" },
        { "",                         "\n" },
        { "",                         "Bar* _Z3wns14_construct_BarENR3BarENUC3BarE(Bar* _this) {" },
        { "",                         "_this->x = 9;" },
        { "",                         "_this->y = _this->x;" },
        { "",                         "return _this;" },
        { "",                         "}" },
        { "",                         "\n" },
        { "",                         "void _Z3wns13_destruct_BarEvNUC3BarE(Bar* _this) {" },
        { "",                         "return;" },
        { "",                         "}" },
        { "",                         "\n" },
        { "",                         "Foo* _Z3wns14_construct_FooENR3FooENUC3FooE(Foo* _this) {" },
        { "",                         "_this->x = 4;" },
        { "",                         "_this->y = _this->x;" },
        { "",                         "return _this;" },
        { "",                         "}" },
        { "",                         "\n" },
        { "",                         "void _Z3wns13_destruct_FooEvNUC3FooE(Foo* _this) {" },
        { "",                         "return;" },
        { "",                         "}" },
      },

      {
        { "class Foo{ Int x = 4; Int y = x; Int z = y; }", "typedef struct {" },
        { "",                         "  int32_t x;" },
        { "",                         "  int32_t y;" },
        { "",                         "  int32_t z;" },
        { "",                         "} Foo;" },
        { "",                         "\n" },
        { "class Bar : Foo { .z = 4; .x = default; Int f = x; }",     "typedef struct {" },
        { "",                         "  int32_t x;" },
        { "",                         "  int32_t y;" },
        { "",                         "  int32_t z;" },
        { "",                         "  int32_t f;" },
        { "",                         "} Bar;" },
        { "",                         "\n" },
        { "",                         "Bar* _Z3wns14_construct_BarENR3BarENUC3BarE(Bar* _this) {" },
        { "",                         "_this->z = 4;" },
        { "",                         "_this->x = 4;" },
        { "",                         "_this->f = _this->x;" },
        { "",                         "_this->y = _this->x;" },
        { "",                         "return _this;" },
        { "",                         "}" },
        { "",                         "\n" },
        { "",                         "void _Z3wns13_destruct_BarEvNUC3BarE(Bar* _this) {" },
        { "",                         "return;" },
        { "",                         "}" },
        { "",                         "\n" },
        { "",                         "Foo* _Z3wns14_construct_FooENR3FooENUC3FooE(Foo* _this) {" },
        { "",                         "_this->x = 4;" },
        { "",                         "_this->y = _this->x;" },
        { "",                         "_this->z = _this->y;" },
        { "",                         "return _this;" },
        { "",                         "}" },
        { "",                         "\n" },
        { "",                         "void _Z3wns13_destruct_FooEvNUC3FooE(Foo* _this) {" },
        { "",                         "return;" },
        { "",                         "}" },
      },
      {
        { "class Foo{ Int x = 4; }", "typedef struct {" },
        { "",                         "  int32_t x;" },
        { "",                         "} Foo;" },
        { "",                         "\n" },
        { "class Bar : Foo { .x = 7; }",     "typedef struct {" },
        { "",                         "  int32_t x;" },
        { "",                         "} Bar;" },
        { "",                         "\n" },
        { "",                         "Bar* _Z3wns14_construct_BarENR3BarENUC3BarE(Bar* _this) {" },
        { "",                         "_this->x = 7;" },
        { "",                         "return _this;" },
        { "",                         "}" },
        { "",                         "\n" },
        { "",                         "void _Z3wns13_destruct_BarEvNUC3BarE(Bar* _this) {" },
        { "",                         "return;" },
        { "",                         "}" },
        { "",                         "\n" },
        { "",                         "Foo* _Z3wns14_construct_FooENR3FooENUC3FooE(Foo* _this) {" },
        { "",                         "_this->x = 4;" },
        { "",                         "return _this;" },
        { "",                         "}" },
        { "",                         "\n" },
        { "",                         "void _Z3wns13_destruct_FooEvNUC3FooE(Foo* _this) {" },
        { "",                         "return;" },
        { "",                         "}" },
        { "",                         "\n" },
        { "Int main() { ",            "int32_t _Z3wns4mainEl() {" },
        { "",                         "Bar __wns_temp_expression0;" },
        { "  Foo f = Bar(); ",        "Foo* f = (Foo*)(_Z3wns14_construct_BarENR3BarENUC3BarE(&__wns_temp_expression0));" },
        { "",                         "{"},
        { "",                         "int32_t __wns_ret_temp0 = f->x;" },
        { "",                         "_Z3wns13_destruct_BarEvNUC3BarE(&__wns_temp_expression0);" },
        { "  return f.x;",            "return __wns_ret_temp0;" },
        { "",                         "}" },
        { "}",                        "}" },
      },
      {
        { "class Foo{ Int x = 4; }", "typedef struct {" },
        { "",                         "  int32_t x;" },
        { "",                         "} Foo;" },
        { "",                         "\n" },
        { "class Bar : Foo { .x = 7; }",     "typedef struct {" },
        { "",                         "  int32_t x;" },
        { "",                         "} Bar;" },
        { "",                         "\n" },
        { "",                         "Bar* _Z3wns14_construct_BarENR3BarENUC3BarE(Bar* _this) {" },
        { "",                         "_this->x = 7;" },
        { "",                         "return _this;" },
        { "",                         "}" },
        { "",                         "\n" },
        { "",                         "void _Z3wns13_destruct_BarEvNUC3BarE(Bar* _this) {" },
        { "",                         "return;" },
        { "",                         "}" },
        { "",                         "\n" },
        { "",                         "Foo* _Z3wns14_construct_FooENR3FooENUC3FooE(Foo* _this) {" },
        { "",                         "_this->x = 4;" },
        { "",                         "return _this;" },
        { "",                         "}" },
        { "",                         "\n" },
        { "",                         "void _Z3wns13_destruct_FooEvNUC3FooE(Foo* _this) {" },
        { "",                         "return;" },
        { "",                         "}" },
        { "",                         "\n" },
        { "Int fnc(Foo f) {",         "int32_t _Z3wns3fncElNP3FooE(Foo* f) {" },
        { " return f.x;",             "return f->x;" },
        { "}",                        "}" },
        { "",                         "\n" },
        { "Int main() { ",            "int32_t _Z3wns4mainEl() {" },
        { "",                         "Bar __wns_temp_expression0;" },
        { "  Bar f = Bar(); ",        "Bar* f = _Z3wns14_construct_BarENR3BarENUC3BarE(&__wns_temp_expression0);" },
        { "",                         "{" },
        { "",                         "int32_t __wns_ret_temp0 = _Z3wns3fncElNP3FooE((Foo*)(f));" },
        { "",                         "_Z3wns13_destruct_BarEvNUC3BarE(&__wns_temp_expression0);" },
        { "  return fnc(f);",         "return __wns_ret_temp0;" },
        { "",                         "}" },
        { "}",                        "}" },
      },
      {
        { "class Foo {",              "typedef struct {" },
        { "Int x = 4;",               "  int32_t x;" },
        { "Int fnc() { return x; }",  "" },
        { "}",                         "} Foo;" },
        { "",                         "\n" },
        { "class Bar : Foo { .x = 7; }",     "typedef struct {" },
        { "",                         "  int32_t x;" },
        { "",                         "} Bar;" },
        { "",                         "\n" },
        { "",                         "Bar* _Z3wns14_construct_BarENR3BarENUC3BarE(Bar* _this) {" },
        { "",                         "_this->x = 7;" },
        { "",                         "return _this;" },
        { "",                         "}" },
        { "",                         "\n" },
        { "",                         "void _Z3wns13_destruct_BarEvNUC3BarE(Bar* _this) {" },
        { "",                         "return;" },
        { "",                         "}" },
        { "",                         "\n" },
        { "",                         "Foo* _Z3wns14_construct_FooENR3FooENUC3FooE(Foo* _this) {" },
        { "",                         "_this->x = 4;" },
        { "",                         "return _this;" },
        { "",                         "}" },
        { "",                         "\n" },
        { "",                         "void _Z3wns13_destruct_FooEvNUC3FooE(Foo* _this) {" },
        { "",                         "return;" },
        { "",                         "}" },
        { "",                         "\n" },
        { "",                         "int32_t _Z3wns3fncElNR3FooE(Foo* _this) {" },
        { "",                         "return _this->x;" },
        { "",                         "}" },
        { "",                         "\n" },
        { "Int main() { ",            "int32_t _Z3wns4mainEl() {" },
        { "",                         "Bar __wns_temp_expression0;" },
        { "  Bar f = Bar(); ",        "Bar* f = _Z3wns14_construct_BarENR3BarENUC3BarE(&__wns_temp_expression0);" },
        { "",                         "{" },
        { "",                         "int32_t __wns_ret_temp0 = _Z3wns3fncElNR3FooE((Foo*)(f));" },
        { "",                         "_Z3wns13_destruct_BarEvNUC3BarE(&__wns_temp_expression0);" },
        { "  return f.fnc();",        "return __wns_ret_temp0;" },
        { "",                         "}" },
        { "}",                        "}" },
      },
})));
// clang-format on

// clang-format off
INSTANTIATE_TEST_CASE_P(
  virtual_calls, c_translator_direct_translation_test,
  ::testing::ValuesIn(
    std::vector<std::vector<source_pair>>({
      {
        {"",                      "typedef struct {" },
        {"",                      "int32_t(*_0)(Foo*);" },
        {"",                      "} _vtable_Foo;" },
        {"",                      "\n" },
        {"",                      "const _vtable_Foo _c_vtable_Foo = {" },
        {"",                      "   &_Z3wns1yElNR3FooE"},
        {"",                      "};" },
        {"",                      "\n" },
        { "class Foo {",           "typedef struct {" },
        { "",                      "  const _vtable_Foo* _vtable;"},
        { "  Int x = 0;",          "  int32_t x;" },
        { "  virtual Int y() {",   "" },
        { "    return x;",         "" },
        { "  }",                   "" },
        { "}",                     "} Foo;" },
        { "",                      "\n" },
        { "",                      "Foo* _Z3wns14_construct_FooENR3FooENUC3FooE(Foo* _this) {" },
        { "",                      "_this->_vtable = &_c_vtable_Foo;"},
        { "",                      "_this->x = 0;" },
        { "",                      "return _this;" },
        { "",                      "}" },
        { "",                      "\n" },
        { "",                      "void _Z3wns13_destruct_FooEvNUC3FooE(Foo* _this) {" },
        { "",                      "return;" },
        { "",                      "}" },
        { "",                      "\n" },
        { "",                      "int32_t _Z3wns1yElNR3FooE(Foo* _this) {"},
        { "",                      "return _this->x;"},
        { "",                      "}" },
        { "",                      "\n" },
        { "Int main(Int x) {",     "int32_t _Z3wns4mainEll(int32_t x) {" },
        { " Foo f = Foo();",       "Foo __wns_temp_expression0;" },
        { "",                      "Foo* f = _Z3wns14_construct_FooENR3FooENUC3FooE(&__wns_temp_expression0);" },
        { "",                      "{" },
        { "",                      "Foo* __wns_c_temp0 = f;"},
        { "",                      "int32_t __wns_ret_temp0 = (*__wns_c_temp0->_vtable->_0)(__wns_c_temp0);" },
        { "",                      "_Z3wns13_destruct_FooEvNUC3FooE(&__wns_temp_expression0);" },
        { " return f.y();",        "return __wns_ret_temp0;" },
        { "",                      "}" },
        { "}",                     "}" },
      },
      {
        {"",                      "typedef struct {" },
        {"",                      "int32_t(*_0)(Foo*);" },
        {"",                      "} _vtable_Foo;" },
        {"",                      "\n" },
        {"",                      "const _vtable_Foo _c_vtable_Foo = {" },
        {"",                      "   &_Z3wns1yElNR3FooE"},
        {"",                      "};" },
        {"",                      "\n" },
        { "class Foo {",          "typedef struct {" },
        { "",                      "  const _vtable_Foo* _vtable;" },
        { "  Int x = 0;",          "  int32_t x;" },
        { "  virtual Int y() {",   "" },
        { "    return x;",         "" },
        { "  }",                   "" },
        { "}",                     "} Foo;" },
        { "",                      "\n" },
        { "",                      "Foo* _Z3wns14_construct_FooENR3FooENUC3FooE(Foo* _this) {" },
        { "",                      "_this->_vtable = &_c_vtable_Foo;"},
        { "",                      "_this->x = 0;" },
        { "",                      "return _this;" },
        { "",                      "}" },
        { "",                      "\n" },
        { "",                      "void _Z3wns13_destruct_FooEvNUC3FooE(Foo* _this) {" },
        { "",                      "return;" },
        { "",                      "}" },
        { "",                      "\n" },
        { "",                      "int32_t _Z3wns1yElNR3FooE(Foo* _this) {"},
        { "",                      "return _this->x;"},
        { "",                      "}" },
        { "",                      "\n" },
        { "Int main(Int x) {",     "int32_t _Z3wns4mainEll(int32_t x) {" },
        { "",                      "Foo __wns_temp_expression0;" },
        { "",                      "{" },
        { "",                      "Foo* __wns_c_temp0 = _Z3wns14_construct_FooENR3FooENUC3FooE(&__wns_temp_expression0);"},
        { "",                      "int32_t __wns_ret_temp0 = (*__wns_c_temp0->_vtable->_0)(__wns_c_temp0);" },
        { "",                      "_Z3wns13_destruct_FooEvNUC3FooE(&__wns_temp_expression0);" },
        { " return Foo().y();",    "return __wns_ret_temp0;" },
        { "",                      "}" },
        { "}",                     "}" }
      },
})));
// clang-format on

// clang-format off
INSTANTIATE_TEST_CASE_P(
    function_calls, c_translator_direct_translation_test,
    ::testing::ValuesIn(
       std::vector<std::vector<source_pair>>({
          {
            {"Int foo() {",           "int32_t _Z3wns3fooEl() {"     },
            {"  return 4;",           "return 4;"                    },
            {"}",                     "}"                            },
            {"",                      "\n"                           },
            {"Int main(Int x) {",     "int32_t _Z3wns4mainEll(int32_t x) {"   },
            {"  return foo() + x;",   "return (_Z3wns3fooEl() + x);" },
            {"}",                     "}"                            }
          },
          {
            {"Int foo(Int x) {",      "int32_t _Z3wns3fooEll(int32_t x) {"    },
            {"  return x + 4;",       "return (x + 4);"              },
            {"}",                     "}"                            },
            {"",                      "\n"                           },
            {"Int main(Int x) {",     "int32_t _Z3wns4mainEll(int32_t x) {"   },
            {"  return foo(x);",      "return _Z3wns3fooEll(x);"     },
            {"}",                     "}"                            }
          },
         {
            {"Int foo(Int x, Int y){","int32_t _Z3wns3fooElll(int32_t x, int32_t y) {"    },
            {"  return x + y;",       "return (x + y);"              },
            {"}",                     "}"                            },
            {"",                      "\n"                           },
            {"Int main(Int x) {",     "int32_t _Z3wns4mainEll(int32_t x) {"   },
            {"  return foo(x, x);",   "return _Z3wns3fooElll(x, x);" },
            {"}",                     "}"                            }
          },
})));
// clang-format on

// clang-format off
INSTANTIATE_TEST_CASE_P(
    temporary_objects, c_translator_direct_translation_test,
    ::testing::ValuesIn(
        std::vector<std::vector<source_pair>>({
          {
            {"struct Foo {",          "typedef struct {"  },
            {"  Int x = 4;",          "  int32_t x;"      },
            {"}",                     "} Foo;"             },
            {"",                      "\n"                           },
            {"",                      "Foo* _Z3wns14_construct_FooENR3FooENUC3FooE(Foo* _this) {" },
            {"",                      "_this->x = 4;"               },
            {"",                      "return _this;"                },
            {"",                      "}"                            },
            {"",                      "\n"                           },
            {"",                      "void _Z3wns13_destruct_FooEvNUC3FooE(Foo* _this) {"   },
            {"",                      "return;"                      },
            {"",                      "}"                            },
            {"",                      "\n"                           },
            {"Int main(Int x) {",     "int32_t _Z3wns4mainEll(int32_t x) {"  },
            {" return Foo().x;",      "Foo __wns_temp_expression0;"             },
            {"",                      "{"                            },
            {"",                      "int32_t __wns_ret_temp0 = _Z3wns14_construct_FooENR3FooENUC3FooE(&__wns_temp_expression0)->x;"},
            {"",                      "_Z3wns13_destruct_FooEvNUC3FooE(&__wns_temp_expression0);"    },
            {"}",                     "return __wns_ret_temp0;"  },
            {"",                      "}"  },
            {"",                      "}"  },
          }
})));
// clang-format on

// clang-format off
INSTANTIATE_TEST_CASE_P(
    shared_objects, c_translator_direct_translation_test,
    ::testing::ValuesIn(
        std::vector<std::vector<source_pair>>({
          {
            {"struct Foo {",          "typedef struct {"             },
            {"  Int x = 0;",          "  int32_t x;"                 },
            {"}",                     "} Foo;"                       },
            {"",                      "\n"                           },
            {"",                      "Foo* _Z3wns14_construct_FooENR3FooENUC3FooE(Foo* _this) {"   },
            {"",                      "_this->x = 0;"                },
            {"",                      "return _this;"                },
            {"",                      "}"                            },
            {"",                      "\n"                           },
            {"",                      "void _Z3wns13_destruct_FooEvNUC3FooE(Foo* _this) {"   },
            {"",                      "return;"                      },
            {"",                      "}"                            },
            {"",                      "\n"                           },
            {"Int main(Int x) {",     "int32_t _Z3wns4mainEll(int32_t x) {"   },
            {" shared Foo f = shared Foo();", "Foo* f = (Foo*)_Z3wns14_assign_sharedEvpvpvp((void*)_Z3wns14_construct_FooENR3FooENUC3FooE((Foo*)_Z3wns16_allocate_sharedEvpsfp(sizeof(Foo), &_Z3wns13_destruct_FooEvNUC3FooE)), NULL);"       },
            {"",                      "{"                            },
            {"",                      "int32_t __wns_ret_temp0 = 4;" },
            {"",                      "_Z3wns13_deref_sharedEvvp((void*)f);"},
            {" return 4;",            "return __wns_ret_temp0;"      },
            {"",                      "}"},
            {"}",                     "}"                            }
          },
          {
            {"struct Foo {",          "typedef struct {"             },
            {"  Int x = 42;",          "  int32_t x;"                 },
            {"}",                     "} Foo;"                       },
            {"",                      "\n"                           },
            {"",                      "Foo* _Z3wns14_construct_FooENR3FooENUC3FooE(Foo* _this) {"   },
            {"",                      "_this->x = 42;"                },
            {"",                      "return _this;"                },
            {"",                      "}"                            },
            {"",                      "\n"                           },
            {"",                      "void _Z3wns13_destruct_FooEvNUC3FooE(Foo* _this) {"   },
            {"",                      "return;"                      },
            {"",                      "}"                            },
            {"",                      "\n"                           },
            {"Int main(Int x) {",     "int32_t _Z3wns4mainEll(int32_t x) {"   },
            {"",                      "Foo* __wns_temp_expression0 = (Foo*)_Z3wns14_assign_sharedEvpvpvp((void*)_Z3wns14_construct_FooENR3FooENUC3FooE((Foo*)_Z3wns16_allocate_sharedEvpsfp(sizeof(Foo), &_Z3wns13_destruct_FooEvNUC3FooE)), NULL);"},
            {"",                      "{"},
            {"",                      "int32_t __wns_ret_temp0 = __wns_temp_expression0->x;" },
            {"",                      "_Z3wns13_deref_sharedEvvp((void*)__wns_temp_expression0);" },
            {" return (shared Foo()).x;", "return __wns_ret_temp0;"      },
            {"",                      "}"},
            {"}",                     "}"                            }
          },
          {
            {"struct Foo {",          "typedef struct {"             },
            {"  Int x = 0;",          "  int32_t x;"                 },
            {"}",                     "} Foo;"                       },
            {"",                      "\n"                           },
            {"",                      "Foo* _Z3wns14_construct_FooENR3FooENUC3FooE(Foo* _this) {"   },
            {"",                      "_this->x = 0;"                },
            {"",                      "return _this;"                },
            {"",                      "}"                            },
            {"",                      "\n"                           },
            {"",                      "void _Z3wns13_destruct_FooEvNUC3FooE(Foo* _this) {"   },
            {"",                      "return;"                      },
            {"",                      "}"                            },
            {"",                      "\n"                           },
            {"Int main(Int x) {",     "int32_t _Z3wns4mainEll(int32_t x) {"   },
            {" shared Foo f = shared Foo();", "Foo* f = (Foo*)_Z3wns14_assign_sharedEvpvpvp((void*)_Z3wns14_construct_FooENR3FooENUC3FooE((Foo*)_Z3wns16_allocate_sharedEvpsfp(sizeof(Foo), &_Z3wns13_destruct_FooEvNUC3FooE)), NULL);" },
            {" f = shared Foo();",     "f = (Foo*)_Z3wns14_assign_sharedEvpvpvp((void*)_Z3wns14_construct_FooENR3FooENUC3FooE((Foo*)_Z3wns16_allocate_sharedEvpsfp(sizeof(Foo), &_Z3wns13_destruct_FooEvNUC3FooE)), (void*)f);" },
            {"",                      "{"                            },
            {"",                      "int32_t __wns_ret_temp0 = 4;" },
            {"",                      "_Z3wns13_deref_sharedEvvp((void*)f);"},
            {" return 4;",            "return __wns_ret_temp0;"      },
            {"",                      "}"},
            {"}",                     "}"                            }
          },
          {
            {"struct Foo {",          "typedef struct {"             },
            {"  Int x = 0;",          "  int32_t x;"                 },
            {"}",                     "} Foo;"                       },
            {"",                      "\n"                           },
            {"",                      "Foo* _Z3wns14_construct_FooENR3FooENUC3FooE(Foo* _this) {"   },
            {"",                      "_this->x = 0;"                },
            {"",                      "return _this;"                },
            {"",                      "}"                            },
            {"",                      "\n"                           },
            {"",                      "void _Z3wns13_destruct_FooEvNUC3FooE(Foo* _this) {"   },
            {"",                      "return;"                      },
            {"",                      "}"                            },
            {"",                      "\n"                           },
            {"shared Foo blah() {",   "Foo* _Z3wns4blahENRR3FooE() {"},
            {" return shared Foo();", "return _Z3wns14_construct_FooENR3FooENUC3FooE((Foo*)_Z3wns16_allocate_sharedEvpsfp(sizeof(Foo), &_Z3wns13_destruct_FooEvNUC3FooE));"},
            {"}",                     "}"},
            {"",                      "\n"                           },
            {"Int main(Int x) {",     "int32_t _Z3wns4mainEll(int32_t x) {"   },
            {" shared Foo f = blah();", "Foo* f = (Foo*)_Z3wns14_assign_sharedEvpvpvp((void*)_Z3wns4blahENRR3FooE(), NULL);"       },
            {"",                      "{"                            },
            {"",                      "int32_t __wns_ret_temp0 = 4;" },
            {"",                      "_Z3wns13_deref_sharedEvvp((void*)f);"},
            {" return 4;",            "return __wns_ret_temp0;"      },
            {"",                      "}"},
            {"}",                     "}"                            }
          },
          {
            {"struct Foo {",          "typedef struct {"             },
            {"  Int x = 0;",          "  int32_t x;"                 },
            {"}",                     "} Foo;"                       },
            {"",                      "\n"                           },
            {"",                      "Foo* _Z3wns14_construct_FooENR3FooENUC3FooE(Foo* _this) {"   },
            {"",                      "_this->x = 0;"                },
            {"",                      "return _this;"                },
            {"",                      "}"                            },
            {"",                      "\n"                           },
            {"",                      "void _Z3wns13_destruct_FooEvNUC3FooE(Foo* _this) {"   },
            {"",                      "return;"                      },
            {"",                      "}"                            },
            {"",                      "\n"                           },
            {"shared Foo blah() {",   "Foo* _Z3wns4blahENRR3FooE() {"},
            {" shared Foo f = shared Foo();", "Foo* f = (Foo*)_Z3wns14_assign_sharedEvpvpvp((void*)_Z3wns14_construct_FooENR3FooENUC3FooE((Foo*)_Z3wns16_allocate_sharedEvpsfp(sizeof(Foo), &_Z3wns13_destruct_FooEvNUC3FooE)), NULL);"},
            {"",                      "{"                            },
            {"",                      "Foo* __wns_ret_temp0 = (Foo*)_Z3wns14_assign_sharedEvpvpvp((void*)f, NULL);" },
            {"",                      "_Z3wns13_deref_sharedEvvp((void*)f);"},
            {" return f;",            "return (Foo*)_Z3wns14_return_sharedEvpvp((void*)__wns_ret_temp0);"      },
            {"",                      "}"},
            {"}",                     "}"},
            {"",                      "\n"                           },
            {"Int main(Int x) {",     "int32_t _Z3wns4mainEll(int32_t x) {"   },
            {" shared Foo f = blah();", "Foo* f = (Foo*)_Z3wns14_assign_sharedEvpvpvp((void*)_Z3wns4blahENRR3FooE(), NULL);"       },
            {"",                      "{"                            },
            {"",                      "int32_t __wns_ret_temp1 = 4;" },
            {"",                      "_Z3wns13_deref_sharedEvvp((void*)f);"},
            {" return 4;",            "return __wns_ret_temp1;"      },
            {"",                      "}"},
            {"}",                     "}"                            }
          },
})));

// clang-format on

// clang-format off
INSTANTIATE_TEST_CASE_P(
    function_taking_struct, c_translator_direct_translation_test,
    ::testing::ValuesIn(
        std::vector<std::vector<source_pair>>({
          {
            {"struct Foo {",          "typedef struct {"  },
            {"  Int x = 4;",          "  int32_t x;"      },
            {"}",                     "} Foo;"             },
            {"",                      "\n"                           },
            {"",                      "Foo* _Z3wns14_construct_FooENR3FooENUC3FooE(Foo* _this) {" },
            {"",                      "_this->x = 4;"               },
            {"",                      "return _this;"                },
            {"",                      "}"                            },
            {"",                      "\n"                           },
            {"",                      "void _Z3wns13_destruct_FooEvNUC3FooE(Foo* _this) {"   },
            {"",                      "return;"                      },
            {"",                      "}"                            },
            {"",                      "\n"                           },
            {"Int blah(Foo f) {",     "int32_t _Z3wns4blahElNP3FooE(Foo* f) {"},
            {" return f.x;",          "return f->x;"  },
            {"}",                     "}"},
            {"",                      "\n"},
            {"Int main(Int x) {",     "int32_t _Z3wns4mainEll(int32_t x) {"   },
            {" Foo f = Foo();",       "Foo __wns_temp_expression0;"             },
            {"",                      "Foo* f = _Z3wns14_construct_FooENR3FooENUC3FooE(&__wns_temp_expression0);"       },
            {"",                      "{"                            },
            {"",                      "int32_t __wns_ret_temp0 = _Z3wns4blahElNP3FooE(f);" },
            {"",                      "_Z3wns13_destruct_FooEvNUC3FooE(&__wns_temp_expression0);"},
            {" return blah(f);",       "return __wns_ret_temp0;"      },
            {"",                      "}"},
            {"}",                     "}"                            }
          }
})));

// clang-format off
INSTANTIATE_TEST_CASE_P(
    struct_in_struct, c_translator_direct_translation_test,
    ::testing::ValuesIn(
        std::vector<std::vector<source_pair>>({
          {
            {"struct Foo {",          "typedef struct {"  },
            {"  Int x = 4;",          "  int32_t x;"      },
            {"}",                     "} Foo;"            },
            {"",                      "\n"                },
            {"struct Bar {",          "typedef struct {"  },
            {"  Foo f = Foo();",      "  Foo* f;"         },
            {"",                      "  Foo __f;"        },
            {"}",                     "} Bar;"            },
            {"",                      "\n"                },
            {"",                      "Bar* _Z3wns14_construct_BarENR3BarENUC3BarE(Bar* _this) {" },
            {"",                      "_this->f = _Z3wns14_construct_FooENR3FooENUC3FooE(&_this->__f);"},
            {"",                      "return _this;"                },
            {"",                      "}"                            },
            {"",                      "\n"                           },
            {"",                      "void _Z3wns13_destruct_BarEvNUC3BarE(Bar* _this) {"   },
            {"",                      "_Z3wns13_destruct_FooEvNUC3FooE(&_this->__f);"           },
            {"",                      "return;"                      },
            {"",                      "}"                            },
            {"",                      "\n"                           },
            {"",                      "Foo* _Z3wns14_construct_FooENR3FooENUC3FooE(Foo* _this) {" },
            {"",                      "_this->x = 4;"               },
            {"",                      "return _this;"                },
            {"",                      "}"                            },
            {"",                      "\n"                           },
            {"",                      "void _Z3wns13_destruct_FooEvNUC3FooE(Foo* _this) {"   },
            {"",                      "return;"                      },
            {"",                      "}"                            }
          },
          {
            {"struct Foo {",          "typedef struct {"  },
            {"  Int x = 4;",          "  int32_t x;"      },
            {"}",                     "} Foo;"            },
            {"",                      "\n"                },
            {"struct Bar {",          "typedef struct {"  },
            {"  Foo f = Foo();",      "  Foo* f;"         },
            {"",                      "  Foo __f;"        },
            {"}",                     "} Bar;"            },
            {"",                      "\n"                },
            {"",                      "Bar* _Z3wns14_construct_BarENR3BarENUC3BarE(Bar* _this) {" },
            {"",                      "_this->f = _Z3wns14_construct_FooENR3FooENUC3FooE(&_this->__f);"},
            {"",                      "return _this;"                },
            {"",                      "}"                            },
            {"",                      "\n"                           },
            {"",                      "void _Z3wns13_destruct_BarEvNUC3BarE(Bar* _this) {"   },
            {"",                      "_Z3wns13_destruct_FooEvNUC3FooE(&_this->__f);"           },
            {"",                      "return;"                      },
            {"",                      "}"                            },
            {"",                      "\n"                           },
            {"",                      "Foo* _Z3wns14_construct_FooENR3FooENUC3FooE(Foo* _this) {" },
            {"",                      "_this->x = 4;"               },
            {"",                      "return _this;"                },
            {"",                      "}"                            },
            {"",                      "\n"                           },
            {"",                      "void _Z3wns13_destruct_FooEvNUC3FooE(Foo* _this) {"   },
            {"",                      "return;"                      },
            {"",                      "}"                            },
            {"",                      "\n"                           },
            {"Int main(Int x) {",     "int32_t _Z3wns4mainEll(int32_t x) {"   },
            {" Bar b = Bar();",       "Bar __wns_temp_expression0;"             },
            {"",                      "Bar* b = _Z3wns14_construct_BarENR3BarENUC3BarE(&__wns_temp_expression0);"       },
            {"",                      "{"                            },
            {"",                      "int32_t __wns_ret_temp0 = b->f->x;" },
            {"",                      "_Z3wns13_destruct_BarEvNUC3BarE(&__wns_temp_expression0);"},
            {" return b.f.x;",        "return __wns_ret_temp0;"      },
            {"",                      "}"},
            {"}",                     "}"                            }
          },
          {
            {"struct Foo {",          "typedef struct {"  },
            {"  Int x = 4;",          "  int32_t x;"      },
            {"}",                     "} Foo;"            },
            {"",                      "\n"                },
            {"struct Bar {",          "typedef struct {"  },
            {"  shared Foo f = shared Foo();", "  Foo* f;"         },
            {"}",                     "} Bar;"            },
            {"",                      "\n"                },
            {"",                      "Bar* _Z3wns14_construct_BarENR3BarENUC3BarE(Bar* _this) {" },
            {"",                      "_this->f = (Foo*)_Z3wns14_assign_sharedEvpvpvp((void*)_Z3wns14_construct_FooENR3FooENUC3FooE((Foo*)_Z3wns16_allocate_sharedEvpsfp(sizeof(Foo), &_Z3wns13_destruct_FooEvNUC3FooE)), NULL);"},
            {"",                      "return _this;"                },
            {"",                      "}"                            },
            {"",                      "\n"                           },
            {"",                      "void _Z3wns13_destruct_BarEvNUC3BarE(Bar* _this) {"   },
            {"",                      "_Z3wns13_deref_sharedEvvp((void*)_this->f);"           },
            {"",                      "return;"                      },
            {"",                      "}"                            },
            {"",                      "\n"                           },
            {"",                      "Foo* _Z3wns14_construct_FooENR3FooENUC3FooE(Foo* _this) {" },
            {"",                      "_this->x = 4;"               },
            {"",                      "return _this;"                },
            {"",                      "}"                            },
            {"",                      "\n"                           },
            {"",                      "void _Z3wns13_destruct_FooEvNUC3FooE(Foo* _this) {"   },
            {"",                      "return;"                      },
            {"",                      "}"                            },
            {"",                      "\n"                           },
            {"Int main(Int x) {",     "int32_t _Z3wns4mainEll(int32_t x) {"   },
            {" Bar b = Bar();",       "Bar __wns_temp_expression0;"             },
            {"",                      "Bar* b = _Z3wns14_construct_BarENR3BarENUC3BarE(&__wns_temp_expression0);"       },
            {"",                      "{"                            },
            {"",                      "int32_t __wns_ret_temp0 = b->f->x;" },
            {"",                      "_Z3wns13_destruct_BarEvNUC3BarE(&__wns_temp_expression0);"},
            {" return b.f.x;",        "return __wns_ret_temp0;"      },
            {"",                      "}"},
            {"}",                     "}"                            }
          },
          {
            {"struct Foo {",          "typedef struct {"  },
            {"  Int x = 4;",          "  int32_t x;"      },
            {"}",                     "} Foo;"            },
            {"",                      "\n"                },
            {"struct Bar {",          "typedef struct {"  },
            {"  shared Foo f = shared Foo();", "  Foo* f;"         },
            {"}",                     "} Bar;"            },
            {"",                      "\n"                },
            {"",                      "Bar* _Z3wns14_construct_BarENR3BarENUC3BarE(Bar* _this) {" },
            {"",                      "_this->f = (Foo*)_Z3wns14_assign_sharedEvpvpvp((void*)_Z3wns14_construct_FooENR3FooENUC3FooE((Foo*)_Z3wns16_allocate_sharedEvpsfp(sizeof(Foo), &_Z3wns13_destruct_FooEvNUC3FooE)), NULL);"},
            {"",                      "return _this;"                },
            {"",                      "}"                            },
            {"",                      "\n"                           },
            {"",                      "void _Z3wns13_destruct_BarEvNUC3BarE(Bar* _this) {"   },
            {"",                      "_Z3wns13_deref_sharedEvvp((void*)_this->f);"           },
            {"",                      "return;"                      },
            {"",                      "}"                            },
            {"",                      "\n"                           },
            {"",                      "Foo* _Z3wns14_construct_FooENR3FooENUC3FooE(Foo* _this) {" },
            {"",                      "_this->x = 4;"               },
            {"",                      "return _this;"                },
            {"",                      "}"                            },
            {"",                      "\n"                           },
            {"",                      "void _Z3wns13_destruct_FooEvNUC3FooE(Foo* _this) {"   },
            {"",                      "return;"                      },
            {"",                      "}"                            },
            {"",                      "\n"                           },
            {"Int main(Int x) {",     "int32_t _Z3wns4mainEll(int32_t x) {"   },
            {" shared Bar b = shared Bar();", "Bar* b = (Bar*)_Z3wns14_assign_sharedEvpvpvp((void*)_Z3wns14_construct_BarENR3BarENUC3BarE((Bar*)_Z3wns16_allocate_sharedEvpsfp(sizeof(Bar), &_Z3wns13_destruct_BarEvNUC3BarE)), NULL);" },
            {"",                      "{"                            },
            {"",                      "int32_t __wns_ret_temp0 = b->f->x;" },
            {"",                      "_Z3wns13_deref_sharedEvvp((void*)b);"},
            {" return b.f.x;",        "return __wns_ret_temp0;"      },
            {"",                      "}"},
            {"}",                     "}"                            }
          }
})));
// clang-format on

// clang-format off
INSTANTIATE_TEST_CASE_P(
  member_function_test, c_translator_direct_translation_test,
  ::testing::ValuesIn(
    std::vector<std::vector<source_pair>>({
      {
        { "class Foo {",          "typedef struct {" },
        { "  Int x = 4;",          "  int32_t x;" },
        { "  Void doF() {",        "" },
        { "    x = x + 4;",        "" },
        { "  }",                   "" },
        { "  Void doY() { ",       "" },
        { "     doF();",           "" },
        { "  }",                   "" },
        { "}",                     "} Foo;" },
        { "",                      "\n" },
        { "",                      "Foo* _Z3wns14_construct_FooENR3FooENUC3FooE(Foo* _this) {" },
        { "",                      "_this->x = 4;" },
        { "",                      "return _this;" },
        { "",                      "}" },
        { "",                      "\n" },
        { "",                      "void _Z3wns13_destruct_FooEvNUC3FooE(Foo* _this) {" },
        { "",                      "return;" },
        { "",                      "}" },
        { "",                      "\n"},
        { "",                      "void _Z3wns3doYEvNR3FooE(Foo* _this) {"},
        { "",                      "_Z3wns3doFEvNR3FooE(_this);"},
        { "",                      "}"},
        { "",                      "\n"},
        { "",                      "void _Z3wns3doFEvNR3FooE(Foo* _this) {"},
        { "",                      "_this->x = (_this->x + 4);"},
        { "",                      "}"},
        { "",                      "\n" },
        { "Int blah(Foo x) {",     "int32_t _Z3wns4blahElNP3FooE(Foo* x) {" },
        { " return x.x;",          "return x->x;" },
        { "}",                     "}" },
        { "",                      "\n" },
        { "Int main(Int x) {",     "int32_t _Z3wns4mainEll(int32_t x) {" },
        { " Foo f = Foo();",       "Foo __wns_temp_expression0;" },
        { "",                      "Foo* f = _Z3wns14_construct_FooENR3FooENUC3FooE(&__wns_temp_expression0);" },
        { " f.doF();",             "_Z3wns3doFEvNR3FooE(f);" },
        { "",                      "{" },
        { "",                      "int32_t __wns_ret_temp0 = _Z3wns4blahElNP3FooE(f);" },
        { "",                      "_Z3wns13_destruct_FooEvNUC3FooE(&__wns_temp_expression0);" },
        { " return blah(f);",      "return __wns_ret_temp0;" },
        { "",                      "}" },
        { "}",                     "}" }
      }
})));
// clang-format on

// clang-format off
INSTANTIATE_TEST_CASE_P(
  relative_construction, c_translator_direct_translation_test,
  ::testing::ValuesIn(
    std::vector<std::vector<source_pair>>({
      {
        { "struct Foo {",         "typedef struct {" },
        { "  Int x = 4;",         "  int32_t x;"  },
        { "  Int y = x + 4;",     "  int32_t y;"  },
        { "}",                    "} Foo;" },
        { "",                     "\n" },
        { "",                     "Foo* _Z3wns14_construct_FooENR3FooENUC3FooE(Foo* _this) {" },
        { "",                     "_this->x = 4;" },
        { "",                     "_this->y = (_this->x + 4);" },
        { "",                     "return _this;" },
        { "",                     "}" },
        { "",                     "\n" },
        { "",                     "void _Z3wns13_destruct_FooEvNUC3FooE(Foo* _this) {" },
        { "",                     "return;" },
        { "",                     "}" },
        { "",                     "\n" },
        { "",                     "" },
        { "Int main(Int x) {",    "int32_t _Z3wns4mainEll(int32_t x) {" },
        { "Foo f = Foo();",       "Foo __wns_temp_expression0;" },
        { "",                     "Foo* f = _Z3wns14_construct_FooENR3FooENUC3FooE(&__wns_temp_expression0);" },
        { "",                     "{" },
        { "",                     "int32_t __wns_ret_temp0 = (f->y + x);" },
        { "",                     "_Z3wns13_destruct_FooEvNUC3FooE(&__wns_temp_expression0);" },
        { "return f.y + x;",      "return __wns_ret_temp0;" },
        { "",                     "}" },
        { "}",                    "}"}
      }
})));
// clang-format on

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
