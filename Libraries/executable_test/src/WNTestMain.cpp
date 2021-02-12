// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "executable_data/inc/executable_data.h"
#include "executable_entry/inc/executable_entry.h"
#include "executable_test/inc/WNTestHarness.h"

// Needed so that wn_main gets linked in.
namespace wn {
namespace testing {

#if defined _WN_ANDROID || defined _WN_LINUX

void test_dummy() {}

#endif  //_WN_ANDROID || _WN_LINUX

const executable::executable_data* k_executable_data = nullptr;

}  // namespace testing
}  // namespace wn

int32_t wn_main(const wn::executable::executable_data* _executable_data) {
  wn::executable::wn_dummy();

  wn::testing::k_executable_data = _executable_data;

  ::testing::InitGoogleTest(
      const_cast<int32_t*>(&_executable_data->argc), _executable_data->argv);
  wn::testing::init_test_framework();

  return RUN_ALL_TESTS();
}
