// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/WNDevice.h"
#include "WNGraphics/inc/WNFactory.h"
#include "WNGraphics/test/inc/WNTestFixture.h"

using factory_test = wn::graphics::testing::test;
TEST_F(factory_test, physical_devices) {
  wn::graphics::factory device_factory(&m_allocator, &m_log);

  for (auto& adapter : device_factory.query_adapters()) {
    EXPECT_NE("", adapter->name());
    EXPECT_NE(0u, adapter->vendor_id());
    EXPECT_NE(0u, adapter->device_id());
    EXPECT_NE(wn::graphics::adapter::api_type::invalid, adapter->api());
  }

  m_log.Flush();
  // On normal operation the log buffer should be empty.
  EXPECT_EQ("", m_buffer);
}

TEST_F(factory_test, device_test) {
  wn::graphics::factory device_factory(&m_allocator, &m_log);
  for (auto& adapter : device_factory.query_adapters()) {
    wn::graphics::device_ptr device =
        adapter->make_device(&m_allocator, &m_log);
    EXPECT_NE(nullptr, device);
  }
  m_log.Flush();
  // On normal operation the log buffer should be empty.
  EXPECT_EQ("", m_buffer);
}
