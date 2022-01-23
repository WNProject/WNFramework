// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNFileSystem/inc/WNFactory.h"
#include "WNFileSystem/inc/WNFile.h"
#include "executable_test/inc/WNTestHarness.h"

using file = ::testing::TestWithParam<wn::file_system::mapping_type>;

TEST_P(file, creation) {
  wn::testing::allocator allocator;

  {
    wn::file_system::factory f(&allocator, wn::testing::k_executable_data,
        wn::logging::get_null_logger());
    const wn::file_system::mapping_ptr mp =
        f.make_mapping(&allocator, GetParam());

    ASSERT_NE(mp, nullptr);

    wn::file_system::result r;
    const wn::file_system::file_ptr fp = mp->create_file("temp.txt", r);

    ASSERT_NE(fp, nullptr);
    EXPECT_EQ(r, wn::file_system::result::ok);

    fp->close();

    EXPECT_TRUE(mp->exists_file("temp.txt"));
  }
}

TEST_P(file, size) {
  wn::testing::allocator allocator;

  {
    wn::file_system::factory f(&allocator, wn::testing::k_executable_data,
        wn::logging::get_null_logger());
    const wn::file_system::mapping_ptr mp =
        f.make_mapping(&allocator, GetParam());

    ASSERT_NE(mp, nullptr);

    wn::file_system::result r;
    wn::file_system::file_ptr fp = mp->create_file("temp.txt", r);

    ASSERT_NE(fp, nullptr);
    EXPECT_EQ(r, wn::file_system::result::ok);
    EXPECT_EQ(fp->size(), 0u);
    EXPECT_TRUE(fp->empty());
    EXPECT_TRUE(fp->resize(5));
    EXPECT_EQ(fp->size(), 5u);
    EXPECT_FALSE(fp->empty());

    fp->close();

    fp = mp->open_file("temp.txt", r);

    ASSERT_NE(fp, nullptr);
    EXPECT_EQ(r, wn::file_system::result::ok);
    EXPECT_EQ(fp->size(), 5u);
    EXPECT_FALSE(fp->empty());

    fp->close();
  }
}

TEST_P(file, info) {
  wn::testing::allocator allocator;

  {
    wn::file_system::factory f(&allocator, wn::testing::k_executable_data,
        wn::logging::get_null_logger());
    const wn::file_system::mapping_ptr mp =
        f.make_mapping(&allocator, GetParam());

    ASSERT_NE(mp, nullptr);

    wn::file_system::result r;
    wn::file_system::file_ptr fp = mp->create_file("temp.txt", r);

    ASSERT_NE(fp, nullptr);
    EXPECT_EQ(r, wn::file_system::result::ok);
    EXPECT_EQ(fp->info().type, wn::file_system::object_type::file);
    EXPECT_EQ(fp->info().size, 0u);
    EXPECT_TRUE(fp->resize(5));
    EXPECT_EQ(fp->info().size, 5u);

    fp->close();

    fp = mp->open_file("temp.txt", r);

    ASSERT_NE(fp, nullptr);
    EXPECT_EQ(r, wn::file_system::result::ok);
    EXPECT_EQ(fp->info().type, wn::file_system::object_type::file);
    EXPECT_EQ(fp->info().size, 5u);

    fp->close();
  }
}

TEST_P(file, resize) {
  wn::testing::allocator allocator;

  {
    wn::file_system::factory f(&allocator, wn::testing::k_executable_data,
        wn::logging::get_null_logger());
    const wn::file_system::mapping_ptr mp =
        f.make_mapping(&allocator, GetParam());

    ASSERT_NE(mp, nullptr);

    wn::file_system::result r;
    wn::file_system::file_ptr fp = mp->create_file("temp.txt", r);

    ASSERT_NE(fp, nullptr);
    EXPECT_EQ(r, wn::file_system::result::ok);
    EXPECT_EQ(fp->size(), 0u);
    EXPECT_EQ(fp->data(), nullptr);
    EXPECT_TRUE(fp->resize(10));
    EXPECT_EQ(fp->size(), 10u);
    EXPECT_NE(fp->data(), nullptr);

    {
      auto range = fp->range();

      EXPECT_EQ(range.size(), 10u);

      for (size_t i = 0; i < range.size(); ++i) {
        range[i] = static_cast<uint8_t>(i);
      }
    }

    EXPECT_TRUE(fp->resize(5));
    EXPECT_EQ(fp->size(), 5u);
    EXPECT_NE(fp->data(), nullptr);

    {
      auto range = fp->range();

      EXPECT_EQ(range.size(), 5u);

      for (size_t i = 0; i < range.size(); ++i) {
        EXPECT_EQ(range[i], static_cast<uint8_t>(i));
      }
    }

    EXPECT_TRUE(fp->resize(0));
    EXPECT_EQ(fp->size(), 0u);
    EXPECT_EQ(fp->data(), nullptr);

    fp->close();

    EXPECT_FALSE(fp->is_open());

    fp = mp->open_file("temp.txt", r);

    ASSERT_NE(fp, nullptr);
    EXPECT_EQ(r, wn::file_system::result::ok);
    EXPECT_EQ(fp->size(), 0u);
    EXPECT_EQ(fp->data(), nullptr);
    EXPECT_EQ(fp->range().size(), 0u);

    fp->close();

    EXPECT_FALSE(fp->is_open());
  }
}

