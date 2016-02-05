// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNEntryPoint/inc/WNEntry.h"
#include "WNTesting/inc/WNTestHarness.h"

#if defined _WN_ANDROID || defined _WN_LINUX
// Needed so that wn_main gets linked in.
namespace wn {
namespace testing {

void test_dummy() {}

}  // testing
}  // wn
#endif  //_WN_ANDROID || _WN_LINUX

int32_t wn_main(int32_t _argc, char* _argv[]) {
  wn_dummy();

  ::testing::InitGoogleTest(&_argc, _argv);
  wn::testing::init_test_framework();

  return RUN_ALL_TESTS();
}