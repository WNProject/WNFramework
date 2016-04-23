// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNGraphics/inc/Internal/Vulkan/WNBufferData.h"
#include "WNGraphics/inc/Internal/Vulkan/WNDevice.h"
#include "WNGraphics/inc/WNCommandAllocator.h"
#include "WNGraphics/inc/WNCommandList.h"
#include "WNGraphics/inc/WNFence.h"
#include "WNGraphics/inc/WNHeap.h"
#include "WNGraphics/inc/WNQueue.h"
#include "WNLogging/inc/WNLog.h"

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/Internal/Vulkan/WNCommandList.h"
#include "WNGraphics/inc/Internal/Vulkan/WNQueue.h"
#else
#include "WNGraphics/inc/WNCommandList.h"
#include "WNGraphics/inc/WNQueue.h"
#endif

namespace wn {
namespace graphics {
namespace internal {
namespace vulkan {

namespace {

static const VkBufferCreateInfo s_upload_buffer{
    VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,  // sType
    nullptr,                               // pNext
    0,                                     // flags
    0,                                     // size
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,      // usage
    VK_SHARING_MODE_EXCLUSIVE,             // sharingMode
    0,                                     // queueFamilyIndexCount
    0                                      // pQueueFamilyIndices
};

static const VkBufferCreateInfo s_download_buffer{
    VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,  // sType
    nullptr,                               // pNext
    0,                                     // flags
    0,                                     // size
    VK_BUFFER_USAGE_TRANSFER_DST_BIT,      // usage
    VK_SHARING_MODE_EXCLUSIVE,             // sharingMode
    0,                                     // queueFamilyIndexCount
    0                                      // pQueueFamilyIndices
};

static const VkCommandPoolCreateInfo s_command_pool_create{
    VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,  // sType
    nullptr,                                     // pNext
    0,                                           // flags
    0                                            // queueFamilyIndex
};

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
using vulkan_command_list_constructable = vulkan_command_list;
using vulkan_queue_constructable = vulkan_queue;
#else
using vulkan_command_list_constructable = command_list;
using vulkan_queue_constructable = queue;
#endif

}  // anonymous namespace

// TODO LIST for when we support multiple queues

// create_upload_heap needs to refer to which queues the uploads will be for
// this may mean switching create_upload_heap to a per-queue heap, or
// we could do something else if we want.

// TODO LIST: When we want to optimize heap types:
//  Only determine the heap type once we know what we are allocating.

vulkan_device::~vulkan_device() {
  vkDestroyDevice(m_device, nullptr);
}

#define LOAD_VK_DEVICE_SYMBOL(device, symbol)                                  \
  symbol =                                                                     \
      reinterpret_cast<PFN_##symbol>(vkGetDeviceProcAddr(device, #symbol));    \
  if (!symbol) {                                                               \
    m_log->Log(WNLogging::eError, 0, "Could not find " #symbol ".");           \
    m_log->Log(WNLogging::eError, 0, "Error configuring device");              \
    return false;                                                              \
  }                                                                            \
  m_log->Log(WNLogging::eDebug, 0, #symbol " is at ", symbol);

#define LOAD_VK_SUB_DEVICE_SYMBOL(device, sub_struct, symbol)                  \
  sub_struct.symbol =                                                          \
      reinterpret_cast<PFN_##symbol>(vkGetDeviceProcAddr(device, #symbol));    \
  if (!sub_struct.symbol) {                                                    \
    m_log->Log(WNLogging::eError, 0, "Could not find " #symbol ".");           \
    m_log->Log(WNLogging::eError, 0, "Error configuring device");              \
    return false;                                                              \
  }                                                                            \
  m_log->Log(WNLogging::eDebug, 0, #symbol " is at ", sub_struct.symbol);

bool vulkan_device::initialize(memory::allocator* _allocator,
    WNLogging::WNLog* _log, VkDevice _device,
    PFN_vkDestroyDevice _destroy_device,
    const VkPhysicalDeviceMemoryProperties* _memory_properties,
    vulkan_context* _context, uint32_t _graphics_and_device_queue) {
  static_assert(sizeof(buffer_data) == sizeof(upload_heap::opaque_data),
      "the data is an unexpected size");

  m_allocator = _allocator;
  m_log = _log;
  vkDestroyDevice = _destroy_device;
  m_device = _device;
  m_queue = VK_NULL_HANDLE;
  m_upload_memory_type_index = uint32_t(-1);
  m_upload_heap_is_coherent = false;
  m_download_memory_type_index = uint32_t(-1);
  m_download_heap_is_coherent = false;
  m_physical_device_memory_properties = _memory_properties;

  vkGetDeviceProcAddr =
      reinterpret_cast<PFN_vkGetDeviceProcAddr>(_context->vkGetInstanceProcAddr(
          _context->instance, "vkGetDeviceProcAddr"));
  if (!vkGetDeviceProcAddr) {
    m_log->Log(WNLogging::eError, 0, "Could not find vkGetDeviceProcAddr.");
    m_log->Log(WNLogging::eError, 0, "Device initialization failed");
    return false;
  }
  m_log->Log(
      WNLogging::eDebug, 0, "vkGetDeviceProcAddr is at ", vkGetDeviceProcAddr);

  LOAD_VK_DEVICE_SYMBOL(m_device, vkGetDeviceQueue);

  LOAD_VK_DEVICE_SYMBOL(m_device, vkAllocateMemory);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkFreeMemory);

  LOAD_VK_DEVICE_SYMBOL(m_device, vkCreateBuffer);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkDestroyBuffer);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkBindBufferMemory);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkMapMemory);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkUnmapMemory);

