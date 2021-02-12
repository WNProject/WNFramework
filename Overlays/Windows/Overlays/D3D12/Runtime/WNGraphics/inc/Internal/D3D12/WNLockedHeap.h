// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_RUNTIME_GRAPHICS_INTERNAL_D3D12_LOCKED_HEAP_H__
#define __WN_RUNTIME_GRAPHICS_INTERNAL_D3D12_LOCKED_HEAP_H__

#include "WNContainers/inc/WNRangePartition.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/allocator.h"
#include "WNMultiTasking/inc/spin_lock.h"

#include <d3d12.h>
#include <wrl.h>

namespace wn {
namespace runtime {
namespace graphics {
namespace internal {
namespace d3d12 {

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
    m_root_gpu_handle = m_heap->GetGPUDescriptorHandleForHeapStart();
  }
  using token = containers::default_range_partition::token;

  token get_partition(size_t num_elements) {
    multi_tasking::spin_lock_guard guard(m_lock);
    return m_parition.get_interval(num_elements);
  }

  void release_partition(token t) {
    multi_tasking::spin_lock_guard guard(m_lock);
    t.free();
  }
  ID3D12DescriptorHeap* heap() const {
    return m_heap.Get();
  }

  D3D12_CPU_DESCRIPTOR_HANDLE get_handle_at(size_t offset) const {
    return D3D12_CPU_DESCRIPTOR_HANDLE{
        m_root_cpu_handle.ptr + (offset * m_descriptor_size)};
  }

  D3D12_GPU_DESCRIPTOR_HANDLE get_gpu_handle_at(size_t offset) const {
    return D3D12_GPU_DESCRIPTOR_HANDLE{
        m_root_gpu_handle.ptr + (offset * m_descriptor_size)};
  }

private:
  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_heap;
  D3D12_CPU_DESCRIPTOR_HANDLE m_root_cpu_handle;
  D3D12_GPU_DESCRIPTOR_HANDLE m_root_gpu_handle;
  containers::default_range_partition m_parition;
  multi_tasking::spin_lock m_lock;
  uint32_t m_descriptor_size;
};

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_RUNTIME_GRAPHICS_INTERNAL_D3D12_LOCKED_HEAP_H__
