// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_INC_INTERNAL_VULKAN_SWAPCHIAN_H__
#define __WN_GRAPHICS_INC_INTERNAL_VULKAN_SWAPCHIAN_H__

#include "WNGraphics/inc/Internal/Vulkan/WNVulkanInclude.h"
#include "WNGraphics/inc/Internal/WNConfig.h"

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/WNSwapchain.h"
#else
#include "core/inc/utilities.h"
#endif

#include "WNContainers/inc/WNDynamicArray.h"
#include "WNGraphics/inc/WNImage.h"

namespace wn {
namespace runtime {
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
  uint32_t get_next_backbuffer_index(
      fence* fence, signal* _signal) const WN_GRAPHICS_OVERRIDE_FINAL;

protected:
  friend class vulkan_device;

  inline vulkan_swapchain() : vulkan_swapchain_base(), m_device(nullptr) {}

  void initialize(memory::allocator* _allocator, vulkan_device* _device,
      uint32_t _width, uint32_t _height,
      const swapchain_create_info& create_info, VkSwapchainKHR swapchain);

  void present_internal(queue* q, const swapchain_create_info& info,
      signal* _signal, uint32_t image_index) const WN_GRAPHICS_OVERRIDE_FINAL;

  VkSwapchainKHR m_swapchain;
  vulkan_device* m_device;
  memory::allocator* m_allocator;
  containers::dynamic_array<image> m_images;
};

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_INC_INTERNAL_vulkan_QUEUE_H__
