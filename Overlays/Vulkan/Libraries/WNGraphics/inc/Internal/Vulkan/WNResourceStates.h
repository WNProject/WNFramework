// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_D3D12_RESOURCE_STATES_H__
#define __WN_GRAPHICS_INTERNAL_D3D12_RESOURCE_STATES_H__

#include "WNGraphics/inc/WNResourceStates.h"

namespace wn {
namespace graphics {
namespace internal {
namespace vulkan {

static WN_FORCE_INLINE VkFlags
resource_state_to_vulkan_access_flags(resource_state _state) {
  static const VkFlags states[] = {
    0,
    VK_ACCESS_TRANSFER_READ_BIT,
    VK_ACCESS_TRANSFER_WRITE_BIT,
    VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
  };

  static_assert(sizeof(states) / sizeof(states[0]) ==
                    static_cast<uint32_t>(resource_state::max),
      "Expected the number of VKImageUsage and resources_states to match");
  WN_DEBUG_ASSERT_DESC(_state < resource_state::max,
      "Resource state out of bounds");

  return states[static_cast<uint32_t>(_state)];
}

static WN_FORCE_INLINE VkImageLayout
resource_state_to_vulkan_layout(resource_state _state) {
  static const VkImageLayout layouts[] = {
    VK_IMAGE_LAYOUT_UNDEFINED,
    VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
  };

  static_assert(sizeof(layouts) / sizeof(layouts[0]) ==
                    static_cast<uint32_t>(resource_state::max),
    "Expected the number of VkImageLayouts and resources_states to match");
  WN_DEBUG_ASSERT_DESC(_state < resource_state::max,
      "Resource state out of bounds");

  return layouts[static_cast<uint32_t>(_state)];
}

static WN_FORCE_INLINE VkFlags
resource_state_to_vulkan_pipeline_stage(resource_state _state) {
  static const VkFlags states[] = {
    VK_PIPELINE_STAGE_HOST_BIT,
    VK_PIPELINE_STAGE_TRANSFER_BIT,
    VK_PIPELINE_STAGE_TRANSFER_BIT,
    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
  };

  static_assert(sizeof(states) / sizeof(states[0]) ==
                    static_cast<uint32_t>(resource_state::max),
    "Expected the number of VkFlags and resources_states to match");
  WN_DEBUG_ASSERT_DESC(_state < resource_state::max,
      "Resource state out of bounds");

  return states[static_cast<uint32_t>(_state)];
}

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_D3D12_RESOURCE_STATES_H__