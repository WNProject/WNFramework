// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/WNArena.h"
#include "WNGraphics/inc/WNArenaProperties.h"
#include "WNGraphics/inc/WNBuffer.h"
#include "WNGraphics/inc/WNCommandAllocator.h"
#include "WNGraphics/inc/WNCommandList.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNGraphics/inc/WNFactory.h"
#include "WNGraphics/inc/WNFence.h"
#include "WNGraphics/inc/WNImage.h"
#include "WNGraphics/inc/WNQueue.h"
#include "WNGraphics/test/inc/WNTestFixture.h"

using image_transfer_tests =
    wn::runtime::graphics::testing::parameterized_test<size_t>;

TEST_P(image_transfer_tests, many_sizes) {
  wn::runtime::graphics::factory device_factory(&m_allocator, m_log);

  for (auto& adapter : device_factory.query_adapters()) {
    wn::runtime::graphics::device_ptr device =
        adapter->make_device(&m_allocator, m_log,
            wn::runtime::graphics::testing::k_empty_adapter_features);
    ASSERT_NE(nullptr, device);
    // Everything should fit into a 16MB buffer.
    const size_t buffer_size = 16 * 1024 * 1024;

    // Time to find an image arena
    wn::containers::contiguous_range<
        const wn::runtime::graphics::arena_properties>
        properties = device->get_arena_properties();

    size_t idx = 0;
    for (idx = 0; idx < properties.size(); ++idx) {
      if (properties[idx].allow_buffers && properties[idx].host_visible) {
        break;
      }
    }

    wn::runtime::graphics::buffer src_buffer =
        device->create_buffer(buffer_size,
            static_cast<wn::runtime::graphics::resource_states>(
                static_cast<uint32_t>(
                    wn::runtime::graphics::resource_state::host_read) |
                static_cast<uint32_t>(
                    wn::runtime::graphics::resource_state::host_write) |
                static_cast<uint32_t>(
                    wn::runtime::graphics::resource_state::copy_source)));

    wn::runtime::graphics::buffer dst_buffer =
        device->create_buffer(buffer_size,
            static_cast<wn::runtime::graphics::resource_states>(
                static_cast<uint32_t>(
                    wn::runtime::graphics::resource_state::host_read) |
                static_cast<uint32_t>(
                    wn::runtime::graphics::resource_state::host_write) |
                static_cast<uint32_t>(
                    wn::runtime::graphics::resource_state::copy_dest)));

    wn::runtime::graphics::buffer_memory_requirements src_reqs =
        src_buffer.get_memory_requirements();
    wn::runtime::graphics::buffer_memory_requirements dst_reqs =
        dst_buffer.get_memory_requirements();

    wn::runtime::graphics::arena src_arena =
        device->create_arena(idx, src_reqs.size);
    src_buffer.bind_memory(&src_arena, 0);

    wn::runtime::graphics::arena dst_arena =
        device->create_arena(idx, dst_reqs.size);
    dst_buffer.bind_memory(&dst_arena, 0);

    wn::runtime::graphics::queue_ptr queue = device->create_queue();
    wn::runtime::graphics::command_allocator alloc =
        device->create_command_allocator();
    wn::runtime::graphics::command_list_ptr list = alloc.create_command_list();
    wn::runtime::graphics::fence completion_fence = device->create_fence();
    wn::runtime::graphics::clear_value value{};

    wn::runtime::graphics::image image = device->create_image(
        wn::runtime::graphics::image_create_info{GetParam(), GetParam(),
            wn::runtime::graphics::data_format::r8g8b8a8_unorm,
            static_cast<wn::runtime::graphics::resource_states>(
                static_cast<uint32_t>(
                    wn::runtime::graphics::resource_state::copy_source) |
                static_cast<uint32_t>(
                    wn::runtime::graphics::resource_state::copy_dest)),
            1},
        value);
    wn::runtime::graphics::image_memory_requirements reqs =
        image.get_memory_requirements();

    for (idx = 0; idx < properties.size(); ++idx) {
      if (properties[idx].allow_images && properties[idx].device_local) {
        break;
      }
    }
    ASSERT_NE(properties.size(), idx);
    wn::runtime::graphics::arena image_arena =
        device->create_arena(idx, reqs.size);
    image.bind_memory(&image_arena, 0);

    const wn::runtime::graphics::image::image_buffer_resource_info&
        resource_info = image.get_buffer_requirements(0);

    void* src_data = src_buffer.map();
    ASSERT_NE(src_data, nullptr);

    uint8_t* src_memory = static_cast<uint8_t*>(src_data);

    for (size_t y = 0; y < GetParam(); ++y) {
      for (size_t x = 0; x < GetParam(); ++x) {
        src_memory[y * resource_info.row_pitch_in_bytes + 4 * x] =
            (x + y) % 0xFF;
        src_memory[y * resource_info.row_pitch_in_bytes + 4 * x + 1] = y & 0xFF;
        src_memory[y * resource_info.row_pitch_in_bytes + 4 * x + 2] = 0x00;
        src_memory[y * resource_info.row_pitch_in_bytes + 4 * x + 3] = 0xFF;
      }
    }
    src_buffer.unmap();

    list->transition_resource(src_buffer,
        wn::runtime::graphics::resource_state::host_write,
        wn::runtime::graphics::resource_state::copy_source);
    list->transition_resource(dst_buffer,
        wn::runtime::graphics::resource_state::initial,
        wn::runtime::graphics::resource_state::copy_dest);
    list->transition_resource(image, 0, 1,
        wn::runtime::graphics::resource_state::initial,
        wn::runtime::graphics::resource_state::copy_dest);
    list->copy_buffer_to_image(src_buffer, 0, image, 0);
    list->transition_resource(image, 0, 1,
        wn::runtime::graphics::resource_state::copy_dest,
        wn::runtime::graphics::resource_state::copy_source);
    list->copy_image_to_buffer(image, 0, dst_buffer, 0);
    list->transition_resource(dst_buffer,
        wn::runtime::graphics::resource_state::copy_dest,
        wn::runtime::graphics::resource_state::host_read);

    list->finalize();

    queue->enqueue_command_list(list.get());
    queue->enqueue_fence(completion_fence);

    completion_fence.wait();

    void* dst_data = dst_buffer.map();
    ASSERT_NE(dst_data, nullptr);

    uint8_t* dst_memory = static_cast<uint8_t*>(dst_data);

    uint32_t num_failed_channels = 0;
    for (size_t y = 0; y < GetParam(); ++y) {
      for (size_t x = 0; x < GetParam(); ++x) {
        if (dst_memory[y * resource_info.row_pitch_in_bytes + 4 * x] !=
            (x + y) % 0xFF) {
          num_failed_channels += 1;
        }
        if (dst_memory[y * resource_info.row_pitch_in_bytes + 4 * x + 1] !=
            (y & 0xFF)) {
          num_failed_channels += 1;
        }
        if (dst_memory[y * resource_info.row_pitch_in_bytes + 4 * x + 2] !=
            0x00) {
          num_failed_channels += 1;
        }
        if (dst_memory[y * resource_info.row_pitch_in_bytes + 4 * x + 3] !=
            0xFF) {
          num_failed_channels += 1;
        }
      }
    }
    ASSERT_EQ(0u, num_failed_channels);
  }

  m_log->flush();
  // On normal operation the log buffer should be empty.
  EXPECT_EQ("", m_buffer);
}

INSTANTIATE_TEST_CASE_P(large_values, image_transfer_tests,
    ::testing::Values(37, 128, 1024 - 1, 1024 + 1));

using image_transfer_with_offset_tests =
    wn::runtime::graphics::testing::parameterized_test<
        std::tuple<size_t, size_t>>;

TEST_P(image_transfer_with_offset_tests, several_offsets) {
  wn::runtime::graphics::factory device_factory(&m_allocator, m_log);

  for (auto& adapter : device_factory.query_adapters()) {
    wn::runtime::graphics::device_ptr device =
        adapter->make_device(&m_allocator, m_log,
            wn::runtime::graphics::testing::k_empty_adapter_features);
    ASSERT_NE(nullptr, device);
    // Everything should fit into a 16MB buffer.
    const size_t buffer_size = 16 * 1024 * 1024;

    // Time to find an image arena
    wn::containers::contiguous_range<
        const wn::runtime::graphics::arena_properties>
        properties = device->get_arena_properties();

    size_t idx = 0;
    for (idx = 0; idx < properties.size(); ++idx) {
      if (properties[idx].allow_buffers && properties[idx].host_visible) {
        break;
      }
    }

    wn::runtime::graphics::buffer src_buffer =
        device->create_buffer(buffer_size,
            static_cast<wn::runtime::graphics::resource_states>(
                static_cast<uint32_t>(
                    wn::runtime::graphics::resource_state::host_read) |
                static_cast<uint32_t>(
                    wn::runtime::graphics::resource_state::host_write) |
                static_cast<uint32_t>(
                    wn::runtime::graphics::resource_state::copy_source)));

    wn::runtime::graphics::buffer dst_buffer =
        device->create_buffer(buffer_size,
            static_cast<wn::runtime::graphics::resource_states>(
                static_cast<uint32_t>(
                    wn::runtime::graphics::resource_state::host_read) |
                static_cast<uint32_t>(
                    wn::runtime::graphics::resource_state::host_write) |
                static_cast<uint32_t>(
                    wn::runtime::graphics::resource_state::copy_dest)));

    wn::runtime::graphics::buffer_memory_requirements src_reqs =
        src_buffer.get_memory_requirements();
    wn::runtime::graphics::buffer_memory_requirements dst_reqs =
        dst_buffer.get_memory_requirements();

    wn::runtime::graphics::arena src_arena =
        device->create_arena(idx, src_reqs.size);
    src_buffer.bind_memory(&src_arena, 0);

    wn::runtime::graphics::arena dst_arena =
        device->create_arena(idx, dst_reqs.size);
    dst_buffer.bind_memory(&dst_arena, 0);

    wn::runtime::graphics::queue_ptr queue = device->create_queue();
    wn::runtime::graphics::command_allocator alloc =
        device->create_command_allocator();
    wn::runtime::graphics::command_list_ptr list = alloc.create_command_list();
    wn::runtime::graphics::fence completion_fence = device->create_fence();
    wn::runtime::graphics::clear_value value{};

    wn::runtime::graphics::image image = device->create_image(
        wn::runtime::graphics::image_create_info{std::get<1>(GetParam()),
            std::get<1>(GetParam()),
            wn::runtime::graphics::data_format::r8g8b8a8_unorm,
            static_cast<wn::runtime::graphics::resource_states>(
                static_cast<uint32_t>(
                    wn::runtime::graphics::resource_state::copy_source) |
                static_cast<uint32_t>(
                    wn::runtime::graphics::resource_state::copy_dest)),
            1},
        value);
    wn::runtime::graphics::image_memory_requirements reqs =
        image.get_memory_requirements();

    for (idx = 0; idx < properties.size(); ++idx) {
      if (properties[idx].allow_images && properties[idx].device_local) {
        break;
      }
    }
    ASSERT_NE(properties.size(), idx);
    wn::runtime::graphics::arena image_arena =
        device->create_arena(idx, reqs.size);
    image.bind_memory(&image_arena, 0);

    list->transition_resource(image, 0, 1,
        wn::runtime::graphics::resource_state::initial,
        wn::runtime::graphics::resource_state::copy_dest);

    const wn::runtime::graphics::image::image_buffer_resource_info&
        resource_info = image.get_buffer_requirements(0);

    void* src_data = src_buffer.map();
    ASSERT_NE(src_data, nullptr);

    const size_t buffer_offset =
        device->get_image_upload_buffer_alignment() * std::get<0>(GetParam());

    uint8_t* src_memory = static_cast<uint8_t*>(src_data) + buffer_offset;

    for (size_t y = 0; y < std::get<1>(GetParam()); ++y) {
      for (size_t x = 0; x < std::get<1>(GetParam()); ++x) {
        src_memory[y * resource_info.row_pitch_in_bytes + 4 * x] =
            (x + y) % 0xFF;
        src_memory[y * resource_info.row_pitch_in_bytes + 4 * x + 1] = y & 0xFF;
        src_memory[y * resource_info.row_pitch_in_bytes + 4 * x + 2] = 0x00;
        src_memory[y * resource_info.row_pitch_in_bytes + 4 * x + 3] = 0xFF;
      }
    }
    src_buffer.unmap();

    list->transition_resource(src_buffer,
        wn::runtime::graphics::resource_state::host_write,
        wn::runtime::graphics::resource_state::copy_source);
    list->transition_resource(dst_buffer,
        wn::runtime::graphics::resource_state::initial,
        wn::runtime::graphics::resource_state::copy_dest);
    list->transition_resource(image, 0, 1,
        wn::runtime::graphics::resource_state::initial,
        wn::runtime::graphics::resource_state::copy_dest);
    list->copy_buffer_to_image(src_buffer, buffer_offset, image, 0);
    list->transition_resource(image, 0, 1,
        wn::runtime::graphics::resource_state::copy_dest,
        wn::runtime::graphics::resource_state::copy_source);
    list->copy_image_to_buffer(image, 0, dst_buffer, buffer_offset);
    list->transition_resource(dst_buffer,
        wn::runtime::graphics::resource_state::copy_dest,
        wn::runtime::graphics::resource_state::host_read);

    list->finalize();

    queue->enqueue_command_list(list.get());
    queue->enqueue_fence(completion_fence);

    completion_fence.wait();

    void* dst_data = dst_buffer.map();
    ASSERT_NE(dst_data, nullptr);

    uint8_t* dst_memory = static_cast<uint8_t*>(dst_data) + buffer_offset;

    for (size_t y = 0; y < std::get<1>(GetParam()); ++y) {
      for (size_t x = 0; x < std::get<1>(GetParam()); ++x) {
        ASSERT_EQ(dst_memory[y * resource_info.row_pitch_in_bytes + 4 * x],
            (x + y) % 0xFF);
        ASSERT_EQ(dst_memory[y * resource_info.row_pitch_in_bytes + 4 * x + 1],
            y & 0xFF);
        ASSERT_EQ(
            dst_memory[y * resource_info.row_pitch_in_bytes + 4 * x + 2], 0x00);
        ASSERT_EQ(
            dst_memory[y * resource_info.row_pitch_in_bytes + 4 * x + 3], 0xFF);
      }
    }
  }
  m_log->flush();
  // On normal operation the log buffer should be empty.
  EXPECT_EQ("", m_buffer);
}

