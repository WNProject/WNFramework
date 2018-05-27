// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/WNArena.h"
#include "WNGraphics/inc/WNBuffer.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNGraphics/inc/WNFactory.h"
#include "WNGraphics/inc/WNQueue.h"
#include "WNGraphics/test/inc/WNTestFixture.h"

using buffer = wn::runtime::graphics::testing::parameterized_test<size_t>;

TEST_P(buffer, many_sizes) {
  wn::runtime::graphics::factory device_factory(&m_allocator, m_log);

  for (auto& adapter : device_factory.query_adapters()) {
    wn::runtime::graphics::device_ptr device =
        adapter->make_device(&m_allocator, m_log,
            wn::runtime::graphics::testing::k_empty_adapter_features);

    ASSERT_NE(nullptr, device);

    const wn::containers::contiguous_range<
        const wn::runtime::graphics::arena_properties>
        arena_properties = device->get_arena_properties();

    ASSERT_FALSE(arena_properties.empty());

    size_t arena_index = 0;
    bool found_arena = false;

    for (; arena_index < arena_properties.size(); ++arena_index) {
      if (arena_properties[arena_index].host_visible) {
        found_arena = true;

        break;
      }
    }

    ASSERT_TRUE(found_arena);

    const size_t size = GetParam();
    wn::runtime::graphics::buffer buffer = device->create_buffer(
        size, static_cast<wn::runtime::graphics::resource_states>(
                  static_cast<uint32_t>(
                      wn::runtime::graphics::resource_state::host_read) |
                  static_cast<uint32_t>(
                      wn::runtime::graphics::resource_state::host_write) |
                  static_cast<uint32_t>(
                      wn::runtime::graphics::resource_state::copy_source)));

    auto reqs = buffer.get_memory_requirements();
    wn::runtime::graphics::arena arena =
        device->create_arena(arena_index, reqs.size);

    ASSERT_TRUE(buffer.bind_memory(&arena));

    void* raw_memory = buffer.map();

    ASSERT_NE(raw_memory, nullptr);

    uint8_t* memory = static_cast<uint8_t*>(raw_memory);

    for (size_t i = 0; i < size; ++i) {
      memory[i] = static_cast<uint8_t>(i + 1);
    }

    buffer.unmap();

    raw_memory = buffer.map();

    ASSERT_NE(raw_memory, nullptr);

    memory = static_cast<uint8_t*>(raw_memory);

    for (size_t i = 0; i < size; ++i) {
      EXPECT_EQ(memory[i], static_cast<uint8_t>(i + 1));
    }

    buffer.unmap();
  }

  m_log->flush();

  // On normal operation the log buffer should be empty.
  EXPECT_EQ("", m_buffer);
}

INSTANTIATE_TEST_CASE_P(
    values, buffer, ::testing::Values<size_t>(1, 8, 64, 128));
