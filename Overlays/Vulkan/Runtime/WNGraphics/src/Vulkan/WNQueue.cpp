// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNGraphics/inc/Internal/Vulkan/WNQueue.h"

#include "WNGraphics/inc/Internal/Vulkan/WNBufferData.h"
#include "WNGraphics/inc/Internal/Vulkan/WNCommandList.h"
#include "WNGraphics/inc/Internal/Vulkan/WNDataTypes.h"
#include "WNGraphics/inc/Internal/Vulkan/WNDevice.h"
#include "WNGraphics/inc/Internal/Vulkan/WNResourceStates.h"
#include "WNGraphics/inc/WNCommandList.h"
#include "WNGraphics/inc/WNFence.h"
#include "WNGraphics/inc/WNSignal.h"

namespace wn {
namespace runtime {
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
  // This is here because there is a horrible bug in the
  // the Samsung driver. (This was found on the S6).
  // If we do not have a SubmitInfo, the fence is immediately
  // signaled, and waiting does not occur.
  const VkSubmitInfo submit_info = {
      VK_STRUCTURE_TYPE_SUBMIT_INFO,  // sType
      nullptr,                        // pNext
      0,                              // waitSemaphoreCount
      nullptr,                        // pWaitSemaphores
      nullptr,                        // pWaitDstTageMask
      0,                              // commandBufferCount
      nullptr,                        // pCoommandBuffers
      0,                              // signalSemaphoreCount
      nullptr,                        // pSignalSemaphores
  };
  m_queue_context->vkQueueSubmit(m_queue, 1, &submit_info, fence);
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

void vulkan_queue::enqueue_commands(
    containers::contiguous_range<command_list*> _command_lists,
    containers::contiguous_range<core::pair<pipeline_stages, signal*>>
        _wait_signals,
    fence* _signal_fence,
    containers::contiguous_range<signal*> _signal_signals) {
  enqueue_command_lists_internal(
      _command_lists, _wait_signals, _signal_fence, _signal_signals);
}

void vulkan_queue::enqueue_command_lists(
    std::initializer_list<command_list*> _command_lists,
    std::initializer_list<core::pair<pipeline_stages, signal*>> _wait_signals,
    fence* _signal_fence, std::initializer_list<signal*> _signal_signals) {
  enqueue_command_lists_internal(
      _command_lists, _wait_signals, _signal_fence, _signal_signals);
}

template <typename command_lists, typename wait_signals,
    typename signal_signals>
void vulkan_queue::enqueue_command_lists_internal(command_lists& _command_lists,
    wait_signals& _wait_signals, fence* _signal_fence,
    signal_signals& _signal_signals) {
  ::VkFence f = _signal_fence ? get_data(_signal_fence) : VK_NULL_HANDLE;
  wn::containers::dynamic_array<VkCommandBuffer> buffers(m_allocator);
  buffers.reserve(_command_lists.size());

  wn::containers::dynamic_array<VkSemaphore> wait_semaphores(m_allocator);
  wn::containers::dynamic_array<VkPipelineStageFlags> wait_flags(m_allocator);
  wait_semaphores.reserve(_wait_signals.size());
  wait_flags.reserve(_wait_signals.size());

  wn::containers::dynamic_array<VkSemaphore> signal_semaphores(m_allocator);
  signal_semaphores.reserve(_signal_signals.size());

  for (auto& cmd_list : _command_lists) {
    vulkan_command_list* command_list =
        reinterpret_cast<vulkan_command_list*>(cmd_list);
    VkCommandBuffer buffer = command_list->get_command_buffer();
    buffers.push_back(buffer);
  }

  for (auto& sig : _wait_signals) {
    signal* s = sig.second;
    ::VkSemaphore& semaphore = get_data(s);
    wait_semaphores.push_back(semaphore);
    wait_flags.push_back(flags_to_vulkan_stage(sig.first));
  }

  for (auto sig : _signal_signals) {
    signal* s = sig;
    ::VkSemaphore& semaphore = get_data(s);
    signal_semaphores.push_back(semaphore);
  }

  const VkSubmitInfo info{
      VK_STRUCTURE_TYPE_SUBMIT_INFO,                    // sType
      nullptr,                                          // pNext
      static_cast<uint32_t>(_wait_signals.size()),      // waitSemaphoreCount
      wait_semaphores.data(),                           // waitSemaphores
      wait_flags.data(),                                // waitDstStageMask
      static_cast<uint32_t>(buffers.size()),            // commandBufferCount
      buffers.data(),                                   // pCommandBuffers
      static_cast<uint32_t>(signal_semaphores.size()),  // signalSemaphoreCount
      signal_semaphores.data()                          // pSignalSemaphores
  };

  m_queue_context->vkQueueSubmit(m_queue, 1, &info, f);
}

#undef get_data

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn
