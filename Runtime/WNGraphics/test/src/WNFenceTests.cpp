// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/WNDevice.h"
#include "WNGraphics/inc/WNFactory.h"
#include "WNGraphics/inc/WNFence.h"
#include "WNGraphics/inc/WNQueue.h"
#include "WNGraphics/test/inc/WNTestFixture.h"

using fence_test = wn::runtime::graphics::testing::test;

TEST_F(fence_test, many_sizes) {
  wn::runtime::graphics::factory device_factory(&m_allocator, m_log);

  for (auto& adapter : device_factory.query_adapters()) {
    wn::runtime::graphics::device_ptr device = adapter->make_device(
        &m_allocator, m_log, wn::runtime::graphics::k_empty_adapter_features);
    ASSERT_NE(nullptr, device);
    wn::runtime::graphics::queue_ptr queue = device->create_queue();
    ASSERT_NE(nullptr, queue);

    wn::runtime::graphics::fence my_fence = device->create_fence();

    ASSERT_TRUE(my_fence.is_valid());

    queue->enqueue_fence(my_fence);
    my_fence.wait();
  }

  m_log->flush();
  // On normal operation the log buffer should be empty.
  EXPECT_EQ("", m_buffer);
}
