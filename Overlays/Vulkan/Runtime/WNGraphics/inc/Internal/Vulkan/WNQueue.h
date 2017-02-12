// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_INC_INTERNAL_VULKAN_QUEUE_H__
#define __WN_GRAPHICS_INC_INTERNAL_VULKAN_QUEUE_H__

#include "WNGraphics/inc/Internal/Vulkan/WNVulkanInclude.h"
#include "WNGraphics/inc/Internal/Vulkan/WNVulkanQueueContext.h"
#include "WNGraphics/inc/Internal/WNConfig.h"

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/WNQueue.h"
#else
#include "WNCore/inc/WNUtility.h"
#endif

namespace wn {
namespace graphics {

class fence;
class command_list;

namespace internal {
namespace vulkan {

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
using vulkan_queue_base = queue;
#else
using vulkan_queue_base = core::non_copyable;
#endif

class vulkan_device;

class vulkan_queue WN_GRAPHICS_FINAL : public vulkan_queue_base {
public:
#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
  ~vulkan_queue();
#else
  ~vulkan_queue() WN_GRAPHICS_OVERRIDE_FINAL = default;
#endif

  void enqueue_fence(fence& _fence) WN_GRAPHICS_OVERRIDE_FINAL;

  void enqueue_command_list(
      command_list* _command_list) WN_GRAPHICS_OVERRIDE_FINAL;

  WN_FORCE_INLINE bool is_valid() const WN_GRAPHICS_OVERRIDE_FINAL {
    return (m_device != nullptr);
  }

protected:
  friend class vulkan_device;
  friend class vulkan_swapchain;

  VkResult present(VkSwapchainKHR swapchain, uint32_t index) {
    VkPresentInfoKHR info{
        VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,  // sType
        nullptr,                             // pNext
        0,                                   // waitSempahoreCount
        nullptr,                             // pWaitSemaphores
        1,                                   // swapchainCount
        &swapchain,                          // pSwapchains
        &index,                              // pImageIndices
        nullptr                              // pResults
    };
    return m_queue_context->vkQueuePresentKHR(m_queue, &info);
  }

  WN_FORCE_INLINE vulkan_queue()
    : vulkan_queue_base(),
      m_device(nullptr),
      m_queue(VK_NULL_HANDLE),
      m_queue_context(nullptr) {}

  WN_FORCE_INLINE void initialize(
      vulkan_device* _device, queue_context* _context, VkQueue queue) {
    m_device = _device;
    m_queue_context = _context;
    m_queue = queue;
  }

  VkQueue m_queue;
  queue_context* m_queue_context;
  vulkan_device* m_device;
};

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_INC_INTERNAL_VULKAN_QUEUE_H__
