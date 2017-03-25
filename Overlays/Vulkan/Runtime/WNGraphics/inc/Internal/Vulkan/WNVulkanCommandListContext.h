// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_VULKAN_COMMAND_LIST_CONTEXT_H__
#define __WN_GRAPHICS_INTERNAL_VULKAN_COMMAND_LIST_CONTEXT_H__

#include "WNGraphics/inc/Internal/Vulkan/WNVulkanInclude.h"

#include "WNMemory/inc/WNAllocator.h"
#include "WNMemory/inc/WNIntrusivePtr.h"

namespace wn {
namespace graphics {
namespace internal {
namespace vulkan {

class vulkan_device;

struct command_list_context WN_FINAL {
  command_list_context() : m_device(nullptr) {}
  vulkan_device* m_device;
  PFN_vkFreeCommandBuffers vkFreeCommandBuffers;
  PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier;
  PFN_vkCmdCopyBuffer vkCmdCopyBuffer;
  PFN_vkEndCommandBuffer vkEndCommandBuffer;

  PFN_vkCmdDraw vkCmdDraw;

  PFN_vkCmdCopyImageToBuffer vkCmdCopyImageToBuffer;
  PFN_vkCmdCopyBufferToImage vkCmdCopyBufferToImage;
};

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_VULKAN_COMMAND_LIST_CONTEXT_H__
