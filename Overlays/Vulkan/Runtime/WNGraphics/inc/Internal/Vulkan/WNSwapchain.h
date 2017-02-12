// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_INC_INTERNAL_VULKAN_SWAPCHIAN_H__
#define __WN_GRAPHICS_INC_INTERNAL_VULKAN_SWAPCHIAN_H__

#include "WNGraphics/inc/Internal/Vulkan/WNVulkanInclude.h"
#include "WNGraphics/inc/Internal/WNConfig.h"

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/WNSwapchain.h"
#else
#include "WNCore/inc/WNUtility.h"
#endif

#include "WNContainers/inc/WNDynamicArray.h"
#include "WNGraphics/inc/WNImage.h"

namespace wn {
namespace graphics {
struct swapchain_create_info;
namespace internal {
namespace vulkan {

class vulkan_device;

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
using vulkan_swapchain_base = swapchain;
#else
using vulkan_swapchain_base = core::non_copyable;
#endif

class vulkan_swapchain WN_GRAPHICS_FINAL : public vulkan_swapchain_base {
public:
#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
  ~vulkan_swapchain();
#else
  ~vulkan_swapchain() WN_GRAPHICS_OVERRIDE_FINAL;
#endif

  image* get_image_for_index(uint32_t index) WN_GRAPHICS_OVERRIDE_FINAL;
  uint32_t get_backbuffer_index() const WN_GRAPHICS_OVERRIDE_FINAL;

protected:
  friend class vulkan_device;

  WN_FORCE_INLINE vulkan_swapchain()
    : vulkan_swapchain_base(), m_device(nullptr) {}

  void initialize(memory::allocator* _allocator, vulkan_device* _device,
      uint32_t _width, uint32_t _height,
      const swapchain_create_info& create_info, VkSwapchainKHR swapchain,
      VkSurfaceKHR surface);

  void present_internal(queue* q, const swapchain_create_info& info,
      uint32_t image_index) const WN_GRAPHICS_OVERRIDE_FINAL;

  VkSurfaceKHR m_surface;
  VkSwapchainKHR m_swapchain;
  vulkan_device* m_device;
  memory::allocator* m_allocator;
  containers::dynamic_array<image> m_images;
};

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_INC_INTERNAL_vulkan_QUEUE_H__
