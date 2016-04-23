// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNContainers/inc/WNFunction.h"
#include "WNGraphics/inc/WNFactory.h"
#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/Internal/WNHelperIncludes.h"

namespace wn {
namespace graphics {

containers::contiguous_range<const adapter_ptr> factory::query_adapters()
    const {
  multi_tasking::call_once(m_query_adapter_once_flag,
      containers::function<void()>(std::bind(&factory::query_devices, this)));

  return containers::contiguous_range<const adapter_ptr>(
      m_adapters.data(), m_adapters.size());
}

#define WN_GRAPHICS_ENUMERATE(unused,engine) \
  internal::engine::enumerate_adapters(m_allocator, m_log, m_adapters);

void factory::query_devices() const {
  FOR_EACH(WN_GRAPHICS_ENUMERATE,unused,_WN_GRAPHICS_TYPE_LIST)
}

#undef WN_GRAPHICS_ENUMERATE
}  // namespace graphics
}  // namespace wn