// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_VULKAN_VULKAN_CONTEXT_H__
#define __WN_GRAPHICS_INTERNAL_VULKAN_VULKAN_CONTEXT_H__

#include "WNGraphics/inc/Internal/Vulkan/WNVulkanInclude.h"
#include "WNMemory/inc/allocator.h"
#include "WNMemory/inc/intrusive_ptr.h"

namespace wn {
namespace runtime {
namespace graphics {
namespace internal {
namespace vulkan {

#ifdef _WN_WINDOWS
using library_type = HMODULE;
#else
#include <dlfcn.h>
using library_type = void*;
#endif

struct vulkan_context : public memory::intrusive_ptr_base {
  vulkan_context(memory::allocator* _allocator)
    : memory::intrusive_ptr_base(_allocator),
      instance(VK_NULL_HANDLE),
      library(0) {}
  virtual ~vulkan_context() {
    if (vkDestroyInstance && instance != VK_NULL_HANDLE) {
      vkDestroyInstance(instance, nullptr);
    }
    if (library) {
#ifdef _WN_WINDOWS
      FreeLibrary(library);
#else
      dlclose(library);
#endif
    }
  }

  VkInstance instance;

  PFN_vkCreateInstance vkCreateInstance;
  PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
  PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
  PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties;
  PFN_vkGetPhysicalDeviceQueueFamilyProperties
      vkGetPhysicalDeviceQueueFamilyProperties;
  PFN_vkDestroyInstance vkDestroyInstance;

  PFN_vkCreateDevice vkCreateDevice;
  PFN_vkDestroyDevice vkDestroyDevice;

  PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties;
  PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR;
  PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR;
  PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR
      vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
  PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures;
  PFN_vkGetPhysicalDeviceFormatProperties vkGetPhysicalDeviceFormatProperties;
  PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties;

  library_type library;

  // TODO(awoloszyn): Fill this out as we need more functions.
  // TODO(awoloszyn): Add allocators to vulkan.
};

struct supported_depth_formats {
  bool allow_d16_u;
  bool allow_d24_u;
  bool allow_d32_f;
  bool allow_s8;
  bool allow_d16_us8;
  bool allow_d24_us8;
  bool allow_d32_fs8;
};

using vulkan_context_ptr = memory::intrusive_ptr<vulkan_context>;

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_VULKAN_VULKAN_CONTEXT_H__
