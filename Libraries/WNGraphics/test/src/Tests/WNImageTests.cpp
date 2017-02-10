// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/WNCommandAllocator.h"
#include "WNGraphics/inc/WNCommandList.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNGraphics/inc/WNFactory.h"
#include "WNGraphics/inc/WNFence.h"
#include "WNGraphics/inc/WNHeap.h"
#include "WNGraphics/inc/WNImage.h"
#include "WNGraphics/inc/WNQueue.h"
#include "WNGraphics/test/inc/WNTestFixture.h"

using image_transfer_tests = wn::graphics::testing::parameterized_test<size_t>;

TEST_P(image_transfer_tests, many_sizes) {
  wn::graphics::factory device_factory(&m_allocator, m_log);

  for (auto& adapter : device_factory.query_adapters()) {
    wn::graphics::device_ptr device = adapter->make_device(&m_allocator, m_log);
    ASSERT_NE(nullptr, device);
    // Everything should fit into a 16MB buffer.
    const size_t buffer_size = 16 * 1024 * 1024;
    wn::graphics::upload_heap upload = device->create_upload_heap(buffer_size);
    wn::graphics::download_heap download =
        device->create_download_heap(buffer_size);
    ASSERT_TRUE(upload.is_valid());
    ASSERT_TRUE(download.is_valid());

    wn::graphics::queue_ptr queue = device->create_queue();
    wn::graphics::command_allocator alloc = device->create_command_allocator();
    wn::graphics::command_list_ptr list = alloc.create_command_list();
    wn::graphics::fence completion_fence = device->create_fence();

    wn::graphics::image image =
      device->create_image(wn::graphics::image_create_info{
          GetParam(), GetParam()});

    list->enqueue_resource_transition(image,
      wn::graphics::resource_state::initial,
      wn::graphics::resource_state::copy_dest);

    const wn::graphics::image::image_buffer_resource_info& resource_info =
        image.get_resource_info();

    wn::graphics::download_heap_buffer<uint8_t> download_buffer =
        download.get_range(0, resource_info.total_memory_required);
    {
      wn::graphics::upload_heap_buffer<uint8_t> upload_buffer =
          upload.get_range<uint8_t>(resource_info.offset_in_bytes,
              resource_info.total_memory_required);

      for (size_t y = 0; y < GetParam(); ++y) {
        for (size_t x = 0; x < GetParam(); ++x) {
          upload_buffer.range()[y * resource_info.row_pitch_in_bytes + 4 * x] =
              (x + y) % 0xFF;
          upload_buffer
              .range()[y * resource_info.row_pitch_in_bytes + 4 * x + 1] = y & 0xFF;
          upload_buffer
              .range()[y * resource_info.row_pitch_in_bytes + 4 * x + 2] = 0x00;
          upload_buffer
              .range()[y * resource_info.row_pitch_in_bytes + 4 * x + 3] = 0xFF;
        }
      }

      list->enqueue_barrier(upload_buffer);
      list->enqueue_copy(upload_buffer, image);
      list->enqueue_resource_transition(image,
          wn::graphics::resource_state::copy_dest,
          wn::graphics::resource_state::copy_source);
      list->enqueue_copy(image, download_buffer);
      list->enqueue_barrier(download_buffer);
      list->finalize();

      upload_buffer.synchronize();
    }

    queue->enqueue_command_list(list.get());
    queue->enqueue_signal(completion_fence);

    completion_fence.wait();

    download_buffer.synchronize();

    for (size_t y = 0; y < GetParam(); ++y) {
      for (size_t x = 0; x < GetParam(); ++x) {
        EXPECT_EQ(download_buffer
                      .range()[y * resource_info.row_pitch_in_bytes + 4 * x],
            (x + y) % 0xFF);
        EXPECT_EQ(
            download_buffer
                .range()[y * resource_info.row_pitch_in_bytes + 4 * x + 1],
            y & 0xFF);
        EXPECT_EQ(
            download_buffer
                .range()[y * resource_info.row_pitch_in_bytes + 4 * x + 2],
            0x00);
        EXPECT_EQ(
            download_buffer
                .range()[y * resource_info.row_pitch_in_bytes + 4 * x + 3],
            0xFF);
      }
    }
  }

  m_log->flush();
  // On normal operation the log buffer should be empty.
  EXPECT_EQ("", m_buffer);
}

INSTANTIATE_TEST_CASE_P(large_values, image_transfer_tests,
    ::testing::Values(37, 128, 1024 - 1, 1024 + 1));

