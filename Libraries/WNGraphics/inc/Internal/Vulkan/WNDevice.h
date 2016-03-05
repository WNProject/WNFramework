// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_VULKAN_DEVICE_H__
#define __WN_GRAPHICS_INTERNAL_VULKAN_DEVICE_H__

#include "WNGraphics/inc/Internal/Vulkan/WNVulkanContext.h"
#include "WNGraphics/inc/Internal/WNDevice.h"

namespace wn {
namespace graphics {
namespace internal {
namespace vulkan {

class device : public internal::device {
public:
  device(memory::allocator* _allocator, WNLogging::WNLog* _log,
      VkDevice _device, PFN_vkDestroyDevice _destroy_device,
      const VkPhysicalDeviceMemoryProperties* memory_properties);
  ~device();
  bool initialize(vulkan_context* _context, uint32_t graphics_and_device_queue);
  upload_heap_ptr create_upload_heap(size_t num_bytes) final;

private:
  void flush_mapped_range(
      upload_heap* _buffer, size_t offset, size_t num_bytes) final;
  void destroy_upload_heap(upload_heap*) final;

  PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;
  PFN_vkGetDeviceQueue vkGetDeviceQueue;
  PFN_vkDestroyDevice vkDestroyDevice;

  PFN_vkAllocateMemory vkAllocateMemory;
  PFN_vkFreeMemory vkFreeMemory;

  PFN_vkCreateBuffer vkCreateBuffer;
  PFN_vkDestroyBuffer vkDestroyBuffer;
  PFN_vkBindBufferMemory vkBindBufferMemory;
  PFN_vkMapMemory vkMapMemory;
  PFN_vkUnmapMemory vkUnmapMemory;

#ifdef _WN_DEBUG
  PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements;
#endif
  VkDevice m_device;
  VkQueue m_queue;
  uint32_t m_upload_memory_type_index;

  friend class wn::graphics::upload_heap;
};

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_VULKAN_DEVICE_H__