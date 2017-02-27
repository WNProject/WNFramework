// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_D3D12_DEVICE_H__
#define __WN_GRAPHICS_INTERNAL_D3D12_DEVICE_H__

#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNHeapTraits.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/WNUniquePtr.h"
#include "WNWindow/inc/WNWindow.h"

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/WNDevice.h"
#else
#include "WNCore/inc/WNUtility.h"
#endif

#include <D3D12.h>
#include <DXGI1_4.h>
#include <wrl.h>
#include <atomic>

namespace wn {
namespace memory {

class allocator;

}  // namespace memory

namespace graphics {

class command_allocator;
class command_list;
class fence;
class queue;
class swapchain;
struct image_create_info;
struct swapchain_create_info;
class image;

using queue_ptr = memory::unique_ptr<queue>;
using swapchain_ptr = memory::unique_ptr<swapchain>;
using command_list_ptr = memory::unique_ptr<command_list>;

namespace internal {
namespace d3d12 {

class d3d12_adapter;

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
using d3d12_device_base = device;
#else
using d3d12_device_base = core::non_copyable;
#endif

class d3d12_device WN_GRAPHICS_FINAL : public d3d12_device_base {
public:
  ~d3d12_device() WN_GRAPHICS_OVERRIDE_FINAL = default;

  queue_ptr create_queue() WN_GRAPHICS_OVERRIDE_FINAL;

  size_t get_image_upload_buffer_alignment() WN_GRAPHICS_OVERRIDE_FINAL;
  size_t get_buffer_upload_buffer_alignment() WN_GRAPHICS_OVERRIDE_FINAL;

  swapchain_ptr create_swapchain(const swapchain_create_info& _info,
      queue* queue, runtime::window::window* window) WN_GRAPHICS_OVERRIDE_FINAL;

protected:
  friend class fence;
  friend class queue;
  friend class d3d12_queue;
  friend class d3d12_adapter;

  WN_FORCE_INLINE d3d12_device()
    : d3d12_device_base(),
      m_allocator(nullptr),
      m_log(nullptr),
      m_num_queues(0) {}

  WN_FORCE_INLINE void initialize(memory::allocator* _allocator,
      logging::log* _log, Microsoft::WRL::ComPtr<IDXGIFactory4> _d3d12_factory,
      Microsoft::WRL::ComPtr<ID3D12Device>&& _d3d12_device) {
    m_allocator = _allocator;
    m_log = _log;
    m_device = core::move(_d3d12_device);
    m_factory = _d3d12_factory;
  }

  void initialize_upload_heap(upload_heap* _upload_heap,
      const size_t _num_bytes) WN_GRAPHICS_OVERRIDE_FINAL;
  uint8_t* acquire_range(upload_heap* _buffer, size_t _offset,
      size_t _num_bytes) WN_GRAPHICS_OVERRIDE_FINAL;
  uint8_t* synchronize(upload_heap* _buffer, size_t _offset,
      size_t _num_bytes) WN_GRAPHICS_OVERRIDE_FINAL;
  void release_range(upload_heap* _buffer, size_t _offset,
      size_t _num_bytes) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_heap(upload_heap* _heap) WN_GRAPHICS_OVERRIDE_FINAL;

  void initialize_download_heap(download_heap* _download_heap,
      const size_t _num_bytes) WN_GRAPHICS_OVERRIDE_FINAL;
  uint8_t* acquire_range(download_heap* _buffer, size_t _offset,
      size_t _num_bytes) WN_GRAPHICS_OVERRIDE_FINAL;
  uint8_t* synchronize(download_heap* _buffer, size_t _offset,
      size_t _num_bytes) WN_GRAPHICS_OVERRIDE_FINAL;
  void release_range(download_heap* _buffer, size_t _offset,
      size_t _num_bytes) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_heap(download_heap* _heap) WN_GRAPHICS_OVERRIDE_FINAL;

  // Destroy methods
  void destroy_queue(queue* _queue) WN_GRAPHICS_OVERRIDE_FINAL;

  // command allocator methods
  void initialize_command_allocator(
      command_allocator* _command_allocator) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_command_allocator(command_allocator*) WN_GRAPHICS_OVERRIDE_FINAL;

  // fence methods
  void initialize_fence(fence* _fence) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_fence(fence* _fence) WN_GRAPHICS_OVERRIDE_FINAL;
  void wait_fence(const fence* _fence) const WN_GRAPHICS_OVERRIDE_FINAL;
  void reset_fence(fence* _fence) WN_GRAPHICS_OVERRIDE_FINAL;

  // image methods
  void initialize_image(
      const image_create_info& _info, image* _image) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_image(image* _image) WN_GRAPHICS_OVERRIDE_FINAL;

  // Templated heap helpers
  template <typename HeapType>
  void initialize_heap(HeapType* _heap, const size_t _num_bytes,
      const D3D12_HEAP_PROPERTIES& _params,
      const D3D12_RESOURCE_STATES& _states);

  template <typename HeapType>
  void destroy_typed_heap(HeapType* type);

  command_list_ptr create_command_list(
      command_allocator*) WN_GRAPHICS_OVERRIDE_FINAL;

public:
  Microsoft::WRL::ComPtr<ID3D12Device> m_device;
  Microsoft::WRL::ComPtr<IDXGIFactory4> m_factory;
  memory::allocator* m_allocator;
  logging::log* m_log;
  std::atomic<uint32_t> m_num_queues;
};

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_D3D12_DEVICE_H__