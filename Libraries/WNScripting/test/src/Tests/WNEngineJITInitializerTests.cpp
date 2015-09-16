// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNMemory/inc/WNAllocator.h"

#include "WNScripting/inc/WNFactory.h"
#include "WNScripting/inc/WNJITEngine.h"
#include "WNScripting/test/inc/Common.h"

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