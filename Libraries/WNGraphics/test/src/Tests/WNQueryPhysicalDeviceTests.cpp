// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/WNFactory.h"
#include "WNLogging/inc/WNBufferLogger.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNTesting/inc/WNTestHarness.h"

#include "WNLogging/inc/WNConsoleLogger.h"

void flush_buffer(void* v, const char* bytes, size_t length,
    const std::vector<WNLogging::WNLogColorElement>&) {
  wn::containers::string* s = static_cast<wn::containers::string*>(v);
  s->append(bytes, length);
}

using buffer_logger = WNLogging::WNBufferLogger<flush_buffer>;
using log_buff = wn::containers::string;

TEST(factory_test, physical_devices) {
  wn::memory::basic_allocator allocator;
  log_buff buff;
  buffer_logger logger(&buff);
  WNLogging::WNLog log(&logger);

  wn::graphics::factory device_factory(&allocator, &log);

  for (auto& physical_device : device_factory.query_physical_devices()) {
    EXPECT_NE("", physical_device->name());
    EXPECT_NE(0u, physical_device->vendor_id());
    EXPECT_NE(0u, physical_device->device_id());
    EXPECT_NE(wn::graphics::physical_device::api_type::invalid,
        physical_device->api());
  }

  log.Flush();
  // On normal operation the log buffer should be empty.
  EXPECT_EQ("", buff);
}


TEST(factory_test, device_test) {
  wn::memory::basic_allocator allocator;
  log_buff buff;
  buffer_logger logger(&buff);
  WNLogging::WNLog log(&logger);

  wn::graphics::factory device_factory(&allocator, &log);

  for (auto& physical_device : device_factory.query_physical_devices()) {
    wn::graphics::device_ptr device =
        physical_device->make_device(&allocator, &log);
    EXPECT_NE(nullptr, device);
  }
  log.Flush();
  // On normal operation the log buffer should be empty.
  EXPECT_EQ("", buff);
}
