// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNContainers/inc/WNDynamicArray.h"
#include "WNGraphics/inc/Internal/Vulkan/WNPhysicalDevice.h"
#include "WNGraphics/inc/Internal/Vulkan/WNDevice.h"
#include "WNLogging/inc/WNLog.h"

namespace wn {
namespace graphics {
namespace internal {
namespace vulkan {

device_ptr physical_device::make_device(
    memory::allocator* _allocator, WNLogging::WNLog* _log) const {
  float queue_priority = 1.0f;
  VkDeviceQueueCreateInfo queue_create_info = {
      VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,  // sType;
      nullptr,                                     // pNext;
      0,                                           // flags;
      m_compute_and_graphics_queue,                // queueFamilyIndex;
      1,                                           // queueCount;
      &queue_priority,                             // pQueuePriorities;
  };

  VkPhysicalDeviceFeatures physical_features = {
      0,
  };

  VkDeviceCreateInfo create_info = {
      VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,  // sType;
      nullptr,                              // pNext;
      0,                                    // flags;
      1,                                    // queueCreateInfoCount;
      &queue_create_info,                   // pQueueCreateInfos;
      0,                                    // enabledLayerCount;
      nullptr,                              // ppEnabledLayerNames;
      0,                                    // enabledExtensionCount;
      nullptr,                              // ppEnabledExtensionNames;
      &physical_features                    // pEnabledFeatures;
  };

  VkDevice vk_device;
  // TODO(awoloszyn): Add alloactors.
  if (VK_SUCCESS !=
      m_context->vkCreateDevice(
          m_physical_device, &create_info, nullptr, &vk_device)) {
    _log->Log(WNLogging::eError, 0, "Could not create device");
    return nullptr;
  }
  auto device_ptr = memory::make_unique<device>(_allocator, _allocator, _log,
      vk_device, m_context->vkDestroyDevice, &m_memory_properties);
  if (!device_ptr->initialize(m_context.get(), m_compute_and_graphics_queue)) {
    return nullptr;
  }

  return core::move(device_ptr);
}


void physical_device::initialize_device() {
  m_context->vkGetPhysicalDeviceMemoryProperties(
      m_physical_device, &m_memory_properties);
}

}  // namespace vulkan
}  // namesapce internal
}  // namespace graphics
}  // namespace wn