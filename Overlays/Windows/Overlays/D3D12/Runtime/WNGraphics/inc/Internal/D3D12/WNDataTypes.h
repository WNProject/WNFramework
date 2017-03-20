// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_RUNTIME_GRAPHICS_INTERNAL_D3D12_DATA_TYPES_H__
#define __WN_RUNTIME_GRAPHICS_INTERNAL_D3D12_DATA_TYPES_H__

#include "WNContainers/inc/WNDynamicArray.h"
#include "WNContainers/inc/WNRangePartition.h"
#include "WNGraphics/inc/WNArena.h"
#include "WNGraphics/inc/WNDescriptorData.h"
#include "WNGraphics/inc/WNImageView.h"
#include "WNGraphics/inc/WNRenderPassTypes.h"
#include "WNGraphics/inc/WNShaderModule.h"

#include <dxgi1_4.h>
#include <wrl.h>

namespace wn {
namespace graphics {
namespace internal {
namespace d3d12 {

template <typename T>
struct data_type {
  using value = void;
};

struct shader_module_data {
  containers::dynamic_array<uint8_t> m_shader;
};

struct descriptor_pool_data {
  descriptor_pool_data(memory::allocator* _allocator, size_t _num_csv_nodes,
      size_t _num_sampler_nodes,
      containers::default_range_partition::token _csv_heap_token,
      containers::default_range_partition::token _sampler_heap_token)
    : csv_heap_partition(_allocator, _num_csv_nodes),
      sampler_heap_partition(_allocator, _num_sampler_nodes),
      csv_heap_token(core::move(_csv_heap_token)),
      sampler_heap_token(core::move(_sampler_heap_token)) {}
  containers::default_range_partition csv_heap_partition;
  containers::default_range_partition sampler_heap_partition;
  containers::default_range_partition::token csv_heap_token;
  containers::default_range_partition::token sampler_heap_token;
  // We do not create our own csv heap. We suballocate from the global
  // csv heap (1M entries). Then we suballocate from there. This is
  // supposedly much more efficient on NVIDIA hardware.
};

struct descriptor_data {
  descriptor_type type;
  containers::default_range_partition::token offset;
};

struct descriptor_set_data {
  descriptor_set_data(
      memory::allocator* _allocator, descriptor_pool_data* _pool)
    : descriptors(_allocator), pool_data(_pool) {}
  containers::dynamic_array<descriptor_data> descriptors;
  descriptor_pool_data* pool_data;
};

struct render_pass_data {
  render_pass_data(memory::allocator* _allocator) : attachments(_allocator) {}
  containers::dynamic_array<render_pass_attachment> attachments;
};

struct framebuffer_data {
  framebuffer_data(memory::allocator* _allocator,
      containers::contiguous_range<const image_view*> _views)
    : image_views(_allocator), image_view_handles(_allocator) {
    image_views.insert(image_views.begin(), _views.begin(), _views.end());
    image_view_handles.reserve(_views.size());
  }
  containers::dynamic_array<const image_view*> image_views;
  containers::dynamic_array<containers::default_range_partition::token>
      image_view_handles;
};

template <>
struct data_type<shader_module> {
  using value = memory::unique_ptr<internal::d3d12::shader_module_data>;
};
template <>
struct data_type<const shader_module> {
  using value = const memory::unique_ptr<internal::d3d12::shader_module_data>;
};

template <>
struct data_type<descriptor_set_layout> {
  using value =
      memory::unique_ptr<containers::dynamic_array<descriptor_binding_info>>;
};

template <>
struct data_type<const descriptor_set_layout> {
  using value = const memory::unique_ptr<
      const containers::dynamic_array<descriptor_binding_info>>;
};

template <>
struct data_type<descriptor_pool> {
  using value = memory::unique_ptr<internal::d3d12::descriptor_pool_data>;
};
template <>
struct data_type<const descriptor_pool> {
  using value = const memory::unique_ptr<internal::d3d12::descriptor_pool_data>;
};

template <>
struct data_type<descriptor_set> {
  using value = memory::unique_ptr<internal::d3d12::descriptor_set_data>;
};

template <>
struct data_type<const descriptor_set> {
  using value = const memory::unique_ptr<internal::d3d12::descriptor_set_data>;
};

template <>
struct data_type<pipeline_layout> {
  using value = Microsoft::WRL::ComPtr<ID3D12RootSignature>;
};

template <>
struct data_type<const pipeline_layout> {
  using value = const Microsoft::WRL::ComPtr<ID3D12RootSignature>;
};

template <>
struct data_type<render_pass> {
  using value = memory::unique_ptr<render_pass_data>;
};

template <>
struct data_type<const render_pass> {
  using value = memory::unique_ptr<const render_pass_data>;
};

struct image_view_info {
  image::image_buffer_resource_info info;
  const image* image;
};

template <>
struct data_type<image_view> {
  using value = memory::unique_ptr<image_view_info>;
};

template <>
struct data_type<const image_view> {
  using value = const memory::unique_ptr<const image_view_info>;
};

template <>
struct data_type<framebuffer> {
  using value = memory::unique_ptr<framebuffer_data>;
};

template <>
struct data_type<const framebuffer> {
  using value = memory::unique_ptr<const framebuffer_data>;
};

template <>
struct data_type<arena> {
  using value = Microsoft::WRL::ComPtr<ID3D12Heap>;
};

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_RUNTIME_GRAPHICS_INTERNAL_D3D12_DATA_TYPES_H__
