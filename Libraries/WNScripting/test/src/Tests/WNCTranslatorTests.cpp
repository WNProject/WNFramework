// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/test/inc/Common.h"
#include "WNScripting/inc/WNCTranslator.h"

TEST(c_translator, simple_c_translation) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::scripting::test_file_manager manager(
      &allocator, {{"file.wns", "Void main() { return; }"}});

  scripting::c_translator translator(&allocator, &manager,
                                     WNLogging::get_null_logger());
  containers::string output_string(&allocator);
  EXPECT_EQ(wn::scripting::parse_error::ok,
            translator.translate_file("file.wns"));
  EXPECT_EQ(
      "void main() {\n"
      "return;\n"
      "}\n",
      std::string(manager.get_file("file.wns.c")->data()));
}

TEST(c_translator, multiple_c_functions) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::scripting::test_file_manager manager(&allocator,
                                           {{"file.wns",
                                             "Void main() { return; }\n"
                                             "Void foo() { return; }\n"}});

  scripting::c_translator translator(&allocator, &manager,
                                     WNLogging::get_null_logger());
  containers::string output_string(&allocator);
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

  scripting::c_translator translator(&allocator, &manager,
                                     WNLogging::get_null_logger());
  containers::string output_string(&allocator);
  EXPECT_EQ(wn::scripting::parse_error::ok,
            translator.translate_file("file.wns"));
  EXPECT_EQ(
      "void main() {\n"
      "return;\n"
      "}\n",
      std::string(manager.get_file("file.wns.c")->data()));
}

class c_int_params
    : public ::testing::TestWithParam<const char*> {
};

TEST_P(c_int_params, int_return) {
  wn::memory::default_expanding_allocator<50> allocator;
  containers::string str(&allocator);
  str += "Int main() { return ";
  str += GetParam();
  str += "; } ";

  containers::string expected(&allocator);
  expected += "int main() {\n"
              "return ";
  expected += GetParam();
  expected += ";\n}\n";
  wn::scripting::test_file_manager manager(
      &allocator, {{"file.wns", str}});

  scripting::c_translator translator(&allocator, &manager,
                                     WNLogging::get_null_logger());
  containers::string output_string(&allocator);
  EXPECT_EQ(wn::scripting::parse_error::ok,
            translator.translate_file("file.wns"));
  EXPECT_EQ(std::string(expected.c_str()),
            std::string(manager.get_file("file.wns.c")->data()));
}

INSTANTIATE_TEST_CASE_P(int_tests, c_int_params,
                        ::testing::Values("0", "-1", "-32", "-4096",
                                          "2147483647", "-2147483648"));