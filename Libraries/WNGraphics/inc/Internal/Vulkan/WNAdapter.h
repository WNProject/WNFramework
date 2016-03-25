// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_VULKAN_ADAPTER_H__
#define __WN_GRAPHICS_INTERNAL_VULKAN_ADAPTER_H__

#include "WNContainers/inc/WNDynamicArray.h"
#include "WNContainers/inc/WNString.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNGraphics/inc/Internal/Vulkan/WNVulkanContext.h"
#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNMemory/inc/WNIntrusivePtr.h"
#include "WNMemory/inc/WNUniquePtr.h"

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/WNAdapter.h"
#else
#include "WNCore/inc/WNUtility.h"
#endif

#define VK_NO_PROTOTYPES

#include <vulkan.h>

namespace WNLogging {

class WNLog;

}  // namespace WNLogging

namespace wn {
namespace memory {

class allocator;

}  // namespace memory

namespace graphics {

class factory;
class adapter;
class device;

using adapter_ptr = memory::unique_ptr<adapter>;
using device_ptr = memory::unique_ptr<device>;

namespace internal {
namespace vulkan {

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
using vulkan_adapter_base = adapter;
#else
using vulkan_adapter_base = core::non_copyable;
#endif

class vulkan_adapter WN_GRAPHICS_FINAL : public vulkan_adapter_base {
public:
#ifdef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
  enum class api_type{invalid, vulkan, d3d12, max};
#endif

  ~vulkan_adapter() WN_GRAPHICS_OVERRIDE_FINAL = default;

  device_ptr make_device(memory::allocator* _allocator,
      WNLogging::WNLog* _log) const WN_GRAPHICS_OVERRIDE_FINAL;

  WN_FORCE_INLINE containers::string_view name() const
      WN_GRAPHICS_OVERRIDE_FINAL {
    return m_name;
  }

  WN_FORCE_INLINE uint32_t vendor_id() const WN_GRAPHICS_OVERRIDE_FINAL {
    return m_vendor_id;
  }

  WN_FORCE_INLINE uint32_t device_id() const WN_GRAPHICS_OVERRIDE_FINAL {
    return m_device_id;
  }

  WN_FORCE_INLINE api_type api() const WN_GRAPHICS_OVERRIDE_FINAL {
    return m_api;
  }

protected:
  friend void enumerate_adapters(memory::allocator*, WNLogging::WNLog*,
      containers::dynamic_array<adapter_ptr>&);

  WN_FORCE_INLINE vulkan_adapter()
    : vulkan_adapter_base(),
      m_physical_device(VK_NULL_HANDLE),
      m_api(api_type::invalid),
      m_vendor_id(uint32_t(-1)),
      m_device_id(uint32_t(-1)),
      m_compute_and_graphics_queue(0) {}

  WN_FORCE_INLINE void initialize(
      const memory::intrusive_ptr<vulkan_context>& _context,
      VkPhysicalDevice _device, containers::string&& _name,
      const uint32_t _vendor_id, const uint32_t _device_id,
      const uint32_t _compute_and_graphics_queue) {
    m_context = _context;
    m_physical_device = _device;
    m_name = core::move(_name);
    m_vendor_id = _vendor_id;
    m_device_id = _device_id;
    m_compute_and_graphics_queue = _compute_and_graphics_queue;
    m_api = api_type::vulkan;
  }

  void initialize_device();

  memory::intrusive_ptr<vulkan_context> m_context;
  VkPhysicalDeviceMemoryProperties m_memory_properties;
  VkPhysicalDevice m_physical_device;
  containers::string m_name;
  api_type m_api;
  uint32_t m_vendor_id;
  uint32_t m_device_id;
  uint32_t m_compute_and_graphics_queue;
};

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_VULKAN_ADAPTER_H__
