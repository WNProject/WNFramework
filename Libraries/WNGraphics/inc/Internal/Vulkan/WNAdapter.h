// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_VULKAN_ADAPTER_H__
#define __WN_GRAPHICS_INTERNAL_VULKAN_ADAPTER_H__

#include "WNGraphics/inc/Internal/Vulkan/WNVulkanContext.h"
#include "WNGraphics/inc/Internal/WNAdapter.h"
#include "WNMemory/inc/WNIntrusivePtr.h"

#include <vulkan.h>

namespace wn {
namespace graphics {
namespace internal {
namespace vulkan {

class vulkan_adapter : public internal_adapter {
public:
  vulkan_adapter(const memory::intrusive_ptr<vulkan_context>& context,
      VkPhysicalDevice device, containers::string&& _name,
      const uint32_t _vendor_id, const uint32_t _device_id,
      const uint32_t _compute_and_graphics_queue)
    : internal_adapter(std::move(_name), _vendor_id, _device_id,
          internal_adapter::api_type::vulkan),
      m_context(context),
      m_physical_device(device),
      m_compute_and_graphics_queue(_compute_and_graphics_queue) {}

  void initialize_device();

  virtual device_ptr make_device(
      memory::allocator* _allocator, WNLogging::WNLog*) const;

  ~vulkan_adapter() {}

private:
  const memory::intrusive_ptr<vulkan_context> m_context;
  VkPhysicalDevice m_physical_device;
  VkPhysicalDeviceMemoryProperties m_memory_properties;
  uint32_t m_compute_and_graphics_queue;
};

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_VULKAN_ADAPTER_H__
