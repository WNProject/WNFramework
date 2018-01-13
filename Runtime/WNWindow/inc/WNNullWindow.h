// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_RUNTIME_WINDOW_NUL_WINDOW_H__
#define __WN_RUNTIME_WINDOW_NUL_WINDOW_H__

#include "WNLogging/inc/WNLog.h"
#include "WNMultiTasking/inc/job_signal.h"
#include "WNWindow/inc/WNWindow.h"
#include "WNWindow/inc/WNWindowErrors.h"

namespace wn {
namespace runtime {
namespace window {

class null_window : public window {
public:
  null_window(logging::log*, multi_tasking::job_pool*,
      multi_tasking::job_signal* _creation_signal,
      const application::application_data*, uint32_t, uint32_t, uint32_t,
      uint32_t) {
    _creation_signal->increment(1);
  }
  window_error initialize() override {
    return window_error::ok;
  }
  bool is_valid() const override {
    return true;
  }

  window_type type() const override {
    return window_type::null;
  }

  uint32_t get_width() const override {
    return 0;
  }

  uint32_t get_height() const override {
    return 0;
  }

  const void* get_native_handle() const override {
    return nullptr;
  }
};

}  // namespace window
}  // namespace runtime
}  // namespace wn

#endif  // __WN_RUNTIME_WINDOW_NUL_WINDOW_H__
