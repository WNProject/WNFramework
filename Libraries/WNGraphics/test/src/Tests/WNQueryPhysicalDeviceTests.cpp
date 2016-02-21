// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNMemory/inc/WNAllocator.h"
#include "WNGraphics/inc/WNFactory.h"
#include "WNTesting/inc/WNTestHarness.h"

TEST(factory_test, physical_devices) {
  wn::memory::basic_allocator allocator;
  wn::graphics::factory device_factory(&allocator);
  for (auto& physical_device : device_factory.query_physical_devices()) {
    EXPECT_NE("", physical_device->name());
    EXPECT_NE(0u, physical_device->vendor_id());
    EXPECT_NE(0u, physical_device->device_id());
    EXPECT_NE(wn::graphics::physical_device::api_type::invalid,
      physical_device->api());
  }
}
