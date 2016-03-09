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
namespace {
static const VkBufferCreateInfo s_upload_buffer{
    VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,  // sType
    nullptr,                               // pNext
    0,                                     // flags
    0,                                     // size
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,      // usage
    VK_SHARING_MODE_EXCLUSIVE,             // sharingMode
    0,                                     // queueFamilyIndexCount
    0,                                     // pQueueFamilyIndices
};
}  // anonymous namespace

// TODO LIST for when we support multiple queues

// create_upload_heap needs to refer to which queues the uploads will be for
// this may mean switching create_upload_heap to a per-queue heap, or
// we could do something else if we want.

// TODO LIST: When we want to optimize heap types:
//  Only determine the heap type once we know what we are allocating.

device::device(memory::allocator* _allocator, WNLogging::WNLog* _log,
    VkDevice _device, PFN_vkDestroyDevice _destroy_device,
    const VkPhysicalDeviceMemoryProperties* _memory_properties)
  : internal::device(_allocator, _log),
    vkDestroyDevice(_destroy_device),
    m_device(_device),
    m_queue(VK_NULL_HANDLE),
    m_upload_memory_type_index(uint32_t(-1)),
    m_upload_heap_is_coherent(false),
    m_physical_device_memory_properties(_memory_properties) {
  static_assert(sizeof(buffer_data) == sizeof(upload_heap::opaque_data),
      "The data is an unexpected size");

  m_upload_memory_type_index = static_cast<uint32_t>(-1);
  for (uint32_t i = 0; i < m_physical_device_memory_properties->memoryTypeCount;
       ++i) {
    if (m_physical_device_memory_properties->memoryTypes[i].propertyFlags &
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
  LOAD_VK_DEVICE_SYMBOL(m_device, vkFlushMappedMemoryRanges);

  LOAD_VK_DEVICE_SYMBOL(m_device, vkGetBufferMemoryRequirements);

  vkGetDeviceQueue(m_device, graphics_and_device_queue, 0, &m_queue);

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
  m_upload_heap_is_coherent = (m_physical_device_memory_properties
                                      ->memoryTypes[m_upload_memory_type_index]
                                      .propertyFlags &
                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0;

  m_log->Log(WNLogging::eDebug, 0, "Upload heap is using ",
      m_upload_heap_is_coherent ? "coherent" : "non-coherent", "memory");
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
  VkBufferCreateInfo create_info = s_upload_buffer;
  create_info.size = _size_in_bytes;
  if (VK_SUCCESS !=
      vkCreateBuffer(m_device, &create_info, nullptr, &res.buffer)) {
    m_log->Log(WNLogging::eError, 0,
        "Could not successfully create upload heap of size ", _size_in_bytes,
        ".");
    return nullptr;
  }

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

void device::destroy_upload_heap(upload_heap* _heap) {
  buffer_data& res = _heap->data_as<buffer_data>();
  if (res.device_memory != VK_NULL_HANDLE) {
    vkFreeMemory(m_device, res.device_memory, nullptr);
  }

  if (res.buffer != VK_NULL_HANDLE) {
    vkDestroyBuffer(m_device, res.buffer, nullptr);
  }
}

void device::flush_mapped_range(
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

uint32_t device::get_memory_type_index(
    uint32_t types, VkFlags properties) const {
  uint32_t i = 0;
  do {
    if (0 != (types & 0x1)) {
      if ((properties &
              m_physical_device_memory_properties->memoryTypes[i]
                  .propertyFlags) == properties) {
        return i;
      }
    }
    ++i;
  } while (types >>= 1);
  return uint32_t(-1);
}

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace wn
