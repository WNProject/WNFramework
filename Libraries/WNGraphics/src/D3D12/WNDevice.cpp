// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNGraphics/inc/Internal/D3D12/WNCommandList.h"
#include "WNGraphics/inc/Internal/D3D12/WNDevice.h"
#include "WNGraphics/inc/Internal/D3D12/WNFenceData.h"
#include "WNGraphics/inc/Internal/D3D12/WNQueue.h"
#include "WNGraphics/inc/WNCommandAllocator.h"
#include "WNGraphics/inc/WNFence.h"
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

namespace {

const D3D12_HEAP_PROPERTIES s_upload_heap_props = {
    D3D12_HEAP_TYPE_UPLOAD,           // Type
    D3D12_CPU_PAGE_PROPERTY_UNKNOWN,  // CPUPageProperty
    D3D12_MEMORY_POOL_UNKNOWN,        // MemoryPoolPreference
    0,                                // CreationNodeMask
    0,                                // VisibleNodeMask
};

const D3D12_HEAP_PROPERTIES s_download_heap_props = {
    D3D12_HEAP_TYPE_READBACK,         // Type
    D3D12_CPU_PAGE_PROPERTY_UNKNOWN,  // CPUPageProperty
    D3D12_MEMORY_POOL_UNKNOWN,        // MemoryPoolPreference
    0,                                // CreationNodeMask
    0,                                // VisibleNodeMask
};

const D3D12_COMMAND_QUEUE_DESC s_command_queue_props = {
    D3D12_COMMAND_LIST_TYPE_DIRECT,     // Type
    D3D12_COMMAND_QUEUE_PRIORITY_HIGH,  // Priority
    D3D12_COMMAND_QUEUE_FLAG_NONE,      // Flags
    0};

}  // anonymous namespace

template <typename heap_type>
heap_type d3d12_device::create_heap(size_t _num_bytes,
    const D3D12_HEAP_PROPERTIES& _params,
    const D3D12_RESOURCE_STATES& _states) {
  WN_DEBUG_ASSERT_DESC(
      _num_bytes >= 1, "Upload heaps must be at least one byte");
  heap_type heap(this);

  Microsoft::WRL::ComPtr<ID3D12Resource>& res =
      heap.data_as<Microsoft::WRL::ComPtr<ID3D12Resource>>();

  D3D12_RESOURCE_DESC heap_desc{
      D3D12_RESOURCE_DIMENSION_BUFFER,  // Dimension
      0,                                // Alignment
      _num_bytes,                       // Width
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

  const HRESULT hr =
      m_device->CreateCommittedResource(&_params, D3D12_HEAP_FLAG_NONE,
          &heap_desc, _states, nullptr, __uuidof(ID3D12Resource), &res);

  if (FAILED(hr)) {
    m_log->Log(WNLogging::eError, 0,
        "Could not successfully create upload heap of size ", _num_bytes, ".");
    return heap_type(nullptr);
  }

  return core::move(heap);
}

template <typename heap_type>
void d3d12_device::destroy_typed_heap(heap_type* _heap) {
  Microsoft::WRL::ComPtr<ID3D12Resource>& res =
      _heap->data_as<Microsoft::WRL::ComPtr<ID3D12Resource>>();
  res.Reset();
}

upload_heap d3d12_device::create_upload_heap(size_t _num_bytes) {
  // TODO(awoloszyn): Try and minimize D3D12_RESOURCE_STATE_GENERIC_READ
  return create_heap<upload_heap>(
      _num_bytes, s_upload_heap_props, D3D12_RESOURCE_STATE_GENERIC_READ);
}

download_heap d3d12_device::create_download_heap(size_t _num_bytes) {
  return create_heap<download_heap>(
      _num_bytes, s_download_heap_props, D3D12_RESOURCE_STATE_COPY_DEST);
}

void d3d12_device::destroy_heap(upload_heap* _heap) {
  return destroy_typed_heap(_heap);
}

void d3d12_device::destroy_heap(download_heap* _heap) {
  return destroy_typed_heap(_heap);
}

uint8_t* d3d12_device::acquire_range(
    upload_heap* _heap, size_t _offset, size_t _size_in_bytes) {
  Microsoft::WRL::ComPtr<ID3D12Resource>& res =
      _heap->data_as<Microsoft::WRL::ComPtr<ID3D12Resource>>();
  uint8_t* addr = 0;
  D3D12_RANGE range{_offset, _size_in_bytes};
  HRESULT hr = res->Map(0, &range, (void**)&addr);
  (void)hr;
  WN_DEBUG_ASSERT_DESC(!FAILED(hr), "Mapping failed");
  return addr;
}

uint8_t* d3d12_device::synchronize(
    upload_heap* _heap, size_t _offset, size_t _size_in_bytes) {
  Microsoft::WRL::ComPtr<ID3D12Resource>& res =
      _heap->data_as<Microsoft::WRL::ComPtr<ID3D12Resource>>();
  uint8_t* addr = 0;
  D3D12_RANGE range{_offset, _size_in_bytes};
  res->Unmap(0, &range);
  HRESULT hr = res->Map(0, &range, (void**)&addr);
  (void)hr;
  WN_DEBUG_ASSERT_DESC(!FAILED(hr), "Synchronizing Failed");
  return addr;
}

uint8_t* d3d12_device::acquire_range(
    download_heap* _heap, size_t _offset, size_t _size_in_bytes) {
  Microsoft::WRL::ComPtr<ID3D12Resource>& res =
      _heap->data_as<Microsoft::WRL::ComPtr<ID3D12Resource>>();
  uint8_t* addr = 0;
  D3D12_RANGE range{_offset, _size_in_bytes};
  const HRESULT hr = res->Map(0, &range, (void**)&addr);
  (void)hr;
  WN_DEBUG_ASSERT_DESC(!FAILED(hr), "Mapping failed");
  return addr;
}

uint8_t* d3d12_device::synchronize(
    download_heap* _heap, size_t _offset, size_t _size_in_bytes) {
  Microsoft::WRL::ComPtr<ID3D12Resource>& res =
      _heap->data_as<Microsoft::WRL::ComPtr<ID3D12Resource>>();
  uint8_t* addr = 0;
  D3D12_RANGE range{_offset, _size_in_bytes};
  res->Unmap(0, nullptr); // We simply want to reduce the number of maps.
                    // We can't actually synchronize in the unmap direction
                    // for our download heap.
  const HRESULT hr = res->Map(0, &range, (void**)&addr);
  (void)hr;
  WN_DEBUG_ASSERT_DESC(!FAILED(hr), "Synchronizing Failed");
  return addr;
}

void d3d12_device::release_range(
    upload_heap* _heap, size_t _offset, size_t _size_in_bytes) {
  Microsoft::WRL::ComPtr<ID3D12Resource>& res =
      _heap->data_as<Microsoft::WRL::ComPtr<ID3D12Resource>>();
  D3D12_RANGE range{_offset, _size_in_bytes};
  res->Unmap(0, &range);
}

void d3d12_device::release_range(
    download_heap* _heap, size_t, size_t) {
  Microsoft::WRL::ComPtr<ID3D12Resource>& res =
      _heap->data_as<Microsoft::WRL::ComPtr<ID3D12Resource>>();
  res->Unmap(0, nullptr);
}

queue_ptr d3d12_device::create_queue() {
  uint32_t expected_created = 0;
  if (!m_num_queues.compare_exchange_strong(expected_created, 1)) {
    return nullptr;
  }

  Microsoft::WRL::ComPtr<ID3D12CommandQueue> res;

  const HRESULT hr = m_device->CreateCommandQueue(
      &s_command_queue_props, __uuidof(ID3D12CommandQueue), &res);

  (void)hr;
  WN_DEBUG_ASSERT_DESC(!FAILED(hr), "Could not create command queue");

  queue_ptr queue = memory::make_unique_delegated<d3d12_queue>(
      m_allocator, [this, &res](void* memory) {
        return new (memory) d3d12_queue(this, core::move(res));
      });

  return core::move(queue);
}

void d3d12_device::destroy_queue(graphics::queue*) {
  uint32_t expected_created = 1;
  m_num_queues.compare_exchange_strong(expected_created, 0);
}

fence d3d12_device::create_fence() {
  fence res(this);

  fence_data& data = res.data_as<fence_data>();

  const HRESULT hr = m_device->CreateFence(
      0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), &data.fence);
  (void)hr;
  WN_DEBUG_ASSERT_DESC(SUCCEEDED(hr), "Cannot create fence");
  data.event = CreateEvent(NULL, TRUE, FALSE, NULL);
  WN_DEBUG_ASSERT_DESC(data.event, "Cannot create event for fence");

  data.fence->SetEventOnCompletion(1, data.event);
  return core::move(res);
}

