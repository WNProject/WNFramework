// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/Internal/Vulkan/WNDevice.h"
#include "WNGraphics/inc/Internal/Vulkan/WNBufferData.h"
#include "WNGraphics/inc/Internal/Vulkan/WNImageFormats.h"
#include "WNGraphics/inc/Internal/Vulkan/WNResourceStates.h"
#include "WNGraphics/inc/Internal/Vulkan/WNSwapchain.h"
#include "WNGraphics/inc/Internal/Vulkan/WNVulkanImage.h"
#include "WNGraphics/inc/WNCommandAllocator.h"
#include "WNGraphics/inc/WNCommandList.h"
#include "WNGraphics/inc/WNFence.h"
#include "WNGraphics/inc/WNHeap.h"
#include "WNGraphics/inc/WNQueue.h"
#include "WNGraphics/inc/WNSwapchain.h"
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
using vulkan_swapchain_constructable = vulkan_swapchain;
#else
using vulkan_command_list_constructable = command_list;
using vulkan_queue_constructable = queue;
using vulkan_swapchain_constructable = swapchain;
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
    m_log->log_error("Could not find " #symbol ".");                           \
    m_log->log_error("Error configuring device");                              \
    return false;                                                              \
  }                                                                            \
  m_log->log_debug(#symbol " is at ", symbol);

#define LOAD_VK_SUB_DEVICE_SYMBOL(device, sub_struct, symbol)                  \
  sub_struct.symbol =                                                          \
      reinterpret_cast<PFN_##symbol>(vkGetDeviceProcAddr(device, #symbol));    \
  if (!sub_struct.symbol) {                                                    \
    m_log->log_error("Could not find " #symbol ".");                           \
    m_log->log_error("Error configuring device");                              \
    return false;                                                              \
  }                                                                            \
  m_log->log_debug(#symbol " is at ", sub_struct.symbol);

bool vulkan_device::initialize(memory::allocator* _allocator,
    logging::log* _log, VkDevice _device, PFN_vkDestroyDevice _destroy_device,
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
    m_log->log_error("Could not find vkGetDeviceProcAddr.");
    m_log->log_error("Device initialization failed");
    return false;
  }
  m_log->log_debug("vkGetDeviceProcAddr is at ", vkGetDeviceProcAddr);

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

  LOAD_VK_DEVICE_SYMBOL(m_device, vkCreateImage);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkDestroyImage);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkGetImageMemoryRequirements);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkBindImageMemory);

  LOAD_VK_DEVICE_SYMBOL(m_device, vkCreateSwapchainKHR);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkGetSwapchainImagesKHR);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkAcquireNextImageKHR);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkDestroySwapchainKHR);

  LOAD_VK_SUB_DEVICE_SYMBOL(m_device, m_queue_context, vkQueueSubmit);
  LOAD_VK_SUB_DEVICE_SYMBOL(m_device, m_queue_context, vkQueuePresentKHR);

  m_queue_context.m_device = this;

  LOAD_VK_SUB_DEVICE_SYMBOL(
      m_device, m_command_list_context, vkFreeCommandBuffers);
  LOAD_VK_SUB_DEVICE_SYMBOL(
      m_device, m_command_list_context, vkCmdPipelineBarrier);
  LOAD_VK_SUB_DEVICE_SYMBOL(m_device, m_command_list_context, vkCmdCopyBuffer);
  LOAD_VK_SUB_DEVICE_SYMBOL(
      m_device, m_command_list_context, vkEndCommandBuffer);
  LOAD_VK_SUB_DEVICE_SYMBOL(
      m_device, m_command_list_context, vkCmdCopyImageToBuffer);
  LOAD_VK_SUB_DEVICE_SYMBOL(
      m_device, m_command_list_context, vkCmdCopyBufferToImage);

  m_command_list_context.m_device = this;

  VkQueue queue;
  vkGetDeviceQueue(m_device, _graphics_and_device_queue, 0, &queue);
  m_queue.exchange(queue);

  m_surface_helper.initialize(
      _context->instance, _context->vkGetInstanceProcAddr);

  {
    // Create 1-byte upload buffer. This seems to be the only way to
    // figure out what memory types will be required.
    VkBufferCreateInfo create_info = s_upload_buffer;
    VkBuffer test_buffer;
    create_info.size = 1;  // 1 byte buffer
    if (VK_SUCCESS !=
        vkCreateBuffer(m_device, &create_info, nullptr, &test_buffer)) {
      m_log->log_error(
          "Could not determine the memory type for an upload heap.");
      return false;
    }

    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(m_device, test_buffer, &requirements);
    m_upload_memory_type_index = get_memory_type_index(
        requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

    vkDestroyBuffer(m_device, test_buffer, nullptr);
    if (m_upload_memory_type_index == uint32_t(-1)) {
      m_log->log_critical("Error in vulkan driver.");
      m_log->log_critical("Must have at least one buffer type that supports",
          "VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT");
      return false;
    }
    m_log->log_debug(
        "Upload heap using memory_type ", m_upload_memory_type_index);
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
      m_log->log_error(
          "Could not determine the memory type for a download heap.");
      return false;
    }

    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(m_device, test_buffer, &requirements);
    m_download_memory_type_index = get_memory_type_index(
        requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

    vkDestroyBuffer(m_device, test_buffer, nullptr);
    if (m_download_memory_type_index == uint32_t(-1)) {
      m_log->log_critical("Error in vulkan driver.");
      m_log->log_critical("Must have at least one buffer type that supports",
          "VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT");
      return false;
    }
    m_log->log_debug(
        "Download heap using memory_type ", m_download_memory_type_index);
    m_download_heap_is_coherent =
        (m_physical_device_memory_properties
                ->memoryTypes[m_download_memory_type_index]
                .propertyFlags &
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0;
  }
  // TODO(awoloszyn): If we plan on supporting sparse binding
  // then we need test that here as well.
  // TODO(awoloszyn): Same thing with Linear Tiling.
  // Create a 1x1 image so we can figure out the memory type
  {
    VkImage test_image;
    VkImageCreateInfo create_info{
        VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,  // sType
        nullptr,                              // pNext
        0,                                    // flags
        VK_IMAGE_TYPE_2D,                     // imageType
        VK_FORMAT_R8G8B8A8_UNORM,             // format
        VkExtent3D{1, 1, 1},                  // extent
        1,                                    // mipLevels
        1,                                    // arrayLayers
        VK_SAMPLE_COUNT_1_BIT,                // samples
        VK_IMAGE_TILING_OPTIMAL,              // tiling
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,  // usage
        VK_SHARING_MODE_EXCLUSIVE,                // sharingMode
        0,                                        // queueFamilyIndexCount
        nullptr,                                  // pQueueFamilyIndices
        VK_IMAGE_LAYOUT_UNDEFINED                 // initialLayout
    };

    if (VK_SUCCESS !=
        vkCreateImage(m_device, &create_info, nullptr, &test_image)) {
      m_log->log_error(
          "Could not determine the memory type for a download heap.");
      return false;
    }

    VkMemoryRequirements requirements;
    vkGetImageMemoryRequirements(m_device, test_image, &requirements);
    m_image_memory_type_index = get_memory_type_index(
        requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    vkDestroyImage(m_device, test_image, nullptr);
  }
  m_log->log_debug("Upload heap is using ",
      m_upload_heap_is_coherent ? "coherent" : "non-coherent", "memory");

  m_log->log_debug("Download heap is using ",
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
    m_log->log_error("Could not successfully create upload heap of size ",
        _size_in_bytes, ".");

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
    m_log->log_error("Could not successfully allocate device memory of size ",
        _size_in_bytes, ".");
    return;
  }

  if (VK_SUCCESS !=
      vkBindBufferMemory(m_device, res.buffer, res.device_memory, 0)) {
    m_log->log_error("Could not bind buffer memory ", _size_in_bytes, ".");
    return;
  }

  if (VK_SUCCESS != vkMapMemory(m_device, res.device_memory, 0,
                        requirements.size, 0, (void**)&_heap->m_root_address)) {
    m_log->log_error("Could not map buffer memory ", _size_in_bytes, ".");
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

void vulkan_device::initialize_image(
    const image_create_info& _info, image* _image) {
  VulkanImage& img = _image->data_as<VulkanImage>();

  VkImageCreateInfo create_info{
      VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,            // sType
      nullptr,                                        // pNext
      0,                                              // flags
      VK_IMAGE_TYPE_2D,                               // imageType
      image_format_to_vulkan_format(_info.m_format),  // format
      VkExtent3D{static_cast<uint32_t>(_info.m_width),
          static_cast<uint32_t>(_info.m_height), 1},  // extent
      1,                                              // mipLevels
      1,                                              // arrayLayers
      VK_SAMPLE_COUNT_1_BIT,                          // samples
      VK_IMAGE_TILING_OPTIMAL,                        // tiling
      VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
          VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,  // usage
      VK_SHARING_MODE_EXCLUSIVE,                // sharingMode
      0,                                        // queueFamilyIndexCount
      nullptr,                                  // pQueueFamilyIndices
      VK_IMAGE_LAYOUT_UNDEFINED                 // initialLayout
  };

  if (VK_SUCCESS !=
      vkCreateImage(m_device, &create_info, nullptr, &img.image)) {
    m_log->log_error("Could not create image.");
  }

  VkMemoryRequirements requirements;
  vkGetImageMemoryRequirements(m_device, img.image, &requirements);

  WN_DEBUG_ASSERT_DESC(
      (requirements.memoryTypeBits & (1 << (m_image_memory_type_index))) != 0,
      "Unexpected memory type");

  VkMemoryAllocateInfo allocate_info{
      VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,  // sType
      nullptr,                                 // pNext
      requirements.size,                       // allocationSize
      m_image_memory_type_index,               // memoryTypeIndex
  };

  if (VK_SUCCESS !=
      vkAllocateMemory(m_device, &allocate_info, nullptr, &img.device_memory)) {
    m_log->log_error("Could not successfully allocate device memory of size ",
        requirements.size, ".");
    return;
  }

  vkBindImageMemory(m_device, img.image, img.device_memory, 0);

  _image->m_resource_info.depth = 1;
  _image->m_resource_info.height = _info.m_height;
  _image->m_resource_info.width = _info.m_width;
  _image->m_resource_info.offset_in_bytes = 0;
  _image->m_resource_info.row_pitch_in_bytes =
      _info.m_width *
      4;  // TODO(awoloszyn): Optimize this, and make it dependent on image type
  _image->m_resource_info.total_memory_required =
      _image->m_resource_info.row_pitch_in_bytes *
      _info.m_height;  // TODO(awoloszyn): Make this dependent on image type
  _image->m_resource_info.format = _info.m_format;
}

// TODO(awoloszyn): As far as I can tell, Vulkan
// has no alignment requirements for buffers. Should probably
// double check this to make sure
size_t vulkan_device::get_image_upload_buffer_alignment() {
  return 32;  // Texel Size, maximum right now is 4*32.
}

size_t vulkan_device::get_buffer_upload_buffer_alignment() {
  return 1;
}

void vulkan_device::destroy_image(image* _image) {
  VulkanImage& img = _image->data_as<VulkanImage>();
  vkFreeMemory(m_device, img.device_memory, nullptr);
  vkDestroyImage(m_device, img.image, nullptr);
}

swapchain_ptr vulkan_device::create_swapchain(
    const swapchain_create_info& _info, queue*,
    runtime::window::window* _window) {
  VkSurfaceKHR surface;
  if (VK_SUCCESS != m_surface_helper.create_surface(_window, &surface)) {
    m_log->log_error("Could not create surface on window");
    return nullptr;
  }

  VkPresentModeKHR mode;
  switch (_info.mode) {
    case swap_mode::immediate:
      mode = VK_PRESENT_MODE_IMMEDIATE_KHR;
      break;
    case swap_mode::mailbox:
      mode = VK_PRESENT_MODE_MAILBOX_KHR;
      break;
    case swap_mode::fifo:
      mode = VK_PRESENT_MODE_FIFO_KHR;
      break;
    case swap_mode::fifo_relaxed:
      mode = VK_PRESENT_MODE_FIFO_RELAXED_KHR;
      break;
    default:
      WN_DEBUG_ASSERT_DESC(false, "Should never get here");
      mode = VK_PRESENT_MODE_FIFO_RELAXED_KHR;
  }

  VkSwapchainCreateInfoKHR create_info = {
      VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,  // sType
      nullptr,                                      // pNext
      0,                                            // flags
      surface,                                      // surface
      _info.num_buffers,                            // minImageCount
      image_format_to_vulkan_format(_info.format),  // format
      VK_COLORSPACE_SRGB_NONLINEAR_KHR,             // colorspace
      {
          _window->get_width(), _window->get_height(),
      },                                    // extent
      1,                                    // layers
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,  // usage
      VK_SHARING_MODE_EXCLUSIVE,
      0, nullptr, VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
      VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, mode, VK_FALSE, VK_NULL_HANDLE};

  VkSwapchainKHR swapchain;
  if (VK_SUCCESS !=
      vkCreateSwapchainKHR(m_device, &create_info, nullptr, &swapchain)) {
    m_surface_helper.destroy_surface(surface);
    m_log->log_error("Could not create swapchain");
    return nullptr;
  }

  uint32_t num_images;
  vkGetSwapchainImagesKHR(m_device, swapchain, &num_images, nullptr);

  swapchain_create_info info = _info;
  info.num_buffers = num_images;

  memory::unique_ptr<vulkan_swapchain_constructable> swp =
      memory::make_unique_delegated<vulkan_swapchain_constructable>(
          m_allocator, [](void* _memory) {
            return new (_memory) vulkan_swapchain_constructable();
          });
  swp->set_create_info(info);
  swp->initialize(m_allocator, this, _window->get_width(),
      _window->get_height(), info, swapchain, surface);
  return core::move(swp);
}

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace wn
