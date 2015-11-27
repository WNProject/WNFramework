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

wn_int32 wn_main(wn_int32 _argc, wn_char* _argv[]) {
  wn_dummy();
  ::testing::InitGoogleTest(&_argc, _argv);
  wn::testing::init_test_framework();

  const wn_uint32 result = RUN_ALL_TESTS();

  return (result);
}