// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNApplicationData/inc/WNApplicationData.h"
#include "WNApplicationTest/inc/WNTestHarness.h"
#include "WNGraphics/inc/WNCommandAllocator.h"
#include "WNGraphics/inc/WNCommandList.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNGraphics/inc/WNFactory.h"
#include "WNGraphics/inc/WNFence.h"
#include "WNGraphics/inc/WNQueue.h"
#include "WNGraphics/inc/WNSignal.h"
#include "WNGraphics/inc/WNSwapchain.h"
#include "WNMultiTasking/inc/WNJobPool.h"
#include "WNMultiTasking/inc/WNJobSignal.h"
#include "WNWindow/inc/WNWindow.h"
#include "WNWindow/inc/WNWindowFactory.h"

TEST(swapchain, basic) {
  auto& data = wn::runtime::testing::k_application_data;
  wn::runtime::graphics::factory device_factory(
      data->system_allocator, data->default_log);
  wn::memory::allocator* allocator = data->system_allocator;

  wn::runtime::window::window_factory factory(
      data->system_allocator, data->default_log);

  for (auto& adapter : device_factory.query_adapters()) {
    wn::multi_tasking::job_signal signal(0);
    // If we make this window too small, as soon as the swapchain
    // is used at least once, it immediately goes out of date.

    // To fix this, make the window bigger. We DO have to handle
    // out of date swapchains, but we don't want to have to do it
    // in this test.
    wn::memory::unique_ptr<wn::runtime::window::window> wind =
        factory.create_window(wn::runtime::window::window_type::system,
            data->default_job_pool, &signal, data, 100, 100, 300, 300);

    // Wait until the window has successfully been created.
    signal.wait_until(1);

    auto device =
        adapter->make_device(data->system_allocator, data->default_log);
    wn::runtime::graphics::queue_ptr queue = device->create_queue();
    wn::runtime::graphics::command_allocator alloc =
        device->create_command_allocator();

    wn::core::pair<wn::runtime::graphics::surface,
        wn::runtime::graphics::graphics_error>
        surface = adapter->make_surface(wind.get());
    ASSERT_EQ(wn::runtime::graphics::graphics_error::ok, surface.second);
    wn::runtime::graphics::fence done_present_fence = device->create_fence();

    // Create a swapchain for the window.
    const wn::runtime::graphics::swapchain_create_info create_info = {
        wn::runtime::graphics::data_format::r8g8b8a8_unorm, 2,
        wn::runtime::graphics::swap_mode::fifo,
        wn::runtime::graphics::discard_policy::discard};

    auto swapchain =
        device->create_swapchain(surface.first, create_info, queue.get());

    struct per_swapchain_image_data {
      per_swapchain_image_data(wn::runtime::graphics::device* _device)
        : m_present_signal(_device->create_signal()) {}
      wn::runtime::graphics::command_list_ptr commmand_lists;
      wn::memory::unique_ptr<wn::runtime::graphics::fence> m_ready_fence;
      wn::memory::unique_ptr<wn::runtime::graphics::signal> m_ready_signal;
      wn::runtime::graphics::signal m_present_signal;
    };

    wn::containers::dynamic_array<per_swapchain_image_data> swapchain_data(
        allocator);
    swapchain_data.reserve(swapchain->info().num_buffers);
    for (size_t i = 0; i < swapchain->info().num_buffers; ++i) {
      swapchain_data.emplace_back(device.get());
    }

    // Let's make sure we can render 10 frames successfully on the swapchain
    for (size_t i = 0; i < 10; ++i) {
      wn::memory::unique_ptr<wn::runtime::graphics::signal> sig =
          wn::memory::make_unique<wn::runtime::graphics::signal>(
              allocator, device->create_signal());

      uint32_t idx = swapchain->get_next_backbuffer_index(nullptr, sig.get());
      data->default_log->log_info("Got backbuffer: ", idx);
      auto& frame_data = swapchain_data[idx];
      if (frame_data.m_ready_fence) {
        frame_data.m_ready_fence->wait();
        frame_data.m_ready_fence->reset();
      } else {
        frame_data.m_ready_fence =
            wn::memory::make_unique<wn::runtime::graphics::fence>(
                allocator, device->create_fence());
      }
      frame_data.m_ready_signal = wn::core::move(sig);

      wn::runtime::graphics::command_list_ptr list =
          alloc.create_command_list();
      list->transition_resource(*swapchain->get_image_for_index(idx),
          wn::runtime::graphics::resource_state::present,
          wn::runtime::graphics::resource_state::render_target);

      list->transition_resource(*swapchain->get_image_for_index(idx),
          wn::runtime::graphics::resource_state::render_target,
          wn::runtime::graphics::resource_state::present);

      list->finalize();

      queue->enqueue_command_lists({list.get()},
          {wn::core::make_pair(
              static_cast<wn::runtime::graphics::pipeline_stages>(wn::runtime::
                      graphics::pipeline_stage::color_attachment_output),
              frame_data.m_ready_signal.get())},
          frame_data.m_ready_fence.get(), {&frame_data.m_present_signal});

      frame_data.commmand_lists = wn::core::move(list);
      swapchain->present(queue.get(), &frame_data.m_present_signal, idx);
    }
    queue->enqueue_fence(done_present_fence);
    done_present_fence.wait();
  }
}
