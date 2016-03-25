// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNContainers/inc/WNFunction.h"
#include "WNGraphics/inc/WNFactory.h"

#ifdef _WN_GRAPHICS_D3D12_DEVICE_TYPE_AVAILABLE
#include "WNGraphics/src/D3D12/WNHelpers.h"
#endif

#ifdef _WN_GRAPHICS_VULKAN_DEVICE_TYPE_AVAILABLE
#include "WNGraphics/src/Vulkan/WNHelpers.h"
#endif

namespace wn {
namespace graphics {

containers::contiguous_range<const adapter_ptr> factory::query_adapters()
    const {
  multi_tasking::call_once(m_query_adapter_once_flag,
      containers::function<void()>(std::bind(&factory::query_devices, this)));

  return containers::contiguous_range<const adapter_ptr>(
      m_adapters.data(), m_adapters.size());
}

void factory::query_devices() const {
#ifdef _WN_GRAPHICS_D3D12_DEVICE_TYPE_AVAILABLE
  internal::d3d12::enumerate_adapters(m_allocator, m_log, m_adapters);
#endif

#ifdef _WN_GRAPHICS_VULKAN_DEVICE_TYPE_AVAILABLE
  internal::vulkan::enumerate_adapters(m_allocator, m_log, m_adapters);
#endif
}

}  // namespace graphics
}  // namespace wn