// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/Internal/D3D12/WNCommandList.h"
#include "WNGraphics/inc/Internal/D3D12/WNDataTypes.h"
#include "WNGraphics/inc/Internal/D3D12/WNDevice.h"
#include "WNGraphics/inc/Internal/D3D12/WNImageFormats.h"
#include "WNGraphics/inc/Internal/D3D12/WNResourceStates.h"
#include "WNGraphics/inc/WNHeap.h"
#include "WNGraphics/inc/WNImage.h"

namespace wn {
namespace graphics {
namespace internal {
namespace d3d12 {

void d3d12_command_list::enqueue_upload_barrier(
    const upload_heap&, size_t, size_t) {
  // No-op for now. We do not need a barrier between data upload and copy
  // operations.
}

void d3d12_command_list::enqueue_download_barrier(
    const download_heap&, size_t, size_t) {
  // No-op for now. We do not need a barrier between copy and data download
  // operations.
}

void d3d12_command_list::enqueue_buffer_copy(const upload_heap& _upload_heap,
    size_t _upload_offset_in_bytes, const download_heap& _download_heap,
    size_t _download_offset_in_bytes, size_t _upload_size) {
  const Microsoft::WRL::ComPtr<ID3D12Resource>& upload_res =
      _upload_heap.data_as<Microsoft::WRL::ComPtr<ID3D12Resource>>();
  const Microsoft::WRL::ComPtr<ID3D12Resource>& download_res =
      _download_heap.data_as<Microsoft::WRL::ComPtr<ID3D12Resource>>();

  m_command_list->CopyBufferRegion(download_res.Get(),
      _download_offset_in_bytes, upload_res.Get(), _upload_offset_in_bytes,
      _upload_size);
}

void d3d12_command_list::enqueue_resource_transition(
    const image& _image, resource_state _from, resource_state _to) {
  const Microsoft::WRL::ComPtr<ID3D12Resource>& image_res =
      _image.data_as<Microsoft::WRL::ComPtr<ID3D12Resource>>();

  if (m_current_render_pass) {
    const memory::unique_ptr<const render_pass_data>& rp =
        get_data(m_current_render_pass);
    const memory::unique_ptr<const framebuffer_data>& fb =
        get_data(m_current_framebuffer);

    for (size_t i = 0; i < rp->attachments.size(); ++i) {
      const image_view* view = fb->image_views[i];
      const memory::unique_ptr<const image_view_info>& view_data =
          get_data(view);
      const Microsoft::WRL::ComPtr<ID3D12Resource>& image_resource =
          view_data->image->data_as<Microsoft::WRL::ComPtr<ID3D12Resource>>();
      if (image_resource == image_res) {
        m_active_framebuffer_resource_states[i] = _to;
        break;
      }
    }
  }

  D3D12_RESOURCE_BARRIER barrier = {
      D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,  // Type
      D3D12_RESOURCE_BARRIER_FLAG_NONE,        // Flags
      D3D12_RESOURCE_TRANSITION_BARRIER{
          image_res.Get(),                                 // pResource
          D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,         // subresource
          resource_state_to_d3d12_resource_states(_from),  // StateBefore
          resource_state_to_d3d12_resource_states(_to),    // StateAfter
      }};
  m_command_list->ResourceBarrier(1, &barrier);
}

void d3d12_command_list::enqueue_texture_upload(const upload_heap& _upload_heap,
    size_t _upload_offset_in_bytes, const image& _image) {
  const Microsoft::WRL::ComPtr<ID3D12Resource>& upload_res =
      _upload_heap.data_as<Microsoft::WRL::ComPtr<ID3D12Resource>>();
  const Microsoft::WRL::ComPtr<ID3D12Resource>& image_res =
      _image.data_as<Microsoft::WRL::ComPtr<ID3D12Resource>>();

  const image::image_buffer_resource_info& info = _image.get_resource_info();

  D3D12_TEXTURE_COPY_LOCATION source = {upload_res.Get(),  // pResource
      D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,            // Type
      D3D12_PLACED_SUBRESOURCE_FOOTPRINT{
          _upload_offset_in_bytes,  // Offset
          D3D12_SUBRESOURCE_FOOTPRINT{
              image_format_to_dxgi_format(info.format),    // Format
              static_cast<UINT>(info.width),               // Width
              static_cast<UINT>(info.height),              // Height
              static_cast<UINT>(info.depth),               // Depth,
              static_cast<UINT>(info.row_pitch_in_bytes),  // RowPitch
          },
      }};

  D3D12_TEXTURE_COPY_LOCATION dest = {
      image_res.Get(),                            // pResource
      D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,  // Type
      UINT{0}                                     // SubresourceIndex
  };

  m_command_list->CopyTextureRegion(&dest, 0, 0, 0, &source, nullptr);
}
void d3d12_command_list::enqueue_texture_download(const image& _image,
    const download_heap& _download_heap, size_t _download_offset_in_bytes) {
  const Microsoft::WRL::ComPtr<ID3D12Resource>& download_res =
      _download_heap.data_as<Microsoft::WRL::ComPtr<ID3D12Resource>>();
  const Microsoft::WRL::ComPtr<ID3D12Resource>& image_res =
      _image.data_as<Microsoft::WRL::ComPtr<ID3D12Resource>>();

  const image::image_buffer_resource_info& info = _image.get_resource_info();

  D3D12_TEXTURE_COPY_LOCATION source = {
      image_res.Get(),                            // pResource
      D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,  // Type
      UINT{0}                                     // SubresourceIndex
  };

  D3D12_TEXTURE_COPY_LOCATION dest = {download_res.Get(),  // pResource
      D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,            // Type
      D3D12_PLACED_SUBRESOURCE_FOOTPRINT{
          _download_offset_in_bytes,  // Offset
          D3D12_SUBRESOURCE_FOOTPRINT{
              image_format_to_dxgi_format(info.format),    // Format
              static_cast<UINT>(info.width),               // Width
              static_cast<UINT>(info.height),              // Height
              static_cast<UINT>(info.depth),               // Depth,
              static_cast<UINT>(info.row_pitch_in_bytes),  // RowPitch
          },
      }};
  m_command_list->CopyTextureRegion(&dest, 0, 0, 0, &source, nullptr);
}

void d3d12_command_list::draw(uint32_t _vertex_count, uint32_t _instance_count,
    uint32_t _vertex_offset, uint32_t _instance_offset) {
  m_command_list->DrawInstanced(
      _vertex_count, _instance_count, _vertex_offset, _instance_offset);
}

void d3d12_command_list::begin_render_pass(render_pass* _pass,
    framebuffer* _framebuffer, const render_area& _render_area,
    const containers::contiguous_range<clear_value>& _clears) {
  const memory::unique_ptr<const render_pass_data>& rp =
      get_data(m_current_render_pass);
  m_current_render_pass = _pass;
  m_current_framebuffer = _framebuffer;
  m_current_subpass = 0;
  m_clear_values.clear();
  m_clear_values.insert(m_clear_values.begin(), _clears.begin(), _clears.end());
  m_render_area = _render_area;
  m_active_framebuffer_resource_states.clear();
  m_active_framebuffer_resource_states.resize(rp->attachments.size());
  for (size_t i = 0; i < m_active_framebuffer_resource_states.size(); ++i) {
    m_active_framebuffer_resource_states[i] = rp->attachments[i].initial_state;
  }
  set_up_subpass();
}

void d3d12_command_list::set_up_subpass() {
  const memory::unique_ptr<const render_pass_data>& rp =
      get_data(m_current_render_pass);
  const memory::unique_ptr<const framebuffer_data>& fb =
      get_data(m_current_framebuffer);
  D3D12_RECT rect = {static_cast<LONG>(m_render_area.x),
      static_cast<LONG>(m_render_area.y),
      static_cast<LONG>(m_render_area.x) +
          static_cast<LONG>(m_render_area.width),
      static_cast<LONG>(m_render_area.y) +
          static_cast<LONG>(m_render_area.width)};

  if (m_current_subpass != 0 && m_current_subpass <= rp->subpasses.size()) {
    uint32_t last_subpass = m_current_subpass - 1;
    for (int32_t color_attachment :
        rp->subpasses[last_subpass].color_attachments) {
      auto& attachment_desc = rp->attachments[color_attachment];
      if (attachment_desc.attachment_store_op == store_op::dont_care) {
        const image_view* view = fb->image_views[color_attachment];
        const memory::unique_ptr<const image_view_info>& view_data =
            get_data(view);
        const Microsoft::WRL::ComPtr<ID3D12Resource>& image_resource =
            view_data->image->data_as<Microsoft::WRL::ComPtr<ID3D12Resource>>();

        m_command_list->DiscardResource(image_resource.Get(), nullptr);
      }
    }
    if (rp->subpasses[last_subpass].depth_attachment >= 0) {
      auto& attachment_desc =
          rp->attachments[rp->subpasses[last_subpass].depth_attachment];
      if (attachment_desc.attachment_store_op == store_op::dont_care &&
          attachment_desc.stencil_store_op == store_op::dont_care) {
        const image_view* view =
            fb->image_views[rp->subpasses[last_subpass].depth_attachment];
        const memory::unique_ptr<const image_view_info>& view_data =
            get_data(view);
        const Microsoft::WRL::ComPtr<ID3D12Resource>& image_resource =
            view_data->image->data_as<Microsoft::WRL::ComPtr<ID3D12Resource>>();

        m_command_list->DiscardResource(image_resource.Get(), nullptr);
      }
    }
  }

  if (m_current_subpass < rp->subpasses.size()) {
    // Now that we have discarded everything from the last pass, lets start
    // clearing/discarding everything from this pass.
    for (int32_t color_attachment :
        rp->subpasses[m_current_subpass].color_attachments) {
      auto& attachment_desc = rp->attachments[color_attachment];
      const image_view* view = fb->image_views[color_attachment];
      const memory::unique_ptr<const image_view_info>& view_data =
          get_data(view);
      const Microsoft::WRL::ComPtr<ID3D12Resource>& image_resource =
          view_data->image->data_as<Microsoft::WRL::ComPtr<ID3D12Resource>>();
      if (attachment_desc.attachment_load_op == load_op::dont_care) {
        m_command_list->DiscardResource(image_resource.Get(), nullptr);
      }
      // TODO(awoloszyn): Maybe use the render_area from
      if (attachment_desc.attachment_load_op == load_op::clear) {
        float color[4] = {0};
        auto& c = m_clear_values[color_attachment].color;
        if (is_format_normalized(attachment_desc.format)) {
          memory::memory_copy(&color[0], &c.float_vals[0], 4);
        } else if (is_format_int(attachment_desc.format)) {
          color[0] = static_cast<float>(c.int_vals[0]);
          color[1] = static_cast<float>(c.int_vals[1]);
          color[2] = static_cast<float>(c.int_vals[2]);
          color[3] = static_cast<float>(c.int_vals[3]);
        } else if (is_format_uint(attachment_desc.format)) {
          color[0] = static_cast<float>(c.uint_vals[0]);
          color[1] = static_cast<float>(c.uint_vals[1]);
          color[2] = static_cast<float>(c.uint_vals[2]);
          color[3] = static_cast<float>(c.uint_vals[3]);
        }
        auto& image_view_handle = fb->image_view_handles[color_attachment];
        D3D12_CPU_DESCRIPTOR_HANDLE handle =
            image_view_handle.heap->get_handle_at(
                image_view_handle.token.offset());
        m_command_list->ClearRenderTargetView(handle, color, 1, &rect);
      }
    }
    if (rp->subpasses[m_current_subpass].depth_attachment >= 0) {
      int32_t depth_attachment =
          rp->subpasses[m_current_subpass].depth_attachment;
      auto& attachment_desc = rp->attachments[depth_attachment];
      const image_view* view = fb->image_views[depth_attachment];
      const memory::unique_ptr<const image_view_info>& view_data =
          get_data(view);
      const Microsoft::WRL::ComPtr<ID3D12Resource>& image_resource =
          view_data->image->data_as<Microsoft::WRL::ComPtr<ID3D12Resource>>();
      if (attachment_desc.attachment_load_op == load_op::dont_care &&
          attachment_desc.stencil_load_op == load_op::dont_care) {
        m_command_list->DiscardResource(image_resource.Get(), nullptr);
      }
      // TODO(awoloszyn): Maybe use the render_area from
      if (attachment_desc.attachment_load_op == load_op::clear ||
          attachment_desc.stencil_load_op == load_op::clear) {
        float depth = m_clear_values[depth_attachment].depth.depth;
        uint8_t stencil = static_cast<uint8_t>(
            m_clear_values[depth_attachment].depth.stencil);
        D3D12_CLEAR_FLAGS flags = D3D12_CLEAR_FLAGS(
            (attachment_desc.attachment_load_op == load_op::clear
                    ? D3D12_CLEAR_FLAG_DEPTH
                    : 0) |
            (attachment_desc.stencil_load_op == load_op::clear
                    ? D3D12_CLEAR_FLAG_STENCIL
                    : 0));
        auto& image_view_handle = fb->image_view_handles[depth_attachment];
        D3D12_CPU_DESCRIPTOR_HANDLE handle =
            image_view_handle.heap->get_handle_at(
                image_view_handle.token.offset());
        m_command_list->ClearDepthStencilView(
            handle, flags, depth, stencil, 1, &rect);
      }
    }
  }
}

void d3d12_command_list::end_render_pass() {
  m_current_subpass += 1;
  set_up_subpass();
  const memory::unique_ptr<const render_pass_data>& rp =
      get_data(m_current_render_pass);
  const memory::unique_ptr<const framebuffer_data>& fb =
      get_data(m_current_framebuffer);
  for (size_t i = 0; i < rp->attachments.size(); ++i) {
    auto& attachment = rp->attachments[i];
    if (m_active_framebuffer_resource_states[i] != attachment.initial_state) {
      // Resource transition time.
      const image_view* view = fb->image_views[i];
      const memory::unique_ptr<const image_view_info>& view_data =
          get_data(view);
      const Microsoft::WRL::ComPtr<ID3D12Resource>& image_resource =
          view_data->image->data_as<Microsoft::WRL::ComPtr<ID3D12Resource>>();

      D3D12_RESOURCE_BARRIER barrier = {
          D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,  // Type
          D3D12_RESOURCE_BARRIER_FLAG_NONE,        // Flags
          D3D12_RESOURCE_TRANSITION_BARRIER{
              image_resource.Get(),                     // pResource
              D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,  // subresource
              resource_state_to_d3d12_resource_states(
                  m_active_framebuffer_resource_states[i]),  // StateBefore
              resource_state_to_d3d12_resource_states(
                  attachment.final_state),  // StateAfter
          }};
      m_command_list->ResourceBarrier(1, &barrier);
    }
  }
  m_current_framebuffer = nullptr;
  m_current_render_pass = nullptr;
}

void d3d12_command_list::bind_graphics_pipeline_layout(
    pipeline_layout* _layout) {
  m_current_graphics_pipeline_layout = _layout;
  memory::unique_ptr<pipeline_layout_object>& layout = get_data(_layout);
  m_command_list->SetGraphicsRootSignature(layout->signature.Get());
}

void d3d12_command_list::bind_graphics_descriptor_sets(
    const containers::contiguous_range<const descriptor_set*> _sets,
    uint32_t base_index) {
  containers::dynamic_array<D3D12_GPU_DESCRIPTOR_HANDLE> handles(
      m_allocator, _sets.size());
  containers::dynamic_array<uint32_t> offsets(m_allocator, _sets.size());
  memory::unique_ptr<pipeline_layout_object>& layout =
      get_data(m_current_graphics_pipeline_layout);
  for (size_t i = 0; i < _sets.size(); ++i) {
    uint32_t slot = layout->descriptor_set_binding_base[i + base_index];
    const descriptor_set* set_object = _sets[i];
    const memory::unique_ptr<const descriptor_set_data>& data =
        get_data(set_object);
    for (size_t j = 0; j < data->descriptors.size(); ++j) {
      m_command_list->SetGraphicsRootDescriptorTable(
          slot, data->descriptors[j].heap->get_gpu_handle_at(
                    data->descriptors[j].offset.offset()));
    }
  }
}

template <typename T>
typename data_type<T>::value& d3d12_command_list::get_data(T* t) {
  return t->data_as<typename data_type<T>::value>();
}

template <typename T>
typename data_type<const T>::value& d3d12_command_list::get_data(
    const T* const t) {
  return t->data_as<typename data_type<const T>::value>();
}

}  // namespace d3d12
}  // namesapce internal
}  // namespace graphics
}  // namspace wn
