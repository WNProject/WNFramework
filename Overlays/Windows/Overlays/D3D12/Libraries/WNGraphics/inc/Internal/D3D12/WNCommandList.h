// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_D3D12_COMMAND_LIST_H__
#define __WN_GRAPHICS_INTERNAL_D3D12_COMMAND_LIST_H__

#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNHeapTraits.h"
#include "WNGraphics/inc/WNResourceStates.h"

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

protected:
  friend class d3d12_device;
  friend class d3d12_queue;

  d3d12_command_list() = default;

  WN_FORCE_INLINE ID3D12CommandList* command_list() const {
    return m_command_list.Get();
  }

  WN_FORCE_INLINE void initialize(
      Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>&& _command_list) {
    m_command_list = core::move(_command_list);
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

  Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_command_list;
};

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_D3D12_DEVICE_H__
