// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNFileSystem/inc/WNFactory.h"
#include "WNFileSystem/inc/WNMapping.h"
#include "WNTesting/inc/WNTestHarness.h"

TEST(mapping, creation) {
  wn::file_system::factory f;
  wn::memory::default_test_allocator allocator;

  {
    const wn::file_system::mapping_ptr mp =
        f.make_mapping(wn::file_system::system_path::scratch, &allocator);

    ASSERT_NE(mp, nullptr);
  }

  EXPECT_EQ(allocator.allocated(), allocator.freed());
}

TEST(mapping, create_file) {
  wn::file_system::factory f;
  wn::memory::default_test_allocator allocator;

  {
    const wn::file_system::mapping_ptr mp =
        f.make_mapping(wn::file_system::system_path::scratch, &allocator);

    ASSERT_NE(mp, nullptr);

    wn::file_system::result r;
    const wn::file_system::file_ptr fp = mp->create_file("temp.txt", r);

    ASSERT_NE(fp, nullptr);
    EXPECT_EQ(r, wn::file_system::result::ok);

    fp->close();

    EXPECT_FALSE(fp->is_open());
  }

  EXPECT_EQ(allocator.allocated(), allocator.freed());
}

TEST(mapping, create_directory) {
  wn::file_system::factory f;
  wn::memory::default_test_allocator allocator;

  {
    const wn::file_system::mapping_ptr mp =
        f.make_mapping(wn::file_system::system_path::scratch, &allocator);

    ASSERT_NE(mp, nullptr);

    const wn::file_system::result r = mp->create_directory("temp");

    EXPECT_EQ(r, wn::file_system::result::ok);
  }

  EXPECT_EQ(allocator.allocated(), allocator.freed());
}

TEST(mapping, exists_file) {
  wn::file_system::factory f;
  wn::memory::default_test_allocator allocator;

  {
    const wn::file_system::mapping_ptr mp =
        f.make_mapping(wn::file_system::system_path::scratch, &allocator);

    ASSERT_NE(mp, nullptr);

    wn::file_system::result r;
    const wn::file_system::file_ptr fp = mp->create_file("temp.txt", r);

    ASSERT_NE(fp, nullptr);
    EXPECT_EQ(r, wn::file_system::result::ok);

    fp->close();

    EXPECT_FALSE(fp->is_open());
    EXPECT_TRUE(mp->exists_file("temp.txt"));
    EXPECT_FALSE(mp->exists_file("notafile.txt"));
  }

  EXPECT_EQ(allocator.allocated(), allocator.freed());
}

TEST(mapping, exists_directory) {
  wn::file_system::factory f;
  wn::memory::default_test_allocator allocator;
  const wn::file_system::mapping_ptr mp =
      f.make_mapping(wn::file_system::system_path::scratch, &allocator);

  ASSERT_NE(mp, nullptr);

  const wn::file_system::result r = mp->create_directory("temp");

  EXPECT_EQ(r, wn::file_system::result::ok);
  EXPECT_TRUE(mp->exists_directory("temp"));
  EXPECT_FALSE(mp->exists_directory("notafile.txt"));
}

TEST(mapping, open_file) {
  wn::file_system::factory f;
  wn::memory::default_test_allocator allocator;
  const wn::file_system::mapping_ptr mp =
      f.make_mapping(wn::file_system::system_path::scratch, &allocator);

  ASSERT_NE(mp, nullptr);

  wn::file_system::result r;
  wn::file_system::file_ptr fp = mp->create_file("temp.txt", r);

  ASSERT_NE(fp, nullptr);
  EXPECT_EQ(r, wn::file_system::result::ok);

  fp->close();

  EXPECT_FALSE(fp->is_open());

  fp = mp->open_file("temp.txt", r);

  ASSERT_NE(fp, nullptr);
  EXPECT_EQ(r, wn::file_system::result::ok);

  fp->close();

  EXPECT_FALSE(fp->is_open());
}

TEST(mapping, delete_file) {
  wn::file_system::factory f;
  wn::memory::default_test_allocator allocator;
  const wn::file_system::mapping_ptr mp =
      f.make_mapping(wn::file_system::system_path::scratch, &allocator);

  ASSERT_NE(mp, nullptr);

  wn::file_system::result r;
  const wn::file_system::file_ptr fp = mp->create_file("temp.txt", r);

  ASSERT_NE(fp, nullptr);
  EXPECT_EQ(r, wn::file_system::result::ok);

  fp->close();

  EXPECT_FALSE(fp->is_open());

  r = mp->delete_file("temp.txt");

  EXPECT_EQ(r, wn::file_system::result::ok);
  EXPECT_FALSE(mp->exists_file("temp.txt"));
}

TEST(mapping, delete_directory) {
  wn::file_system::factory f;
  wn::memory::default_test_allocator allocator;
  const wn::file_system::mapping_ptr mp =
      f.make_mapping(wn::file_system::system_path::scratch, &allocator);

  ASSERT_NE(mp, nullptr);

  wn::file_system::result r = mp->create_directory("temp");

  EXPECT_EQ(r, wn::file_system::result::ok);

  wn::file_system::file_ptr fp = mp->create_file("temp/temp1.txt", r);

  ASSERT_NE(fp, nullptr);
  EXPECT_EQ(r, wn::file_system::result::ok);

  r = mp->create_directory("temp/temp2");

  EXPECT_EQ(r, wn::file_system::result::ok);

  fp = mp->create_file("temp/temp2/temp2.txt", r);

  ASSERT_NE(fp, nullptr);
  EXPECT_EQ(r, wn::file_system::result::ok);

  fp->close();

  r = mp->delete_directory("temp");

  EXPECT_EQ(r, wn::file_system::result::ok);
  EXPECT_FALSE(mp->exists_directory("temp"));
}
