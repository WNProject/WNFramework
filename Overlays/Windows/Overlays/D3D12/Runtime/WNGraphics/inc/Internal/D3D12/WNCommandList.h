// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_D3D12_COMMAND_LIST_H__
#define __WN_GRAPHICS_INTERNAL_D3D12_COMMAND_LIST_H__

#include "WNGraphics/inc/Internal/D3D12/WND3D12ResourceCache.h"
#include "WNGraphics/inc/Internal/D3D12/WNDataTypes.h"
#include "WNGraphics/inc/Internal/D3D12/WNLockedHeap.h"
#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNGraphicsEnums.h"
#include "WNGraphics/inc/WNGraphicsTypes.h"

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/WNCommandList.h"
#else
#include "core/inc/utilities.h"
#endif

#include <d3d12.h>
#include <wrl.h>

WN_GRAPHICS_FORWARD(image)

namespace wn {
namespace runtime {
namespace graphics {

class pipeline_layout;
class descriptor_set;
class render_pass;
class framebuffer;
class graphics_pipeline;
union clear_value;
struct render_area;
struct render_pass_data;
struct framebuffer_data;

namespace internal {
namespace d3d12 {

class d3d12_device;
class d3d12_queue;

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
using d3d12_command_list_base = command_list;
#else
using d3d12_command_list_base = core::non_copyable;
#endif

class d3d12_command_list WN_GRAPHICS_FINAL : public d3d12_command_list_base {
public:
  ~d3d12_command_list() WN_GRAPHICS_OVERRIDE_FINAL = default;

  inline void finalize() WN_GRAPHICS_OVERRIDE_FINAL {
    auto retval = m_command_list->Close();
    WN_RELEASE_ASSERT(S_OK == retval, "Unknown error closing command list.");
  };

  void transition_resource(const image& _image,
      const image_components& _components, uint32_t _base_mip,
      uint32_t _mip_count, resource_state _from,
      resource_state _to) WN_GRAPHICS_OVERRIDE_FINAL;
  void transition_resource(const buffer& _buffer, resource_state _from,
      resource_state _to) WN_GRAPHICS_OVERRIDE_FINAL;

  void copy_buffer(const buffer& _src_buffer, size_t _src_offset,
      const buffer& _dst_buffer, size_t _dst_offset,
      size_t _size) WN_GRAPHICS_OVERRIDE_FINAL;

  void copy_buffer_to_image(const buffer& _src_buffer,
      size_t _src_offset_in_bytes, const image& _dst_image,
      uint32_t _mip_level) WN_GRAPHICS_OVERRIDE_FINAL;
  void copy_image_to_buffer(const image& _image, uint32_t _mip_level,
      const buffer& _dst_buffer,
      size_t _buffer_offset_in_bytes) WN_GRAPHICS_OVERRIDE_FINAL;
  void copy_image(const image& _src, uint32_t _src_mip_level, const image& _dst,
      uint32_t _dst_mip_level) WN_GRAPHICS_OVERRIDE_FINAL;

  void blit_image(const image& _src, uint32_t _src_mip_level, const image& _dst,
      uint32_t _dst_mip_level) WN_GRAPHICS_OVERRIDE_FINAL;

  void draw(uint32_t _vertex_count, uint32_t _instance_count,
      uint32_t _vertex_offset,
      uint32_t _instance_offset) WN_GRAPHICS_OVERRIDE_FINAL;

  void draw_indexed(uint32_t _index_count, uint32_t _instance_count,
      uint32_t _first_index, uint32_t _vertex_offset,
      uint32_t _instance_offset) WN_GRAPHICS_OVERRIDE_FINAL;

  void set_scissor(const scissor& _scissor) WN_GRAPHICS_OVERRIDE_FINAL;
  void set_viewport(const viewport& _viewport) WN_GRAPHICS_OVERRIDE_FINAL;

  void begin_render_pass(render_pass* _pass, framebuffer* _framebuffer,
      const render_area& _area,
      const containers::contiguous_range<clear_value>& _clears)
      WN_GRAPHICS_OVERRIDE_FINAL;

  void end_render_pass() WN_GRAPHICS_OVERRIDE_FINAL;

  void bind_graphics_descriptor_sets(
      const containers::contiguous_range<const descriptor_set*> _sets,
      uint32_t base_index) WN_GRAPHICS_OVERRIDE_FINAL;

  void bind_graphics_pipeline_layout(
      pipeline_layout* _layout) WN_GRAPHICS_OVERRIDE_FINAL;

  void bind_graphics_pipeline(
      graphics_pipeline* _pipeline) WN_GRAPHICS_OVERRIDE_FINAL;

  void bind_vertex_buffer(
      uint32_t stream, const buffer* _buffer) WN_GRAPHICS_OVERRIDE_FINAL;

  void bind_index_buffer(
      index_type type, const buffer* _buffer) WN_GRAPHICS_OVERRIDE_FINAL;

  void push_graphics_contants(uint32_t index, uint32_t offset_in_uint32s,
      const uint32_t* data, uint32_t num_values) WN_GRAPHICS_OVERRIDE_FINAL;

protected:
  friend class d3d12_device;
  friend class d3d12_queue;

  d3d12_command_list() = default;

  inline ID3D12CommandList* command_list() const {
    return m_command_list.Get();
  }

  inline void initialize(memory::allocator* _allocator, ID3D12Device* _device,
      Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>&& _command_list,
      d3d12_resource_cache* _cache) {
    m_allocator = _allocator;
    m_device = _device;
    m_command_list = core::move(_command_list);
    m_current_render_pass = nullptr;
    m_current_framebuffer = nullptr;
    m_current_subpass = 0;
    m_current_graphics_pipeline_layout = nullptr;
    m_clear_values = containers::dynamic_array<clear_value>(m_allocator);
    m_active_framebuffer_resource_states =
        containers::dynamic_array<resource_state>(m_allocator);
    m_resource_cache = _cache;
    m_temporary_descriptors.set_allocator(_allocator);
  }

private:
  // Inserts all of the commands needed to set up the current subpass.
  // It also discards previous subpass elements if possible.
  // If this is AFTER the last subpass, it discards previous
  // subpass data if possible.
  void set_up_subpass();

  template <typename T>
  typename data_type<T>::value& get_data(T* t);

  template <typename T>
  typename data_type<const T>::value& get_data(const T* const t);

  ID3D12Device* m_device;
  Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_command_list;
  const render_pass_data* m_current_render_pass;
  const framebuffer_data* m_current_framebuffer;
  containers::dynamic_array<resource_state>
      m_active_framebuffer_resource_states;
  uint32_t m_current_subpass;
  D3D12_RECT m_render_area;
  containers::dynamic_array<clear_value> m_clear_values;
  memory::allocator* m_allocator;
  pipeline_layout_object* m_current_graphics_pipeline_layout;
  graphics_pipeline_data* m_current_graphics_pipeline;
  d3d12_resource_cache* m_resource_cache;
  containers::dynamic_array<locked_heap::token> m_temporary_descriptors;
};

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_D3D12_DEVICE_H__
