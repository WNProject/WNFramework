// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/Internal/D3D12/WNDevice.h"
#include "WNContainers/inc/WNDeque.h"
#include "WNGraphics/inc/Internal/D3D12/WNCommandList.h"
#include "WNGraphics/inc/Internal/D3D12/WNDataTypes.h"
#include "WNGraphics/inc/Internal/D3D12/WNFenceData.h"
#include "WNGraphics/inc/Internal/D3D12/WNImageFormats.h"
#include "WNGraphics/inc/Internal/D3D12/WNResourceStates.h"
#include "WNGraphics/inc/Internal/D3D12/WNSwapchain.h"
#include "WNGraphics/inc/WNCommandAllocator.h"
#include "WNGraphics/inc/WNDescriptors.h"
#include "WNGraphics/inc/WNFence.h"
#include "WNGraphics/inc/WNHeap.h"
#include "WNGraphics/inc/WNHeapTraits.h"
#include "WNGraphics/inc/WNImage.h"
#include "WNGraphics/inc/WNSwapchain.h"
#include "WNLogging/inc/WNLog.h"

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/Internal/D3D12/WNCommandList.h"
#include "WNGraphics/inc/Internal/D3D12/WNQueue.h"
#else
#include "WNGraphics/inc/WNCommandList.h"
#include "WNGraphics/inc/WNQueue.h"
#endif

#include "WNMath/inc/WNBasic.h"
#include "WNMemory/inc/WNUniquePtr.h"

#include "WNWindow/inc/WNWindow.h"
#include "WNWindow/inc/WNWindowsWindow.h"

