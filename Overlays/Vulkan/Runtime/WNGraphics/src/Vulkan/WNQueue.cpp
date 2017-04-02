// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/Internal/Vulkan/WNQueue.h"
#include "WNGraphics/inc/Internal/Vulkan/WNBufferData.h"
#include "WNGraphics/inc/Internal/Vulkan/WNCommandList.h"
#include "WNGraphics/inc/Internal/Vulkan/WNDataTypes.h"
#include "WNGraphics/inc/Internal/Vulkan/WNDevice.h"
#include "WNGraphics/inc/WNCommandList.h"
#include "WNGraphics/inc/WNFence.h"

namespace wn {
namespace graphics {
namespace internal {
namespace vulkan {

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
vulkan_queue::~vulkan_queue() {
  if (is_valid()) {
    m_device->destroy_queue(this);
  }
}
#endif

#define get_data(f)                                                            \
  (f)->data_as<                                                                \
      typename data_type<core::remove_pointer<decltype(f)>::type>::value>()

void vulkan_queue::enqueue_fence(fence& _fence) {
  ::VkFence& fence = get_data(&_fence);
  m_queue_context->vkQueueSubmit(m_queue, 0, 0, fence);
}

void vulkan_queue::enqueue_command_list(command_list* _command) {
  vulkan_command_list* command_list =
      reinterpret_cast<vulkan_command_list*>(_command);
  VkCommandBuffer buffer = command_list->get_command_buffer();
  const VkSubmitInfo info{
      VK_STRUCTURE_TYPE_SUBMIT_INFO,  // sType
      nullptr,                        // pNext
      0,                              // waitSemaphoreCount
      nullptr,                        // waitSemaphores
      0,                              // waitDstStageMask
      1,                              // commandBufferCount
      &buffer,                        // pCommandBuffers
      0,                              // signalSemaphoreCount
      nullptr                         // pSignalSemaphores
  };

  m_queue_context->vkQueueSubmit(m_queue, 1, &info, VK_NULL_HANDLE);
}

#undef get_data

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace wn
