// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNApplicationData/inc/WNApplicationData.h"
#include "WNApplicationTest/inc/WNTestHarness.h"
#include "WNMultiTasking/inc/job_pool.h"
#include "WNMultiTasking/inc/job_signal.h"
#include "WNWindow/inc/WNWindow.h"
#include "WNWindow/inc/WNWindowFactory.h"

TEST(window, basic) {
  wn::multi_tasking::job_signal signal(0);
  wn::runtime::window::window_factory factory(
      wn::runtime::testing::k_application_data->system_allocator,
      wn::runtime::testing::k_application_data->default_log);
  wn::memory::unique_ptr<wn::runtime::window::window> wind =
      factory.create_window(wn::runtime::window::window_type::system,
          wn::runtime::testing::k_application_data->default_job_pool, &signal,
          wn::runtime::testing::k_application_data, 100, 100, 100, 100);
  signal.wait_until(1);
  if (!wind->is_valid()) {
    wn::runtime::testing::k_application_data->default_log->log_critical(
        "Could not create window");
  }

  wn::runtime::testing::k_application_data->default_job_pool
      ->call_blocking_function([&]() {
        wn::multi_tasking::this_thread::sleep_for(
            std::chrono::milliseconds(100));
      });
}