#include <DXGI1_4.h>

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
using d3d12_swapchain_constructable = d3d12_swapchain;
#else
using d3d12_command_list_constructable = command_list;
using d3d12_queue_constructable = queue;
using d3d12_swapchain_constructable = swapchain;
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
    m_log->log_error(
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
      D3D12_HEAP_FLAG_NONE, &texture_desc, D3D12_RESOURCE_STATE_COMMON, nullptr,
      __uuidof(ID3D12Resource), &resource);

  if (FAILED(hr)) {
    m_log->log_error("Could not successfully create texture of size (",
        _info.m_width, "x", _info.m_height, ").");
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

swapchain_ptr d3d12_device::create_swapchain(const swapchain_create_info& _info,
    queue* _queue, runtime::window::window* _window) {
  memory::unique_ptr<d3d12_swapchain_constructable> swapchain =
      memory::make_unique_delegated<d3d12_swapchain_constructable>(
          m_allocator, [](void* _memory) {
            return new (_memory) d3d12_swapchain_constructable();
          });
  WN_DEBUG_ASSERT_DESC(
      _info.mode != swap_mode::fifo_relaxed, "Not Implemented: fifo_relaxed");

  DXGI_SWAP_EFFECT swap_effect;
  bool discard = _info.discard == discard_policy::discard;
  swap_effect = discard ? DXGI_SWAP_EFFECT_FLIP_DISCARD
                        : DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

  WN_RELEASE_ASSERT_DESC(
      _window->type() == runtime::window::window_type::system,
      "Unsupported: Null-windows");
  const runtime::window::native_handle* handle =
      reinterpret_cast<const runtime::window::native_handle*>(
          _window->get_native_handle());

  DXGI_SWAP_CHAIN_DESC1 swapchain_desc = {_window->get_width(),
      _window->get_height(), image_format_to_dxgi_format(_info.format), false,
      {1, 0},                           // SampleDesc
      DXGI_USAGE_RENDER_TARGET_OUTPUT,  // Usage
      _info.num_buffers, DXGI_SCALING_STRETCH, swap_effect,
      DXGI_ALPHA_MODE_IGNORE};
  swapchain->set_create_info(_info);

  d3d12_queue* q = reinterpret_cast<d3d12_queue*>(_queue);

  Microsoft::WRL::ComPtr<IDXGISwapChain1> swap;

  const HRESULT hr = m_factory->CreateSwapChainForHwnd(q->m_queue.Get(),
      handle->handle, &swapchain_desc, nullptr, nullptr, &swap);

  if (FAILED(hr)) {
    m_log->log_error("Could not successfully create swapchain.");
    return nullptr;
  }

  Microsoft::WRL::ComPtr<IDXGISwapChain3> swp3;
  if (0 != swap.As(&swp3)) {
    m_log->log_error("Could not successfully create swapchain.");
    return nullptr;
  }
  swapchain->initialize(m_allocator, this, _window->get_width(),
      _window->get_height(), _info, core::move(swp3));
  return core::move(swapchain);
}

void d3d12_device::initialize_shader_module(shader_module* _module,
    const containers::contiguous_range<const uint8_t>& bytes) {
  auto& data = get_data(_module);
  data = core::move(memory::make_unique<shader_module_data>(m_allocator));
  data->m_shader = containers::dynamic_array<uint8_t>(
      m_allocator, bytes.data(), bytes.data() + bytes.size());
}

void d3d12_device::destroy_shader_module(shader_module* _module) {
  auto& data = get_data(_module);
  data.reset();
}

void d3d12_device::initialize_descriptor_set_layout(
    descriptor_set_layout* _layout,
    const containers::contiguous_range<const descriptor_binding_info>&
        _binding_infos) {
  auto& data = get_data(_layout);
  data =
      memory::make_unique<containers::dynamic_array<descriptor_binding_info>>(
          m_allocator, m_allocator, _binding_infos.data(),
          _binding_infos.size());
}

void d3d12_device::destroy_descriptor_set_layout(
    descriptor_set_layout* _layout) {
  auto& data = get_data(_layout);
  data.reset();
}

void d3d12_device::initialize_descriptor_pool(descriptor_pool* _pool,
    const containers::contiguous_range<const descriptor_pool_create_info>&
        _pool_data) {
  size_t num_csv_types = 0;
  size_t num_sampler_types = 0;
  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> sampler_heap;
  containers::default_range_partition::token csv_heap_token;

  for (const descriptor_pool_create_info& data : _pool_data) {
    switch (data.type) {
      case descriptor_type::sampler: {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {
            D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,         // type
            static_cast<UINT>(data.max_descriptors),    // count
            D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,  // flags
            0,                                          // nodemask
        };
        num_sampler_types += data.max_descriptors;
        HRESULT hr = m_device->CreateDescriptorHeap(
            &desc, __uuidof(ID3D12DescriptorHeap), &sampler_heap);
        (void)hr;
        WN_DEBUG_ASSERT_DESC(
            !FAILED(hr), "Failed initializing the descriptor pool");
      } break;
      case descriptor_type::sampled_image: {
        num_csv_types += data.max_descriptors;
      } break;
      case descriptor_type::read_only_buffer: {
        num_csv_types += data.max_descriptors;
      } break;
      case descriptor_type::read_only_image_buffer: {
        num_csv_types += data.max_descriptors;
      } break;
      case descriptor_type::read_only_sampled_buffer: {
        num_csv_types += data.max_descriptors;
      } break;
      case descriptor_type::read_write_buffer: {
        num_csv_types += data.max_descriptors;
      } break;
      case descriptor_type::read_write_image_buffer: {
        num_csv_types += data.max_descriptors;
      } break;
      case descriptor_type::read_write_sampled_buffer: {
        num_csv_types += data.max_descriptors;
      } break;
    }
  }

  if (num_csv_types > 0) {
    // We try not to lock ever inside the graphics devices, but
    // we make an exception here, as the optimization for Nvidia hardware
    // requires only a single heap. We only have to lock around the creation
    // of the pool, not the creation of individual descriptors, since the
    // pools are externally synchronized.
    {
      multi_tasking::spin_lock_guard guard(m_csv_heap_lock);
      csv_heap_token = m_csv_partition.get_interval(num_csv_types);
    }
    WN_DEBUG_ASSERT_DESC(
        csv_heap_token.is_valid(), "Could not find enough space for csv heap");
  }

  memory::unique_ptr<descriptor_pool_data>& dat = get_data(_pool);
  dat = memory::make_unique<descriptor_pool_data>(m_allocator, m_allocator,
      num_csv_types, num_sampler_types, core::move(csv_heap_token),
      m_root_csv_heap.Get(), core::move(sampler_heap));
}

void d3d12_device::destroy_descriptor_pool(descriptor_pool* _pool) {
  memory::unique_ptr<descriptor_pool_data> dat = core::move(get_data(_pool));
  if (dat->csv_heap_partition.size() > 0) {
    multi_tasking::spin_lock_guard guard(m_csv_heap_lock);
    dat->csv_heap_token = containers::default_range_partition::token();
  }
}

void d3d12_device::initialize_descriptor_set(descriptor_set* _set,
    descriptor_pool* _pool, const descriptor_set_layout* _layout) {
  auto& pool = get_data(_pool);
  const auto& layout = get_data(_layout);
  memory::unique_ptr<descriptor_set_data>& set = get_data(_set);
  set = memory::make_unique<descriptor_set_data>(
      m_allocator, m_allocator, pool.get());
  containers::default_range_partition::token tok;
  for (auto& l : (*layout)) {
    switch (l.type) {
      case descriptor_type::sampler:
        tok = pool->sampler_heap_partition.get_interval(l.array_size);
        break;
      case descriptor_type::sampled_image:
      case descriptor_type::read_only_buffer:
      case descriptor_type::read_only_image_buffer:
      case descriptor_type::read_only_sampled_buffer:
      case descriptor_type::read_write_buffer:
      case descriptor_type::read_write_image_buffer:
      case descriptor_type::read_write_sampled_buffer:
        tok = pool->csv_heap_partition.get_interval(l.array_size);
        break;
      default:
        WN_DEBUG_ASSERT_DESC(false, "Should not end up here");
    }
    descriptor_data dat = {l.type, core::move(tok)};
    set->descriptors.push_back(core::move(dat));
  }
}

void d3d12_device::destroy_descriptor_set(descriptor_set* _set) {
  auto& set = get_data(_set);
  set.reset();
}

void d3d12_device::initialize_pipeline_layout(pipeline_layout* _layout,
    const containers::contiguous_range<const descriptor_set_layout*>&
        _descriptor_set_layouts) {
  auto& layout = get_data(_layout);
  containers::dynamic_array<D3D12_ROOT_PARAMETER> parameters(m_allocator);

  D3D12_ROOT_SIGNATURE_DESC desc = {
      0,
      nullptr,                         // parameters go here
      0,                               // NumStaticSamplers
      nullptr,                         // pStaticSamplers
      D3D12_ROOT_SIGNATURE_FLAG_NONE,  // flags
      // TODO(awoloszyn): Set up the DENY flags as well as
      // stream output (if we want?)
  };

  containers::deque<D3D12_DESCRIPTOR_RANGE> ranges(m_allocator);

  for (size_t i = 0; i < _descriptor_set_layouts.size(); ++i) {
    const auto& set = (_descriptor_set_layouts)[i];
    const auto& set_layout = get_data(set);
    for (size_t j = 0; j < set_layout->size(); ++j) {
      parameters.push_back({});
      auto& parameter = parameters.back();
      parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
      parameter.DescriptorTable.NumDescriptorRanges = 1;
      ranges.push_back({});
      parameter.DescriptorTable.pDescriptorRanges = &ranges.back();

      auto& range = ranges.back();
      const auto& descriptor = (*set_layout)[j];

      if (math::popcount(descriptor.shader_stages) > 1) {
        parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
      }

      switch (descriptor.shader_stages) {
        case static_cast<uint32_t>(shader_stage::vertex):
          parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
          break;
        case static_cast<uint32_t>(shader_stage::pixel):
          parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
          break;
        case static_cast<uint32_t>(shader_stage::tessellation_control):
          parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_HULL;
          break;
        case static_cast<uint32_t>(shader_stage::tessellation_evaluation):
          parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_DOMAIN;
          break;
        case static_cast<uint32_t>(shader_stage::geometry):
          parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_GEOMETRY;
          break;
        case static_cast<uint32_t>(shader_stage::compute):
        default:
          parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
      }

      range.NumDescriptors = static_cast<UINT>(descriptor.array_size);
      range.BaseShaderRegister = static_cast<UINT>(descriptor.register_base);
      range.RegisterSpace = 0;  // TODO(awoloszyn) Expose this?
      range.OffsetInDescriptorsFromTableStart = static_cast<UINT>(j);

      switch (descriptor.type) {
        case descriptor_type::sampler:
          range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
          break;
        case descriptor_type::read_only_buffer:
          range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
          break;
        case descriptor_type::sampled_image:
        case descriptor_type::read_only_image_buffer:
        case descriptor_type::read_only_sampled_buffer:
          range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
          break;
        case descriptor_type::read_write_buffer:
        case descriptor_type::read_write_image_buffer:
        case descriptor_type::read_write_sampled_buffer:
          range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
          break;
        default:
          m_log->log_error("Invalid descriptor type");
          break;
      }
    }
  }
  desc.pParameters = parameters.data();
  desc.NumParameters = static_cast<UINT>(parameters.size());

  Microsoft::WRL::ComPtr<ID3DBlob> serialized_sig;
  Microsoft::WRL::ComPtr<ID3DBlob> error;
  HRESULT hr = D3D12SerializeRootSignature(
      &desc, D3D_ROOT_SIGNATURE_VERSION_1, &serialized_sig, &error);
  if (FAILED(hr)) {
    m_log->log_error("Could not serialize root signature:",
        static_cast<char*>(error->GetBufferPointer()));
    return;
  }

  hr = m_device->CreateRootSignature(0, serialized_sig->GetBufferPointer(),
      serialized_sig->GetBufferSize(), __uuidof(ID3D12RootSignature), &layout);

  if (FAILED(hr)) {
    m_log->log_error("Error constructing the root signature.");
  }
}

void d3d12_device::destroy_pipeline_layout(pipeline_layout* _layout) {
  auto& layout = get_data(_layout);
  layout.Reset();
}

template <typename T>
typename data_type<T>::value& d3d12_device::get_data(T* t) {
  return t->data_as<typename data_type<T>::value>();
}

template <typename T>
typename data_type<const T>::value& d3d12_device::get_data(const T* const t) {
  return t->data_as<typename data_type<const T>::value>();
}

}  // namespace d3d12
}  // namesapce internal
}  // namespace graphics
}  // namespace wn
