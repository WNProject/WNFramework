// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNApplicationData/inc/WNApplicationData.h"
#include "WNApplicationTest/inc/WNTestHarness.h"
#include "WNMultiTasking/inc/job_pool.h"
#include "WNMultiTasking/inc/job_signal.h"
#include "platform_utils/inc/subprocess.h"

TEST(subprocess, basic) {
  wn::multi_tasking::job_signal signal(0);

#ifdef _WN_WINDOWS
  const char* file = "C:\\Windows\\System32\\cmd.exe";
  wn::containers::string_view views[] = {{"/c"}, {"echo"}, {"Hello World"}};
#else
  const char* file = "/bin/sh";
  wn::containers::string_view views[] = {{"-c"}, {"echo Hello World"}};
#endif

  auto ret = wn::runtime::platform_utils::call_subprocess(
      wn::runtime::testing::k_application_data->system_allocator, file, views);
  ASSERT_EQ(wn::runtime::platform_utils::subprocess_error::ok, ret.err);
  ASSERT_EQ(0u, ret.return_code);
  ASSERT_LE(static_cast<size_t>(strlen("Hello World")), ret.std_out.size());
  ASSERT_EQ(std::string("Hello World"),
      std::string(ret.std_out.substr(0, strlen("Hello World")).c_str()));
}

TEST(subprocess, input) {
  wn::multi_tasking::job_signal signal(0);

#ifdef _WN_WINDOWS
  const char* file = "C:\\Windows\\System32\\cmd.exe";
  wn::containers::string_view input("echo Hello World\n");
#else
  const char* file = "/bin/sh";
  wn::containers::string_view input("echo Hello World");
#endif

  auto ret = wn::runtime::platform_utils::call_subprocess(
      wn::runtime::testing::k_application_data->system_allocator, file, input,
      wn::containers::contiguous_range<wn::containers::string_view>());
  ASSERT_EQ(wn::runtime::platform_utils::subprocess_error::ok, ret.err);
  ASSERT_EQ(0u, ret.return_code);
  ASSERT_LE(static_cast<size_t>(strlen("Hello World")), ret.std_out.size());
  ASSERT_NE(static_cast<size_t>(wn::containers::string::npos),
      ret.std_out.find_first_of("Hello World"));
}