TEST_P(file, clear) {
  wn::testing::allocator allocator;

  {
    wn::file_system::factory f(&allocator, wn::testing::k_executable_data,
        wn::logging::get_null_logger());
    const wn::file_system::mapping_ptr mp =
        f.make_mapping(&allocator, GetParam());

    ASSERT_NE(mp, nullptr);

    wn::file_system::result r;
    wn::file_system::file_ptr fp = mp->create_file("temp.txt", r);

    ASSERT_NE(fp, nullptr);
    EXPECT_EQ(r, wn::file_system::result::ok);
    EXPECT_EQ(fp->size(), 0u);
    EXPECT_TRUE(fp->resize(10));
    EXPECT_EQ(fp->size(), 10u);
    EXPECT_NE(fp->data(), nullptr);
    EXPECT_TRUE(fp->clear());
    EXPECT_EQ(fp->size(), 0u);
    EXPECT_EQ(fp->data(), nullptr);

    fp->close();

    EXPECT_FALSE(fp->is_open());

    fp = mp->open_file("temp.txt", r);

    ASSERT_NE(fp, nullptr);
    EXPECT_EQ(r, wn::file_system::result::ok);
    EXPECT_EQ(fp->size(), 0u);
    EXPECT_EQ(fp->data(), nullptr);

    fp->close();

    EXPECT_FALSE(fp->is_open());
  }
}

TEST_P(file, flush) {
  wn::testing::allocator allocator;

  {
    wn::file_system::factory f(&allocator, wn::testing::k_executable_data,
        wn::logging::get_null_logger());
    const wn::file_system::mapping_ptr mp =
        f.make_mapping(&allocator, GetParam());

    ASSERT_NE(mp, nullptr);

    wn::file_system::result r;
    wn::file_system::file_ptr fp = mp->create_file("temp.txt", r);

    ASSERT_NE(fp, nullptr);
    EXPECT_EQ(r, wn::file_system::result::ok);
    EXPECT_TRUE(fp->resize(10));
    EXPECT_EQ(fp->size(), 10u);

    auto range = fp->range();

    for (size_t i = 0; i < fp->size(); ++i) {
      range[i] = static_cast<uint8_t>(i);
    }

    EXPECT_TRUE(fp->flush());

    fp->close();

    EXPECT_FALSE(fp->is_open());

    fp = mp->open_file("temp.txt", r);

    ASSERT_NE(fp, nullptr);
    EXPECT_EQ(r, wn::file_system::result::ok);
    EXPECT_EQ(fp->size(), 10u);

    range = fp->range();

    for (size_t i = 0; i < fp->size(); ++i) {
      EXPECT_EQ(range[i], static_cast<uint8_t>(i));
    }

    fp->close();

    EXPECT_FALSE(fp->is_open());
  }
}

TEST_P(file, close) {
  wn::testing::allocator allocator;

  {
    wn::file_system::factory f(&allocator, wn::testing::k_executable_data,
        wn::logging::get_null_logger());
    const wn::file_system::mapping_ptr mp =
        f.make_mapping(&allocator, GetParam());

    ASSERT_NE(mp, nullptr);

    wn::file_system::result r;
    wn::file_system::file_ptr fp = mp->create_file("temp.txt", r);

    ASSERT_NE(fp, nullptr);
    EXPECT_EQ(r, wn::file_system::result::ok);

    fp->close();

    EXPECT_FALSE(fp->is_open());
  }
}

INSTANTIATE_TEST_SUITE_P(all_mappings, file,
    ::testing::Values(wn::file_system::mapping_type::scratch_directory,
        wn::file_system::mapping_type::memory_backed));
