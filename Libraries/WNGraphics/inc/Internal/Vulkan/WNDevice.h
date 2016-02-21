// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_VULKAN_DEVICE_H__
#define __WN_GRAPHICS_INTERNAL_VULKAN_DEVICE_H__

#include "WNGraphics/inc/Internal/WNDevice.h"
#include "WNGraphics/inc/Internal/Vulkan/WNVulkanContext.h"

namespace wn {
namespace graphics {
namespace internal {
namespace vulkan {

class device : public internal::device {
public:
  device(memory::allocator* _allocator, WNLogging::WNLog* _log,
      VkDevice _device);
  bool initialize(vulkan_context* _context, uint32_t graphics_and_device_queue);

private:

  PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;
  PFN_vkGetDeviceQueue vkGetDeviceQueue;

  VkDevice m_device;
  VkQueue m_queue;
};

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_VULKAN_DEVICE_H__