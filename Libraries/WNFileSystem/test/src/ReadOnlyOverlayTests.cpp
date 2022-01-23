// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNFileSystem/inc/WNFactory.h"
#include "WNFileSystem/inc/WNMapping.h"
#include "executable_test/inc/WNTestHarness.h"

wn::file_system::mapping_ptr get_test_mapping(
    wn::file_system::factory& _f, wn::memory::allocator* _allocator) {
  // mapping 1
  //  a.txt
  //  a/b.txt
  //  c.txt
  // mapping 2
  //  b.txt
  //  a/a.txt
  //  c.txt
  static const char _0_0[] = "a.txt";
  static const char _0_1[] = "a/b.txt";
  static const char _0_2[] = "c.txt";

  static const char _1_0[] = "b.txt";
  static const char _1_1[] = "a/a.txt";
  static const char _1_2[] = "c.txt";
  static const char _1_2_d[] = "c_2.txt";
  static const char _1_3[] = "b/a.txt";

  static const wn::core::pair<wn::containers::string_view,
      wn::containers::string_view>
      file_data_0[] = {{wn::containers::string_view(_0_0),
                           wn::containers::string_view(_0_0)},
          {wn::containers::string_view(_0_1),
              wn::containers::string_view(_0_1)},
          {wn::containers::string_view(_0_2),
              wn::containers::string_view(_0_2)}};
  static const wn::core::pair<wn::containers::string_view,
      wn::containers::string_view>
      file_data_1[] = {{wn::containers::string_view(_1_0),
                           wn::containers::string_view(_1_0)},
          {wn::containers::string_view(_1_1),
              wn::containers::string_view(_1_1)},
          {wn::containers::string_view(_1_2),
              wn::containers::string_view(_1_2_d)},
          {wn::containers::string_view(_1_3),
              wn::containers::string_view(_1_3)}};
  wn::file_system::mapping_ptr mappings[2] = {
      _f.make_mapping(_allocator, wn::file_system::mapping_type::memory_backed),
      _f.make_mapping(
          _allocator, wn::file_system::mapping_type::memory_backed)};

  mappings[0]->initialize_files(
      wn::containers::contiguous_range<const wn::core::pair<
          wn::containers::string_view, wn::containers::string_view>>(
          &file_data_0[0], sizeof(file_data_0) / sizeof(file_data_0[0])));
  mappings[1]->initialize_files(
      wn::containers::contiguous_range<const wn::core::pair<
          wn::containers::string_view, wn::containers::string_view>>(
          &file_data_1[0], sizeof(file_data_1) / sizeof(file_data_1[0])));

  return _f.overlay_readonly_mappings(_allocator, mappings);
}

bool diff_file(
    wn::file_system::file_ptr f, wn::containers::string_view contents) {
  auto range = f->typed_range<const char>();
  return contents == range;
}

TEST(overlay_mapping_tests, test_file_existence) {
  wn::testing::allocator test_allocator;
  wn::file_system::factory f(&test_allocator, wn::testing::k_executable_data,
      wn::logging::get_null_logger());
  auto mapping = get_test_mapping(f, &test_allocator);
  {
    EXPECT_TRUE(mapping->exists_file("a.txt"));
    EXPECT_TRUE(mapping->exists_file("a/b.txt"));

    EXPECT_TRUE(mapping->exists_file("b.txt"));
    EXPECT_TRUE(mapping->exists_file("a/a.txt"));
    EXPECT_TRUE(mapping->exists_file("b/a.txt"));

    EXPECT_TRUE(mapping->exists_file("c.txt"));

    EXPECT_FALSE(mapping->exists_file("d.txt"));
    EXPECT_FALSE(mapping->exists_file("a/d.txt"));
    EXPECT_FALSE(mapping->exists_file("d/d.txt"));
  }
}

TEST(overlay_mapping_tests, test_directory_existence) {
  wn::testing::allocator test_allocator;
  wn::file_system::factory f(&test_allocator, wn::testing::k_executable_data,
      wn::logging::get_null_logger());
  auto mapping = get_test_mapping(f, &test_allocator);
  {
    EXPECT_TRUE(mapping->exists_directory("a"));
    EXPECT_TRUE(mapping->exists_directory("b"));

    EXPECT_FALSE(mapping->exists_directory("c"));
    EXPECT_FALSE(mapping->exists_directory("a/b"));
  }
}

TEST(overlay_mapping_tests, normal_contents) {
  wn::testing::allocator test_allocator;
  wn::file_system::factory f(&test_allocator, wn::testing::k_executable_data,
      wn::logging::get_null_logger());
  auto mapping = get_test_mapping(f, &test_allocator);
  {
    wn::file_system::result res;
    EXPECT_TRUE(diff_file(mapping->open_file("a.txt", res), "a.txt"));
    EXPECT_EQ(wn::file_system::result::ok, res);
    EXPECT_TRUE(diff_file(mapping->open_file("a/b.txt", res), "a/b.txt"));
    EXPECT_EQ(wn::file_system::result::ok, res);

    EXPECT_TRUE(diff_file(mapping->open_file("b.txt", res), "b.txt"));
    EXPECT_EQ(wn::file_system::result::ok, res);
    EXPECT_TRUE(diff_file(mapping->open_file("a/a.txt", res), "a/a.txt"));
    EXPECT_EQ(wn::file_system::result::ok, res);
    EXPECT_TRUE(diff_file(mapping->open_file("b/a.txt", res), "b/a.txt"));
    EXPECT_EQ(wn::file_system::result::ok, res);

    EXPECT_TRUE(diff_file(mapping->open_file("c.txt", res), "c_2.txt"));
    EXPECT_EQ(wn::file_system::result::ok, res);
  }
}

TEST(overlay_mapping_tests, invalid_ops) {
  wn::testing::allocator test_allocator;
  wn::file_system::factory f(&test_allocator, wn::testing::k_executable_data,
      wn::logging::get_null_logger());
  auto mapping = get_test_mapping(f, &test_allocator);
  {
    EXPECT_EQ(wn::file_system::result::invalid_operation,
        mapping->delete_directory("a"));
    EXPECT_EQ(wn::file_system::result::invalid_operation,
        mapping->delete_file("a.txt"));

    EXPECT_EQ(wn::file_system::result::invalid_operation,
        mapping->create_directory("a.txt"));

    wn::file_system::result res;
    EXPECT_EQ(nullptr, mapping->create_file("a.txt", res));
    EXPECT_EQ(wn::file_system::result::invalid_operation, res);
  }
}
