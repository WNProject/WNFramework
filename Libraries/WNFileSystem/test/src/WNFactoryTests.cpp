// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNFileSystem/inc/WNFactory.h"
#include "WNTesting/inc/WNTestHarness.h"

TEST(factory, make_mapping) {
  wn::file_system::factory f;
  wn::memory::default_test_allocator allocator;

  {
    const wn::file_system::mapping_ptr mp =
        f.make_mapping(wn::file_system::system_path::scratch, &allocator);

    ASSERT_NE(mp, nullptr);
  }

  EXPECT_EQ(allocator.allocated(), allocator.freed());
}