// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/Internal/D3D12/WNCommandList.h"
#include "WNGraphics/inc/Internal/D3D12/WNDataTypes.h"
#include "WNGraphics/inc/Internal/D3D12/WNDevice.h"
#include "WNGraphics/inc/Internal/D3D12/WNImageFormats.h"
#include "WNGraphics/inc/Internal/D3D12/WNResourceStates.h"
#include "WNGraphics/inc/WNImage.h"
#include "WNGraphics/inc/WNRenderPass.h"

namespace wn {
namespace runtime {
namespace graphics {
namespace internal {
namespace d3d12 {

void d3d12_command_list::transition_resource(
    const image& _image, resource_state _from, resource_state _to) {
  const image* img = &_image;
  const auto& image_res = get_data(img);

  if (m_current_render_pass) {
    const memory::unique_ptr<const render_pass_data>& rp =
        get_data(m_current_render_pass);
    const memory::unique_ptr<const framebuffer_data>& fb =
        get_data(m_current_framebuffer);

    for (size_t i = 0; i < rp->attachments.size(); ++i) {
      const image_view* view = fb->image_views[i];
      const memory::unique_ptr<const image_view_info>& view_data =
          get_data(view);

      const auto& im = get_data((const image*)view_data->image);
      if (image_res->image == im->image) {
        m_active_framebuffer_resource_states[i] = _to;
        break;
      }
    }
  }

  D3D12_RESOURCE_BARRIER barrier = {
      D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,  // Type
      D3D12_RESOURCE_BARRIER_FLAG_NONE,        // Flags
      D3D12_RESOURCE_TRANSITION_BARRIER{
          image_res->image.Get(),                          // pResource
          D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,         // subresource
          resource_state_to_d3d12_resource_states(_from),  // StateBefore
          resource_state_to_d3d12_resource_states(_to),    // StateAfter
      }};
  m_command_list->ResourceBarrier(1, &barrier);
}

void d3d12_command_list::transition_resource(
    const buffer& _buffer, resource_state _from, resource_state _to) {
  const buffer* buff = &_buffer;
  const memory::unique_ptr<const buffer_info>& data = get_data(buff);
  D3D12_RESOURCE_BARRIER barrier = {
      D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,  // Type
      D3D12_RESOURCE_BARRIER_FLAG_NONE,        // Flags
      D3D12_RESOURCE_TRANSITION_BARRIER{
          data->resource.Get(),                            // pResource
          D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,         // subresource
          resource_state_to_d3d12_resource_states(_from),  // StateBefore
          resource_state_to_d3d12_resource_states(_to),    // StateAfter
      }};
  m_command_list->ResourceBarrier(1, &barrier);
}

void d3d12_command_list::copy_buffer(const buffer& _src_buffer,
    size_t _src_offset, const buffer& _dst_buffer, size_t _dst_offset,
    size_t _size) {
  const buffer* srcbuff = &_src_buffer;
  const memory::unique_ptr<const buffer_info>& src_data = get_data(srcbuff);
  const buffer* dstbuff = &_dst_buffer;
  const memory::unique_ptr<const buffer_info>& dst_data = get_data(dstbuff);

  m_command_list->CopyBufferRegion(dst_data->resource.Get(), _dst_offset,
      src_data->resource.Get(), _src_offset, _size);
}

void d3d12_command_list::copy_buffer_to_image(const buffer& _src_buffer,
    size_t _src_offset_in_bytes, const image& _dst_image) {
  const buffer* srcbuff = &_src_buffer;
  const memory::unique_ptr<const buffer_info>& src_data = get_data(srcbuff);
  const auto& image_res = get_data((const image*)&_dst_image);

  const image::image_buffer_resource_info& info =
      _dst_image.get_buffer_requirements();

  D3D12_TEXTURE_COPY_LOCATION source = {src_data->resource.Get(),  // pResource
      D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,                    // Type
      D3D12_PLACED_SUBRESOURCE_FOOTPRINT{
          _src_offset_in_bytes,  // Offset
          D3D12_SUBRESOURCE_FOOTPRINT{
              image_format_to_dxgi_format(info.format),    // Format
              static_cast<UINT>(info.width),               // Width
              static_cast<UINT>(info.height),              // Height
              static_cast<UINT>(info.depth),               // Depth,
              static_cast<UINT>(info.row_pitch_in_bytes),  // RowPitch
          },
      }};

  D3D12_TEXTURE_COPY_LOCATION dest = {
      image_res->image.Get(),                     // pResource
      D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,  // Type
      UINT{0}                                     // SubresourceIndex
  };
  m_command_list->CopyTextureRegion(&dest, 0, 0, 0, &source, nullptr);
}

void d3d12_command_list::copy_image_to_buffer(const image& _src_image,
    const buffer& _dst_buffer, size_t _dst_offset_in_bytes) {
  const buffer* dstbuff = &_dst_buffer;
  const memory::unique_ptr<const buffer_info>& dst_data = get_data(dstbuff);
  const auto& image_res = get_data((const image*)&_src_image);

  const image::image_buffer_resource_info& info =
      _src_image.get_buffer_requirements();

  D3D12_TEXTURE_COPY_LOCATION source = {
      image_res->image.Get(),                     // pResource
      D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,  // Type
      UINT{0}                                     // SubresourceIndex
  };

  D3D12_TEXTURE_COPY_LOCATION dest = {dst_data->resource.Get(),  // pResource
      D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,                  // Type
      D3D12_PLACED_SUBRESOURCE_FOOTPRINT{
          _dst_offset_in_bytes,  // Offset
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

void d3d12_command_list::set_scissor(const scissor& _scissor) {
  D3D12_RECT scissor{static_cast<LONG>(_scissor.x),
      static_cast<LONG>(_scissor.y),
      static_cast<LONG>(_scissor.x + _scissor.width),
      static_cast<LONG>(_scissor.y + _scissor.height)};
  m_command_list->RSSetScissorRects(1, &scissor);
}

void d3d12_command_list::begin_render_pass(render_pass* _pass,
    framebuffer* _framebuffer, const render_area& _render_area,
    const containers::contiguous_range<clear_value>& _clears) {
  m_current_render_pass = _pass;
  const memory::unique_ptr<const render_pass_data>& rp =
      get_data(m_current_render_pass);
  m_current_framebuffer = _framebuffer;
  m_current_subpass = 0;
  m_clear_values.clear();
  m_clear_values.insert(m_clear_values.begin(), _clears.begin(), _clears.end());
  m_render_area = {static_cast<LONG>(_render_area.x),
      static_cast<LONG>(_render_area.y),
      static_cast<LONG>(_render_area.x) + static_cast<LONG>(_render_area.width),
      static_cast<LONG>(_render_area.y) +
          static_cast<LONG>(_render_area.width)};
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

  if (m_current_subpass != 0 && m_current_subpass <= rp->subpasses.size()) {
    uint32_t last_subpass = m_current_subpass - 1;
    for (int32_t color_attachment :
        rp->subpasses[last_subpass].color_attachments) {
      auto& attachment_desc = rp->attachments[color_attachment];
      if (attachment_desc.attachment_store_op == store_op::dont_care) {
        const image_view* view = fb->image_views[color_attachment];
        const memory::unique_ptr<const image_view_info>& view_data =
            get_data(view);
        const auto& image_res = get_data((const image*)view_data->image);

        m_command_list->DiscardResource(image_res->image.Get(), nullptr);
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
        const auto& image_res = get_data((const image*)view_data->image);

        m_command_list->DiscardResource(image_res->image.Get(), nullptr);
      }
    }
  }

  if (m_current_subpass < rp->subpasses.size()) {
    containers::dynamic_array<D3D12_CPU_DESCRIPTOR_HANDLE>
        render_target_handles(m_allocator);
    render_target_handles.reserve(
        rp->subpasses[m_current_subpass].color_attachments.size());
    bool has_depth_stencil = false;
    D3D12_CPU_DESCRIPTOR_HANDLE depth_stencil_handle;

    // Now that we have discarded everything from the last pass, lets start
    // clearing/discarding everything from this pass.
    for (int32_t color_attachment :
        rp->subpasses[m_current_subpass].color_attachments) {
      auto& attachment_desc = rp->attachments[color_attachment];
      const image_view* view = fb->image_views[color_attachment];
      const memory::unique_ptr<const image_view_info>& view_data =
          get_data(view);
      const auto& image_res = get_data((const image*)view_data->image);

      if (attachment_desc.attachment_load_op == load_op::dont_care) {
        m_command_list->DiscardResource(image_res->image.Get(), nullptr);
      }

      auto& image_view_handle = fb->image_view_handles[color_attachment];
      D3D12_CPU_DESCRIPTOR_HANDLE handle =
          image_view_handle.heap->get_handle_at(
              image_view_handle.token.offset());
      render_target_handles.push_back(handle);

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
        m_command_list->ClearRenderTargetView(handle, color, 1, &m_render_area);
      }
    }
    if (rp->subpasses[m_current_subpass].depth_attachment >= 0) {
      int32_t depth_attachment =
          rp->subpasses[m_current_subpass].depth_attachment;
      auto& attachment_desc = rp->attachments[depth_attachment];
      const image_view* view = fb->image_views[depth_attachment];
      const memory::unique_ptr<const image_view_info>& view_data =
          get_data(view);
      const auto& image_res = get_data((const image*)view_data->image);
      if (attachment_desc.attachment_load_op == load_op::dont_care &&
          attachment_desc.stencil_load_op == load_op::dont_care) {
        m_command_list->DiscardResource(image_res->image.Get(), nullptr);
      }

      auto& image_view_handle = fb->image_view_handles[depth_attachment];
      D3D12_CPU_DESCRIPTOR_HANDLE handle =
          image_view_handle.heap->get_handle_at(
              image_view_handle.token.offset());
      has_depth_stencil = true;
      depth_stencil_handle = handle;
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
        m_command_list->ClearDepthStencilView(
            handle, flags, depth, stencil, 1, &m_render_area);
      }
    }
    m_command_list->OMSetRenderTargets(
        static_cast<UINT>(render_target_handles.size()),
        render_target_handles.data(), false,
        has_depth_stencil ? &depth_stencil_handle : nullptr);
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
      const auto& image_res = get_data((const image*)view_data->image);

      D3D12_RESOURCE_BARRIER barrier = {
          D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,  // Type
          D3D12_RESOURCE_BARRIER_FLAG_NONE,        // Flags
          D3D12_RESOURCE_TRANSITION_BARRIER{
              image_res->image.Get(),                   // pResource
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
          static_cast<UINT>(slot + j),
          data->descriptors[j].heap->get_gpu_handle_at(
              data->descriptors[j].offset.offset()));
    }
  }
}

void d3d12_command_list::bind_graphics_pipeline(graphics_pipeline* _pipeline) {
  memory::unique_ptr<graphics_pipeline_data>& pipeline = get_data(_pipeline);

  static_graphics_pipeline_types static_datums = pipeline->static_datums;

  uint32_t next_datum = 1;
  while (static_datums) {
    uint8_t trailing_zeros = math::trailing_zeros(static_datums);
    next_datum <<= trailing_zeros;
    static_datums >>= trailing_zeros + 1;
    switch (static_cast<static_graphics_pipeline_type>(next_datum)) {
      case static_graphics_pipeline_type::blend_constants:
        m_command_list->OMSetBlendFactor(pipeline->static_blend_constants);
        break;
      case static_graphics_pipeline_type::scissors: {
        m_command_list->RSSetScissorRects(
            static_cast<UINT>(pipeline->static_scissors.size()),
            pipeline->static_scissors.data());
      } break;
      case static_graphics_pipeline_type::stencil_ref:
        m_command_list->OMSetStencilRef(pipeline->static_stencil_ref);
        break;
      case static_graphics_pipeline_type::viewports:
        m_command_list->RSSetViewports(
            static_cast<UINT>(pipeline->static_viewports.size()),
            pipeline->static_viewports.data());
        break;
      default:
        WN_DEBUG_ASSERT_DESC(false, "We should not end up here");
    }
    next_datum <<= 1;
  }

  m_command_list->SetPipelineState(pipeline->pipeline.Get());
  m_command_list->IASetPrimitiveTopology(pipeline->topology);
  m_current_graphics_pipeline = pipeline.get();
}

void d3d12_command_list::bind_vertex_buffer(
    uint32_t stream, const buffer* _buffer) {
  const memory::unique_ptr<const buffer_info>& buffer = get_data(_buffer);

  D3D12_VERTEX_BUFFER_VIEW view = {
      buffer->resource->GetGPUVirtualAddress(),  // BufferLocation
      static_cast<UINT>(_buffer->size()),        // SizeInBytes
      static_cast<UINT>(m_current_graphics_pipeline
                            ->vertex_stream_strides[stream])  // StrideInBytes
  };
  m_command_list->IASetVertexBuffers(stream, 1, &view);
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
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn
