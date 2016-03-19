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

class queue : public internal::queue {
public:
  ~queue();
  void enqueue_signal(fence& _fence) final;
private:
  queue(
      vulkan::device* _device, queue_context* _context, VkQueue queue);

  friend class vulkan::device;

  VkQueue m_queue;
  queue_context* const m_queue_context;
};

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#if _WN_GRAPHICS_DEVICE_TYPES_AVAILABLE == 1
#if defined _WN_GRAPHICS_VULKAN_DEVICE_TYPE_AVAILABLE
#include "WNGraphics/inc/Internal/Vulkan/WNQueue.h"
#elif defined _WN_GRAPHICS_D3D12_DEVICE_TYPE_AVAILABLE
#include "WNGraphics/inc/Internal/D3D12/WNQueue.h"
#endif
#endif

#endif  // __WN_GRAPHICS_INC_INTERNAL_VULKAN_QUEUE_H__
