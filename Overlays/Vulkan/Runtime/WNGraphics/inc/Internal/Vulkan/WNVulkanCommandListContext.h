// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_VULKAN_COMMAND_LIST_CONTEXT_H__
#define __WN_GRAPHICS_INTERNAL_VULKAN_COMMAND_LIST_CONTEXT_H__

#include "WNGraphics/inc/Internal/Vulkan/WNVulkanInclude.h"
#include "WNMemory/inc/allocator.h"
#include "WNMemory/inc/intrusive_ptr.h"

namespace wn {
namespace runtime {
namespace graphics {
namespace internal {
namespace vulkan {

class vulkan_device;

struct command_list_context final {
  command_list_context() : m_device(nullptr) {}
  vulkan_device* m_device;
  PFN_vkFreeCommandBuffers vkFreeCommandBuffers;
  PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier;
  PFN_vkCmdCopyBuffer vkCmdCopyBuffer;
  PFN_vkEndCommandBuffer vkEndCommandBuffer;

  PFN_vkCmdDraw vkCmdDraw;
  PFN_vkCmdDrawIndexed vkCmdDrawIndexed;

  PFN_vkCmdBeginRenderPass vkCmdBeginRenderPass;
  PFN_vkCmdEndRenderPass vkCmdEndRenderPass;

  PFN_vkCmdBindDescriptorSets vkCmdBindDescriptorSets;
  PFN_vkCmdPushConstants vkCmdPushConstants;

  PFN_vkCmdBindPipeline vkCmdBindPipeline;
  PFN_vkCmdBindVertexBuffers vkCmdBindVertexBuffers;
  PFN_vkCmdBindIndexBuffer vkCmdBindIndexBuffer;

  PFN_vkCmdCopyImageToBuffer vkCmdCopyImageToBuffer;
  PFN_vkCmdCopyBufferToImage vkCmdCopyBufferToImage;
  PFN_vkCmdCopyImage vkCmdCopyImage;
  PFN_vkCmdBlitImage vkCmdBlitImage;

  PFN_vkCmdSetViewport vkCmdSetViewport;
  PFN_vkCmdSetScissor vkCmdSetScissor;
};

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_VULKAN_COMMAND_LIST_CONTEXT_H__