INSTANTIATE_TEST_CASE_P(large_values, image_transfer_with_offset_tests,
    ::testing::Combine(
        ::testing::Values(1, 7, 10), ::testing::Values(10, 37, 128)));

using image_mip_tests =
    wn::runtime::graphics::testing::parameterized_test<size_t>;

TEST_P(image_mip_tests, two_levels) {
  wn::runtime::graphics::factory device_factory(&m_allocator, m_log);

  for (auto& adapter : device_factory.query_adapters()) {
    wn::runtime::graphics::device_ptr device =
        adapter->make_device(&m_allocator, m_log,
            wn::runtime::graphics::testing::k_empty_adapter_features);
    ASSERT_NE(nullptr, device);
    // Everything should fit into a 16MB buffer.
    const size_t buffer_size = 16 * 1024 * 1024;

    // Time to find an image arena
    wn::containers::contiguous_range<
        const wn::runtime::graphics::arena_properties>
        properties = device->get_arena_properties();

    size_t idx = 0;
    for (idx = 0; idx < properties.size(); ++idx) {
      if (properties[idx].allow_buffers && properties[idx].host_visible) {
        break;
      }
    }

    wn::runtime::graphics::buffer src_buffer =
        device->create_buffer(buffer_size,
            static_cast<wn::runtime::graphics::resource_states>(
                static_cast<uint32_t>(
                    wn::runtime::graphics::resource_state::host_read) |
                static_cast<uint32_t>(
                    wn::runtime::graphics::resource_state::host_write) |
                static_cast<uint32_t>(
                    wn::runtime::graphics::resource_state::copy_source)));

    wn::runtime::graphics::buffer dst_buffer =
        device->create_buffer(buffer_size,
            static_cast<wn::runtime::graphics::resource_states>(
                static_cast<uint32_t>(
                    wn::runtime::graphics::resource_state::host_read) |
                static_cast<uint32_t>(
                    wn::runtime::graphics::resource_state::host_write) |
                static_cast<uint32_t>(
                    wn::runtime::graphics::resource_state::copy_dest)));

    wn::runtime::graphics::buffer_memory_requirements src_reqs =
        src_buffer.get_memory_requirements();
    wn::runtime::graphics::buffer_memory_requirements dst_reqs =
        dst_buffer.get_memory_requirements();

    wn::runtime::graphics::arena src_arena =
        device->create_arena(idx, src_reqs.size);
    src_buffer.bind_memory(&src_arena, 0);

    wn::runtime::graphics::arena dst_arena =
        device->create_arena(idx, dst_reqs.size);
    dst_buffer.bind_memory(&dst_arena, 0);

    wn::runtime::graphics::queue_ptr queue = device->create_queue();
    wn::runtime::graphics::command_allocator alloc =
        device->create_command_allocator();
    wn::runtime::graphics::clear_value value{};

    wn::runtime::graphics::image image = device->create_image(
        wn::runtime::graphics::image_create_info{GetParam(), GetParam(),
            wn::runtime::graphics::data_format::r8g8b8a8_unorm,
            static_cast<wn::runtime::graphics::resource_states>(
                static_cast<uint32_t>(
                    wn::runtime::graphics::resource_state::copy_source) |
                static_cast<uint32_t>(
                    wn::runtime::graphics::resource_state::copy_dest)),
            2},
        value);
    wn::runtime::graphics::image_memory_requirements reqs =
        image.get_memory_requirements();

    for (idx = 0; idx < properties.size(); ++idx) {
      if (properties[idx].allow_images && properties[idx].device_local) {
        break;
      }
    }
    ASSERT_NE(properties.size(), idx);

    wn::runtime::graphics::arena image_arena =
        device->create_arena(idx, reqs.size);
    image.bind_memory(&image_arena, 0);

    for (size_t i = 0; i < 2; ++i) {
      const wn::runtime::graphics::image::image_buffer_resource_info&
          resource_info = image.get_buffer_requirements(static_cast<uint32_t>(i));

      void* src_data = src_buffer.map();
      ASSERT_NE(src_data, nullptr);

      uint8_t* src_memory = static_cast<uint8_t*>(src_data);

      for (size_t y = 0;
           y < GetParam() / (static_cast<size_t>(1) << static_cast<size_t>(i));
           ++y) {
        for (size_t x = 0; x < GetParam() / (static_cast<size_t>(1)
                                                << static_cast<size_t>(i));
             ++x) {
          src_memory[y * resource_info.row_pitch_in_bytes + 4 * x] =
              (x + y) % 0xFF;
          src_memory[y * resource_info.row_pitch_in_bytes + 4 * x + 1] =
              y & 0xFF;
          src_memory[y * resource_info.row_pitch_in_bytes + 4 * x + 2] = 0x00;
          src_memory[y * resource_info.row_pitch_in_bytes + 4 * x + 3] = 0xFF;
        }
      }
      src_buffer.unmap();

      wn::runtime::graphics::command_list_ptr list =
          alloc.create_command_list();
      wn::runtime::graphics::fence completion_fence = device->create_fence();

      list->transition_resource(src_buffer,
          wn::runtime::graphics::resource_state::host_write,
          wn::runtime::graphics::resource_state::copy_source);
      list->transition_resource(dst_buffer,
          wn::runtime::graphics::resource_state::initial,
          wn::runtime::graphics::resource_state::copy_dest);
      list->transition_resource(image, 0, 2,
          wn::runtime::graphics::resource_state::initial,
          wn::runtime::graphics::resource_state::copy_dest);
      list->copy_buffer_to_image(
          src_buffer, 0, image, static_cast<uint32_t>(i));
      list->transition_resource(image, 0, 2,
          wn::runtime::graphics::resource_state::copy_dest,
          wn::runtime::graphics::resource_state::copy_source);
      list->copy_image_to_buffer(
          image, static_cast<uint32_t>(i), dst_buffer, 0);
      list->transition_resource(dst_buffer,
          wn::runtime::graphics::resource_state::copy_dest,
          wn::runtime::graphics::resource_state::host_read);

      list->finalize();

      queue->enqueue_command_list(list.get());
      queue->enqueue_fence(completion_fence);

      completion_fence.wait();

      void* dst_data = dst_buffer.map();
      ASSERT_NE(dst_data, nullptr);

      uint8_t* dst_memory = static_cast<uint8_t*>(dst_data);

      uint32_t num_failed_channels = 0;
      for (size_t y = 0;
           y < GetParam() / (static_cast<size_t>(1) << static_cast<size_t>(i));
           ++y) {
        for (size_t x = 0; x < GetParam() / (static_cast<size_t>(1)
                                                << static_cast<size_t>(i));
             ++x) {
          if (dst_memory[y * resource_info.row_pitch_in_bytes + 4 * x] !=
              (x + y) % 0xFF) {
            num_failed_channels += 1;
          }
          if (dst_memory[y * resource_info.row_pitch_in_bytes + 4 * x + 1] !=
              (y & 0xFF)) {
            num_failed_channels += 1;
          }
          if (dst_memory[y * resource_info.row_pitch_in_bytes + 4 * x + 2] !=
              0x00) {
            num_failed_channels += 1;
          }
          if (dst_memory[y * resource_info.row_pitch_in_bytes + 4 * x + 3] !=
              0xFF) {
            num_failed_channels += 1;
          }
        }
      }
      ASSERT_EQ(0u, num_failed_channels);
    }
  }

  m_log->flush();
  // On normal operation the log buffer should be empty.
  EXPECT_EQ("", m_buffer);
}

INSTANTIATE_TEST_CASE_P(one_value, image_mip_tests, ::testing::Values(128));
