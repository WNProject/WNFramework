// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/Internal/Vulkan/WNAdapter.h"

#include "WNContainers/inc/WNDynamicArray.h"
#include "WNGraphics/inc/Internal/Vulkan/WNImageFormats.h"
#include "WNGraphics/inc/WNSurface.h"
#include "WNLogging/inc/WNLog.h"

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/Internal/Vulkan/WNDevice.h"
#else
#include "WNGraphics/inc/WNDevice.h"
#endif

namespace wn {
namespace runtime {
namespace graphics {
namespace internal {
namespace vulkan {
namespace {

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
using vulkan_device_constructable = vulkan_device;
#else
using vulkan_device_constructable = device;
#endif

}  // anonymous namespace

device_ptr vulkan_adapter::make_device(memory::allocator* _allocator,
    logging::log* _log, const adapter_features& enabled_features) const {
  const float queue_priority = 1.0f;
  VkDeviceQueueCreateInfo queue_create_info{
      VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,  // sType;
      nullptr,                                     // pNext;
      0,                                           // flags;
      m_compute_and_graphics_queue,                // queueFamilyIndex;
      1,                                           // queueCount;
      &queue_priority,                             // pQueuePriorities;
  };

  VkPhysicalDeviceFeatures physical_features{0};
  if (enabled_features.non_solid_fill) {
    physical_features.fillModeNonSolid = VK_TRUE;
  }
  if (enabled_features.tessellation) {
    physical_features.tessellationShader = VK_TRUE;
  }
  if (enabled_features.geometry) {
    physical_features.geometryShader = VK_TRUE;
  }
  if (enabled_features.depth_clamp) {
    physical_features.depthClamp = VK_TRUE;
  }
  if (enabled_features.depth_bias_clamp) {
    physical_features.depthBiasClamp = VK_TRUE;
  }
  if (enabled_features.dual_src_blending) {
    physical_features.dualSrcBlend = VK_TRUE;
  }
  if (enabled_features.sample_rate_shading) {
    physical_features.sampleRateShading = VK_TRUE;
  }
  if (enabled_features.etc2_textures) {
    physical_features.textureCompressionETC2 = VK_TRUE;
  }
  if (enabled_features.astc_ldr_textures) {
    physical_features.textureCompressionASTC_LDR = VK_TRUE;
  }
  if (enabled_features.bc_textures) {
    physical_features.textureCompressionBC = VK_TRUE;
  }
  if (enabled_features.clip_distance) {
    physical_features.shaderClipDistance = VK_TRUE;
  }
  if (enabled_features.cull_distance) {
    physical_features.shaderCullDistance = VK_TRUE;
  }

  const uint32_t num_device_extensions = 1;
  const char* device_extensions[num_device_extensions] = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  VkDeviceCreateInfo create_info{
      VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,  // sType;
      nullptr,                               // pNext;
      0,                                     // flags;
      1,                                     // queueCreateInfoCount;
      &queue_create_info,                    // pQueueCreateInfos;
      0,                                     // enabledLayerCount;
      nullptr,                               // ppEnabledLayerNames;
      num_device_extensions,                 // enabledExtensionCount;
      device_extensions,                     // ppEnabledExtensionNames;
      &physical_features                     // pEnabledFeatures;
  };

  VkDevice vk_device;

  // TODO(awoloszyn): Add allocators.
  if (m_context->vkCreateDevice(
          m_physical_device, &create_info, nullptr, &vk_device) != VK_SUCCESS) {
    _log->log_error("Could not create device");

    return nullptr;
  }

  memory::unique_ptr<vulkan_device_constructable> ptr(
      memory::make_unique_delegated<vulkan_device_constructable>(
          _allocator, [](void* _memory) {
            return new (_memory) vulkan_device_constructable();
          }));

  if (ptr) {
    if (!ptr->initialize(_allocator, _log, vk_device,
            m_context->vkDestroyDevice, &m_memory_properties,
            &m_adapter_formats, m_context.get(),
            m_compute_and_graphics_queue)) {
      return nullptr;
    }
  }

  return core::move(ptr);
}

void vulkan_adapter::initialize_device() {
  m_context->vkGetPhysicalDeviceMemoryProperties(
      m_physical_device, &m_memory_properties);
  VkPhysicalDeviceFeatures features;

  m_context->vkGetPhysicalDeviceFeatures(m_physical_device, &features);
  m_adapter_features.astc_ldr_textures =
      features.textureCompressionASTC_LDR == VK_TRUE;
  m_adapter_features.etc2_textures = features.textureCompressionETC2 == VK_TRUE;
  m_adapter_features.bc_textures = features.textureCompressionBC == VK_TRUE;
  m_adapter_features.geometry = features.geometryShader == VK_TRUE;
  m_adapter_features.tessellation = features.tessellationShader == VK_TRUE;
  m_adapter_features.input_attachments = true;

  struct {
    VkFormat format;
    bool* format_supported;
  } format_checks[] = {{VK_FORMAT_D16_UNORM, &m_adapter_formats.has_d16_unorm},
      {VK_FORMAT_X8_D24_UNORM_PACK32, &m_adapter_formats.has_d24_unorm},
      {VK_FORMAT_D32_SFLOAT, &m_adapter_formats.has_d32_float},
      {VK_FORMAT_S8_UINT, &m_adapter_formats.has_s8_unorm},
      {VK_FORMAT_D16_UNORM_S8_UINT, &m_adapter_formats.has_d16_unorm_s8_unorm},
      {VK_FORMAT_D24_UNORM_S8_UINT, &m_adapter_formats.has_d24_unorm_s8_unorm},
      {VK_FORMAT_D32_SFLOAT_S8_UINT,
          &m_adapter_formats.has_d32_float_s8_unorm}};

  VkFormatProperties properties;
  for (auto& format : format_checks) {
    m_context->vkGetPhysicalDeviceFormatProperties(
        m_physical_device, format.format, &properties);
    *format.format_supported =
        (properties.optimalTilingFeatures &
            (VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)) != 0;
  }
}

graphics_error vulkan_adapter::initialize_surface(
    surface* _surface, runtime::window::window* _window) {
  VkSurfaceKHR vksurface;

  if (VK_SUCCESS != m_surface_helper.create_surface(_window, &vksurface)) {
    m_log->log_error("Could not create surface on window");
    return graphics_error::error;
  }
  VkBool32 support = false;
  VkResult error_code = m_context->vkGetPhysicalDeviceSurfaceSupportKHR(
      m_physical_device, m_compute_and_graphics_queue, vksurface, &support);

  if (VK_SUCCESS == error_code && support) {
    uint32_t count = 0;
    error_code = m_context->vkGetPhysicalDeviceSurfaceFormatsKHR(
        m_physical_device, vksurface, &count, nullptr);
    if (count == 0 || VK_SUCCESS != error_code) {
      m_surface_helper.destroy_surface(vksurface);
      m_log->log_error("Could not get valid formats");
      return graphics_error::error;
    }
    containers::dynamic_array<VkSurfaceFormatKHR> formats(m_allocator, count);
    error_code = m_context->vkGetPhysicalDeviceSurfaceFormatsKHR(
        m_physical_device, vksurface, &count, formats.data());

    for (uint32_t i = 0; i < count; ++i) {
      data_format f = vulkan_format_to_image_format(formats[i].format);
      if (f == data_format::max) {
        continue;
      }
      _surface->m_valid_formats.push_back(f);
    }
    VkSurfaceCapabilitiesKHR caps;
    error_code = m_context->vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        m_physical_device, vksurface, &caps);
    if (VK_SUCCESS != error_code) {
      m_surface_helper.destroy_surface(vksurface);
      m_log->log_error("Could not get surface capabilities");
      return graphics_error::error;
    }
    _surface->m_width = caps.currentExtent.width;
    _surface->m_height = caps.currentExtent.height;
    _surface->data_as<VkSurfaceKHR>() = vksurface;
    return graphics_error::ok;
  }

  m_surface_helper.destroy_surface(vksurface);

  if (VK_SUCCESS != error_code) {
    m_log->log_error("Unexpected error calling Vulkan function");
    return graphics_error::error;
  }
  // Only log as info, this is not an exceptional case it is simply unsupported.
  m_log->log_info("Surface is not supported for vulkan device.");
  return graphics_error::eUnsupported;
}

void vulkan_adapter::destroy_surface(surface* _surface) {
  m_surface_helper.destroy_surface(_surface->data_as<VkSurfaceKHR>());
}

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn