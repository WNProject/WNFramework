// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_RUNTIME_WINDOW_XCB_WINDOW_H__
#define __WN_RUNTIME_WINDOW_XCB_WINDOW_H__

#include "WNLogging/inc/WNLog.h"
#include "WNMultiTasking/inc/job_signal.h"
#include "WNWindow/inc/WNWindow.h"

#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>

typedef struct _XDisplay Display;

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

struct xcb_native_data {
  Display* display;
  xcb_connection_t* connection;
  xcb_window_t window;
};

class xcb_window : public window {
public:
  xcb_window(memory::allocator* _allocator, logging::log* _log,
      multi_tasking::job_pool* _job_pool,
      multi_tasking::job_signal* _creation_signal,
      const application::application_data* _data, uint32_t _x, uint32_t _y,
      uint32_t _width, uint32_t _height)
    : window(_allocator),
      m_log(_log),
      m_job_pool(_job_pool),
      m_app_data(_data),
      m_x(_x),
      m_y(_y),
      m_width(_width),
      m_height(_height),
      m_exit(false),
      m_data({nullptr, 0}),
      m_create_signal(_job_pool, 0),
      m_destroy_signal(_job_pool, 0),
      m_creation_signal(_creation_signal),
      m_key_symbols(nullptr) {}
  ~xcb_window() {
    m_create_signal.wait_until(1);
    if (m_data.connection) {
      xcb_destroy_notify_event_t* event =
          reinterpret_cast<xcb_destroy_notify_event_t*>(alloca(32));
      event->event = m_data.window;
      event->window = m_data.window;
      event->response_type = XCB_DESTROY_NOTIFY;
      xcb_key_symbols_free(m_key_symbols);
      xcb_send_event(m_data.connection, false, m_data.window,
          XCB_EVENT_MASK_STRUCTURE_NOTIFY, (char*)event);
      xcb_flush(m_data.connection);
      m_destroy_signal.wait_until(1);
      xcb_disconnect(m_data.connection);
    }
  }

  const void* get_native_handle() const override {
    return reinterpret_cast<const void*>(&m_data);
  }

  window_error initialize() override;
  bool is_valid() const override {
    return m_data.window != 0;
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

  bool get_key_state(key_code _key) const override {
    return m_key_states[static_cast<uint32_t>(_key)];
  }

  bool get_mouse_state(mouse_button _button) const override {
    return m_mouse_states[static_cast<uint32_t>(_button)];
  }

  uint32_t get_cursor_x() const override {
    return m_cursor_x;
  }

  uint32_t get_cursor_y() const override {
    return m_cursor_y;
  }

private:
  void dispatch_loop(void* _unused);

  logging::log* m_log;
  multi_tasking::job_pool* m_job_pool;
  const application::application_data* m_app_data;
  uint32_t m_x;
  uint32_t m_y;
  uint32_t m_width;
  uint32_t m_height;
  bool m_exit;
  xcb_native_data m_data;
  xcb_screen_t* m_screen;
  xcb_intern_atom_reply_t* m_delete_window;

  multi_tasking::job_signal m_create_signal;
  multi_tasking::job_signal m_destroy_signal;
  multi_tasking::job_signal* m_creation_signal;

  std::atomic<uint32_t> m_cursor_x;
  std::atomic<uint32_t> m_cursor_y;

  xcb_key_symbols_t* m_key_symbols;
  bool m_has_mouse_focus = false;
};

}  // namespace window
}  // namespace runtime
}  // namespace wn

#endif  // __WN_RUNTIME_WINDOW_XCB_WINDOW_H__
