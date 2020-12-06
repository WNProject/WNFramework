// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNGraphics/inc/WNQueue.h"

#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNDevice.h"

#ifdef _WN_GRAPHICS_SINGLE_DEVICE_TYPE

namespace wn {
namespace runtime {
namespace graphics {

queue::~queue() {
  if (is_valid()) {
    m_device->destroy_queue(this);
  }
}

}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif
