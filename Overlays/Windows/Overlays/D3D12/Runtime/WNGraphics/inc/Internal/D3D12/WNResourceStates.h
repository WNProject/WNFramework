// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_D3D12_RESOURCE_STATES_H__
#define __WN_GRAPHICS_INTERNAL_D3D12_RESOURCE_STATES_H__

#include "WNGraphics/inc/WNGraphicsEnums.h"

namespace wn {
namespace graphics {
namespace internal {
namespace d3d12 {

static WN_FORCE_INLINE D3D12_RESOURCE_STATES
resource_state_to_d3d12_resource_states(resource_state _state) {
  static const D3D12_RESOURCE_STATES states[] = {D3D12_RESOURCE_STATE_COMMON,
      D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COPY_DEST,
      D3D12_RESOURCE_STATE_RENDER_TARGET,
      D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE |
          D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
      D3D12_RESOURCE_STATE_PRESENT};

  static_assert((1 << (sizeof(states) / sizeof(states[0]) - 2)) + 1 ==
                    static_cast<uint32_t>(resource_state::max),
      "Expected the number of resource states and D3D12_RESOURCE_STATES to "
      "match");
  WN_DEBUG_ASSERT_DESC(
      _state < resource_state::max, "Resource state out of bounds");

  return states[static_cast<uint32_t>(_state)];
}

static WN_FORCE_INLINE D3D12_RESOURCE_FLAGS resources_states_to_resource_flags(
    resource_states _state) {
  D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;

  if (_state & static_cast<uint32_t>(resource_state::render_target)) {
    flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
  }
  return flags;
}

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_D3D12_RESOURCE_STATES_H__