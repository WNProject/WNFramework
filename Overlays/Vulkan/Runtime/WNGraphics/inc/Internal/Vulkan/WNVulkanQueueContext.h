// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_VULKAN_VULKAN_QUEUE_CONTEXT_H__
#define __WN_GRAPHICS_INTERNAL_VULKAN_VULKAN_QUEUE_CONTEXT_H__

#include "WNGraphics/inc/Internal/Vulkan/WNVulkanInclude.h"
#include "WNMemory/inc/allocator.h"
#include "WNMemory/inc/intrusive_ptr.h"

namespace wn {
namespace runtime {
namespace graphics {
namespace internal {
namespace vulkan {

class vulkan_device;

struct queue_context final {
  queue_context() : m_device(nullptr) {}
  vulkan_device* m_device;
  PFN_vkQueueSubmit vkQueueSubmit;
  PFN_vkQueuePresentKHR vkQueuePresentKHR;
};

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_VULKAN_VULKAN_QUEUE_CONTEXT_H__
