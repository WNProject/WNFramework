// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_VULKAN_ADAPTER_H__
#define __WN_GRAPHICS_INTERNAL_VULKAN_ADAPTER_H__

#include "WNContainers/inc/WNDynamicArray.h"
#include "WNContainers/inc/WNString.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNGraphics/inc/Internal/Vulkan/WNVulkanContext.h"
#include "WNGraphics/inc/Internal/Vulkan/WNVulkanInclude.h"
#include "WNGraphics/inc/Internal/Vulkan/WNVulkanSurfaceHelper.h"
#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNAdapterFeatures.h"
#include "WNGraphics/inc/WNErrors.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/intrusive_ptr.h"
#include "WNMemory/inc/unique_ptr.h"
#include "WNWindow/inc/WNWindow.h"
#include "core/inc/pair.h"

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/WNAdapter.h"
#else
#include "core/inc/utilities.h"
#endif

namespace wn {
namespace memory {

class allocator;

}  // namespace memory

namespace runtime {
namespace graphics {

class factory;
class adapter;
class device;
class surface;
struct surface_capabilities;

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

  device_ptr make_device(memory::allocator* _allocator, logging::log* _log,
      const adapter_features& enabled_features) const
      WN_GRAPHICS_OVERRIDE_FINAL;

  graphics_error initialize_surface(surface* _surface,
      runtime::window::window* _window) WN_GRAPHICS_OVERRIDE_FINAL;

  void destroy_surface(surface* _surface) WN_GRAPHICS_OVERRIDE_FINAL;

  graphics_error get_surface_capabilities(surface* _surface,
      surface_capabilities* _capabilities) WN_GRAPHICS_OVERRIDE_FINAL;

  const adapter_features& get_features() const WN_GRAPHICS_OVERRIDE_FINAL {
    return m_adapter_features;
  }

  const adapter_formats& get_formats() const WN_GRAPHICS_OVERRIDE_FINAL {
    return m_adapter_formats;
  }

  inline containers::string_view name() const WN_GRAPHICS_OVERRIDE_FINAL {
    return m_name;
  }

  inline uint32_t vendor_id() const WN_GRAPHICS_OVERRIDE_FINAL {
    return m_vendor_id;
  }

  inline uint32_t device_id() const WN_GRAPHICS_OVERRIDE_FINAL {
    return m_device_id;
  }

  inline api_type api() const WN_GRAPHICS_OVERRIDE_FINAL {
    return m_api;
  }

protected:
  friend void enumerate_adapters(memory::allocator*, logging::log*,
      containers::dynamic_array<adapter_ptr>&);
  friend class vulkan_queue_profiler;

  inline vulkan_adapter()
    : vulkan_adapter_base(),
      m_physical_device(VK_NULL_HANDLE),
      m_api(api_type::invalid),
      m_vendor_id(uint32_t(-1)),
      m_device_id(uint32_t(-1)),
      m_compute_and_graphics_queue(0) {}

  inline void initialize(memory::allocator* _allocator,
      const memory::intrusive_ptr<vulkan_context>& _context,
      VkPhysicalDevice _device, containers::string&& _name, logging::log* _log,
      const uint32_t _vendor_id, const uint32_t _device_id,
      const uint32_t _compute_and_graphics_queue, const float _timestamp_period,
      VkPhysicalDeviceLimits* _physical_device) {
    m_allocator = _allocator;
    m_context = _context;
    m_physical_device = _device;
    m_physical_device_limits = *_physical_device;
    m_name = core::move(_name);
    m_log = _log;
    m_vendor_id = _vendor_id;
    m_device_id = _device_id;
    m_compute_and_graphics_queue = _compute_and_graphics_queue;
    m_api = api_type::vulkan;
    m_surface_helper.initialize(
        _context->instance, _context->vkGetInstanceProcAddr);
    m_timestamp_period = _timestamp_period;
  }

  void initialize_device();

  memory::allocator* m_allocator;
  memory::intrusive_ptr<vulkan_context> m_context;
  VkPhysicalDeviceMemoryProperties m_memory_properties;
  VkPhysicalDeviceLimits m_physical_device_limits;
  VkPhysicalDevice m_physical_device;

  adapter_features m_adapter_features;
  adapter_formats m_adapter_formats;
  containers::string m_name;
  logging::log* m_log;
  surface_helper m_surface_helper;
  api_type m_api;
  uint32_t m_vendor_id;
  uint32_t m_device_id;
  uint32_t m_compute_and_graphics_queue;
  float m_timestamp_period;
};

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_VULKAN_ADAPTER_H__
