// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNMultiTasking/inc/WNSynchronized.h"
#include "WNTesting/inc/WNTestHarness.h"

struct s : public wn::multi_tasking::synchronized<> {
  void func() {}
  void func2() {}
};

struct s2 {
  void func() {}
};

TEST(synchronized, all_same) {
  s my_s;
  ASSERT_EQ(true, wn::multi_tasking::is_synchronized<s>::type::value);
  ASSERT_NE(nullptr, my_s.get_synchronization_data());
}

TEST(synchronized, not_synchronized) {
  ASSERT_EQ(false, wn::multi_tasking::is_synchronized<s2>::type::value);
}
