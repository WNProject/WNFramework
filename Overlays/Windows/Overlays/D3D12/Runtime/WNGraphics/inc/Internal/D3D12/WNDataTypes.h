// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_D3D12_DATA_TYPES_H__
#define __WN_GRAPHICS_INTERNAL_D3D12_DATA_TYPES_H__

#include "WNContainers/inc/WNDynamicArray.h"
#include "WNContainers/inc/WNRangePartition.h"
#include "WNGraphics/inc/WNDescriptorData.h"
#include "WNGraphics/inc/WNShaderModule.h"

#include <DXGI1_4.h>
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
      ID3D12DescriptorHeap* _csv_heap,
      Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _sampler_heap)
    : csv_heap_partition(_allocator, _num_csv_nodes),
      sampler_heap_partition(_allocator, _num_sampler_nodes),
      csv_heap_token(core::move(_csv_heap_token)),
      csv_heap(_csv_heap),
      sampler_heap(core::move(_sampler_heap)) {}
  containers::default_range_partition csv_heap_partition;
  containers::default_range_partition sampler_heap_partition;
  containers::default_range_partition::token csv_heap_token;
  // We do not create our own csv heap. We suballocate from the global
  // csv heap (1M entries). Then we suballocate from there. This is
  // supposedly much more efficient on NVIDIA hardware.
  ID3D12DescriptorHeap* csv_heap;
  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> sampler_heap;
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

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_D3D12_DATA_TYPES_H__
