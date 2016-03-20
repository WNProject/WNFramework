// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNCore/inc/WNUtility.h"
#include "WNGraphics/inc/Internal/Vulkan/WNDevice.h"
#include "WNGraphics/inc/Internal/Vulkan/WNQueue.h"
#include "WNGraphics/inc/WNFence.h"

namespace wn {
namespace graphics {
namespace internal {
namespace vulkan {

vulkan_queue::vulkan_queue(
    vulkan_device* _device, queue_context* _context, VkQueue queue)
  : internal_queue(_device), m_queue_context(_context), m_queue(queue) {}

vulkan_queue::~vulkan_queue() {
  m_device->destroy_queue(this);
}

void vulkan_queue::enqueue_signal(fence& _fence) {
  m_queue_context->vkQueueSubmit(m_queue, 0, 0, _fence.data_as<VkFence>());
}

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace wn
