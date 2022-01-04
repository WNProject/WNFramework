// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNGraphics/inc/WNQueueProfiler.h"

#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNDevice.h"

#ifdef _WN_GRAPHICS_SINGLE_DEVICE_TYPE

namespace wn {
namespace runtime {
namespace graphics {

queue_profiler::~queue_profiler() {
  if (is_valid()) {
    m_device->destroy_queue_profiler(this);
  }
}

}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif
