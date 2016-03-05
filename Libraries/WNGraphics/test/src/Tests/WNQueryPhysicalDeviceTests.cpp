// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/WNFactory.h"
#include "WNGraphics/test/inc/WNTestFixture.h"

using factory_test = wn::graphics::testing::test;
TEST_F(factory_test, physical_devices) {
  wn::graphics::factory device_factory(&m_allocator, &m_log);

  for (auto& physical_device : device_factory.query_physical_devices()) {
    EXPECT_NE("", physical_device->name());
    EXPECT_NE(0u, physical_device->vendor_id());
    EXPECT_NE(0u, physical_device->device_id());
    EXPECT_NE(wn::graphics::physical_device::api_type::invalid,
        physical_device->api());
  }

  m_log.Flush();
  // On normal operation the log buffer should be empty.
  EXPECT_EQ("", m_buffer);
}

TEST_F(factory_test, device_test) {
  wn::graphics::factory device_factory(&m_allocator, &m_log);
  for (auto& physical_device : device_factory.query_physical_devices()) {
    wn::graphics::device_ptr device =
        physical_device->make_device(&m_allocator, &m_log);
    EXPECT_NE(nullptr, device);
  }
  m_log.Flush();
  // On normal operation the log buffer should be empty.
  EXPECT_EQ("", m_buffer);
}
