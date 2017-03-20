// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_RUNTIME_GRAPHICS_INTERNAL_D3D12_DEVICE_H__
#define __WN_RUNTIME_GRAPHICS_INTERNAL_D3D12_DEVICE_H__

#include "WNContainers/inc/WNDynamicArray.h"
#include "WNContainers/inc/WNRangePartition.h"
#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNArenaProperties.h"
#include "WNGraphics/inc/WNDescriptorData.h"
#include "WNGraphics/inc/WNFramebufferData.h"
#include "WNGraphics/inc/WNHeapTraits.h"
#include "WNGraphics/inc/WNRenderPassTypes.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/WNUniquePtr.h"
#include "WNMultiTasking/inc/WNSpinLock.h"
#include "WNWindow/inc/WNWindow.h"

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/WNDevice.h"
#else
#include "WNCore/inc/WNUtility.h"
#endif

#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl.h>
#include <atomic>

namespace wn {
namespace memory {

class allocator;

}  // namespace memory

namespace graphics {

class arena;
class command_allocator;
class command_list;
class descriptor_set;
class descriptor_set_layout;
class descriptor_pool;
class pipeline_layout;
class fence;
class queue;
class shader_module;
class swapchain;
class render_pass;
struct image_create_info;
struct swapchain_create_info;
class image;
class image_view;
class framebuffer;

using queue_ptr = memory::unique_ptr<queue>;
using swapchain_ptr = memory::unique_ptr<swapchain>;
using command_list_ptr = memory::unique_ptr<command_list>;

namespace internal {
namespace d3d12 {

template <typename T>
struct data_type;
class d3d12_adapter;

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
using d3d12_device_base = device;
#else
using d3d12_device_base = core::non_copyable;
#endif

class d3d12_device WN_GRAPHICS_FINAL : public d3d12_device_base {
public:
  ~d3d12_device() WN_GRAPHICS_OVERRIDE_FINAL = default;

  containers::contiguous_range<const arena_properties> get_arena_properties()
      const WN_GRAPHICS_OVERRIDE_FINAL;

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

  struct heap_info WN_FINAL {
    const D3D12_HEAP_PROPERTIES heap_properties;
    const D3D12_HEAP_FLAGS heap_flags;
  };

  const static size_t k_reserved_resource_size = 1000000;
  const static size_t k_reserved_samplers_size = 2048;
  const static size_t k_reserved_view_size = 10000;

  WN_FORCE_INLINE d3d12_device()
    : d3d12_device_base(),
      m_allocator(nullptr),
      m_log(nullptr),
      m_num_queues(0) {}

  bool initialize(memory::allocator* _allocator, logging::log* _log,
      const Microsoft::WRL::ComPtr<IDXGIFactory4>& _d3d12_factory,
      Microsoft::WRL::ComPtr<ID3D12Device>&& _d3d12_device);

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

  void initialize_shader_module(shader_module* s,
      const containers::contiguous_range<const uint8_t>& bytes)
      WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_shader_module(
      shader_module* shader_module) WN_GRAPHICS_OVERRIDE_FINAL;

  void initialize_descriptor_set_layout(descriptor_set_layout* _layout,
      const containers::contiguous_range<const descriptor_binding_info>&
          _binding_infos) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_descriptor_set_layout(
      descriptor_set_layout* _layout) WN_GRAPHICS_OVERRIDE_FINAL;

  void initialize_descriptor_pool(descriptor_pool* _pool,
      const containers::contiguous_range<const descriptor_pool_create_info>&
          _pool_data) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_descriptor_pool(
      descriptor_pool* _pool) WN_GRAPHICS_OVERRIDE_FINAL;

  void initialize_descriptor_set(descriptor_set* _set, descriptor_pool* _pool,
      const descriptor_set_layout* _pool_data) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_descriptor_set(descriptor_set* _set) WN_GRAPHICS_OVERRIDE_FINAL;

  void initialize_pipeline_layout(pipeline_layout* _layout,
      const containers::contiguous_range<const descriptor_set_layout*>&
          _descriptor_sets) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_pipeline_layout(
      pipeline_layout* _layout) WN_GRAPHICS_OVERRIDE_FINAL;

