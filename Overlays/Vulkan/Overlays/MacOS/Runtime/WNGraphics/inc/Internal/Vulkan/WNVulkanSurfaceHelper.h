// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _WNGRAPHICS_INTERNAL_VULKAN_SURFACE_HELPER_H__
#define _WNGRAPHICS_INTERNAL_VULKAN_SURFACE_HELPER_H__

#include "WNGraphics/inc/Internal/Vulkan/WNVulkanInclude.h"
#include "WNWindow/inc/WNMacOSWindow.h"

namespace wn {
namespace runtime {
namespace graphics {
namespace internal {
namespace vulkan {

struct surface_helper {
  void initialize(
      VkInstance instance, PFN_vkGetInstanceProcAddr get_proc_addr) {
    m_instance = instance;
    vkCreateMacOSSurfaceMVK = reinterpret_cast<PFN_vkCreateMacOSSurfaceMVK>(
        get_proc_addr(instance, "vkCreateMacOSSurfaceMVK"));
    vkDestroySurfaceKHR = reinterpret_cast<PFN_vkDestroySurfaceKHR>(
        get_proc_addr(instance, "vkDestroySurfaceKHR"));
  }

  VkResult create_surface(
      const runtime::window::window* window, VkSurfaceKHR* surface) const {
    const void* data = window->get_native_handle();
    VkMacOSSurfaceCreateInfoMVK create_info = {
        VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK,  // sType
        nullptr,                                          // pNext
        0,                                                // flags
        data,                                             // view
    };
    return vkCreateMacOSSurfaceMVK(m_instance, &create_info, nullptr, surface);
  }

  void destroy_surface(VkSurfaceKHR surface) {
    vkDestroySurfaceKHR(m_instance, surface, nullptr);
  }

  VkInstance m_instance;
  PFN_vkCreateMacOSSurfaceMVK vkCreateMacOSSurfaceMVK;
  PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR;
};

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // _WNGRAPHICS_INTERNAL_VULKAN_SURFACE_HELPER_H__
