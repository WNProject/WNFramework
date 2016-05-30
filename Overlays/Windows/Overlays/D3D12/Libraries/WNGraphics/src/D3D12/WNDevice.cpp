// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNGraphics/inc/Internal/D3D12/WNCommandList.h"
#include "WNGraphics/inc/Internal/D3D12/WNDevice.h"
#include "WNGraphics/inc/Internal/D3D12/WNFenceData.h"
#include "WNGraphics/inc/Internal/D3D12/WNImageFormats.h"
#include "WNGraphics/inc/Internal/D3D12/WNResourceStates.h"
#include "WNGraphics/inc/WNCommandAllocator.h"
#include "WNGraphics/inc/WNFence.h"
#include "WNGraphics/inc/WNHeap.h"
#include "WNGraphics/inc/WNHeapTraits.h"
#include "WNGraphics/inc/WNImage.h"
#include "WNLogging/inc/WNLog.h"

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/Internal/D3D12/WNCommandList.h"
#include "WNGraphics/inc/Internal/D3D12/WNQueue.h"
#else
#include "WNGraphics/inc/WNCommandList.h"
#include "WNGraphics/inc/WNQueue.h"
#endif

namespace wn {
namespace graphics {
namespace internal {
namespace d3d12 {

static_assert(
    sizeof(upload_heap) >= (sizeof(Microsoft::WRL::ComPtr<ID3D12Resource>) +
                               sizeof(void*) + sizeof(void*)),
    "the data is an unexpected size");

namespace {

const D3D12_HEAP_PROPERTIES s_upload_heap_props = {
    D3D12_HEAP_TYPE_UPLOAD,           // Type
    D3D12_CPU_PAGE_PROPERTY_UNKNOWN,  // CPUPageProperty
    D3D12_MEMORY_POOL_UNKNOWN,        // MemoryPoolPreference
    0,                                // CreationNodeMask
    0                                 // VisibleNodeMask
};

const D3D12_HEAP_PROPERTIES s_download_heap_props = {
    D3D12_HEAP_TYPE_READBACK,         // Type
    D3D12_CPU_PAGE_PROPERTY_UNKNOWN,  // CPUPageProperty
    D3D12_MEMORY_POOL_UNKNOWN,        // MemoryPoolPreference
    0,                                // CreationNodeMask
    0                                 // VisibleNodeMask
};

const D3D12_HEAP_PROPERTIES s_default_heap_props = {
    D3D12_HEAP_TYPE_DEFAULT,          // Type
    D3D12_CPU_PAGE_PROPERTY_UNKNOWN,  // CPUPageProperty
    D3D12_MEMORY_POOL_UNKNOWN,        // MemoryPoolPreference
    0,                                // CreationNodeMask
    0                                 // VisibleNodeMask
};

const D3D12_COMMAND_QUEUE_DESC s_command_queue_props = {
    D3D12_COMMAND_LIST_TYPE_DIRECT,     // Type
    D3D12_COMMAND_QUEUE_PRIORITY_HIGH,  // Priority
    D3D12_COMMAND_QUEUE_FLAG_NONE,      // Flags
    0                                   // NodeMask
};

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
using d3d12_command_list_constructable = d3d12_command_list;
using d3d12_queue_constructable = d3d12_queue;
#else
using d3d12_command_list_constructable = command_list;
using d3d12_queue_constructable = queue;
#endif

}  // anonymous namespace

template <typename HeapType>
void d3d12_device::initialize_heap(HeapType* _heap, size_t _num_bytes,
    const D3D12_HEAP_PROPERTIES& _params,
    const D3D12_RESOURCE_STATES& _states) {
  WN_DEBUG_ASSERT_DESC(
      _num_bytes >= 1, "Upload heaps must be at least one byte");

  Microsoft::WRL::ComPtr<ID3D12Resource>& resource =
      _heap->data_as<Microsoft::WRL::ComPtr<ID3D12Resource>>();

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
          &heap_desc, _states, nullptr, __uuidof(ID3D12Resource), &resource);

