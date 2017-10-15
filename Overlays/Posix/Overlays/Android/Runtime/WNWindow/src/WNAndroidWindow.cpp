// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNWindow/inc/WNAndroidWindow.h"
#include "WNApplicationData/inc/WNApplicationData.h"
#include "WNExecutable/inc/WNEntryData.h"
#include "WNMultiTasking/inc/WNJobPool.h"
#include "WNMultiTasking/inc/WNJobSignal.h"

namespace wn {
namespace runtime {
namespace window {

window_error android_window::initialize() {
  return window_error::ok;
}

void android_window::wait_for_window_loop(void*) {
  m_job_pool->call_blocking_function([&]() {
    if (!m_app_data->system_data->host_data->window_initialized ||
        !m_app_data->system_data->host_data->android_app) {
      if (m_creation_signal) {
        m_creation_signal->increment(1);
      }
      return;
    }
    while (true) {
      if (m_destroy.load()) {
        if (m_creation_signal) {
          m_creation_signal->increment(1);
        }
        return;
      } else if (m_app_data->system_data->host_data->window_initialized
                     ->load()) {
        m_data.window = m_app_data->system_data->host_data->android_app->window;
        if (m_creation_signal) {
          m_creation_signal->increment(1);
        }
        return;
      }
      wn::multi_tasking::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  });
}

}  // namespace window
}  // namespace runtime
}  // namespace wn
