// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNWindow/inc/WNXCBWindow.h"

#include "WNApplicationData/inc/WNApplicationData.h"
#include "WNMultiTasking/inc/job_pool.h"
#include "WNMultiTasking/inc/job_signal.h"
#include "executable_data/inc/executable_data.h"

#include <X11/Xlib-xcb.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

namespace wn {
namespace runtime {
namespace window {

window_error xcb_window::initialize() {
  m_job_pool->add_job(
      &m_destroy_signal, &xcb_window::dispatch_loop, this, nullptr);
  return window_error::ok;
}

inline key_code x_code_to_keycode(xcb_keysym_t keycode) {
  if (keycode >= XK_A && keycode <= XK_Z) {
    return static_cast<key_code>(
        static_cast<size_t>(key_code::key_a) + keycode - XK_A);
  }
  if (keycode >= XK_a && keycode <= XK_z) {
    return static_cast<key_code>(
        static_cast<size_t>(key_code::key_a) + keycode - XK_a);
  }
  if (keycode >= XK_KP_0 && keycode <= XK_KP_9) {
    return static_cast<key_code>(
        static_cast<size_t>(key_code::key_num_0) + keycode - XK_KP_0);
  }
  if (keycode >= XK_0 && keycode <= XK_9) {
    return static_cast<key_code>(
        static_cast<size_t>(key_code::key_0) + keycode - XK_0);
  }

  switch (keycode) {
    case XK_Shift_L:
      return key_code::key_lshift;
    case XK_Shift_R:
      return key_code::key_rshift;
    case XK_Control_L:
      return key_code::key_lctrl;
    case XK_Control_R:
      return key_code::key_rctrl;
    case XK_Alt_L:
      return key_code::key_lalt;
    case XK_Alt_R:
      return key_code::key_ralt;
    case XK_Escape:
      return key_code::key_esc;
    case XK_space:
      return key_code::key_space;
    case XK_Left:
      return key_code::key_left;
    case XK_Up:
      return key_code::key_up;
    case XK_Right:
      return key_code::key_right;
    case XK_Down:
      return key_code::key_down;
    case XK_Delete:
      return key_code::key_del;
    case XK_Return:
      return key_code::key_return;
    case XK_Tab:
      return key_code::key_tab;
    case XK_BackSpace:
      return key_code::key_backspace;
  }

  return key_code::key_max;
}

void xcb_window::dispatch_loop(void*) {
  m_job_pool->call_blocking_function([&]() {
    m_data.display = XOpenDisplay(NULL);
    if (m_data.display == nullptr) {
      return;
    }

    m_data.connection = XGetXCBConnection(m_data.display);
    if (m_data.connection == nullptr) {
      return;
    }

    m_key_symbols = xcb_key_symbols_alloc(m_data.connection);

    const xcb_setup_t* setup = xcb_get_setup(m_data.connection);
    m_screen = xcb_setup_roots_iterator(setup).data;

    if (m_screen == nullptr) {
      xcb_disconnect(m_data.connection);
      m_data.connection = nullptr;
      if (m_creation_signal) {
        m_creation_signal->increment(1);
      }

      m_create_signal.increment(1);
      return;
    }

    m_data.window = xcb_generate_id(m_data.connection);

    uint32_t values[1] = {
        XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_BUTTON_PRESS |
        XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION |
        XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW |
        XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE};

    xcb_create_window(m_data.connection, XCB_COPY_FROM_PARENT, m_data.window,
        m_screen->root, m_x, m_y, m_width, m_height, 10,
        XCB_WINDOW_CLASS_INPUT_OUTPUT, m_screen->root_visual, XCB_CW_EVENT_MASK,
        values);

    const char* protocol = "WM_PROTOCOLS";
    xcb_intern_atom_cookie_t cookie =
        xcb_intern_atom(m_data.connection, true, strlen(protocol), protocol);
    xcb_intern_atom_reply_t* reply =
        xcb_intern_atom_reply(m_data.connection, cookie, NULL);

    const char* delete_window = "WM_DELETE_WINDOW";
    cookie = xcb_intern_atom(
        m_data.connection, true, strlen(delete_window), delete_window);
    m_delete_window = xcb_intern_atom_reply(m_data.connection, cookie, NULL);

    xcb_change_property(m_data.connection, XCB_PROP_MODE_REPLACE, m_data.window,
        reply->atom, 4, 32, 1, &m_delete_window->atom);

    free(reply);

    xcb_map_window(m_data.connection, m_data.window);

    xcb_flush(m_data.connection);

    if (m_creation_signal) {
      m_creation_signal->increment(1);
    }

    m_create_signal.increment(1);
    std::array<char, 16> buf{};

    xcb_generic_event_t* event;
    while ((event = xcb_wait_for_event(m_data.connection))) {
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
        case XCB_BUTTON_PRESS: {
          xcb_button_press_event_t* bp = (xcb_button_press_event_t*)event;
          switch (bp->detail) {
            case XCB_BUTTON_INDEX_1:
              m_mouse_states[static_cast<size_t>(mouse_button::mouse_l)] = true;
              dispatch_input(input_event::mouse_down(mouse_button::mouse_l));
              break;
            case XCB_BUTTON_INDEX_2:
              m_mouse_states[static_cast<size_t>(mouse_button::mouse_r)] = true;
              dispatch_input(input_event::mouse_down(mouse_button::mouse_r));
              break;
            case XCB_BUTTON_INDEX_3:
              m_mouse_states[static_cast<size_t>(mouse_button::mouse_m)] = true;
              dispatch_input(input_event::mouse_down(mouse_button::mouse_m));
              break;
          }
          break;
        }
        case XCB_BUTTON_RELEASE: {
          xcb_button_press_event_t* bp = (xcb_button_press_event_t*)event;
          switch (bp->detail) {
            case XCB_BUTTON_INDEX_1:
              m_mouse_states[static_cast<size_t>(mouse_button::mouse_l)] =
                  false;
              dispatch_input(input_event::mouse_up(mouse_button::mouse_l));
              break;
            case XCB_BUTTON_INDEX_2:
              m_mouse_states[static_cast<size_t>(mouse_button::mouse_r)] =
                  false;
              dispatch_input(input_event::mouse_up(mouse_button::mouse_r));
              break;
            case XCB_BUTTON_INDEX_3:
              m_mouse_states[static_cast<size_t>(mouse_button::mouse_m)] =
                  false;
              dispatch_input(input_event::mouse_up(mouse_button::mouse_m));
              break;
          }
          break;
        }
        case XCB_ENTER_NOTIFY:
          m_has_mouse_focus = true;
          break;
        case XCB_LEAVE_NOTIFY:
          for (size_t i = 0; i < static_cast<size_t>(key_code::key_max); ++i) {
            m_key_states[i] = false;
          }
          for (size_t i = 0; i < static_cast<size_t>(mouse_button::mouse_max);
               ++i) {
            m_mouse_states[i] = false;
          }
          m_has_mouse_focus = false;
          break;
        case XCB_MOTION_NOTIFY: {
          xcb_motion_notify_event_t* motion = (xcb_motion_notify_event_t*)event;
          m_cursor_x = motion->event_x;
          m_cursor_y = motion->event_y;
          dispatch_input(input_event::mouse_move(m_cursor_x, m_cursor_y));

          break;
        }
        case XCB_KEY_PRESS: {
          if (!m_has_mouse_focus) {
            break;
          }
          xcb_key_press_event_t* kp = (xcb_key_press_event_t*)event;
          key_code key = x_code_to_keycode(
              xcb_key_press_lookup_keysym(m_key_symbols, kp, 0));
          m_key_states[static_cast<size_t>(key)] = true;
          dispatch_input(input_event::key_event(event_type::key_down, key));
          XKeyEvent evt;
          evt.display = m_data.display;
          evt.keycode = kp->detail;
          evt.state = kp->state;

          if (XLookupString(&evt, buf.data(), buf.size(), nullptr, nullptr)) {
            if (buf[0] != '\b') {
              dispatch_input(input_event::text_input(buf[0]));
            }
          }
          break;
        }
        case XCB_KEY_RELEASE: {
          xcb_key_release_event_t* kr = (xcb_key_release_event_t*)event;
          key_code key = x_code_to_keycode(
              xcb_key_release_lookup_keysym(m_key_symbols, kr, 0));
          m_key_states[static_cast<size_t>(key)] = false;
          dispatch_input(input_event::key_event(event_type::key_up, key));
          break;
        }
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
