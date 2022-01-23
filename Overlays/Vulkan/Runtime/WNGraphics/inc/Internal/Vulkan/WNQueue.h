// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_INC_INTERNAL_VULKAN_QUEUE_H__
#define __WN_GRAPHICS_INC_INTERNAL_VULKAN_QUEUE_H__

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNGraphics/inc/Internal/Vulkan/WNVulkanInclude.h"
#include "WNGraphics/inc/Internal/Vulkan/WNVulkanQueueContext.h"
#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNGraphicsEnums.h"
#include "core/inc/pair.h"

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/WNQueue.h"
#else
#include "core/inc/utilities.h"
#endif

#include <initializer_list>

namespace wn {
namespace runtime {
namespace graphics {

class fence;
class command_list;
class signal;

namespace internal {
namespace vulkan {

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
using vulkan_queue_base = queue;
#else
using vulkan_queue_base = core::non_copyable;
#endif

class vulkan_device;

class vulkan_queue WN_GRAPHICS_FINAL : public vulkan_queue_base {
public:
#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
  ~vulkan_queue();
#else
  ~vulkan_queue() WN_GRAPHICS_OVERRIDE_FINAL = default;
#endif

  void enqueue_fence(fence& _fence) WN_GRAPHICS_OVERRIDE_FINAL;

  void enqueue_command_list(
      command_list* _command_list) WN_GRAPHICS_OVERRIDE_FINAL;

  void enqueue_commands(
      containers::contiguous_range<command_list*> _command_lists,
      containers::contiguous_range<core::pair<pipeline_stages, signal*>>
          _wait_signals,
      fence* _signal_fence,
      containers::contiguous_range<signal*> _signal_signals)
      WN_GRAPHICS_OVERRIDE_FINAL;

  void enqueue_command_lists(
      std::initializer_list<command_list*> _command_lists,
      std::initializer_list<core::pair<pipeline_stages, signal*>> _wait_signals,
      fence* _signal_fence, std::initializer_list<signal*> _signal_signals)
      WN_GRAPHICS_OVERRIDE_FINAL;

  inline bool is_valid() const WN_GRAPHICS_OVERRIDE_FINAL {
    return (m_device != nullptr);
  }

protected:
  friend class vulkan_device;
  friend class vulkan_swapchain;
  friend class vulkan_queue_profiler;

  VkResult present(
      VkSwapchainKHR swapchain, uint32_t index, VkSemaphore _semaphore) {
    bool semaphore = _semaphore != VK_NULL_HANDLE;
    VkPresentInfoKHR info{
        VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,     // sType
        nullptr,                                // pNext
        semaphore ? uint32_t(1) : uint32_t(0),  // waitSempahoreCount
        semaphore ? &_semaphore : nullptr,      // pWaitSemaphores
        1,                                      // swapchainCount
        &swapchain,                             // pSwapchains
        &index,                                 // pImageIndices
        nullptr                                 // pResults
    };
    return m_queue_context->vkQueuePresentKHR(m_queue, &info);
  }

  inline vulkan_queue()
    : vulkan_queue_base(),
      m_device(nullptr),
      m_queue(VK_NULL_HANDLE),
      m_queue_context(nullptr),
      m_allocator(nullptr) {}

  inline void initialize(vulkan_device* _device, memory::allocator* _allocator,
      queue_context* _context, VkQueue queue, uint32_t queue_index) {
    m_device = _device;
    m_queue_context = _context;
    m_queue = queue;
    m_allocator = _allocator;
    m_queue_index = queue_index;
  }

  template <typename command_lists, typename wait_signals,
      typename signal_signals>
  void enqueue_command_lists_internal(command_lists& _command_lists,
      wait_signals& _wait_signals, fence* _signal_fence,
      signal_signals& _signal_signals);

  vulkan_device* m_device;
  VkQueue m_queue;
  queue_context* m_queue_context;
  memory::allocator* m_allocator;
  uint32_t m_queue_index;
};

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_INC_INTERNAL_VULKAN_QUEUE_H__
