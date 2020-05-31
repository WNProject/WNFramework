// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNWindow/inc/WNMacOSWindow.h"

#include "WNApplicationData/inc/WNApplicationData.h"
#include "WNMultiTasking/inc/job_pool.h"
#include "WNMultiTasking/inc/job_signal.h"
#include "WNMultiTasking/inc/work_queue.h"
#include "executable_data/inc/executable_data.h"

extern "C" {
void* macos_internal_create_window(uint32_t width, uint32_t height);
void macos_internal_single_loop();
void macos_internal_close_window(void* _view);
}

namespace wn {
namespace runtime {
namespace window {

window_error macos_window::initialize() {
  m_job_pool->call_blocking_function([this]() {
    m_app_data->main_thread_work_queue->run_work_sync(
        functional::function<void()>(m_allocator, [this]() {
          void* v = macos_internal_create_window(m_width, m_height);
          m_native_window_handle = v;
          if (m_creation_signal) {
            m_creation_signal->increment(1);
          }
          m_create_signal.increment(1);
        }));
  });
  m_app_data->main_thread_work_queue->run_defer_async(
      functional::function<void()>(m_allocator, [this]() { run_loop(); }));

  return window_error::ok;
}

void macos_window::run_loop() {
  macos_internal_single_loop();
  if (!m_exit) {
    m_app_data->main_thread_work_queue->run_defer_async(
        functional::function<void()>(m_allocator, [this]() { run_loop(); }));
  } else {
    macos_internal_close_window(m_native_window_handle);
    m_destroy_signal.increment(1);
  }
}

}  // namespace window
}  // namespace runtime
}  // namespace wn
