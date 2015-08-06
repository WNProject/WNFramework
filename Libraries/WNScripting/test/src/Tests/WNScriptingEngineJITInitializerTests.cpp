// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNMemory/inc/WNAllocator.h"

#include "WNScripting/inc/WNScriptingEngineFactory.h"
#include "WNScripting/inc/WNScriptingJITEngine.h"
#include "WNScripting/test/inc/Common.h"

TEST(scripting_engine_factory, creation) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::scripting::file_based_manager manager(&allocator);
  wn::scripting::jit_engine jit_engine(&allocator, &manager);
}

TEST(scripting_engine_factory, basic_parsing) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::scripting::test_file_manager manager(
      &allocator, {{"file.wns", "Int main() { return (0); }"},
                   {"file2.wns",
                    "Int main() { return (0);} \n"
                    "Void bar() { return; }"}});
  wn::scripting::jit_engine jit_engine(&allocator, &manager);
  EXPECT_EQ(wn::scripting::parse_error::ok, jit_engine.parse_file("file.wns"));
  EXPECT_EQ(wn::scripting::parse_error::ok, jit_engine.parse_file("file2.wns"));
}

TEST(scripting_engine_factory, parse_error) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::scripting::test_file_manager manager(&allocator,
                                           {{"file.wns", "Int main"}});
  wn::scripting::jit_engine jit_engine(&allocator, &manager);
  EXPECT_EQ(wn::scripting::parse_error::parse_failed,
            jit_engine.parse_file("file.wns"));
}