  if (FAILED(hr)) {
    m_log->Log(WNLogging::eError, 0,
        "Could not successfully create upload heap of size ", _num_bytes, ".");
  }
}

template <typename heap_type>
void d3d12_device::destroy_typed_heap(heap_type* _heap) {
  Microsoft::WRL::ComPtr<ID3D12Resource>& resource =
      _heap->data_as<Microsoft::WRL::ComPtr<ID3D12Resource>>();

  resource.Reset();
}

void d3d12_device::initialize_upload_heap(
    upload_heap* _upload_heap, const size_t _num_bytes) {
  initialize_heap(_upload_heap, _num_bytes, s_upload_heap_props,
      D3D12_RESOURCE_STATE_GENERIC_READ);
}

void d3d12_device::initialize_download_heap(
    download_heap* _download_heap, const size_t _num_bytes) {
  initialize_heap(_download_heap, _num_bytes, s_download_heap_props,
      D3D12_RESOURCE_STATE_COPY_DEST);
}

void d3d12_device::destroy_heap(upload_heap* _heap) {
  return destroy_typed_heap(_heap);
}

void d3d12_device::destroy_heap(download_heap* _heap) {
  return destroy_typed_heap(_heap);
}

uint8_t* d3d12_device::acquire_range(
    upload_heap* _heap, size_t _offset, size_t _size_in_bytes) {
  Microsoft::WRL::ComPtr<ID3D12Resource>& resource =
      _heap->data_as<Microsoft::WRL::ComPtr<ID3D12Resource>>();
  uint8_t* addr = 0;
  D3D12_RANGE range{_offset, _offset + _size_in_bytes};
  HRESULT hr = resource->Map(0, &range, (void**)&addr);
  (void)hr;
  WN_DEBUG_ASSERT_DESC(!FAILED(hr), "Mapping failed");
  return addr + _offset;
}

uint8_t* d3d12_device::synchronize(
    upload_heap* _heap, size_t _offset, size_t _size_in_bytes) {
  Microsoft::WRL::ComPtr<ID3D12Resource>& resource =
      _heap->data_as<Microsoft::WRL::ComPtr<ID3D12Resource>>();
  uint8_t* addr = 0;
  D3D12_RANGE range{_offset, _offset + _size_in_bytes};
  resource->Unmap(0, &range);
  HRESULT hr = resource->Map(0, &range, (void**)&addr);
  (void)hr;
  WN_DEBUG_ASSERT_DESC(!FAILED(hr), "Synchronizing Failed");
  return addr;
}

uint8_t* d3d12_device::acquire_range(
    download_heap* _heap, size_t _offset, size_t _size_in_bytes) {
  Microsoft::WRL::ComPtr<ID3D12Resource>& resource =
      _heap->data_as<Microsoft::WRL::ComPtr<ID3D12Resource>>();
  uint8_t* addr = 0;
  D3D12_RANGE range{_offset, _offset + _size_in_bytes};
  const HRESULT hr = resource->Map(0, &range, (void**)&addr);
  (void)hr;
  WN_DEBUG_ASSERT_DESC(!FAILED(hr), "Mapping failed");
  return addr + _offset;
}

uint8_t* d3d12_device::synchronize(
    download_heap* _heap, size_t _offset, size_t _size_in_bytes) {
  Microsoft::WRL::ComPtr<ID3D12Resource>& resource =
      _heap->data_as<Microsoft::WRL::ComPtr<ID3D12Resource>>();
  uint8_t* addr = 0;
  D3D12_RANGE range{_offset, _offset + _size_in_bytes};
  resource->Unmap(0, nullptr);  // We simply want to reduce the number of maps.
  // We can't actually synchronize in the unmap direction
  // for our download heap.
  const HRESULT hr = resource->Map(0, &range, (void**)&addr);
  (void)hr;
  WN_DEBUG_ASSERT_DESC(!FAILED(hr), "Synchronizing Failed");
  return addr + _offset;
}

void d3d12_device::release_range(
    upload_heap* _heap, size_t _offset, size_t _size_in_bytes) {
  Microsoft::WRL::ComPtr<ID3D12Resource>& resource =
      _heap->data_as<Microsoft::WRL::ComPtr<ID3D12Resource>>();
  D3D12_RANGE range{_offset, _offset + _size_in_bytes};

  resource->Unmap(0, &range);
}

void d3d12_device::release_range(download_heap* _heap, size_t, size_t) {
  Microsoft::WRL::ComPtr<ID3D12Resource>& resource =
      _heap->data_as<Microsoft::WRL::ComPtr<ID3D12Resource>>();

  resource->Unmap(0, nullptr);
}

// queue methods

queue_ptr d3d12_device::create_queue() {
  uint32_t expected_created = 0;

  if (!m_num_queues.compare_exchange_strong(expected_created, 1)) {
    return nullptr;
  }

  Microsoft::WRL::ComPtr<ID3D12CommandQueue> command_queue;
  const HRESULT hr = m_device->CreateCommandQueue(
      &s_command_queue_props, __uuidof(ID3D12CommandQueue), &command_queue);

  WN_DEBUG_ASSERT_DESC(SUCCEEDED(hr), "Cannot create command queue");

#ifndef _WN_DEBUG
  WN_UNUSED_ARGUMENT(hr);
#endif

  memory::unique_ptr<d3d12_queue_constructable> ptr(
      memory::make_unique_delegated<d3d12_queue_constructable>(
          m_allocator, [](void* _memory) {
            return new (_memory) d3d12_queue_constructable();
          }));

  if (ptr) {
    ptr->initialize(this, core::move(command_queue));
  }

  return core::move(ptr);
}

void d3d12_device::destroy_queue(queue*) {
  uint32_t expected_created = 1;

  m_num_queues.compare_exchange_strong(expected_created, 0);
}

// command allocator methods

void d3d12_device::initialize_command_allocator(
    command_allocator* _command_allocator) {
  Microsoft::WRL::ComPtr<ID3D12CommandAllocator>& data =
      _command_allocator
          ->data_as<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>>();
  const HRESULT hr = m_device->CreateCommandAllocator(
      D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), &data);

