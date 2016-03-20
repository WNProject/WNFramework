// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/WNCommandAllocator.h"
#include "WNGraphics/inc/WNCommandList.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNGraphics/inc/WNFactory.h"
#include "WNGraphics/inc/WNFence.h"
#include "WNGraphics/inc/WNHeap.h"
#include "WNGraphics/inc/WNQueue.h"
#include "WNGraphics/test/inc/WNTestFixture.h"

using heap_copy_test = wn::graphics::testing::parameterized_test<size_t>;

TEST_P(heap_copy_test, many_sizes) {
  wn::graphics::factory device_factory(&m_allocator, &m_log);

  for (auto& adapter : device_factory.query_adapters()) {
    wn::graphics::device_ptr device =
        adapter->make_device(&m_allocator, &m_log);
    ASSERT_NE(nullptr, device);

    wn::graphics::upload_heap upload = device->create_upload_heap(GetParam());
    wn::graphics::download_heap download =
        device->create_download_heap(GetParam());
    ASSERT_TRUE(upload.is_valid());
    ASSERT_TRUE(download.is_valid());

    wn::graphics::queue_ptr queue = device->create_queue();
    wn::graphics::command_allocator alloc = device->create_command_allocator();
    wn::graphics::command_list_ptr list = alloc.create_command_list();
    wn::graphics::fence completion_fence = device->create_fence();
    wn::graphics::download_heap_buffer<uint8_t> download_buffer =
        download.get_range(0, GetParam());

    {
      wn::graphics::upload_heap_buffer<uint8_t> upload_buffer =
          upload.get_range(0, GetParam());
      list->enqueue_barrier(upload_buffer);
      list->enqueue_buffer_copy(upload_buffer, download_buffer);
      list->enqueue_barrier(download_buffer);
      list->finalize();

      for (size_t i = 0; i < GetParam(); ++i) {
        upload_buffer.range()[i] = i & 0xFF;
      }
      upload_buffer.synchronize();
    }

    queue->enqueue_command_list(list.get());
    queue->enqueue_signal(completion_fence);

    completion_fence.wait();

    download_buffer.synchronize();
    for (size_t i = 0; i < GetParam(); ++i) {
      EXPECT_EQ(download_buffer.range()[i], i & 0xFF);
    }
  }

  m_log.Flush();
  // On normal operation the log buffer should be empty.
  EXPECT_EQ("", m_buffer);
}

INSTANTIATE_TEST_CASE_P(
    small_values, heap_copy_test, ::testing::Range<size_t>(1, 50, 7));

INSTANTIATE_TEST_CASE_P(
    large_values, heap_copy_test, ::testing::Values(1024, 1024 - 1, 1024 * 1024,
                                      1024 * 1024 - 1));

// TODO(awoloszyn): Create a null device, and hook it up.
// Then create some tests around it for making sure ranges
// are flushed, and heaps are cleaned up.
