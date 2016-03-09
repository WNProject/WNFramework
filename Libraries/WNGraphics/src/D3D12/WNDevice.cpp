// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNGraphics/inc/Internal/D3D12/WNDevice.h"
#include "WNGraphics/inc/WNHeap.h"
#include "WNLogging/inc/WNLog.h"

namespace wn {
namespace graphics {
namespace internal {
namespace d3d12 {

static_assert(
    sizeof(upload_heap) >= (sizeof(Microsoft::WRL::ComPtr<ID3D12Resource>) +
                               sizeof(void*) + sizeof(void*)),
    "The data is an unexpected size");

const static D3D12_HEAP_PROPERTIES s_upload_heap_props = {
    D3D12_HEAP_TYPE_UPLOAD,           // Type
    D3D12_CPU_PAGE_PROPERTY_UNKNOWN,  // CPUPageProperty
    D3D12_MEMORY_POOL_UNKNOWN,        // MemoryPoolPreference
    0,                                // CreationNodeMask
    0,                                // VisibleNodeMask
};

upload_heap_ptr device::create_upload_heap(size_t num_bytes) {
  WN_DEBUG_ASSERT_DESC(
      num_bytes >= 1, "Upload heaps must be at least one byte");

  // This is a bit convoluted, but we can't use make_unique on a
  // friended class.
  upload_heap* heap_mem =
      static_cast<upload_heap*>(m_allocator->allocate(sizeof(upload_heap)));
  heap_mem = new (heap_mem) upload_heap(this);
  upload_heap_ptr heap(m_allocator, heap_mem);

  Microsoft::WRL::ComPtr<ID3D12Resource>& res =
      heap->data_as<Microsoft::WRL::ComPtr<ID3D12Resource>>();

  D3D12_RESOURCE_DESC heap_desc{
      D3D12_RESOURCE_DIMENSION_BUFFER,  // Dimension
      0,                                // Alignment
      num_bytes,                        // Width
      1,                                // Height
      1,                                // DepthOrArraySize
      1,                                // MipLevels
      DXGI_FORMAT_UNKNOWN,              // Format
      {
          // SampleDesc
          1,  // SampleCount
          0,  // SampleQuality
      },
      D3D12_TEXTURE_LAYOUT_ROW_MAJOR,  // Layout
      D3D12_RESOURCE_FLAG_NONE,        // Flags
  };

  HRESULT hr = m_d3d12_device->CreateCommittedResource(&s_upload_heap_props,
      D3D12_HEAP_FLAG_NONE, &heap_desc, D3D12_RESOURCE_STATE_GENERIC_READ,
      nullptr, __uuidof(ID3D12Resource), &res);
  if (FAILED(hr)) {
    m_log->Log(WNLogging::eError, 0,
        "Could not successfully create upload heap of size ", num_bytes, ".");
    heap.reset();
  } else {
    hr = res->Map(0, nullptr, &heap->m_root_address);
    if (FAILED(hr)) {
      heap.reset();
    }
  }

  return core::move(heap);
}

void device::destroy_upload_heap(upload_heap* _heap) {
  Microsoft::WRL::ComPtr<ID3D12Resource>& res =
      _heap->data_as<Microsoft::WRL::ComPtr<ID3D12Resource>>();
  res.Reset();
}

void device::flush_mapped_range(upload_heap*, size_t, size_t) {}

}  // namespace d3d12
}  // namesapce internal
}  // namespace graphics
}  // namspace wn
