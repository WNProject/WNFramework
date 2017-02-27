// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNWindow/inc/WNAndroidWindow.h"
#include "WNApplicationData/inc/WNApplicationData.h"
#include "WNExecutable/inc/WNEntryData.h"
#include "WNMultiTasking/inc/WNJobPool.h"
#include "WNMultiTasking/inc/WNJobSignal.h"

namespace wn {
namespace runtime {
namespace window {

window_error android_window::initialize() {
  return window_error::ok;
}

}  // namespace window
}  // namespace runtime
}  // namespace wn
