// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/WNArena.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNGraphics/inc/WNFactory.h"
#include "WNGraphics/inc/WNQueue.h"
#include "WNGraphics/test/inc/WNTestFixture.h"

using arena = wn::graphics::testing::parameterized_test<size_t>;

TEST_P(arena, many_sizes) {
  wn::graphics::factory device_factory(&m_allocator, m_log);

  for (auto& adapter : device_factory.query_adapters()) {
    wn::graphics::device_ptr device = adapter->make_device(&m_allocator, m_log);

    ASSERT_NE(nullptr, device);

    const wn::containers::contiguous_range<const wn::graphics::arena_properties>
        arena_properties = device->get_arena_properties();

    EXPECT_FALSE(arena_properties.empty());

    for (size_t i = 0; i < arena_properties.size(); ++i) {
      const wn::graphics::arena arena = device->create_arena(i, GetParam());
      const wn::graphics::arena arena_with_multisampling =
          device->create_arena(i, GetParam(), true);

      ASSERT_TRUE(arena.is_valid());
      ASSERT_TRUE(arena_with_multisampling.is_valid());
    }
  }

  m_log->flush();

  // On normal operation the log buffer should be empty.
  EXPECT_EQ("", m_buffer);
}

INSTANTIATE_TEST_CASE_P(
    small_values, arena, ::testing::Values<size_t>(1, 8, 64, 128));

INSTANTIATE_TEST_CASE_P(
    large_values, arena, ::testing::Values<size_t>(1024, 4 * 1024,
                             4 * 1024 * 1024, 40 * 1024 * 1024));