  void initialize_render_pass(render_pass* _pass,
      const containers::contiguous_range<const render_pass_attachment>&
          _attachments,
      const containers::contiguous_range<const subpass_description>& _subpasses,
      const containers::contiguous_range<const subpass_dependency>& _deps)
      WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_render_pass(render_pass* _pass) WN_GRAPHICS_OVERRIDE_FINAL;

  void initialize_image_view(
      image_view* _view, const image* image) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_image_view(image_view* _view) WN_GRAPHICS_OVERRIDE_FINAL;

  // arena methods
  bool initialize_arena(arena* _arena, const size_t _index, const size_t _size,
      const bool _multisampled) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_arena(arena* _arena) WN_GRAPHICS_OVERRIDE_FINAL;

  void initialize_framebuffer(framebuffer* _framebuffer,
      const framebuffer_create_info& _info) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_framebuffer(
      framebuffer* _framebuffer) WN_GRAPHICS_OVERRIDE_FINAL;

private:
  template <typename T>
  typename data_type<T>::value& get_data(T* t);

  template <typename T>
  typename data_type<const T>::value& get_data(const T* const t);

  D3D12_FEATURE_DATA_D3D12_OPTIONS m_options;
  containers::dynamic_array<heap_info> m_heap_info;
  containers::dynamic_array<arena_properties> m_arena_properties;

public:
  struct locked_heap {
  public:
    void initialize(memory::allocator* _allocator, logging::log* _log,
        ID3D12Device* _device, size_t _size, D3D12_DESCRIPTOR_HEAP_TYPE _type) {
      m_descriptor_size = _device->GetDescriptorHandleIncrementSize(_type);
      _log->log_info(
          "Descriptor Size for ", _type, " heap is ", m_descriptor_size);

      m_parition = containers::default_range_partition(_allocator, _size);

      D3D12_DESCRIPTOR_HEAP_DESC desc = {
          _type,  // Type
          // The maximum we are able to put in a heap. (TODO: figure out if this
          // is too big)
          static_cast<UINT>(_size),
          ((_type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) ||
              (_type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER))
              ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
              : D3D12_DESCRIPTOR_HEAP_FLAG_NONE,  // flags
          0,                                      // nodemask
      };

      HRESULT hr = _device->CreateDescriptorHeap(
          &desc, __uuidof(ID3D12DescriptorHeap), &m_heap);

      if (FAILED(hr)) {
        _log->log_error("Could not create root descriptors: ", hr);
      }

      m_root_cpu_handle = m_heap->GetCPUDescriptorHandleForHeapStart();
    }

    containers::default_range_partition::token get_partition(
        size_t num_elements) {
      multi_tasking::spin_lock_guard guard(m_lock);
      return m_parition.get_interval(num_elements);
    }

    void release_partition(containers::default_range_partition::token t) {
      multi_tasking::spin_lock_guard guard(m_lock);
      t.free();
    }
    const ID3D12DescriptorHeap* heap() const {
      return m_heap.Get();
    }

    D3D12_CPU_DESCRIPTOR_HANDLE get_handle_at(size_t offset) {
      return D3D12_CPU_DESCRIPTOR_HANDLE{
          m_root_cpu_handle.ptr + (offset * m_descriptor_size)};
    }

  private:
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_heap;
    D3D12_CPU_DESCRIPTOR_HANDLE m_root_cpu_handle;
    containers::default_range_partition m_parition;
    multi_tasking::spin_lock m_lock;
    uint32_t m_descriptor_size;
  };
  locked_heap m_csv_heap;
  locked_heap m_sampler_heap;
  locked_heap m_rtv_heap;
  locked_heap m_dsv_heap;

  Microsoft::WRL::ComPtr<ID3D12Device> m_device;
  Microsoft::WRL::ComPtr<IDXGIFactory4> m_factory;
  // We create a single largest (1M entries) root heap.
  // When we create a descriptor-pool we suballocate from this heap.

  memory::allocator* m_allocator;
  logging::log* m_log;
  std::atomic<uint32_t> m_num_queues;
};

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_RUNTIME_GRAPHICS_INTERNAL_D3D12_DEVICE_H__
