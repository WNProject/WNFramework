// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNApplicationTest/inc/WNTestHarness.h"
#include "WNFileSystem/inc/WNFactory.h"

#ifdef _WN_POSIX
#include <sys/stat.h>
#endif

using factory = ::testing::TestWithParam<wn::file_system::mapping_type>;

TEST_P(factory, make_mapping) {
  wn::testing::allocator allocator;

  {
    wn::file_system::factory f(
        &allocator, wn::runtime::testing::k_application_data->executable_data);
    const wn::file_system::mapping_ptr mp =
        f.make_mapping(&allocator, GetParam());

    ASSERT_NE(mp, nullptr);
  }
}

INSTANTIATE_TEST_SUITE_P(all_mappings, factory,
    ::testing::Values(wn::file_system::mapping_type::scratch_directory,
        wn::file_system::mapping_type::memory_backed,
        wn::file_system::mapping_type::executable_directory,
        wn::file_system::mapping_type::current_working_directory));