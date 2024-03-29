// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNWindow/inc/WNMacOSWindow.h"

#include "WNApplicationData/inc/WNApplicationData.h"
#include "WNContainers/inc/WNHashMap.h"
#include "WNMemory/inc/string_utility.h"
#include "WNMultiTasking/inc/job_pool.h"
#include "WNWindow/inc/helpers.h"
#include "executable_data/inc/executable_data.h"

extern "C" {
void* macos_internal_create_window(
    uint32_t* width, uint32_t* height, int* window_number);
bool macos_handle_event(macos_event* mcevent);
void macos_internal_close_window(void* _view);
}

namespace wn {
namespace runtime {
namespace window {

window_error macos_window::initialize() {
  m_job_pool->call_blocking_job_on_main(
      JOB_NAME, functional::function<void()>(m_allocator, [this]() {
        void* v =
            macos_internal_create_window(&m_width, &m_height, &m_window_number);
        m_native_window_handle = v;
        if (m_creation_signal) {
          m_creation_signal.increment_by(1);
        }
        m_create_signal.increment_by(1);
      }));
  m_job_pool->add_job_on_main(JOB_NAME,
      functional::function<void()>(m_allocator, [this]() { run_loop(); }),
      nullptr, multi_tasking::job_priority::standard, true);

  return window_error::ok;
}

// These run_lops are helpful, in that no matter how many windows
//   they will always be run on the main thread, and are therefore
//   threadsafe.
//   Since the events that are run from the ObjectiveC may come back for
//    ANY window we will fix the dispatching of that here in a static.
//   Statics are sad, but necessary I think here.
void macos_window::run_loop() {
  static containers::hash_map<int, macos_window*> _windows(
      m_app_data->system_allocator);
  auto it = _windows.find(m_window_number);
  if (it == _windows.end()) {
    _windows[m_window_number] = this;
  }
  macos_event evt;

  while (macos_handle_event(&evt)) {
    auto it = _windows.find(evt.window_number);
    if (it == _windows.end()) {
      continue;
    }
    switch (evt.type) {
      case macos_event_key_down: {
        m_key_states[static_cast<uint32_t>(
            static_cast<key_code>(evt.kb.code))] = true;
        it->second->dispatch_input(input_event::key_event(
            event_type::key_down, static_cast<key_code>(evt.kb.code)));

        size_t len = wn::memory::strlen(evt.kb.characters);
        for (size_t i = 0; i < len; ++i) {
          switch (evt.kb.characters[i]) {
            case 0x08:  // backspace
            case 0x0A:  // linefeed
            case 0x1B:  // escape
            case 0x09:  // tab
            case 0x0D:  // carriage return
              break;    // Ignore these for now
            default:
              it->second->dispatch_input(
                  input_event::text_input(evt.kb.characters[i]));
          }
        }
        break;
      }
      case macos_event_key_up: {
        m_key_states[static_cast<uint32_t>(
            static_cast<key_code>(evt.kb.code))] = false;
        it->second->dispatch_input(input_event::key_event(
            event_type::key_up, static_cast<key_code>(evt.kb.code)));
        break;
      }
      case macos_event_mouse_down:
        it->second->dispatch_input(
            input_event::mouse_down(static_cast<mouse_button>(evt.mb.button)));
        break;
      case macos_event_mouse_up:
        it->second->dispatch_input(
            input_event::mouse_up(static_cast<mouse_button>(evt.mb.button)));
      case macos_event_mouse_move:
        it->second->dispatch_input(input_event::mouse_move(evt.mb.x, evt.mb.y));
        break;
      case macos_event_mouse_wheel:
        it->second->dispatch_input(
            input_event::mouse_wheel(static_cast<int32_t>(evt.we.amount)));
        break;
      case macos_event_flags_changed:
        if (m_key_states[static_cast<uint32_t>(key_code::key_any_alt)] !=
            evt.fl.option) {
          m_key_states[static_cast<uint32_t>(key_code::key_any_alt)] =
              evt.fl.option;
          if (evt.fl.option) {
            it->second->dispatch_input(
                input_event::key_event(event_type::key_down,
                    static_cast<key_code>(key_code::key_any_alt)));
          } else {
            it->second->dispatch_input(
                input_event::key_event(event_type::key_up,
                    static_cast<key_code>(key_code::key_any_alt)));
          }
        }
        if (m_key_states[static_cast<uint32_t>(key_code::key_any_ctrl)] !=
            evt.fl.control) {
          m_key_states[static_cast<uint32_t>(key_code::key_any_ctrl)] =
              evt.fl.control;
          if (evt.fl.control) {
            it->second->dispatch_input(
                input_event::key_event(event_type::key_down,
                    static_cast<key_code>(key_code::key_any_ctrl)));
          } else {
            it->second->dispatch_input(
                input_event::key_event(event_type::key_up,
                    static_cast<key_code>(key_code::key_any_ctrl)));
          }
        }
        if (m_key_states[static_cast<uint32_t>(key_code::key_any_shift)] !=
            evt.fl.shift) {
          m_key_states[static_cast<uint32_t>(key_code::key_any_shift)] =
              evt.fl.shift;
          if (evt.fl.shift) {
            it->second->dispatch_input(
                input_event::key_event(event_type::key_down,
                    static_cast<key_code>(key_code::key_any_shift)));
          } else {
            it->second->dispatch_input(
                input_event::key_event(event_type::key_up,
                    static_cast<key_code>(key_code::key_any_shift)));
          }
        }
      default:
        break;
    }
  }
  if (!m_exit) {
    m_job_pool->add_job_on_main(JOB_NAME,
        functional::function<void()>(m_allocator, [this]() { run_loop(); }),
        nullptr, multi_tasking::job_priority::standard, true);
  } else {
    macos_internal_close_window(m_native_window_handle);
    m_destroy_signal.increment_by(1);
    _windows.erase(m_window_number);
  }
}

}  // namespace window
}  // namespace runtime
}  // namespace wn
