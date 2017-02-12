// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_RUNTIME_WINDOW_NUL_WINDOW_H__
#define __WN_RUNTIME_WINDOW_NUL_WINDOW_H__

#include "WNLogging/inc/WNLog.h"
#include "WNMultiTasking/inc/WNJobSignal.h"
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

  virtual window_type type() const {
    return window_type::null;
  }

  const void* get_native_handle() const override {
    return nullptr;
  }
};

}  // namespace window
}  // namespace runtime
}  // namespace wn

#endif  // __WN_RUNTIME_WINDOW_NUL_WINDOW_H__
