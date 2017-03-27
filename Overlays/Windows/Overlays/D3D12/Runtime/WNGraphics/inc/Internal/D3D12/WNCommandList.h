// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_D3D12_COMMAND_LIST_H__
#define __WN_GRAPHICS_INTERNAL_D3D12_COMMAND_LIST_H__

#include "WNGraphics/inc/Internal/D3D12/WNDataTypes.h"
#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNGraphicsEnums.h"
#include "WNGraphics/inc/WNGraphicsTypes.h"
#include "WNGraphics/inc/WNHeapTraits.h"

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/WNCommandList.h"
#else
#include "WNCore/inc/WNUtility.h"
#endif

#include <d3d12.h>
#include <wrl.h>

WN_GRAPHICS_FORWARD(image)

namespace wn {
namespace graphics {
class pipeline_layout;
class descriptor_set;
class render_pass;
class framebuffer;
class graphics_pipeline;
union clear_value;
struct render_area;

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

  WN_FORCE_INLINE void finalize() WN_GRAPHICS_OVERRIDE_FINAL {
    m_command_list->Close();
  };

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
      pipeline_layout* _layout) WN_GRAPHICS_OVERRIDE_FINAL;

  void bind_graphics_pipeline(
      graphics_pipeline* _pipeline) WN_GRAPHICS_OVERRIDE_FINAL;

protected:
  friend class d3d12_device;
  friend class d3d12_queue;

  d3d12_command_list() = default;

  WN_FORCE_INLINE ID3D12CommandList* command_list() const {
    return m_command_list.Get();
  }

  WN_FORCE_INLINE void initialize(memory::allocator* _allocator,
      Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>&& _command_list) {
    m_allocator = _allocator;
    m_command_list = core::move(_command_list);
    m_current_render_pass = nullptr;
    m_current_framebuffer = nullptr;
    m_current_subpass = 0;
    m_current_graphics_pipeline_layout = nullptr;
    m_clear_values = containers::dynamic_array<clear_value>(m_allocator);
    m_active_framebuffer_resource_states =
        containers::dynamic_array<resource_state>(m_allocator);
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

  Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_command_list;
  const render_pass* m_current_render_pass;
  const framebuffer* m_current_framebuffer;
  containers::dynamic_array<resource_state>
      m_active_framebuffer_resource_states;
  uint32_t m_current_subpass;
  D3D12_RECT m_render_area;
  containers::dynamic_array<clear_value> m_clear_values;
  memory::allocator* m_allocator;
  pipeline_layout* m_current_graphics_pipeline_layout;
};

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_D3D12_DEVICE_H__
