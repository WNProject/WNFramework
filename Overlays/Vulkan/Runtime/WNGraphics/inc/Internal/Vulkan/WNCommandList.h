// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_VULKAN_COMMAND_LIST_H__
#define __WN_GRAPHICS_INTERNAL_VULKAN_COMMAND_LIST_H__

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNGraphics/inc/Internal/Vulkan/WNVulkanCommandListContext.h"
#include "WNGraphics/inc/Internal/Vulkan/WNVulkanInclude.h"
#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNGraphicsEnums.h"
#include "WNGraphics/inc/WNGraphicsTypes.h"

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/WNCommandList.h"
#else
#include "WNCore/inc/WNUtility.h"
#endif

namespace wn {
namespace runtime {
namespace graphics {

class buffer;
class render_pass;
class framebuffer;
class graphics_pipeline;
class image;
class descriptor_set;
class pipeline_layout;
struct render_area;
union clear_value;

namespace internal {
namespace vulkan {

struct pipeline_layout_data;
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
  void transition_resource(const image& _image, resource_state _from,
      resource_state _to) WN_GRAPHICS_OVERRIDE_FINAL;
  void transition_resource(const buffer& _buffer, resource_state _from,
      resource_state _to) WN_GRAPHICS_OVERRIDE_FINAL;

  void copy_buffer(const buffer& _src_buffer, size_t _src_offset,
      const buffer& _dst_buffer, size_t _dst_offset,
      size_t _size) WN_GRAPHICS_OVERRIDE_FINAL;

  void copy_buffer_to_image(const buffer& _src_buffer,
      size_t _src_offset_in_bytes,
      const image& _dst_image) WN_GRAPHICS_OVERRIDE_FINAL;
  void copy_image_to_buffer(const image& _image, const buffer& _dst_buffer,
      size_t _buffer_offset_in_bytes) WN_GRAPHICS_OVERRIDE_FINAL;

  void draw(uint32_t _vertex_count, uint32_t _instance_count,
      uint32_t _vertex_offset,
      uint32_t _instance_offset) WN_GRAPHICS_OVERRIDE_FINAL;

  void draw_indexed(uint32_t _index_count, uint32_t _instance_count,
      uint32_t _first_index, uint32_t _vertex_offset,
      uint32_t _instance_offset) WN_GRAPHICS_OVERRIDE_FINAL;

  void set_scissor(const scissor& _scissor) WN_GRAPHICS_OVERRIDE_FINAL;

  void begin_render_pass(render_pass* _pass, framebuffer* _framebuffer,
      const render_area& _area,
      const containers::contiguous_range<clear_value>& _clears)
      WN_GRAPHICS_OVERRIDE_FINAL;

  void end_render_pass() WN_GRAPHICS_OVERRIDE_FINAL;

  void bind_graphics_descriptor_sets(
      const containers::contiguous_range<const descriptor_set*> _sets,
      uint32_t base_index) WN_GRAPHICS_OVERRIDE_FINAL;

  void bind_graphics_pipeline_layout(
      pipeline_layout* _pipeline) WN_GRAPHICS_OVERRIDE_FINAL;

  void bind_graphics_pipeline(
      graphics_pipeline* _pipeline) WN_GRAPHICS_OVERRIDE_FINAL;

  void bind_vertex_buffer(
      uint32_t stream, const buffer* _buffer) WN_GRAPHICS_OVERRIDE_FINAL;

  void bind_index_buffer(
      index_type type, const buffer* _buffer) WN_GRAPHICS_OVERRIDE_FINAL;

  void push_graphics_contants(uint32_t index, uint32_t offset_in_uint32s,
      const uint32_t* data, uint32_t num_values) WN_GRAPHICS_OVERRIDE_FINAL;

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

  VkCommandBuffer m_command_buffer;
  VkCommandPool m_command_pool;
  command_list_context* m_context;
  memory::allocator* m_allocator;
  pipeline_layout_data* m_current_graphics_pipeline_layout;
  index_type m_current_index_type;
};

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_VULKAN_COMMAND_LIST_H__
