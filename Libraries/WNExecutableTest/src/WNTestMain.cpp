// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNExecutable/inc/WNEntry.h"
#include "WNExecutableTest/inc/WNTestHarness.h"

#if defined _WN_ANDROID || defined _WN_LINUX
// Needed so that wn_main gets linked in.
namespace wn {
namespace testing {

void test_dummy() {}

}  // namespace testing
}  // namespace wn
#endif  //_WN_ANDROID || _WN_LINUX

int32_t wn_main(const wn::entry::system_data*, int32_t _argc, char* _argv[]) {
  wn::entry::wn_dummy();

  ::testing::InitGoogleTest(&_argc, _argv);
  wn::testing::init_test_framework();

  return RUN_ALL_TESTS();
}