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
int32_t k_arg_c = 0;
char** k_arg_v = nullptr;
}  // namespace testing
}  // namespace wn

int32_t wn_main(const wn::entry::system_data*, int32_t _argc, char* _argv[]) {
  wn::entry::wn_dummy();
  ::testing::InitGoogleTest(&_argc, _argv);
  wn::testing::k_arg_c = _argc;
  wn::testing::k_arg_v = _argv;

  wn::testing::init_test_framework();

  return RUN_ALL_TESTS();
}