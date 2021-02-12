// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_RUNTIME_WINDOW_WINDOW_H__
#define __WN_RUNTIME_WINDOW_WINDOW_H__

#include "WNContainers/inc/WNHashMap.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/unique_ptr.h"
#include "WNMultiTasking/inc/spin_lock.h"
#include "WNWindow/inc/WNInputContext.h"
#include "WNWindow/inc/WNWindowEnums.h"
#include "WNWindow/inc/WNWindowErrors.h"

namespace wn {
namespace memory {
class allocator;
}  // namespace memory

namespace multi_tasking {
class job_pool;
}  // namespace multi_tasking

namespace runtime {
namespace window {
const uint32_t k_default_density = 96;
class window {
public:
  virtual window_error initialize() = 0;
  virtual ~window();
  virtual bool is_valid() const = 0;
  virtual window_type type() const = 0;
  virtual const void* get_native_handle() const = 0;

  virtual uint32_t get_width() const = 0;
  virtual uint32_t get_height() const = 0;

  virtual bool get_key_state(key_code _code) const = 0;
  virtual bool get_mouse_state(mouse_button _button) const = 0;

  virtual uint32_t get_cursor_x() const = 0;
  virtual uint32_t get_cursor_y() const = 0;

  input_context* get_input_context();
  void release_input_context(input_context* _context);
  void dispatch_input(const input_event& event);

  virtual void show_keyboard() {}
  virtual void hide_keyboard() {}
  // TODO: Fix this up later, stick with this for now
  virtual uint32_t get_dpi() const {
    return 96;
  }

protected:
  window(memory::allocator* _allocator)
    : m_allocator(_allocator), m_input_contexts(_allocator) {}

  // This is context_sensitive w.r.t. current key modifiers.
  // HOWEVER this is not the best way forward, just a way
  // to quickly unblock.
  uint64_t get_char_from_key(key_code _code) {
    bool is_shift =
        m_key_states[static_cast<uint64_t>(key_code::key_lshift)] ||
        m_key_states[static_cast<uint64_t>(key_code::key_rshift)] ||
        m_key_states[static_cast<uint64_t>(key_code::key_any_shift)];
    uint64_t code_as_num = static_cast<uint64_t>(_code);
    if (_code >= key_code::key_0 && _code <= key_code::key_9) {
      return '0' + (code_as_num - static_cast<uint64_t>(key_code::key_0));
    }
    if (_code >= key_code::key_a && _code <= key_code::key_z) {
      return (is_shift ? 'A' : 'a') +
             (code_as_num - static_cast<uint64_t>(key_code::key_a));
    }
    if (_code == key_code::key_space) {
      return (' ');
    }
    if (_code == key_code::key_tab) {
      return '\t';
    }
    if (_code == key_code::key_return) {
      return '\n';
    }
    return 0;
  }

  std::atomic_bool m_key_states[static_cast<uint32_t>(key_code::key_max) + 1] =
      {};
  std::atomic_bool
      m_mouse_states[static_cast<uint32_t>(mouse_button::mouse_max) + 1] = {};

  memory::allocator* m_allocator;

private:
  multi_tasking::spin_lock m_input_context_lock;
  containers::hash_map<input_context*, memory::unique_ptr<input_context>>
      m_input_contexts;
};

}  // namespace window
}  // namespace runtime
}  // namespace wn

#endif  // __WN_RUNTIME_WINDOW_WINDOW_H__
