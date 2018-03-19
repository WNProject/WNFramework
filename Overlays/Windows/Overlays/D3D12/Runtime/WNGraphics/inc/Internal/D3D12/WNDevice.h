// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_RUNTIME_GRAPHICS_INTERNAL_D3D12_DEVICE_H__
#define __WN_RUNTIME_GRAPHICS_INTERNAL_D3D12_DEVICE_H__

#include "WNContainers/inc/WNDynamicArray.h"
#include "WNContainers/inc/WNHashMap.h"
#include "WNContainers/inc/WNRangePartition.h"
#include "WNCore/inc/pair.h"
#include "WNGraphics/inc/Internal/D3D12/WNLockedHeap.h"
#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNArenaProperties.h"
#include "WNGraphics/inc/WNDescriptorData.h"
#include "WNGraphics/inc/WNFramebufferData.h"
#include "WNGraphics/inc/WNGraphicsTypes.h"
#include "WNGraphics/inc/WNRenderPassTypes.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/unique_ptr.h"
#include "WNMultiTasking/inc/spin_lock.h"
#include "WNWindow/inc/WNWindow.h"

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/WNDevice.h"
#else
#include "WNCore/inc/utilities.h"
#endif

#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl.h>
#include <atomic>

namespace wn {
namespace memory {

class allocator;

}  // namespace memory

namespace runtime {
namespace graphics {

class arena;
class buffer;
class command_allocator;
class command_list;
class descriptor_set;
class descriptor_set_layout;
class descriptor_pool;
class pipeline_layout;
class fence;
class queue;
class shader_module;
class sampler;
struct sampler_create_info;
class signal;
class surface;
struct surface_create_info;
class swapchain;
class render_pass;
struct buffer_memory_requirements;
struct image_create_info;
struct swapchain_create_info;
struct image_memory_requirements;
class graphics_pipeline_description;
class image;
class image_view;
class framebuffer;
class graphics_pipeline;

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

  swapchain_ptr create_swapchain(const surface& _surface,
      const swapchain_create_info& _info,
      queue* queue) WN_GRAPHICS_OVERRIDE_FINAL;

  math::mat44f get_perspective_fixup_matrix() WN_GRAPHICS_OVERRIDE_FINAL {
    return math::mat44f({        //
        1.0f, 0.0f, 0.0f, 0.0f,  //
        0.0f, 1.0f, 0.0f, 0.0f,  //
        0.0f, 0.0f, 1.0f, 0.0f,  //
        0.0, 0.0f, 0.0f, 1.0f});
  }

protected:
  friend class fence;
  friend class queue;
  friend class d3d12_queue;
  friend class d3d12_adapter;

  struct heap_info final {
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
      m_num_queues(0),
      m_sampler_cache(nullptr) {}

  bool initialize(memory::allocator* _allocator, logging::log* _log,
      const Microsoft::WRL::ComPtr<IDXGIFactory4>& _d3d12_factory,
      Microsoft::WRL::ComPtr<ID3D12Device>&& _d3d12_device);

  // Destroy methods
  void destroy_queue(queue* _queue) WN_GRAPHICS_OVERRIDE_FINAL;

  // command allocator methods
  void initialize_command_allocator(
      command_allocator* _command_allocator) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_command_allocator(command_allocator*) WN_GRAPHICS_OVERRIDE_FINAL;
  void reset_command_allocator(
      command_allocator* _alloc) WN_GRAPHICS_OVERRIDE_FINAL;

  // fence methods
  void initialize_fence(fence* _fence) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_fence(fence* _fence) WN_GRAPHICS_OVERRIDE_FINAL;
  void wait_fence(const fence* _fence) const WN_GRAPHICS_OVERRIDE_FINAL;
  void reset_fence(fence* _fence) WN_GRAPHICS_OVERRIDE_FINAL;

  // signal methods
  void initialize_signal(signal* _signal) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_signal(signal* _signal) WN_GRAPHICS_OVERRIDE_FINAL;

