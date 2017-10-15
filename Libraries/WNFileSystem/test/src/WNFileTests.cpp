// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNExecutableTest/inc/WNTestHarness.h"
#include "WNFileSystem/inc/WNFactory.h"
#include "WNFileSystem/inc/WNFile.h"

using file = ::testing::TestWithParam<wn::file_system::mapping_type>;

TEST_P(file, creation) {
  wn::testing::allocator allocator;

  {
    wn::file_system::factory f(&allocator, wn::testing::k_system_data);
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
    wn::file_system::factory f(&allocator, wn::testing::k_system_data);
    const wn::file_system::mapping_ptr mp =
        f.make_mapping(&allocator, GetParam());

    ASSERT_NE(mp, nullptr);

    wn::file_system::result r;
    wn::file_system::file_ptr fp = mp->create_file("temp.txt", r);

    ASSERT_NE(fp, nullptr);
    EXPECT_EQ(r, wn::file_system::result::ok);
    EXPECT_EQ(fp->size(), 0);
    EXPECT_TRUE(fp->empty());
    EXPECT_TRUE(fp->resize(5));
    EXPECT_EQ(fp->size(), 5);
    EXPECT_FALSE(fp->empty());

    fp->close();

    fp = mp->open_file("temp.txt", r);

    ASSERT_NE(fp, nullptr);
    EXPECT_EQ(r, wn::file_system::result::ok);
    EXPECT_EQ(fp->size(), 5);
    EXPECT_FALSE(fp->empty());

    fp->close();
  }
}

TEST_P(file, resize) {
  wn::testing::allocator allocator;

  {
    wn::file_system::factory f(&allocator, wn::testing::k_system_data);
    const wn::file_system::mapping_ptr mp =
        f.make_mapping(&allocator, GetParam());

    ASSERT_NE(mp, nullptr);

    wn::file_system::result r;
    wn::file_system::file_ptr fp = mp->create_file("temp.txt", r);

    ASSERT_NE(fp, nullptr);
    EXPECT_EQ(r, wn::file_system::result::ok);
    EXPECT_EQ(fp->size(), 0);
    EXPECT_EQ(fp->data(), nullptr);
    EXPECT_TRUE(fp->resize(10));
    EXPECT_EQ(fp->size(), 10);
    EXPECT_NE(fp->data(), nullptr);

    {
      auto range = fp->range();

      EXPECT_EQ(range.size(), 10);

      for (size_t i = 0; i < range.size(); ++i) {
        range[i] = static_cast<uint8_t>(i);
      }
    }

    EXPECT_TRUE(fp->resize(5));
    EXPECT_EQ(fp->size(), 5);
    EXPECT_NE(fp->data(), nullptr);

    {
      auto range = fp->range();

      EXPECT_EQ(range.size(), 5);

      for (size_t i = 0; i < range.size(); ++i) {
        EXPECT_EQ(range[i], static_cast<uint8_t>(i));
      }
    }

    EXPECT_TRUE(fp->resize(0));
    EXPECT_EQ(fp->size(), 0);
    EXPECT_EQ(fp->data(), nullptr);

    fp->close();

    EXPECT_FALSE(fp->is_open());

    fp = mp->open_file("temp.txt", r);

    ASSERT_NE(fp, nullptr);
    EXPECT_EQ(r, wn::file_system::result::ok);
    EXPECT_EQ(fp->size(), 0);
    EXPECT_EQ(fp->data(), nullptr);
    EXPECT_EQ(fp->range().size(), 0);

    fp->close();

    EXPECT_FALSE(fp->is_open());
  }
}

TEST_P(file, clear) {
  wn::testing::allocator allocator;

  {
    wn::file_system::factory f(&allocator, wn::testing::k_system_data);
    const wn::file_system::mapping_ptr mp =
        f.make_mapping(&allocator, GetParam());

    ASSERT_NE(mp, nullptr);

    wn::file_system::result r;
    wn::file_system::file_ptr fp = mp->create_file("temp.txt", r);

    ASSERT_NE(fp, nullptr);
    EXPECT_EQ(r, wn::file_system::result::ok);
    EXPECT_EQ(fp->size(), 0);
    EXPECT_TRUE(fp->resize(10));
    EXPECT_EQ(fp->size(), 10);
    EXPECT_NE(fp->data(), nullptr);
    EXPECT_TRUE(fp->clear());
    EXPECT_EQ(fp->size(), 0);
    EXPECT_EQ(fp->data(), nullptr);

    fp->close();

    EXPECT_FALSE(fp->is_open());

    fp = mp->open_file("temp.txt", r);

    ASSERT_NE(fp, nullptr);
    EXPECT_EQ(r, wn::file_system::result::ok);
    EXPECT_EQ(fp->size(), 0);
    EXPECT_EQ(fp->data(), nullptr);

    fp->close();

    EXPECT_FALSE(fp->is_open());
  }
}

TEST_P(file, flush) {
  wn::testing::allocator allocator;

  {
    wn::file_system::factory f(&allocator, wn::testing::k_system_data);
    const wn::file_system::mapping_ptr mp =
        f.make_mapping(&allocator, GetParam());

    ASSERT_NE(mp, nullptr);

    wn::file_system::result r;
    wn::file_system::file_ptr fp = mp->create_file("temp.txt", r);

    ASSERT_NE(fp, nullptr);
    EXPECT_EQ(r, wn::file_system::result::ok);
    EXPECT_TRUE(fp->resize(10));
    EXPECT_EQ(fp->size(), 10);

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
    EXPECT_EQ(fp->size(), 10);

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
    wn::file_system::factory f(&allocator, wn::testing::k_system_data);
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

INSTANTIATE_TEST_CASE_P(all_mappings, file,
    ::testing::Values(wn::file_system::mapping_type::scratch_directory,
        wn::file_system::mapping_type::memory_backed));
