// Copyright (c) 2019, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_RUNTIME_WINDOW_INPUT_CONTEXT_H__
#define __WN_RUNTIME_WINDOW_INPUT_CONTEXT_H__

#include "WNContainers/inc/WNDeque.h"
#include "WNCore/inc/assert.h"
#include "WNCore/inc/types.h"
#include "WNMemory/inc/allocator.h"
#include "WNMemory/inc/intrusive_ptr.h"
#include "WNMultiTasking/inc/lock_guard.h"
#include "WNMultiTasking/inc/spin_lock.h"
#include "WNWindow/inc/WNWindowEnums.h"

namespace wn {
namespace runtime {
namespace window {

enum class event_type {
  key_down,
  key_up,
  mouse_down,
  mouse_up,
  mouse_move,
  text_input,
};

class input_event {
public:
  event_type type() {
    return m_type;
  }

  key_code get_keycode() {
    WN_DEBUG_ASSERT(
        m_type == event_type::key_down || m_type == event_type::key_up,
        "Cannot get keycode for this event type");
    return m_key;
  }

  mouse_button get_mouse_button() {
    WN_DEBUG_ASSERT(
        m_type == event_type::mouse_down || m_type == event_type::mouse_up,
        "Cannot get mouse button for this event type");
    return m_button;
  }

  uint64_t get_character() {
    WN_DEBUG_ASSERT(m_type == event_type::text_input,
        "Cannot get text input for this event type");
    return m_input_text;
  }

  uint32_t get_mouse_x() {
    WN_DEBUG_ASSERT(m_type == event_type::mouse_move,
        "Cannot get mouse move for this event type");
    return m_coordinates & 0xFFFF;
  }

  uint32_t get_mouse_y() {
    WN_DEBUG_ASSERT(m_type == event_type::mouse_move,
        "Cannot get mouse move for this event type");
    return (m_coordinates >> 16) & 0xFFFF;
  }

  static input_event key_event(event_type _type, key_code _code) {
    input_event evt;
    evt.m_type = _type;
    evt.m_key = _code;
    return evt;
  }

  static input_event mouse_down(mouse_button _button) {
    input_event evt;
    evt.m_type = event_type::mouse_down;
    evt.m_button = _button;
    return evt;
  }

  static input_event mouse_up(mouse_button _button) {
    input_event evt;
    evt.m_type = event_type::mouse_up;
    evt.m_button = _button;
    return evt;
  }

  static input_event mouse_move(uint16_t _x, uint16_t _y) {
    input_event evt;
    evt.m_type = event_type::mouse_move;
    evt.m_coordinates = uint32_t(_x) | (uint32_t(_y) << 16);
    return evt;
  }

  static input_event text_input(uint64_t _char) {
    input_event evt;
    evt.m_type = event_type::text_input;
    evt.m_input_text = _char;
    return evt;
  }

private:
  event_type m_type;
  union {
    key_code m_key;
    mouse_button m_button;
    uint32_t m_coordinates;
    uint64_t m_input_text;
  };
};

// TODO(awoloszyn): We should definitely start pooling these events.
//     We should be able to hit a steady-state, and not have to allocate/free
class input_context {
public:
  input_context(memory::allocator* _allocator)
    : m_events(_allocator), m_max_events(static_cast<uint32_t>(-1)) {}

  input_context(memory::allocator* _allocator, uint32_t _queue_depth)
    : m_events(_allocator), m_max_events(_queue_depth) {}

  // get_event tries to pop the next event from the queue.
  // It returns true, and fills _event with the event,
  // if there was an event in the queue.
  // It returns false, and leaves _event untouched,
  // if there was no event present.
  bool get_event(input_event* _event) {
    multi_tasking::spin_lock_guard guard(m_event_lock);
    if (m_events.empty()) {
      return false;
    }
    *_event = m_events.front();
    m_events.pop_front();
    return true;
  }

  // peek_event is the same as get_event, but does not
  // pop it from the queue
  bool peek_event(input_event* _event) {
    multi_tasking::spin_lock_guard guard(m_event_lock);
    if (m_events.empty()) {
      return false;
    }
    *_event = m_events.front();
    return true;
  }

  // push_event pushes an event into the queue.
  // If the queue is full, then the oldest event is
  // dropped.
  void push_event(const input_event& _event) {
    multi_tasking::spin_lock_guard guard(m_event_lock);
    while (m_events.size() >= m_max_events) {
      m_events.pop_front();
    }
    m_events.push_back(_event);
  }

private:
  containers::deque<input_event> m_events;
  const uint32_t m_max_events;
  multi_tasking::spin_lock m_event_lock;
};

}  // namespace window
}  // namespace runtime
}  // namespace wn

#endif  // __WN_RUNTIME_WINDOW_INPUT_CONTEXT_H__
