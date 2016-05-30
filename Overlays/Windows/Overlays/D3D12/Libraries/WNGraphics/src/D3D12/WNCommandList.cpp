// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNGraphics/inc/Internal/D3D12/WNCommandList.h"
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

}  // namespace d3d12
}  // namesapce internal
}  // namespace graphics
}  // namspace wn