  LOAD_VK_DEVICE_SYMBOL(m_device, vkFlushMappedMemoryRanges);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkInvalidateMappedMemoryRanges);

  LOAD_VK_DEVICE_SYMBOL(m_device, vkGetBufferMemoryRequirements);

  LOAD_VK_DEVICE_SYMBOL(m_device, vkCreateFence);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkDestroyFence);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkWaitForFences);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkResetFences);

  LOAD_VK_DEVICE_SYMBOL(m_device, vkCreateCommandPool);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkDestroyCommandPool);

  LOAD_VK_DEVICE_SYMBOL(m_device, vkAllocateCommandBuffers);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkFreeCommandBuffers);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkBeginCommandBuffer);

  LOAD_VK_SUB_DEVICE_SYMBOL(m_device, m_queue_context, vkQueueSubmit);

  m_queue_context.m_device = this;

  LOAD_VK_SUB_DEVICE_SYMBOL(
      m_device, m_command_list_context, vkFreeCommandBuffers);
  LOAD_VK_SUB_DEVICE_SYMBOL(
      m_device, m_command_list_context, vkCmdPipelineBarrier);
  LOAD_VK_SUB_DEVICE_SYMBOL(m_device, m_command_list_context, vkCmdCopyBuffer);
  LOAD_VK_SUB_DEVICE_SYMBOL(
      m_device, m_command_list_context, vkEndCommandBuffer);

  m_command_list_context.m_device = this;

  VkQueue queue;
  vkGetDeviceQueue(m_device, _graphics_and_device_queue, 0, &queue);
  m_queue.exchange(queue);

  {
    // Create 1-byte upload buffer. This seems to be the only way to
    // figure out what memory types will be required.
    VkBufferCreateInfo create_info = s_upload_buffer;
    VkBuffer test_buffer;
    create_info.size = 1;  // 1 byte buffer
    if (VK_SUCCESS !=
        vkCreateBuffer(m_device, &create_info, nullptr, &test_buffer)) {
      m_log->Log(WNLogging::eError, 0,
          "Could not determine the memory type for an upload heap.");
      return false;
    }

    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(m_device, test_buffer, &requirements);
    m_upload_memory_type_index = get_memory_type_index(
        requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

    vkDestroyBuffer(m_device, test_buffer, nullptr);
    if (m_upload_memory_type_index == uint32_t(-1)) {
      m_log->Log(WNLogging::eCritical, 0, "Error in vulkan driver.");
      m_log->Log(WNLogging::eCritical, 0,
          "Must have at least one buffer type that supports",
          "VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT");
      return false;
    }
    m_log->Log(WNLogging::eDebug, 0, "Upload heap using memory_type ",
        m_upload_memory_type_index);
    m_upload_heap_is_coherent =
        (m_physical_device_memory_properties
                ->memoryTypes[m_upload_memory_type_index]
                .propertyFlags &
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0;
  }
  {
    // Create 1-byte download buffer. This seems to be the only way to
    // figure out what memory types will be required.
    VkBufferCreateInfo create_info = s_download_buffer;
    VkBuffer test_buffer;
    create_info.size = 1;  // 1 byte buffer
    if (VK_SUCCESS !=
        vkCreateBuffer(m_device, &create_info, nullptr, &test_buffer)) {
      m_log->Log(WNLogging::eError, 0,
          "Could not determine the memory type for a download heap.");
      return false;
    }

    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(m_device, test_buffer, &requirements);
    m_download_memory_type_index = get_memory_type_index(
        requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

    vkDestroyBuffer(m_device, test_buffer, nullptr);
    if (m_download_memory_type_index == uint32_t(-1)) {
      m_log->Log(WNLogging::eCritical, 0, "Error in vulkan driver.");
      m_log->Log(WNLogging::eCritical, 0,
          "Must have at least one buffer type that supports",
          "VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT");
      return false;
    }
    m_log->Log(WNLogging::eDebug, 0, "Download heap using memory_type ",
        m_download_memory_type_index);
    m_download_heap_is_coherent =
        (m_physical_device_memory_properties
                ->memoryTypes[m_download_memory_type_index]
                .propertyFlags &
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0;
  }

  m_log->Log(WNLogging::eDebug, 0, "Upload heap is using ",
      m_upload_heap_is_coherent ? "coherent" : "non-coherent", "memory");

  m_log->Log(WNLogging::eDebug, 0, "Download heap is using ",
      m_download_heap_is_coherent ? "coherent" : "non-coherent", "memory");
  return true;
}

template <typename HeapType>
void vulkan_device::initialize_heap(HeapType* _heap,
    const size_t _size_in_bytes, const VkBufferCreateInfo& _info,
    uint32_t _memory_type_index) {
  buffer_data& res = _heap->template data_as<buffer_data>();
  VkBufferCreateInfo create_info = _info;
  create_info.size = _size_in_bytes;
  if (VK_SUCCESS !=
      vkCreateBuffer(m_device, &create_info, nullptr, &res.buffer)) {
    m_log->Log(WNLogging::eError, 0,
        "Could not successfully create upload heap of size ", _size_in_bytes,
        ".");

    return;
  }

  VkMemoryRequirements requirements;
  vkGetBufferMemoryRequirements(m_device, res.buffer, &requirements);

  WN_DEBUG_ASSERT_DESC(
      (requirements.memoryTypeBits & (1 << (_memory_type_index))) != 0,
      "Unexpected memory type");

  VkMemoryAllocateInfo allocate_info{
      VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,  // sType
      nullptr,                                 // pNext
      requirements.size,                       // allocationSize
      _memory_type_index,                      // memoryTypeIndex
  };

  if (VK_SUCCESS !=
      vkAllocateMemory(m_device, &allocate_info, nullptr, &res.device_memory)) {
    m_log->Log(WNLogging::eError, 0,
        "Could not successfully allocate device memory of size ",
        _size_in_bytes, ".");
    return;
  }

  if (VK_SUCCESS !=
      vkBindBufferMemory(m_device, res.buffer, res.device_memory, 0)) {
    m_log->Log(WNLogging::eError, 0, "Could not bind buffer memory ",
        _size_in_bytes, ".");
    return;
  }

  if (VK_SUCCESS != vkMapMemory(m_device, res.device_memory, 0,
                        requirements.size, 0, (void**)&_heap->m_root_address)) {
    m_log->Log(WNLogging::eError, 0, "Could not map buffer memory ",
        _size_in_bytes, ".");
  }
}

void vulkan_device::initialize_upload_heap(
    upload_heap* _upload_heap, const size_t _size_in_bytes) {
  initialize_heap(_upload_heap, _size_in_bytes, s_upload_buffer,
      m_upload_memory_type_index);
}

void vulkan_device::initialize_download_heap(
    download_heap* _download_heap, const size_t _size_in_bytes) {
  initialize_heap(_download_heap, _size_in_bytes, s_download_buffer,
      m_download_memory_type_index);
}

template <typename HeapType>
void WN_FORCE_INLINE vulkan_device::destroy_typed_heap(HeapType* _heap) {
  buffer_data& res = _heap->template data_as<buffer_data>();
  if (res.device_memory != VK_NULL_HANDLE) {
    vkFreeMemory(m_device, res.device_memory, nullptr);
  }

  if (res.buffer != VK_NULL_HANDLE) {
    vkDestroyBuffer(m_device, res.buffer, nullptr);
  }
}

void vulkan_device::destroy_heap(upload_heap* _heap) {
  return destroy_typed_heap(_heap);
}

void vulkan_device::destroy_heap(download_heap* _heap) {
  return destroy_typed_heap(_heap);
}

void vulkan_device::release_range(
    upload_heap* _heap, size_t _offset_in_bytes, size_t _num_bytes) {
  // If our upload heap was coherent, then we do not have to ever
  // call flush on it.
  if (m_upload_heap_is_coherent) {
    return;
  }

  buffer_data& res = _heap->data_as<buffer_data>();

  VkMappedMemoryRange range{
      VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,  // sType
      nullptr,                                // pNext
      res.device_memory,                      // memory
      _offset_in_bytes,                       // offset
      _num_bytes                              // size
  };

  vkFlushMappedMemoryRanges(m_device, 1, &range);
}

void vulkan_device::release_range(download_heap*, size_t, size_t) {}

uint32_t vulkan_device::get_memory_type_index(
    uint32_t _types, VkFlags _properties) const {
  uint32_t i = 0;
  do {
    if (0 != (_types & 0x1)) {
      if ((_properties &
              m_physical_device_memory_properties->memoryTypes[i]
                  .propertyFlags) == _properties) {
        return i;
      }
    }
    ++i;
  } while (_types >>= 1);
  return uint32_t(-1);
}

uint8_t* vulkan_device::acquire_range(
    upload_heap* _heap, size_t _offset, size_t) {
  return _heap->m_root_address + _offset;
}

uint8_t* vulkan_device::synchronize(
    upload_heap* _heap, size_t _offset, size_t _num_bytes) {
  if (!m_upload_heap_is_coherent) {
    buffer_data& res = _heap->data_as<buffer_data>();

    VkMappedMemoryRange range{
        VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,  // sType
        nullptr,                                // pNext
        res.device_memory,                      // memory
        _offset,                                // offset
        _num_bytes                              // size
    };

    vkFlushMappedMemoryRanges(m_device, 1, &range);
  }
  return _heap->m_root_address + _offset;
}

uint8_t* vulkan_device::acquire_range(
    download_heap* _heap, size_t _offset, size_t _num_bytes) {
  // If our upload heap was coherent, then we do not have to ever
  // call flush on it.
  if (!m_download_heap_is_coherent) {
    buffer_data& res = _heap->data_as<buffer_data>();

    VkMappedMemoryRange range{
        VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,  // sType
        nullptr,                                // pNext
        res.device_memory,                      // memory
        _offset,                                // offset
        _num_bytes                              // size
    };

    vkInvalidateMappedMemoryRanges(m_device, 1, &range);
  }
  return _heap->m_root_address + _offset;
}

uint8_t* vulkan_device::synchronize(
    download_heap* _heap, size_t _offset, size_t _num_bytes) {
  return acquire_range(_heap, _offset, _num_bytes);
}

queue_ptr vulkan_device::create_queue() {
  VkQueue q = nullptr;

  q = m_queue.exchange(q);

  if (!q) {
    return nullptr;
  }

  memory::unique_ptr<vulkan_queue_constructable> ptr(
      memory::make_unique_delegated<vulkan_queue_constructable>(
          m_allocator, [](void* _memory) {
            return new (_memory) vulkan_queue_constructable();
          }));

  if (ptr) {
    ptr->initialize(this, &m_queue_context, q);
  }

  return core::move(ptr);
}

void vulkan_device::destroy_queue(queue* _queue) {
  vulkan_queue_constructable* queue =
      reinterpret_cast<vulkan_queue_constructable*>(_queue);

  m_queue.exchange(queue->m_queue);
}

const static VkFenceCreateInfo s_create_fence{
    VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,  // sType
    nullptr,                              // pNext
    0,                                    // flags
};

void vulkan_device::initialize_fence(fence* _fence) {
  VkFence& data = _fence->data_as<VkFence>();

  VkResult hr = vkCreateFence(m_device, &s_create_fence, nullptr, &data);
  (void)hr;
  WN_DEBUG_ASSERT_DESC(hr == VK_SUCCESS, "Cannot create fence");
}

void vulkan_device::destroy_fence(fence* _fence) {
  VkFence& data = _fence->data_as<VkFence>();
  vkDestroyFence(m_device, data, nullptr);
}

void vulkan_device::wait_fence(const fence* _fence) const {
  const VkFence& data = _fence->data_as<VkFence>();
  while (VK_TIMEOUT ==
         vkWaitForFences(m_device, 1, &data, false, static_cast<uint32_t>(-1)))
    ;
}

void vulkan_device::reset_fence(fence* _fence) {
  VkFence& data = _fence->data_as<VkFence>();
  vkResetFences(m_device, 1, &data);
}

void vulkan_device::initialize_command_allocator(
    command_allocator* _command_allocator) {
  VkCommandPool& pool = _command_allocator->data_as<VkCommandPool>();

  vkCreateCommandPool(m_device, &s_command_pool_create, nullptr, &pool);
}

void vulkan_device::destroy_command_allocator(command_allocator* _alloc) {
  VkCommandPool& pool = _alloc->data_as<VkCommandPool>();
  vkDestroyCommandPool(m_device, pool, nullptr);
}

command_list_ptr vulkan_device::create_command_list(command_allocator* _alloc) {
  VkCommandPool& pool = _alloc->data_as<VkCommandPool>();

  VkCommandBufferAllocateInfo allocation{
      VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,  // sType
      nullptr,                                         // pNext
      pool,                                            // commandPool
      VK_COMMAND_BUFFER_LEVEL_PRIMARY,                 // level
      1                                                // commandBufferCount
  };
  VkCommandBuffer buffer;
  vkAllocateCommandBuffers(m_device, &allocation, &buffer);

  VkCommandBufferBeginInfo begin_info{
      VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,  // sType
      nullptr,                                      // pNext
      0,                                            // flags
      nullptr,                                      // pInheritanceInfo
  };

  vkBeginCommandBuffer(buffer, &begin_info);

  memory::unique_ptr<vulkan_command_list_constructable> ptr(
      memory::make_unique_delegated<vulkan_command_list_constructable>(
          m_allocator, [](void* _memory) {
            return new (_memory) vulkan_command_list_constructable();
          }));

  if (ptr) {
    ptr->initialize(buffer, pool, &m_command_list_context);
  }

  return core::move(ptr);
}

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace wn
