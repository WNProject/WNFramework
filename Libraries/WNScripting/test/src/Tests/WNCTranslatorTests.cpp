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
