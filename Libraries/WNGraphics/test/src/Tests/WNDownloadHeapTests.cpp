// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/WNDevice.h"
#include "WNGraphics/inc/WNFactory.h"
#include "WNGraphics/inc/WNHeap.h"
#include "WNGraphics/test/inc/WNTestFixture.h"

using download_heap_creation_test =
    wn::graphics::testing::parameterized_test<size_t>;

using download_heap_synchronization_test = wn::graphics::testing::test;

TEST_P(download_heap_creation_test, many_sizes) {
  wn::graphics::factory device_factory(&m_allocator, &m_log);

  for (auto& physical_device : device_factory.query_adapters()) {
    wn::graphics::device_ptr device =
        physical_device->make_device(&m_allocator, &m_log);
    ASSERT_NE(nullptr, device);

    wn::graphics::download_heap download =
        device->create_download_heap(GetParam());
    EXPECT_TRUE(download.is_valid());
  }
  m_log.Flush();
  // On normal operation the log buffer should be empty.
  EXPECT_EQ("", m_buffer);
}

INSTANTIATE_TEST_CASE_P(small_values, download_heap_creation_test,
    ::testing::Range<size_t>(1, 50, 7));

INSTANTIATE_TEST_CASE_P(large_values, download_heap_creation_test,
    ::testing::Values(1024, 1024 - 1, 1024 * 1024, 1024 * 1024 - 1,
                            128 * 1024 * 1024));

using download_heap_writing_test =
    wn::graphics::testing::parameterized_test<std::tuple<size_t, size_t>>;

TEST_P(download_heap_writing_test, read_values) {
  wn::graphics::factory device_factory(&m_allocator, &m_log);

  for (auto& physical_device : device_factory.query_adapters()) {
    wn::graphics::device_ptr device =
        physical_device->make_device(&m_allocator, &m_log);
    ASSERT_NE(nullptr, device);

    wn::graphics::download_heap download =
        device->create_download_heap(std::get<0>(GetParam()));
    ASSERT_TRUE(download.is_valid());

    auto buffer = download.get_range(0, std::get<0>(GetParam()));
    for (size_t i = 0; i < buffer.range().size();
         i += std::get<1>(GetParam())) {
      volatile char x = buffer.range()[i];
      (void)x;
    }
  }
  m_log.Flush();
  // On normal operation the log buffer should be empty.
  EXPECT_EQ("", m_buffer);
}

INSTANTIATE_TEST_CASE_P(small_values, download_heap_writing_test,
    ::testing::Combine(::testing::Range<size_t>(1, 50, 7),
                            ::testing::Values(1)));

INSTANTIATE_TEST_CASE_P(large_values, download_heap_writing_test,
    ::testing::ValuesIn(std::vector<std::tuple<size_t, size_t>>(
        {std::make_tuple(1024, 1), std::make_tuple(1024 - 1, 1),
            std::make_tuple(1024 * 1024, 121),
            std::make_tuple(1024 * 1024 - 1, 119)})));

TEST_F(download_heap_synchronization_test, synchronize_writes) {
  wn::graphics::factory device_factory(&m_allocator, &m_log);

  for (auto& adapter : device_factory.query_adapters()) {
    wn::graphics::device_ptr device =
        adapter->make_device(&m_allocator, &m_log);
    ASSERT_NE(nullptr, device);

    wn::graphics::download_heap download =
        device->create_download_heap(1024 * 1024);  // 1MB Heap
    ASSERT_TRUE(download.is_valid());

    auto buffer = download.get_range(0, 1024 * 1024);
    for (size_t i = 0; i < 1024 * 1024; i += 967) {
      buffer.synchronize();
      volatile int j = buffer.range()[i];
      (void)j;
    }
  }
}

// TODO(awoloszyn): Create a null device, and hook it up.
// Then create some tests around it for making sure ranges
// are flushed, and heaps are cleaned up.
