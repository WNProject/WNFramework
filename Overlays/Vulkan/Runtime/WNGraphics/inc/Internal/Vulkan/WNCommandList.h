// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_VULKAN_COMMAND_LIST_H__
#define __WN_GRAPHICS_INTERNAL_VULKAN_COMMAND_LIST_H__

#include "WNGraphics/inc/Internal/Vulkan/WNVulkanCommandListContext.h"
#include "WNGraphics/inc/Internal/Vulkan/WNVulkanInclude.h"
#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNGraphicsEnums.h"
#include "WNGraphics/inc/WNHeapTraits.h"

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/WNCommandList.h"
#else
#include "WNCore/inc/WNUtility.h"
#endif

WN_GRAPHICS_FORWARD(image)

namespace wn {
namespace graphics {
class render_pass;
class framebuffer;
class descriptor_set;
class pipeline_layout;
struct render_area;
union clear_value;
namespace internal {
namespace vulkan {
class vulkan_device;
class vulkan_queue;

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
using vulkan_command_list_base = command_list;
#else
using vulkan_command_list_base = core::non_copyable;
#endif

class vulkan_command_list WN_GRAPHICS_FINAL : public vulkan_command_list_base {
public:
  ~vulkan_command_list() WN_GRAPHICS_OVERRIDE_FINAL;

  void finalize() WN_GRAPHICS_OVERRIDE_FINAL;
  void enqueue_resource_transition(const image& _image, resource_state _from,
      resource_state _to) WN_GRAPHICS_OVERRIDE_FINAL;

  void draw(uint32_t _vertex_count, uint32_t _instance_count,
      uint32_t _vertex_offset,
      uint32_t _instance_offset) WN_GRAPHICS_OVERRIDE_FINAL;

  void begin_render_pass(render_pass* _pass, framebuffer* _framebuffer,
      const render_area& _area,
      const containers::contiguous_range<clear_value>& _clears)
      WN_GRAPHICS_OVERRIDE_FINAL;

  void end_render_pass() WN_GRAPHICS_OVERRIDE_FINAL;

  void bind_graphics_descriptor_sets(
      const containers::contiguous_range<const descriptor_set*> _sets,
      uint32_t base_index) WN_GRAPHICS_OVERRIDE_FINAL;

  void bind_graphics_pipeline_layout(
      pipeline_layout* _pipeline) WN_GRAPHICS_OVERRIDE_FINAL {
    m_current_graphics_pipeline_layout = _pipeline;
  }

protected:
  friend class vulkan_device;
  friend class vulkan_queue;

  WN_FORCE_INLINE vulkan_command_list()
    : vulkan_command_list_base(),
      m_command_buffer(VK_NULL_HANDLE),
      m_command_pool(VK_NULL_HANDLE),
      m_context(nullptr) {}

  WN_FORCE_INLINE void initialize(memory::allocator* _allocator,
      VkCommandBuffer _command_buffer, VkCommandPool _command_pool,
      command_list_context* _context) {
    m_command_buffer = _command_buffer;
    m_command_pool = _command_pool;
    m_context = _context;
    m_allocator = _allocator;
    m_current_graphics_pipeline_layout = nullptr;
  }

  VkCommandBuffer get_command_buffer() const {
    return m_command_buffer;
  }

  void enqueue_upload_barrier(const upload_heap& upload_heap,
      size_t _offset_in_bytes, size_t _size) WN_GRAPHICS_OVERRIDE_FINAL;
  void enqueue_download_barrier(const download_heap& download_heap,
      size_t _offset_in_bytes, size_t _size) WN_GRAPHICS_OVERRIDE_FINAL;
  void enqueue_buffer_copy(const upload_heap& upload_heap,
      size_t _upload_offset_in_bytes, const download_heap& download_heap,
      size_t _download_offset_in_bytes,
      size_t _upload_size) WN_GRAPHICS_OVERRIDE_FINAL;

  void enqueue_texture_upload(const upload_heap& upload_heap,
      size_t _upload_offset_in_bytes,
      const image& _image) WN_GRAPHICS_OVERRIDE_FINAL;
  void enqueue_texture_download(const image& _image,
      const download_heap& _download_heap,
      size_t _download_offset_in_bytes) WN_GRAPHICS_OVERRIDE_FINAL;

  VkCommandBuffer m_command_buffer;
  VkCommandPool m_command_pool;
  command_list_context* m_context;
  memory::allocator* m_allocator;
  pipeline_layout* m_current_graphics_pipeline_layout;
};

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_VULKAN_COMMAND_LIST_H__
