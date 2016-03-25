// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_VULKAN_VULKAN_CONTEXT_H__
#define __WN_GRAPHICS_INTERNAL_VULKAN_VULKAN_CONTEXT_H__

#include "WNMemory/inc/WNAllocator.h"
#include "WNMemory/inc/WNIntrusivePtr.h"

#define VK_NO_PROTOTYPES

#include <vulkan.h>

namespace wn {
namespace graphics {
namespace internal {
namespace vulkan {

#ifdef _WN_WINDOWS
#include <Windows.h>
using library_type = HMODULE;
#else
#include <dlfcn.h>
using library_type = void*;
#endif

struct vulkan_context : public memory::intrusive_ptr_base {
  vulkan_context(memory::allocator* _allocator)
    : memory::intrusive_ptr_base(_allocator),
      library(0),
      instance(VK_NULL_HANDLE) {}
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
  library_type library;
  // TODO(awoloszyn): Fill this out as we need more functions.
  // TODO(awoloszyn): Add allocators to vulkan.
};
using vulkan_context_ptr = memory::intrusive_ptr<vulkan_context>;
}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_VULKAN_VULKAN_CONTEXT_H__
