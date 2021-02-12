// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNWindow/inc/WNMacOSWindow.h"

#include "WNApplicationData/inc/WNApplicationData.h"
#include "WNContainers/inc/WNHashMap.h"
#include "WNMemory/inc/string_utility.h"
#include "WNMultiTasking/inc/job_pool.h"
#include "WNMultiTasking/inc/job_signal.h"
#include "WNMultiTasking/inc/work_queue.h"
#include "WNWindow/inc/helpers.h"
#include "executable_data/inc/executable_data.h"

extern "C" {
void* macos_internal_create_window(
    uint32_t width, uint32_t height, int* window_number);
bool macos_handle_event(macos_event* mcevent);
void macos_internal_close_window(void* _view);
}

namespace wn {
namespace runtime {
namespace window {

window_error macos_window::initialize() {
  m_job_pool->call_blocking_function([this]() {
    m_app_data->main_thread_work_queue->run_work_sync(
        functional::function<void()>(m_allocator, [this]() {
          void* v =
              macos_internal_create_window(m_width, m_height, &m_window_number);
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

// These run_lops are helpul, in that no matter how many windows
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
      case macos_event_key_up:
        it->second->dispatch_input(input_event::key_event(
            event_type::key_down, static_cast<key_code>(evt.kb.code)));
        break;
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
      default:
        break;
    }
  }
  if (!m_exit) {
    m_app_data->main_thread_work_queue->run_defer_async(
        functional::function<void()>(m_allocator, [this]() { run_loop(); }));
  } else {
    macos_internal_close_window(m_native_window_handle);
    m_destroy_signal.increment(1);
    _windows.erase(m_window_number);
  }
}

}  // namespace window
}  // namespace runtime
}  // namespace wn
