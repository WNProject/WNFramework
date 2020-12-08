// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNApplicationTest/inc/WNTestHarness.h"
#include "executable_entry/inc/executable_entry.h"

namespace wn {
namespace runtime {
namespace testing {

// Needed so that wn_main gets linked in.
#if defined _WN_ANDROID || defined _WN_LINUX

void test_dummy() {}

#endif  //_WN_ANDROID || _WN_LINUX

const application::application_data* k_application_data = nullptr;

}  // namespace testing
}  // namespace runtime
}  // namespace wn

int32_t wn_application_main(
    const wn::runtime::application::application_data* _data) {
  wn::runtime::testing::k_application_data = _data;

  ::testing::InitGoogleTest(const_cast<int32_t*>(&_data->executable_data->argc),
      _data->executable_data->argv);
  wn::runtime::testing::init_test_framework();

  return RUN_ALL_TESTS();
}
