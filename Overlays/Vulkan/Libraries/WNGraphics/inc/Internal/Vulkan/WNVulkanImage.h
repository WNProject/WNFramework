// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_VULKAN_IMAGE_H__
#define __WN_GRAPHICS_INTERNAL_VULKAN_IMAGE_H__

#include <vulkan.h>

namespace wn {
namespace graphics {
namespace internal {
namespace vulkan {

struct VulkanImage {
  VkImage image;
  VkDeviceMemory device_memory;
};

}  // namespace vulkan
}  // namespace graphics
}  // namespace internal
}  // namespace wn
#endif  // __WN_GRAPHICS_INTERNAL_VULKAN_IMAGE_H__