// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNWindow/inc/WNXCBWindow.h"
#include "WNApplication/inc/WNApplicationData.h"
#include "WNEntryPoint/inc/WNEntryData.h"
#include "WNMultiTasking/inc/WNJobPool.h"
#include "WNMultiTasking/inc/WNJobSignal.h"

namespace wn {
namespace runtime {
namespace window {

window_error xcb_window::initialize() {
  m_job_pool->add_job(
      &m_destroy_signal, &xcb_window::dispatch_loop, this, nullptr);
  return window_error::ok;
}

void xcb_window::dispatch_loop(void*) {
  m_job_pool->call_blocking_function([&]() {
    m_connection = xcb_connect(NULL, NULL);
    if (m_connection == nullptr) {
      return;
    }

    const xcb_setup_t* setup = xcb_get_setup(m_connection);
    m_screen = xcb_setup_roots_iterator(setup).data;

    if (m_screen == nullptr) {
      xcb_disconnect(m_connection);
      m_connection = nullptr;
      m_creation_signal->increment(1);
      m_create_signal.increment(1);
      return;
    }

    m_window = xcb_generate_id(m_connection);

    uint32_t values[1] = {XCB_EVENT_MASK_STRUCTURE_NOTIFY};

    xcb_create_window(m_connection, XCB_COPY_FROM_PARENT, m_window,
        m_screen->root, m_x, m_y, m_width, m_height, 10,
        XCB_WINDOW_CLASS_INPUT_OUTPUT, m_screen->root_visual, XCB_CW_EVENT_MASK,
        values);

    if (m_creation_signal) {
      m_creation_signal->increment(1);
    }

    const char* protocol = "WM_PROTOCOLS";
    xcb_intern_atom_cookie_t cookie =
        xcb_intern_atom(m_connection, true, strlen(protocol), protocol);
    xcb_intern_atom_reply_t* reply =
        xcb_intern_atom_reply(m_connection, cookie, NULL);

    const char* delete_window = "WM_DELETE_WINDOW";
    cookie = xcb_intern_atom(
        m_connection, true, strlen(delete_window), delete_window);
    m_delete_window = xcb_intern_atom_reply(m_connection, cookie, NULL);

    xcb_change_property(m_connection, XCB_PROP_MODE_REPLACE, m_window,
        reply->atom, 4, 32, 1, &m_delete_window->atom);

    free(reply);

    xcb_map_window(m_connection, m_window);

    xcb_flush(m_connection);

    m_create_signal.increment(1);

    xcb_generic_event_t* event;
    while ((event = xcb_wait_for_event(m_connection))) {
      switch (event->response_type & 0x7F) {
        case XCB_CLIENT_MESSAGE:
          if ((*(xcb_client_message_event_t*)event).data.data32[0] ==
              m_delete_window->atom) {
            free(event);
            return;
          }
        case XCB_DESTROY_NOTIFY:
          free(event);
          return;
        default:
          break;
      }
      free(event);
    }
  });
}

}  // namespace window
}  // namespace runtime
}  // namespace wn
