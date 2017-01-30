// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_D3D12_RESOURCE_STATES_H__
#define __WN_GRAPHICS_INTERNAL_D3D12_RESOURCE_STATES_H__

#include "WNGraphics/inc/WNResourceStates.h"

namespace wn {
namespace graphics {
namespace internal {
namespace d3d12 {

static WN_FORCE_INLINE D3D12_RESOURCE_STATES
resource_state_to_d3d12_resource_states(resource_state _state) {
  static const D3D12_RESOURCE_STATES states[] = {
    D3D12_RESOURCE_STATE_COMMON,
    D3D12_RESOURCE_STATE_COPY_SOURCE,
    D3D12_RESOURCE_STATE_COPY_DEST,
    D3D12_RESOURCE_STATE_RENDER_TARGET
  };
  static_assert(sizeof(states) / sizeof(states[0]) ==
                    static_cast<uint32_t>(resource_state::max),
      "Expected the number of dxgi formats and image formats to match");
  WN_DEBUG_ASSERT_DESC(_state < resource_state::max,
      "Resource state out of bounds");

  return states[static_cast<uint32_t>(_state)];
}


}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_D3D12_RESOURCE_STATES_H__