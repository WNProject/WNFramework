// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNScripting/inc/WNASTWalker.h"
#include "WNContainers/inc/WNHashMap.h"
#include "WNContainers/inc/WNString.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNCore/inc/WNPair.h"
#include "WNExecutableTest/inc/WNTestHarness.h"
#include "WNFileSystem/inc/WNFactory.h"
#include "WNLogging/inc/WNLog.h"
#include "WNScripting/inc/WNASTPasses.h"
#include "WNScripting/test/inc/Common.h"
#include "WNScripting/test/inc/Common.h"

using ::testing::_;
using ::testing::Eq;
using ::testing::Matcher;
using ::testing::TypedEq;

class mock_walk {
public:
  MOCK_METHOD1(walk_expression, void(const wn::scripting::expression*));
  MOCK_METHOD1(walk_instruction, void(const wn::scripting::instruction*));
  MOCK_METHOD1(
      walk_instruction, void(const wn::scripting::return_instruction*));
  MOCK_METHOD1(walk_function, void(const wn::scripting::function*));
  MOCK_METHOD1(walk_parameter, void(const wn::scripting::parameter*));
  MOCK_METHOD1(walk_script_file, void(const wn::scripting::script_file*));
  MOCK_METHOD1(pre_walk_script_file, void(const wn::scripting::script_file*));
  MOCK_METHOD1(post_walk_structs, void(const wn::scripting::script_file*));
  MOCK_METHOD1(walk_type, void(const wn::scripting::type*));
  MOCK_METHOD1(
      walk_struct_definition, void(const wn::scripting::struct_definition*));
  MOCK_METHOD1(
      walk_instruction_list, void(const wn::scripting::instruction_list*));
  MOCK_METHOD1(enter_scope_block, void(const wn::scripting::node*));
  MOCK_METHOD1(leave_scope_block, void(const wn::scripting::node*));
};

template <typename T>
void RunMatcherTest(T& _t,
    std::initializer_list<wn::core::pair<wn::containers::string_view,
        wn::containers::string_view>>
        _files,
    const char* _file, size_t* _num_warnings, size_t* _num_errors) {
  wn::testing::allocator allocator;
  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory().make_mapping(
          &allocator, wn::file_system::mapping_type::memory_backed);

  mapping->initialize_files(_files);

  auto a = wn::scripting::test_parse_file(
      &allocator, _file, mapping.get(), _num_warnings, _num_errors);
  wn::scripting::ast_walker<T, true> walker(&_t);
  walker.walk_script_file(
      static_cast<const wn::scripting::script_file*>(a.get()));
}

TEST(ast_code_generator, simplest_function) {
  mock_walk walk;
  wn::scripting::ast_walker<mock_walk> walker(&walk);
  size_t num_warnings, num_errors;
  num_warnings = num_errors = 0;

  EXPECT_CALL(walk, walk_expression(_)).Times(0);
  EXPECT_CALL(walk, walk_function(_)).Times(1);
  EXPECT_CALL(walk, walk_type(_)).Times(1);
  EXPECT_CALL(walk,
      walk_instruction(Matcher<const wn::scripting::return_instruction*>(_)))
      .Times(1);
  RunMatcherTest(walk, {{"file.wns", "Void main() { return; }"}}, "file.wns",
      &num_warnings, &num_errors);
  EXPECT_THAT(num_warnings, Eq(0u));
  EXPECT_THAT(num_errors, Eq(0u));
}

TEST(ast_code_generator, type_association_test) {
  wn::testing::allocator allocator;
  wn::scripting::type_validator validator(&allocator);
  size_t num_warnings, num_errors;
  num_warnings = num_errors = 0;

  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory().make_mapping(
          &allocator, wn::file_system::mapping_type::memory_backed);

  mapping->initialize_files(
      {{"file.wns", "Int main() { return 0 + 4 * 32; }"}});

  auto a = wn::scripting::test_parse_file(
      &allocator, "file.wns", mapping.get(), &num_warnings, &num_errors);
  EXPECT_TRUE(wn::scripting::run_type_association_pass(a.get(),
      wn::logging::get_null_logger(), &validator, &num_warnings, &num_errors));

  EXPECT_THAT(num_warnings, Eq(0u));
  EXPECT_THAT(num_errors, Eq(0u));
}
