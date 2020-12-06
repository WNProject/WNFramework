// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNApplicationData/inc/WNApplicationData.h"
#include "WNMultiTasking/inc/WNJobPool.h"
#include "WNMultiTasking/inc/WNJobSignal.h"
#include "WNWindow/inc/WNWindow.h"
#include "WNWindow/inc/WNWindowFactory.h"

int32_t wn_application_main(
    const wn::runtime::application::application_data* _data) {
  wn::multi_tasking::job_signal signal(0);
  wn::runtime::window::window_factory factory(
      _data->system_allocator, _data->default_log);
  wn::memory::unique_ptr<wn::runtime::window::window> wind =
      factory.create_window(wn::runtime::window::window_type::system,
          _data->default_job_pool, &signal, _data, 100, 100, 100, 100);
  signal.wait_until(1);
  if (!wind->is_valid()) {
    _data->default_log->log_critical("Could not create window");
  }

  _data->default_job_pool->call_blocking_function([&]() {
    wn::multi_tasking::this_thread::sleep_for(std::chrono::seconds(5));
  });
  return 0;
}
