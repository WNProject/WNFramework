// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNExecutable/inc/WNEntry.h"
#include "WNExecutableTest/inc/WNTestHarness.h"

// Needed so that wn_main gets linked in.
namespace wn {
namespace testing {

#if defined _WN_ANDROID || defined _WN_LINUX

void test_dummy() {}

#endif  //_WN_ANDROID || _WN_LINUX

const entry::system_data* k_system_data = nullptr;

}  // namespace testing
}  // namespace wn

int32_t wn_main(const wn::entry::system_data* _system_data) {
  wn::entry::wn_dummy();

  wn::testing::k_system_data = _system_data;

  ::testing::InitGoogleTest(
      const_cast<int32_t*>(&_system_data->argc), _system_data->argv);
  wn::testing::init_test_framework();

  return RUN_ALL_TESTS();
}