// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/inc/WNHashMap.h"
#include "WNContainers/inc/WNString.h"
#include "WNLogging/inc/WNLog.h"
#include "WNScripting/test/inc/Common.h"
#include "WNScripting/inc/WNASTWalker.h"
#include "WNScripting/inc/WNASTPasses.h"
#include "WNTesting/inc/WNTestHarness.h"

using ::testing::_;
using ::testing::Matcher;
using ::testing::TypedEq;

class mock_walk {
 public:
  MOCK_METHOD1(walk_expression, void(wn::scripting::expression*));
  MOCK_METHOD1(walk_instruction, void(wn::scripting::instruction*));
  MOCK_METHOD1(walk_instruction, void(wn::scripting::return_instruction*));
  MOCK_METHOD1(walk_function, void(wn::scripting::function*));
  MOCK_METHOD1(walk_parameter, void(wn::scripting::parameter*));
  MOCK_METHOD1(walk_script_file, void(wn::scripting::script_file*));
  MOCK_METHOD1(walk_type, void(wn::scripting::type*));
};

template <typename T>
void RunMatcherTest(
    T& _t,
    wn::containers::hash_map<wn::containers::string, wn::containers::string>&& _files,
    const char* _file) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::scripting::test_file_manager manager(
      &allocator, std::move(_files));

  auto a = test_parse_file(_file, &manager, &allocator);
  wn::scripting::ast_walker<T> walker(&_t);
  walker.walk_script_file(a.get());
}

TEST(ast_code_generator, simplest_function) {
  mock_walk walk;
  wn::scripting::ast_walker<mock_walk> walker(&walk);

  EXPECT_CALL(walk, walk_expression(_)).Times(0);
  EXPECT_CALL(walk, walk_function(_)).Times(1);
  EXPECT_CALL(walk, walk_type(_)).Times(1);
  EXPECT_CALL(walk,
              walk_instruction(Matcher<wn::scripting::return_instruction*>(_)))
      .Times(1);
  RunMatcherTest(walk, {{"file.wns", "Void main() { return; }"}}, "file.wns");
}

TEST(ast_code_generator, type_association_test) {
  wn::memory::default_expanding_allocator<50> allocator;
  wn::scripting::test_file_manager manager(
      &allocator, {{"file.wns", "Int main() { return 0 + 4; }"}});
  auto a = test_parse_file("file.wns", &manager, &allocator);
  EXPECT_TRUE(wn::scripting::run_type_association_pass(
      a.get(), WNLogging::get_null_logger()));
}
