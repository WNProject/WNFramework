// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _WNGRAPHICS_INTERNAL_VULKAN_SURFACE_HELPER_H__
#define _WNGRAPHICS_INTERNAL_VULKAN_SURFACE_HELPER_H__

#include "WNGraphics/inc/Internal/Vulkan/WNVulkanInclude.h"
#include "WNWindow/inc/WNAndroidWindow.h"

namespace wn {
namespace runtime {
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
      const runtime::window::window* window, VkSurfaceKHR* surface) const {
    const runtime::window::android_native_data* data =
        reinterpret_cast<const runtime::window::android_native_data*>(
            window->get_native_handle());
    VkAndroidSurfaceCreateInfoKHR create_info = {
        VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,  // sType
        nullptr,                                            // pNext
        0,                                                  // flags
        data->window,                                       // window
    };
    return vkCreateAndroidSurfaceKHR(
        m_instance, &create_info, nullptr, surface);
  }

  void destroy_surface(VkSurfaceKHR surface) {
    vkDestroySurfaceKHR(m_instance, surface, nullptr);
  }

  VkInstance m_instance;
  PFN_vkCreateAndroidSurfaceKHR vkCreateAndroidSurfaceKHR;
  PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR;
};

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // _WNGRAPHICS_INTERNAL_VULKAN_SURFACE_HELPER_H__
