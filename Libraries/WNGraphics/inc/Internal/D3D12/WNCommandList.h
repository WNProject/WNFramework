// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_D3D12_COMMAND_LIST_H__
#define __WN_GRAPHICS_INTERNAL_D3D12_COMMAND_LIST_H__

#include "WNCore/inc/WNBase.h"
#include "WNGraphics/inc/WNCommandList.h"

#include <wrl.h>
#include <d3d12.h>
namespace wn {
namespace graphics {
namespace internal {
namespace d3d12 {

class d3d12_command_list final : public internal_command_list {
public:
  WN_FORCE_INLINE d3d12_command_list(
      Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>&& _command_list)
    : m_command_list(_command_list) {}
  ~d3d12_command_list() final = default;
  ID3D12CommandList* command_list() const {
    return m_command_list.Get();
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

  Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_command_list;
};

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_D3D12_DEVICE_H__
