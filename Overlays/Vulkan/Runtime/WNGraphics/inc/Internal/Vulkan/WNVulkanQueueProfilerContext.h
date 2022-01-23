// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_VULKAN_VULKAN_QUEUE_PROFILER_CONTEXT_H__
#define __WN_GRAPHICS_INTERNAL_VULKAN_VULKAN_QUEUE_PROFILER_CONTEXT_H__

#include "WNGraphics/inc/Internal/Vulkan/WNVulkanInclude.h"
#include "WNMemory/inc/allocator.h"
#include "WNMemory/inc/intrusive_ptr.h"

namespace wn {
namespace runtime {
namespace graphics {
namespace internal {
namespace vulkan {

class vulkan_device;

struct vulkan_queue_profiler_context final {
  PFN_vkBeginCommandBuffer vkBeginCommandBuffer;
  PFN_vkResetCommandBuffer vkResetCommandBuffer;
  PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers;
  PFN_vkFreeCommandBuffers vkFreeCommandBuffers;
  PFN_vkCmdResetQueryPool vkCmdResetQueryPool;
  PFN_vkCmdWriteTimestamp vkCmdWriteTimestamp;
  PFN_vkCreateQueryPool vkCreateQueryPool;
  PFN_vkDestroyQueryPool vkDestroyQueryPool;
  PFN_vkEndCommandBuffer vkEndCommandBuffer;
  PFN_vkGetQueryPoolResults vkGetQueryPoolResults;
  PFN_vkQueueSubmit vkQueueSubmit;
  PFN_vkQueueWaitIdle vkQueueWaitIdle;

  PFN_vkCreateCommandPool vkCreateCommandPool;
  PFN_vkDestroyCommandPool vkDestroyCommandPool;

  PFN_vkCreateFence vkCreateFence;
  PFN_vkDestroyFence vkDestroyFence;
  PFN_vkGetFenceStatus vkGetFenceStatus;
  PFN_vkWaitForFences vkWaitForFences;
  PFN_vkResetFences vkResetFences;

  // Optional
  PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT
      vkGetPhysicalDeviceCalibrateableTimeDomainsEXT;
  PFN_vkGetCalibratedTimestampsEXT vkGetCalibratedTimestampsEXT;

  VkPhysicalDevice physical_device;
};

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_VULKAN_VULKAN_QUEUE_PROFILER_CONTEXT_H__