  // image methods
  void initialize_image(const image_create_info& _info,
      const clear_value& _optimized_clear,
      image* _image) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_image(image* _image) WN_GRAPHICS_OVERRIDE_FINAL;
  void bind_image_memory(
      image* _image, arena* _arena, size_t _offset) WN_GRAPHICS_OVERRIDE_FINAL;
  image_memory_requirements get_image_memory_requirements(
      const image* _image) WN_GRAPHICS_OVERRIDE_FINAL;

  void initialize_sampler(const sampler_create_info& _info,
      sampler* _sampler) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_sampler(sampler* _sampler) WN_GRAPHICS_OVERRIDE_FINAL;

  void update_descriptors(descriptor_set* _set,
      const containers::contiguous_range<buffer_descriptor>& _buffer_updates,
      const containers::contiguous_range<image_descriptor>& _image_updates,
      const containers::contiguous_range<sampler_descriptor>& _sampler_updates)
      WN_GRAPHICS_OVERRIDE_FINAL;

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
          _descriptor_sets,
      const containers::contiguous_range<const push_constant_range>&
          _push_constants) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_pipeline_layout(
      pipeline_layout* _layout) WN_GRAPHICS_OVERRIDE_FINAL;

  void initialize_render_pass(render_pass* _pass,
      const containers::contiguous_range<const render_pass_attachment>&
          _attachments,
      const containers::contiguous_range<const subpass_description>& _subpasses,
      const containers::contiguous_range<const subpass_dependency>& _deps)
      WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_render_pass(render_pass* _pass) WN_GRAPHICS_OVERRIDE_FINAL;

  void initialize_image_view(image_view* _view, const image* image,
      uint32_t _base_mip_level,
      uint32_t _num_mip_levels) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_image_view(image_view* _view) WN_GRAPHICS_OVERRIDE_FINAL;

  // arena methods
  bool initialize_arena(arena* _arena, const size_t _index, const size_t _size,
      const bool _multisampled) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_arena(arena* _arena) WN_GRAPHICS_OVERRIDE_FINAL;

  void initialize_framebuffer(framebuffer* _framebuffer,
      const framebuffer_create_info& _info) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_framebuffer(
      framebuffer* _framebuffer) WN_GRAPHICS_OVERRIDE_FINAL;

  void initialize_graphics_pipeline(graphics_pipeline* _pipeline,
      const graphics_pipeline_description& _create_info,
      const pipeline_layout* _layout, const render_pass* _renderpass,
      uint32_t _subpass) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_graphics_pipeline(
      graphics_pipeline* _pipeline) WN_GRAPHICS_OVERRIDE_FINAL;

  // buffer methods
  bool initialize_buffer(buffer* _buffer, const size_t _size,
      const resource_states _usage) WN_GRAPHICS_OVERRIDE_FINAL;
  bool bind_buffer(buffer* _buffer, arena* _arena,
      const size_t _offset) WN_GRAPHICS_OVERRIDE_FINAL;
  void* map_buffer(buffer* _buffer) WN_GRAPHICS_OVERRIDE_FINAL;
  void unmap_buffer(buffer* _buffer) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_buffer(buffer* _buffer) WN_GRAPHICS_OVERRIDE_FINAL;
  buffer_memory_requirements get_buffer_memory_requirements(
      const buffer* _buffer) WN_GRAPHICS_OVERRIDE_FINAL;

private:
  template <typename T>
  typename data_type<T>::value& get_data(T* t) const;

  template <typename T>
  typename data_type<const T>::value& get_data(const T* const t) const;

  D3D12_FEATURE_DATA_D3D12_OPTIONS m_options;
  containers::dynamic_array<heap_info> m_heap_info;
  containers::dynamic_array<arena_properties> m_arena_properties;

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
  struct sampler_data {
    uint32_t m_sampler_data_count = 0;
    containers::default_range_partition::token m_range_token = {};
    D3D12_GPU_DESCRIPTOR_HANDLE m_sampler_handle = {0};
  };
  containers::hash_map<sampler_create_info, memory::unique_ptr<sampler_data>,
      sampler_create_info_hasher>
      m_sampler_cache;
  multi_tasking::spin_lock m_sampler_cache_lock;
};

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_RUNTIME_GRAPHICS_INTERNAL_D3D12_DEVICE_H__
