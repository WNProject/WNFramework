// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNGraphics/inc/Internal/D3D12/WNCommandList.h"
#include "WNGraphics/inc/WNHeap.h"

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

void d3d12_command_list::enqueue_copy(const upload_heap& _upload_heap,
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

}  // namespace d3d12
}  // namesapce internal
}  // namespace graphics
}  // namspace wn
