// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_RUNTIME_WINDOW_WINDOW_H__
#define __WN_RUNTIME_WINDOW_WINDOW_H__

#include "WNLogging/inc/WNLog.h"
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

class window {
public:
  virtual window_error initialize() = 0;
  virtual ~window() {}
  virtual bool is_valid() const = 0;
  virtual window_type type() const = 0;
  virtual const void* get_native_handle() const = 0;

  virtual uint32_t get_width() const = 0;
  virtual uint32_t get_height() const = 0;

  virtual bool get_key_state(key_code _code) const = 0;
  virtual bool get_mouse_state(mouse_button _button) const = 0;

  virtual uint32_t get_cursor_x() const = 0;
  virtual uint32_t get_cursor_y() const = 0;

protected:
  window() {}
};

}  // namespace window
}  // namespace runtime
}  // namespace wn

#endif  // __WN_RUNTIME_WINDOW_WINDOW_H__
