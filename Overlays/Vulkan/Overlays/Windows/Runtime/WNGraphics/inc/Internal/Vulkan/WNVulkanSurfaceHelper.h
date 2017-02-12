// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef _WNGRAPHICS_INTERNAL_VULKAN_SURFACE_HELPER_H__
#define _WNGRAPHICS_INTERNAL_VULKAN_SURFACE_HELPER_H__

#include "WNGraphics/inc/Internal/Vulkan/WNVulkanInclude.h"
#include "WNWindow/inc/WNWindowsWindow.h"

namespace wn {
namespace graphics {
namespace internal {
namespace vulkan {

struct surface_helper {
  void initialize(
      VkInstance instance, PFN_vkGetInstanceProcAddr get_proc_addr) {
    m_instance = instance;
    vkCreateWin32SurfaceKHR = reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(
        get_proc_addr(instance, "vkCreateWin32SurfaceKHR"));
    vkDestroySurfaceKHR = reinterpret_cast<PFN_vkDestroySurfaceKHR>(
        get_proc_addr(instance, "vkDestroySurfaceKHR"));
  }

  VkResult create_surface(
      runtime::window::window* window, VkSurfaceKHR* surface) {
    const runtime::window::native_handle* data =
        reinterpret_cast<const runtime::window::native_handle*>(
            window->get_native_handle());
    VkWin32SurfaceCreateInfoKHR create_info = {
        VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,  // sType
        nullptr,                                          // pNext
        0,                                                // flags
        data->instance,                                   // hinstance
        data->handle,                                     // hwnd
    };
    return vkCreateWin32SurfaceKHR(m_instance, &create_info, nullptr, surface);
  }

  void destroy_surface(VkSurfaceKHR surface) {
    vkDestroySurfaceKHR(m_instance, surface, nullptr);
  }

  VkInstance m_instance;
  PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR;
  PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR;
};

}  // namespace wn
}  // namespace graphics
}  // namespace internal
}  // namespace vulkan

#endif  // _WNGRAPHICS_INTERNAL_VULKAN_SURFACE_HELPER_H__