  WN_DEBUG_ASSERT_DESC(SUCCEEDED(hr), "Cannot create command allocator");

#ifndef _WN_DEBUG
  WN_UNUSED_ARGUMENT(hr);
#endif
}

void d3d12_device::destroy_command_allocator(
    command_allocator* _command_allocator) {
  Microsoft::WRL::ComPtr<ID3D12CommandAllocator>& data =
      _command_allocator
          ->data_as<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>>();

  data.Reset();
}

// command list methods

command_list_ptr d3d12_device::create_command_list(command_allocator* _alloc) {
  Microsoft::WRL::ComPtr<ID3D12CommandAllocator>& pool =
      _alloc->data_as<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>>();
  Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> command_list;
  const HRESULT hr =
      m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pool.Get(),
          nullptr, __uuidof(ID3D12GraphicsCommandList), &command_list);

  WN_DEBUG_ASSERT_DESC(SUCCEEDED(hr), "Cannot create command list");

#ifndef _WN_DEBUG
  WN_UNUSED_ARGUMENT(hr);
#endif

  memory::unique_ptr<d3d12_command_list_constructable> ptr(
      memory::make_unique_delegated<d3d12_command_list_constructable>(
          m_allocator, [](void* _memory) {
            return new (_memory) d3d12_command_list_constructable();
          }));

  if (ptr) {
    ptr->initialize(core::move(command_list));
  }

  return core::move(ptr);
}

// fence methods

void d3d12_device::initialize_fence(fence* _fence) {
  fence_data& data = _fence->data_as<fence_data>();
  const HRESULT hr = m_device->CreateFence(
      0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), &data.fence);

  WN_DEBUG_ASSERT_DESC(SUCCEEDED(hr), "Cannot create fence");

#ifndef _WN_DEBUG
  WN_UNUSED_ARGUMENT(hr);
#endif

  data.event = ::CreateEventW(NULL, TRUE, FALSE, NULL);

  WN_DEBUG_ASSERT_DESC(data.event != nullptr, "Cannot create event for fence");

  data.fence->SetEventOnCompletion(1, data.event.value());
}

void d3d12_device::destroy_fence(fence* _fence) {
  fence_data& data = _fence->data_as<fence_data>();

  data.fence.Reset();
  data.event.dispose();
}

void d3d12_device::wait_fence(const fence* _fence) const {
  const fence_data& data = _fence->data_as<fence_data>();

  ::WaitForSingleObject(data.event.value(), INFINITE);
}

void d3d12_device::reset_fence(fence* _fence) {
  fence_data& data = _fence->data_as<fence_data>();
  const BOOL result = ::ResetEvent(data.event.value());

  WN_DEBUG_ASSERT_DESC(result, "Failed to reset fence");

#ifndef _WN_DEBUG
  WN_UNUSED_ARGUMENT(result);
#endif

  const HRESULT hr = data.fence->Signal(0);

  WN_DEBUG_ASSERT_DESC(SUCCEEDED(hr), "Failed to reset fence");

#ifndef _WN_DEBUG
  WN_UNUSED_ARGUMENT(hr);
#endif
}

void d3d12_device::initialize_image(
    const image_create_info& _info, image* _image) {
  Microsoft::WRL::ComPtr<ID3D12Resource>& resource =
      _image->data_as<Microsoft::WRL::ComPtr<ID3D12Resource>>();

  D3D12_RESOURCE_DESC texture_desc = {
      D3D12_RESOURCE_DIMENSION_TEXTURE2D,           // Dimension
      0,                                            // Alignment
      static_cast<UINT>(_info.m_width),             // Width
      static_cast<UINT>(_info.m_height),            // Height
      1,                                            // DepthOrArraySize
      1,                                            // MipLevels
      image_format_to_dxgi_format(_info.m_format),  // Format
      {
          1,                         // Count
          0,                         // Quality
      },                             // SampleDesc
      D3D12_TEXTURE_LAYOUT_UNKNOWN,  // Layout
      D3D12_RESOURCE_FLAG_NONE       // Flags
  };

  const HRESULT hr = m_device->CreateCommittedResource(&s_default_heap_props,
      D3D12_HEAP_FLAG_NONE, &texture_desc,
      D3D12_RESOURCE_STATE_COMMON, nullptr,
      __uuidof(ID3D12Resource), &resource);

  if (FAILED(hr)) {
    m_log->Log(WNLogging::eError, 0,
        "Could not successfully create texture of size (", _info.m_width, "x",
        _info.m_height, ").");
  }

  uint64_t required_size = 0;
  D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout;
  uint32_t num_rows;
  uint64_t row_sizes_in_bytes;

  m_device->GetCopyableFootprints(&texture_desc, 0, 1, 0, &layout, &num_rows,
      &row_sizes_in_bytes, &required_size);

  _image->m_resource_info.depth = 1;
  _image->m_resource_info.height = _info.m_height;
  _image->m_resource_info.width = _info.m_width;
  _image->m_resource_info.offset_in_bytes = static_cast<size_t>(layout.Offset);
  _image->m_resource_info.row_pitch_in_bytes =
      static_cast<size_t>(layout.Footprint.RowPitch);
  _image->m_resource_info.total_memory_required =
      static_cast<size_t>(required_size);
  _image->m_resource_info.format = _info.m_format;
}

// TODO(awoloszyn): D3D12 guarantees that 64k will work
// for everything but MSAA textures (which require 4MB).
// There are some conditions under which that number
// can be smaller (down to 512 bytes). Figure out what those are,
// and have this function be more dynamic.
size_t d3d12_device::get_image_upload_buffer_alignment() {
  return 64 * 1024;
}

size_t d3d12_device::get_buffer_upload_buffer_alignment() {
  return 64 * 1024;
}

void d3d12_device::destroy_image(image* _image) {
  Microsoft::WRL::ComPtr<ID3D12Resource>& resource =
      _image->data_as<Microsoft::WRL::ComPtr<ID3D12Resource>>();
  resource.Reset();
}

}  // namespace d3d12
}  // namesapce internal
}  // namespace graphics
}  // namespace wn
