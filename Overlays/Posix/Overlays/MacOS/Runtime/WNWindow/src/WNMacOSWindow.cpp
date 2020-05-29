// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNWindow/inc/WNMacOSWindow.h"

#include "WNApplicationData/inc/WNApplicationData.h"
#include "WNMultiTasking/inc/job_pool.h"
#include "WNMultiTasking/inc/job_signal.h"
#include "executable_data/inc/executable_data.h"

namespace wn {
namespace runtime {
namespace window {

window_error macos_window::initialize() {
  return window_error::ok;
}

}  // namespace window
}  // namespace runtime
}  // namespace wn
