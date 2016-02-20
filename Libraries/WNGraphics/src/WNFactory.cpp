// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNContainers/inc/WNFunction.h"
#include "WNGraphics/inc/WNFactory.h"

namespace wn {
namespace graphics {

containers::contiguous_range<const physical_device_ptr>
factory::query_physical_devices() const {
  std::call_once(m_query_physical_device_once_flag,
      containers::function<void()>(std::bind(&factory::query_devices, this)));

  return containers::contiguous_range<const physical_device_ptr>(
      m_physical_devices.data(), m_physical_devices.size());
}

void factory::query_devices() const {}

}  // namespace graphics
}  // namespace wn