void d3d12_device::destroy_fence(fence* _fence) {
  fence_data& data = _fence->data_as<fence_data>();
  data.fence.Reset();
  ::CloseHandle(data.event);
}

void d3d12_device::wait_fence(const fence* _fence) const {
  const fence_data& data = _fence->data_as<fence_data>();
  ::WaitForSingleObject(data.event, INFINITE);
}

void d3d12_device::reset_fence(fence* _fence) {
  fence_data& data = _fence->data_as<fence_data>();
  ::ResetEvent(data.event);
  data.fence->Signal(0);
}

command_allocator d3d12_device::create_command_allocator() {
  command_allocator alloc(this);
  Microsoft::WRL::ComPtr<ID3D12CommandAllocator>& pool =
      alloc.data_as<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>>();
  const HRESULT hr = m_device->CreateCommandAllocator(
      D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), &pool);
  (void)hr;
  WN_DEBUG_ASSERT_DESC(SUCCEEDED(hr), "Could not create command allocator");
  return core::move(alloc);
}

void d3d12_device::destroy_command_allocator(command_allocator* _alloc) {
  Microsoft::WRL::ComPtr<ID3D12CommandAllocator>& pool =
      _alloc->data_as<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>>();
  pool.Reset();
}


command_list_ptr d3d12_device::create_command_list(command_allocator* _alloc) {
  Microsoft::WRL::ComPtr<ID3D12CommandAllocator>& pool =
      _alloc->data_as<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>>();

  Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> command_list;

  m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pool.Get(),
      nullptr, __uuidof(ID3D12GraphicsCommandList), &command_list);

  return memory::make_unique_delegated<d3d12_command_list>(
      m_allocator, [this, &command_list](void* memory) {
        return new (memory) d3d12_command_list(core::move(command_list));
      });
}


}  // namespace d3d12
}  // namesapce internal
}  // namespace graphics
}  // namspace wn
