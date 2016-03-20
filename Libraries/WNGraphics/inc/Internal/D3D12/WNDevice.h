// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_D3D12_DEVICE_H__
#define __WN_GRAPHICS_INTERNAL_D3D12_DEVICE_H__

#include "WNCore/inc/WNUtility.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNGraphics/inc/WNDeviceForward.h"

#include <atomic>
#include <D3D12.h>
#include <wrl.h>

namespace WNLogging {

class WNLog;

}  // namespace WNLogging

namespace wn {
namespace memory {

class allocator;

}  // namespace memory

namespace graphics {
namespace internal {
namespace d3d12 {

class device final : public internal::device {
public:
  WN_FORCE_INLINE device(memory::allocator* _allocator, WNLogging::WNLog* _log,
      Microsoft::WRL::ComPtr<ID3D12Device>&& _d3d12_device)
    : internal::device(_allocator, _log),
      m_device(core::move(_d3d12_device)),
      m_num_queues(0) {}

  upload_heap create_upload_heap(size_t _num_bytes) final;
  download_heap create_download_heap(size_t _num_bytes) final;

  queue_ptr create_queue() final;
  fence create_fence() final;

private:
  template <typename T>
  friend class graphics::heap;
  friend class graphics::fence;
  friend class queue;

  uint8_t* acquire_range(
      upload_heap* _buffer, size_t _offset, size_t _num_bytes) final;
  uint8_t* synchronize(
      upload_heap* _buffer, size_t _offset, size_t _num_bytes) final;
  void release_range(
      upload_heap* _buffer, size_t _offset, size_t _num_bytes) final;
  void destroy_heap(upload_heap* _heap) final;

  uint8_t* acquire_range(
      download_heap* _buffer, size_t _offset, size_t _num_bytes) final;
  uint8_t* synchronize(
      download_heap* _buffer, size_t _offset, size_t _num_bytes) final;
  void release_range(
      download_heap* _buffer, size_t _offset, size_t _num_bytes) final;
  void destroy_heap(download_heap* _heap) final;

  // Destroy methods
  void destroy_queue(graphics::queue* _queue) final;
  void destroy_fence(fence* _fence) final;

  // Fance methods
  void wait_fence(const fence* _fence) const final;
  void reset_fence(fence* _fence) final;

  // Templated heap helpers
  template <typename heap_type>
  heap_type create_heap(size_t _num_bytes, const D3D12_HEAP_PROPERTIES& _params,
      const D3D12_RESOURCE_STATES& _states);
  template <typename heap_type>
  void destroy_typed_heap(heap_type* type);

  Microsoft::WRL::ComPtr<ID3D12Device> m_device;
  std::atomic<uint32_t> m_num_queues;
};

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_D3D12_DEVICE_H__
