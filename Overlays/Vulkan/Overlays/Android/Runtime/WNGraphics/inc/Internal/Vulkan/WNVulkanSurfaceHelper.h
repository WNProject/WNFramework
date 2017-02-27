// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef _WNGRAPHICS_INTERNAL_VULKAN_SURFACE_HELPER_H__
#define _WNGRAPHICS_INTERNAL_VULKAN_SURFACE_HELPER_H__

#include "WNGraphics/inc/Internal/Vulkan/WNVulkanInclude.h"
#include "WNWindow/inc/WNAndroidWindow.h"

namespace wn {
namespace graphics {
namespace internal {
namespace vulkan {

struct surface_helper {
  void initialize(
      VkInstance instance, PFN_vkGetInstanceProcAddr get_proc_addr) {
    m_instance = instance;
    vkCreateAndroidSurfaceKHR = reinterpret_cast<PFN_vkCreateAndroidSurfaceKHR>(
        get_proc_addr(instance, "vkCreateAndroidSurfaceKHR"));
    vkDestroySurfaceKHR = reinterpret_cast<PFN_vkDestroySurfaceKHR>(
        get_proc_addr(instance, "vkDestroySurfaceKHR"));
  }

  VkResult create_surface(
      runtime::window::window* window, VkSurfaceKHR* surface) {
    // TODO(awoloszyn): Implement this for Android.
    return VK_SUCCESS;
  }

  void destroy_surface(VkSurfaceKHR surface) {
    vkDestroySurfaceKHR(m_instance, surface, nullptr);
  }

  VkInstance m_instance;
  PFN_vkCreateAndroidSurfaceKHR vkCreateAndroidSurfaceKHR;
  PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR;
};

}  // namespace wn
}  // namespace graphics
}  // namespace internal
}  // namespace vulkan

#endif  // _WNGRAPHICS_INTERNAL_VULKAN_SURFACE_HELPER_H__