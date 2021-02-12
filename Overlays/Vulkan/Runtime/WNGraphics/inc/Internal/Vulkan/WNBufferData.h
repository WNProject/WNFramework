// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_VULKAN_BUFFER_DATA_H__
#define __WN_GRAPHICS_INTERNAL_VULKAN_BUFFER_DATA_H__

#include "WNGraphics/inc/Internal/Vulkan/WNVulkanInclude.h"

namespace wn {
namespace runtime {
namespace graphics {
namespace internal {
namespace vulkan {

struct buffer_data {
  VkDeviceMemory device_memory;
  VkBuffer buffer;
};

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_VULKAN_BUFFER_DATA_H__
