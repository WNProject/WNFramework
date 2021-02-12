// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNContainers/inc/WNArray.h"
#include "WNMultiTasking/inc/call_once.h"
#include "WNMultiTasking/inc/thread.h"
#include "executable_test/inc/WNTestHarness.h"

namespace {

const uint32_t k_count = 100u;

}  // namespace

TEST(call_once, basic) {
  wn::testing::allocator allocator;

  {
    uint32_t count = 0u;
    wn::multi_tasking::once_flag flag;
    wn::containers::array<wn::multi_tasking::thread, k_count> threads;

    for (uint32_t i = 0u; i < k_count; ++i) {
      threads[i] = wn::multi_tasking::thread(&allocator, [&count, &flag]() {
        wn::multi_tasking::call_once(flag, [&count]() { count++; });
      });
    }

    for (uint32_t i = 0u; i < k_count; ++i) {
      threads[i].join();
    }

    EXPECT_EQ(count, 1u);
  }
}
