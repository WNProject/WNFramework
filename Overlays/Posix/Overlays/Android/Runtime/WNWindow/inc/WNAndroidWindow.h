// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_RUNTIME_WINDOW_XCB_WINDOW_H__
#define __WN_RUNTIME_WINDOW_XCB_WINDOW_H__

#include "WNLogging/inc/WNLog.h"
#include "WNMultiTasking/inc/job_pool.h"
#include "WNMultiTasking/inc/job_signal.h"
#include "WNMultiTasking/inc/thread.h"
#include "WNWindow/inc/WNWindow.h"

#include <android/native_window.h>

namespace wn {
namespace memory {
class allocator;
}  // namespace memory

namespace multi_tasking {
class job_pool;
}  // namespace multi_tasking

namespace runtime {
namespace application {
struct application_data;
}  // namespace application
namespace window {

struct android_native_data {
  ANativeWindow* window = nullptr;
};

class android_window : public window {
public:
  android_window(logging::log* _log, multi_tasking::job_pool* _job_pool,
      multi_tasking::job_signal* _creation_signal,
      const application::application_data* _data, uint32_t _x, uint32_t _y,
      uint32_t _width, uint32_t _height)
    : m_log(_log),
      m_job_pool(_job_pool),
      m_app_data(_data),
      m_destroy(false),
      m_valid(false),
      m_x(_x),
      m_y(_y),
      m_width(_width),
      m_height(_height),
      m_creation_signal(_creation_signal),
      m_destroy_signal(_job_pool, 0) {
    m_job_pool->add_job(&m_destroy_signal,
        &android_window::wait_for_window_loop, this, nullptr);
  }
  ~android_window() {
    m_destroy.store(true);
    m_destroy_signal.wait_until(1);
  }

  const void* get_native_handle() const override {
    return reinterpret_cast<const void*>(&m_data);
  }

  window_error initialize() override;
  bool is_valid() const override {
    return m_data.window != nullptr;
  }

  window_type type() const override {
    return window_type::system;
  }

  uint32_t get_width() const override {
    return m_width;
  }

  uint32_t get_height() const override {
    return m_height;
  }

  void wait_for_window_loop(void* _unused);

  bool get_key_state(key_code _code) const override {
    return false;
  }
  bool get_mouse_state(mouse_button _button) const override {
    return false;
  }

  uint32_t get_cursor_x() const override {
    return 0;
  }
  uint32_t get_cursor_y() const override {
    return 0;
  }

private:
  logging::log* m_log;
  multi_tasking::job_pool* m_job_pool;
  const application::application_data* m_app_data;
  std::atomic<bool> m_destroy;
  bool m_valid;
  uint32_t m_x;
  uint32_t m_y;
  uint32_t m_width;
  uint32_t m_height;

  multi_tasking::job_signal* m_creation_signal;
  multi_tasking::job_signal m_destroy_signal;

  android_native_data m_data;
};

}  // namespace window
}  // namespace runtime
}  // namespace wn

#endif  // __WN_RUNTIME_WINDOW_XCB_WINDOW_H__
