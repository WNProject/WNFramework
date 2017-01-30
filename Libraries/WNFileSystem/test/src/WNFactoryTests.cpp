// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNFileSystem/inc/WNFactory.h"
#include "WNTesting/inc/WNTestHarness.h"

using factory_tests = ::testing::TestWithParam<wn::file_system::mapping_type>;

TEST_P(factory_tests, make_mapping) {
  wn::testing::allocator allocator;

  {
    wn::file_system::factory f;
    const wn::file_system::mapping_ptr mp =
        f.make_mapping(GetParam(), &allocator);

    ASSERT_NE(mp, nullptr);
  }
}

INSTANTIATE_TEST_CASE_P(all_mappings, factory_tests,
    ::testing::Values(wn::file_system::mapping_type::scratch,
                            wn::file_system::mapping_type::memory_backed));