using image_transfer_with_offset_tests =
    wn::graphics::testing::parameterized_test<std::tuple<size_t, size_t>>;

TEST_P(image_transfer_with_offset_tests, several_offsets) {
  wn::graphics::factory device_factory(&m_allocator, m_log);

  for (auto& adapter : device_factory.query_adapters()) {
    wn::graphics::device_ptr device = adapter->make_device(&m_allocator, m_log);
    ASSERT_NE(nullptr, device);

    // Everything should fit into a 16MB buffer.
    const size_t buffer_size = 16 * 1024 * 1024;
    wn::graphics::upload_heap upload = device->create_upload_heap(buffer_size);
    wn::graphics::download_heap download =
        device->create_download_heap(buffer_size);
    ASSERT_TRUE(upload.is_valid());
    ASSERT_TRUE(download.is_valid());

    wn::graphics::queue_ptr queue = device->create_queue();
    wn::graphics::command_allocator alloc = device->create_command_allocator();
    wn::graphics::command_list_ptr list = alloc.create_command_list();
    wn::graphics::fence completion_fence = device->create_fence();

    wn::graphics::image image =
      device->create_image(wn::graphics::image_create_info{
          std::get<1>(GetParam()), std::get<1>(GetParam())});

    list->enqueue_resource_transition(image,
      wn::graphics::resource_state::initial,
      wn::graphics::resource_state::copy_dest);

    const wn::graphics::image::image_buffer_resource_info& resource_info =
        image.get_resource_info();

    const size_t buffer_offset =
        device->get_image_upload_buffer_alignment() * std::get<0>(GetParam());

    wn::graphics::download_heap_buffer<uint8_t> download_buffer =
        download.get_range(buffer_offset + resource_info.offset_in_bytes,
            resource_info.total_memory_required);
    {
      wn::graphics::upload_heap_buffer<uint8_t> upload_buffer =
          upload.get_range<uint8_t>(
              buffer_offset + resource_info.offset_in_bytes,
              resource_info.total_memory_required);
      for (size_t y = 0; y < std::get<1>(GetParam()); ++y) {
        for (size_t x = 0; x < std::get<1>(GetParam()); ++x) {
          upload_buffer.range()[y * resource_info.row_pitch_in_bytes + 4 * x] =
              (x + y) & 0xFF;
          upload_buffer
              .range()[y * resource_info.row_pitch_in_bytes + 4 * x + 1] = (y & 0xFF);
          upload_buffer
              .range()[y * resource_info.row_pitch_in_bytes + 4 * x + 2] = 0x00;
          upload_buffer
              .range()[y * resource_info.row_pitch_in_bytes + 4 * x + 3] = 0xFF;
        }
      }

      list->enqueue_barrier(upload_buffer);
      list->enqueue_copy(upload_buffer, image);
      list->enqueue_resource_transition(image,
          wn::graphics::resource_state::copy_dest,
          wn::graphics::resource_state::copy_source);
      list->enqueue_copy(image, download_buffer);
      list->enqueue_barrier(download_buffer);
      list->finalize();

      upload_buffer.synchronize();
    }

    queue->enqueue_command_list(list.get());
    queue->enqueue_signal(completion_fence);

    completion_fence.wait();

    download_buffer.synchronize();

    for (size_t y = 0; y < std::get<1>(GetParam()); ++y) {
      for (size_t x = 0; x < std::get<1>(GetParam()); ++x) {
        EXPECT_EQ((x + y) & 0xFF,
            download_buffer
                .range()[y * resource_info.row_pitch_in_bytes + 4 * x]);
        EXPECT_EQ(y & 0xFF,
            download_buffer
                .range()[y * resource_info.row_pitch_in_bytes + 4 * x + 1]);
        EXPECT_EQ(0x00,
            download_buffer
                .range()[y * resource_info.row_pitch_in_bytes + 4 * x + 2]);
        EXPECT_EQ(0xFF,
            download_buffer
                .range()[y * resource_info.row_pitch_in_bytes + 4 * x + 3]);
      }
    }
  }

  m_log->flush();
  // On normal operation the log buffer should be empty.
  EXPECT_EQ("", m_buffer);
}

INSTANTIATE_TEST_CASE_P(large_values, image_transfer_with_offset_tests,
    ::testing::Combine(::testing::Values(1, 7, 10),
                            ::testing::Values(10, 37, 128)));
