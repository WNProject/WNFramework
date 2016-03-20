// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNCore/inc/WNUtility.h"
#include "WNGraphics/inc/Internal/Vulkan/WNVulkanQueueContext.h"
#include "WNGraphics/inc/WNQueue.h"

#pragma once

#ifndef __WN_GRAPHICS_INC_INTERNAL_VULKAN_QUEUE_H__
#define __WN_GRAPHICS_INC_INTERNAL_VULKAN_QUEUE_H__

namespace wn {
namespace graphics {
namespace internal {
namespace vulkan {

class device;

class vulkan_queue : public internal::internal_queue {
public:
  ~vulkan_queue();
  void enqueue_signal(fence& _fence) final;
  void enqueue_command_list(command_list* _command) final;

private:
  vulkan_queue(vulkan_device* _device, queue_context* _context, VkQueue queue);

  friend class vulkan_device;

  VkQueue m_queue;
  queue_context* const m_queue_context;
};

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_INC_INTERNAL_VULKAN_QUEUE_H__
