// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_VULKAN_COMMAND_LIST_H__
#define __WN_GRAPHICS_INTERNAL_VULKAN_COMMAND_LIST_H__

#include <vulkan.h>

#include "WNCore/inc/WNBase.h"
#include "WNGraphics/inc/Internal/Vulkan/WNVulkanCommandListContext.h"
#include "WNGraphics/inc/WNCommandList.h"

namespace wn {
namespace graphics {
namespace internal {
namespace vulkan {

class vulkan_command_list final : public internal_command_list {
public:
  WN_FORCE_INLINE vulkan_command_list(VkCommandBuffer _command_buffer,
      VkCommandPool _command_pool, command_list_context* _context)
    : m_command_buffer(_command_buffer),
      m_command_pool(_command_pool),
      m_context(_context) {}

  ~vulkan_command_list() final;
  VkCommandBuffer get_command_buffer() const {
    return m_command_buffer;
  }

  void finalize() final;
private:
  void enqueue_upload_barrier(const upload_heap& upload_heap,
      size_t _offset_in_bytes, size_t _size) final;
  void enqueue_download_barrier(const download_heap& download_heap,
      size_t _offset_in_bytes, size_t _size) final;
  void enqueue_copy(const upload_heap& upload_heap,
      size_t _upload_offset_in_bytes, const download_heap& download_heap,
      size_t _download_offset_in_bytes, size_t _upload_size) final;

  VkCommandBuffer m_command_buffer;
  VkCommandPool m_command_pool;
  command_list_context* m_context;
};

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_VULKAN_COMMAND_LIST_H__
