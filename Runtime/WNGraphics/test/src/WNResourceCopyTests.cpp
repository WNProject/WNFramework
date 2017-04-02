// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/WNBuffer.h"
#include "WNGraphics/inc/WNCommandAllocator.h"
#include "WNGraphics/inc/WNCommandList.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNGraphics/inc/WNFactory.h"
#include "WNGraphics/inc/WNFence.h"
#include "WNGraphics/inc/WNQueue.h"
#include "WNGraphics/test/inc/WNTestFixture.h"

using resource_copy_test = wn::graphics::testing::parameterized_test<size_t>;

TEST_P(resource_copy_test, many_sizes) {
  wn::graphics::factory device_factory(&m_allocator, m_log);
  const size_t size = GetParam();
  for (auto& adapter : device_factory.query_adapters()) {
    wn::graphics::device_ptr device = adapter->make_device(&m_allocator, m_log);
    ASSERT_NE(nullptr, device);

    // Time to find an image arena
    wn::containers::contiguous_range<const wn::graphics::arena_properties>
        properties = device->get_arena_properties();
    size_t idx = 0;
    for (idx = 0; idx < properties.size(); ++idx) {
      if (properties[idx].allow_buffers && properties[idx].host_visible) {
        break;
      }
    }
    ASSERT_NE(properties.size(), idx);

    wn::graphics::buffer src_buffer = device->create_buffer(size,
        static_cast<wn::graphics::resource_states>(
            static_cast<uint32_t>(wn::graphics::resource_state::host_read) |
            static_cast<uint32_t>(wn::graphics::resource_state::host_write) |
            static_cast<uint32_t>(wn::graphics::resource_state::copy_source)));

    wn::graphics::buffer dst_buffer = device->create_buffer(size,
        static_cast<wn::graphics::resource_states>(
            static_cast<uint32_t>(wn::graphics::resource_state::host_read) |
            static_cast<uint32_t>(wn::graphics::resource_state::host_write) |
            static_cast<uint32_t>(wn::graphics::resource_state::copy_dest)));

    wn::graphics::buffer_memory_requirements src_reqs =
        src_buffer.get_memory_requirements();
    wn::graphics::buffer_memory_requirements dst_reqs =
        dst_buffer.get_memory_requirements();

    wn::graphics::arena src_arena = device->create_arena(idx, src_reqs.size);
    src_buffer.bind_memory(&src_arena, 0);

    wn::graphics::arena dst_arena = device->create_arena(idx, dst_reqs.size);
    dst_buffer.bind_memory(&dst_arena, 0);

    wn::graphics::queue_ptr queue = device->create_queue();
    wn::graphics::command_allocator alloc = device->create_command_allocator();
    wn::graphics::command_list_ptr list = alloc.create_command_list();
    wn::graphics::fence completion_fence = device->create_fence();

    void* raw_memory = src_buffer.map();

    ASSERT_NE(raw_memory, nullptr);

    uint8_t* memory = static_cast<uint8_t*>(raw_memory);

    for (size_t i = 0; i < size; ++i) {
      memory[i] = static_cast<uint8_t>(i + 1);
    }
    src_buffer.unmap();

    list->transition_resource(src_buffer,
        wn::graphics::resource_state::host_write,
        wn::graphics::resource_state::copy_source);
    list->transition_resource(dst_buffer, wn::graphics::resource_state::initial,
        wn::graphics::resource_state::copy_dest);
    list->copy_buffer(src_buffer, 0, dst_buffer, 0, size);
    list->transition_resource(dst_buffer,
        wn::graphics::resource_state::copy_dest,
        wn::graphics::resource_state::host_read);
    list->finalize();
    queue->enqueue_command_list(list.get());
    queue->enqueue_fence(completion_fence);

    completion_fence.wait();

    raw_memory = dst_buffer.map();
    ASSERT_NE(raw_memory, nullptr);

    memory = static_cast<uint8_t*>(raw_memory);

    for (size_t i = 0; i < size; ++i) {
      EXPECT_EQ(static_cast<uint8_t>(i + 1), memory[i]);
    }
  }

  m_log->flush();
  // On normal operation the log buffer should be empty.
  EXPECT_EQ("", m_buffer);
}

INSTANTIATE_TEST_CASE_P(
    small_values, resource_copy_test, ::testing::Range<size_t>(1, 50, 7));

INSTANTIATE_TEST_CASE_P(large_values, resource_copy_test,
    ::testing::Values(1024, 1024 - 1, 1024 * 1024, 1024 * 1024 - 1));

// TODO(awoloszyn): Create a null device, and hook it up.
// Then create some tests around it for making sure ranges
// are flushed, and heaps are cleaned up.
