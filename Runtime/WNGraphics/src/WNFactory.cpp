// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNGraphics/inc/WNFactory.h"

#include "WNFunctional/inc/WNFunction.h"
#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/Internal/WNHelperIncludes.h"

namespace wn {
namespace runtime {
namespace graphics {

containers::contiguous_range<const adapter_ptr> factory::query_adapters()
    const {
  multi_tasking::call_once(
      m_query_adapter_once_flag, [this]() { query_devices(); });

  WN_RELEASE_ASSERT(m_adapters.size() > 0, "No graphics devices available");

  return containers::contiguous_range<const adapter_ptr>(
      m_adapters.data(), m_adapters.size());
}

#define WN_GRAPHICS_CLEAN(unused, engine) internal::engine::cleanup(m_log);

factory::~factory() {
  FOR_EACH(WN_GRAPHICS_CLEAN, unused, _WN_GRAPHICS_TYPE_LIST)
}

#define WN_GRAPHICS_ENUMERATE(unused, engine)                                  \
  internal::engine::enumerate_adapters(m_allocator, m_log, m_adapters);

void factory::query_devices() const {
  FOR_EACH(WN_GRAPHICS_ENUMERATE, unused, _WN_GRAPHICS_TYPE_LIST)
}

}  // namespace graphics
}  // namespace runtime
}  // namespace wn
