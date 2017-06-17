// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/WNSwapchain.h"
#include "WNApplicationData/inc/WNApplicationData.h"
#include "WNApplicationData/inc/WNApplicationData.h"
#include "WNApplicationTest/inc/WNTestHarness.h"
#include "WNGraphics/inc/WNCommandAllocator.h"
#include "WNGraphics/inc/WNCommandList.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNGraphics/inc/WNFactory.h"
#include "WNGraphics/inc/WNFence.h"
#include "WNGraphics/inc/WNQueue.h"
#include "WNMultiTasking/inc/WNJobPool.h"
#include "WNMultiTasking/inc/WNJobSignal.h"
#include "WNWindow/inc/WNWindow.h"
#include "WNWindow/inc/WNWindowFactory.h"

TEST(swapchain, basic) {
  auto& data = wn::runtime::testing::k_application_data;
  wn::runtime::graphics::factory device_factory(
      data->system_allocator, data->default_log);
  auto& adapter = device_factory.query_adapters()[0];
  auto device = adapter->make_device(data->system_allocator, data->default_log);
  wn::runtime::graphics::queue_ptr queue = device->create_queue();
  wn::runtime::graphics::command_allocator alloc =
      device->create_command_allocator();

  wn::multi_tasking::job_signal signal(0);
  wn::runtime::window::window_factory factory(
      data->system_allocator, data->default_log);
  wn::memory::unique_ptr<wn::runtime::window::window> wind =
      factory.create_window(wn::runtime::window::window_type::system,
          data->default_job_pool, &signal, data, 100, 100, 100, 100);

  // Wait until the window has successfully been created.
  signal.wait_until(1);

  // Create a swapchain for the window.
  const wn::runtime::graphics::swapchain_create_info create_info = {
      wn::runtime::graphics::data_format::r8g8b8a8_unorm, 2,
      wn::runtime::graphics::swap_mode::fifo,
      wn::runtime::graphics::discard_policy::discard};
  auto swapchain =
      device->create_swapchain(create_info, queue.get(), wind.get());
  wn::runtime::graphics::fence ready_fence = device->create_fence();

  // Let's make sure we can render 10 frames successfully on the swapchain
  for (size_t i = 0; i < 10; ++i) {
    ready_fence.reset();
    uint32_t idx = swapchain->get_backbuffer_index(&ready_fence);
    data->default_log->log_info("Got backbuffer: ", idx);

    // This will be our normal flow: transition the image from
    //  present to render_target, do something, transition back.

    ready_fence.wait();
    // In fact, this interface should change slightly, the first
    // transition should implicitly be enqueued on one of the queues.
    wn::runtime::graphics::command_list_ptr list = alloc.create_command_list();
    wn::runtime::graphics::fence completion_fence = device->create_fence();
    list->transition_resource(*swapchain->get_image_for_index(idx),
        wn::runtime::graphics::resource_state::present,
        wn::runtime::graphics::resource_state::render_target);

    list->transition_resource(*swapchain->get_image_for_index(idx),
        wn::runtime::graphics::resource_state::render_target,
        wn::runtime::graphics::resource_state::present);

    list->finalize();
    queue->enqueue_command_list(list.get());
    queue->enqueue_fence(completion_fence);

    completion_fence.wait();
    completion_fence.reset();

    swapchain->present(queue.get(), idx);
  }
}
