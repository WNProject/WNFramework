// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNGraphics/inc/Internal/Vulkan/WNDevice.h"
#include "WNGraphics/inc/WNHeap.h"
#include "WNLogging/inc/WNLog.h"

namespace wn {
namespace graphics {
namespace internal {
namespace vulkan {

struct buffer_data {
  VkDeviceMemory device_memory;
  VkBuffer buffer;
};

// TODO LIST for when we support multiple queues

// create_upload_heap needs to refer to which queues the uploads will be for
// this may mean switching create_upload_heap to a per-queue heap, or
// we could do something else if we want.

// TODO LIST: When we want to optimize heap types:
//  Only determine the heap type once we know what we are allocating.

device::device(memory::allocator* _allocator, WNLogging::WNLog* _log,
    VkDevice _device, PFN_vkDestroyDevice _destroy_device,
    const VkPhysicalDeviceMemoryProperties* memory_properties)
  : internal::device(_allocator, _log),
    m_device(_device),
    vkDestroyDevice(_destroy_device) {
  static_assert(sizeof(buffer_data) == sizeof(upload_heap::opaque_data),
      "The data is an unexpected size");

  m_upload_memory_type_index = static_cast<uint32_t>(-1);
  for (uint32_t i = 0; i < memory_properties->memoryTypeCount; ++i) {
    if (memory_properties->memoryTypes[i].propertyFlags &
        (VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
      m_upload_memory_type_index = i;
      break;
    }
  }
  WN_DEBUG_ASSERT_DESC(m_upload_memory_type_index != static_cast<uint32_t>(-1),
      "This should be impossbile according to the spec");
}

device::~device() {
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

bool device::initialize(
    vulkan_context* _context, uint32_t graphics_and_device_queue) {
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

#ifdef _WN_DEBUG
  LOAD_VK_DEVICE_SYMBOL(m_device, vkGetBufferMemoryRequirements);
#endif

  vkGetDeviceQueue(m_device, graphics_and_device_queue, 0, &m_queue);
  return true;
}

upload_heap_ptr device::create_upload_heap(size_t _size_in_bytes) {
  // This is a bit convoluted, but we can't use make_unique on a
  // friended class.
  upload_heap* heap_mem =
      static_cast<upload_heap*>(m_allocator->allocate(sizeof(upload_heap)));
  heap_mem = new (heap_mem) upload_heap(this);
  upload_heap_ptr heap(m_allocator, heap_mem);

  buffer_data& res = heap->data_as<buffer_data>();

  VkBufferCreateInfo create_info{
      VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,  // sType
      nullptr,                               // pNext
      0,                                     // flags
      _size_in_bytes,                        // size
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,      // usage
      VK_SHARING_MODE_EXCLUSIVE,             // sharingMode
      0,                                     // queueFamilyIndexCount
      0,                                     // pQueueFamilyIndices
  };
  if (VK_SUCCESS !=
      vkCreateBuffer(m_device, &create_info, nullptr, &res.buffer)) {
    m_log->Log(WNLogging::eError, 0,
        "Could not successfully create upload heap of size ", _size_in_bytes,
        ".");
    return nullptr;
  }

#ifdef _WN_DEBUG
  VkMemoryRequirements requirements;
  vkGetBufferMemoryRequirements(m_device, res.buffer, &requirements);
  WN_DEBUG_ASSERT_DESC(
      requirements.memoryTypeBits & (1 << m_upload_memory_type_index),
      "Unexpected memory requirement. We should have a device type of "
      "VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | "
      "VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT");
#endif

  VkMemoryAllocateInfo allocate_info{
      VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,  // sType
      nullptr,                                 // pNext
      _size_in_bytes,                          // allocationSize
      m_upload_memory_type_index,              // memoryTypeIndex
  };
  if (VK_SUCCESS !=
      vkAllocateMemory(m_device, &allocate_info, nullptr, &res.device_memory)) {
    m_log->Log(WNLogging::eError, 0,
        "Could not successfully allocate device memory of size ",
        _size_in_bytes, ".");
    return nullptr;
  }

  if (VK_SUCCESS !=
      vkBindBufferMemory(m_device, res.buffer, res.device_memory, 0)) {
    m_log->Log(WNLogging::eError, 0, "Could not bind buffer memory ",
        _size_in_bytes, ".");
    return nullptr;
  }
  if (VK_SUCCESS != vkMapMemory(m_device, res.device_memory, 0, VK_WHOLE_SIZE,
                        0, &heap->m_root_address)) {
    m_log->Log(WNLogging::eError, 0, "Could not bind buffer memory ",
        _size_in_bytes, ".");
    return nullptr;
  }
  return core::move(heap);
}

void device::destroy_upload_heap(upload_heap* heap) {
  buffer_data& res = heap->data_as<buffer_data>();
  if (res.device_memory != VK_NULL_HANDLE) {
    vkFreeMemory(m_device, res.device_memory, nullptr);
  }

  if (res.buffer != VK_NULL_HANDLE) {
    vkDestroyBuffer(m_device, res.buffer, nullptr);
  }
}

void device::flush_mapped_range(upload_heap*, size_t, size_t) {}

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace wn
