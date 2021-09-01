// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNGraphics/inc/Internal/Vulkan/WNDevice.h"

#include "WNContainers/inc/WNArray.h"
#include "WNContainers/inc/WNDeque.h"
#include "WNGraphics/inc/Internal/Vulkan/WNBufferData.h"
#include "WNGraphics/inc/Internal/Vulkan/WNDataTypes.h"
#include "WNGraphics/inc/Internal/Vulkan/WNImageFormats.h"
#include "WNGraphics/inc/Internal/Vulkan/WNQueueProfiler.h"
#include "WNGraphics/inc/Internal/Vulkan/WNResourceStates.h"
#include "WNGraphics/inc/Internal/Vulkan/WNSwapchain.h"
#include "WNGraphics/inc/WNArena.h"
#include "WNGraphics/inc/WNBuffer.h"
#include "WNGraphics/inc/WNCommandAllocator.h"
#include "WNGraphics/inc/WNCommandList.h"
#include "WNGraphics/inc/WNDescriptors.h"
#include "WNGraphics/inc/WNFence.h"
#include "WNGraphics/inc/WNFramebuffer.h"
#include "WNGraphics/inc/WNGraphicsPipeline.h"
#include "WNGraphics/inc/WNGraphicsPipelineDescription.h"
#include "WNGraphics/inc/WNImageView.h"
#include "WNGraphics/inc/WNQueue.h"
#include "WNGraphics/inc/WNQueueProfiler.h"
#include "WNGraphics/inc/WNRenderPass.h"
#include "WNGraphics/inc/WNShaderModule.h"
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
namespace runtime {
namespace graphics {
namespace internal {
namespace vulkan {

#define get_data(f)                                                            \
  f->data_as<                                                                  \
      typename data_type<core::remove_pointer<decltype(f)>::type>::value>()

namespace {

static const VkCommandPoolCreateInfo s_command_pool_create{
    VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,  // sType
    nullptr,                                     // pNext
    0,                                           // flags
    0};                                          // queueFamilyIndex

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

#define TRY_LOAD_VK_DEVICE_SYMBOL(device, symbol)                              \
  symbol =                                                                     \
      reinterpret_cast<PFN_##symbol>(vkGetDeviceProcAddr(device, #symbol));    \
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

#define TRY_LOAD_VK_SUB_DEVICE_SYMBOL(device, sub_struct, symbol)              \
  sub_struct.symbol =                                                          \
      reinterpret_cast<PFN_##symbol>(vkGetDeviceProcAddr(device, #symbol));    \
  m_log->log_debug(#symbol " is at ", sub_struct.symbol);

bool vulkan_device::initialize(memory::allocator* _allocator,
    logging::log* _log, VkDevice _device, VkPhysicalDevice _phys_dev,
    PFN_vkDestroyDevice _destroy_device,
    const VkPhysicalDeviceMemoryProperties* _memory_properties,
    const VkPhysicalDeviceLimits* _device_limits,
    const adapter_formats* _formats, vulkan_context* _context,
    uint32_t _graphics_and_device_queue, float _timestamp_period) {
  m_allocator = _allocator;
  m_log = _log;
  vkDestroyDevice = _destroy_device;
  m_device = _device;
  m_queue = VK_NULL_HANDLE;
  m_queue_index = _graphics_and_device_queue;
  m_physical_device_memory_properties = _memory_properties;
  m_arena_properties = containers::dynamic_array<arena_properties>(m_allocator);
  m_arena_to_vulkan_index = containers::dynamic_array<uint32_t>(m_allocator);
  m_device_limits = _device_limits;
  m_supported_formats = _formats;
  m_timestamp_period = _timestamp_period;

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

  LOAD_VK_DEVICE_SYMBOL(m_device, vkCreateSemaphore);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkDestroySemaphore);

  LOAD_VK_DEVICE_SYMBOL(m_device, vkCreateCommandPool);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkDestroyCommandPool);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkResetCommandPool);

  LOAD_VK_DEVICE_SYMBOL(m_device, vkAllocateCommandBuffers);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkFreeCommandBuffers);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkBeginCommandBuffer);

  LOAD_VK_DEVICE_SYMBOL(m_device, vkCreateImage);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkDestroyImage);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkGetImageMemoryRequirements);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkBindImageMemory);

  LOAD_VK_DEVICE_SYMBOL(m_device, vkCreateSampler);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkDestroySampler);

  LOAD_VK_DEVICE_SYMBOL(m_device, vkCreateSwapchainKHR);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkGetSwapchainImagesKHR);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkAcquireNextImageKHR);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkDestroySwapchainKHR);

  LOAD_VK_DEVICE_SYMBOL(m_device, vkCreateShaderModule);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkDestroyShaderModule);

  LOAD_VK_DEVICE_SYMBOL(m_device, vkCreateDescriptorPool);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkDestroyDescriptorPool);

  LOAD_VK_DEVICE_SYMBOL(m_device, vkAllocateDescriptorSets);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkFreeDescriptorSets);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkUpdateDescriptorSets);

  LOAD_VK_DEVICE_SYMBOL(m_device, vkCreateDescriptorSetLayout);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkDestroyDescriptorSetLayout);

  LOAD_VK_DEVICE_SYMBOL(m_device, vkCreatePipelineLayout);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkDestroyPipelineLayout);

  LOAD_VK_DEVICE_SYMBOL(m_device, vkCreateRenderPass);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkDestroyRenderPass);

  LOAD_VK_DEVICE_SYMBOL(m_device, vkCreateImageView);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkDestroyImageView);

  LOAD_VK_DEVICE_SYMBOL(m_device, vkCreateFramebuffer);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkDestroyFramebuffer);

  LOAD_VK_DEVICE_SYMBOL(m_device, vkCreateGraphicsPipelines);
  LOAD_VK_DEVICE_SYMBOL(m_device, vkDestroyPipeline);

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
  LOAD_VK_SUB_DEVICE_SYMBOL(m_device, m_command_list_context, vkCmdCopyImage);
  LOAD_VK_SUB_DEVICE_SYMBOL(m_device, m_command_list_context, vkCmdBlitImage);
  LOAD_VK_SUB_DEVICE_SYMBOL(m_device, m_command_list_context, vkCmdDraw);
  LOAD_VK_SUB_DEVICE_SYMBOL(m_device, m_command_list_context, vkCmdDrawIndexed);

  LOAD_VK_SUB_DEVICE_SYMBOL(
      m_device, m_command_list_context, vkCmdBeginRenderPass);
  LOAD_VK_SUB_DEVICE_SYMBOL(
      m_device, m_command_list_context, vkCmdEndRenderPass);
  LOAD_VK_SUB_DEVICE_SYMBOL(
      m_device, m_command_list_context, vkCmdBindDescriptorSets);
  LOAD_VK_SUB_DEVICE_SYMBOL(
      m_device, m_command_list_context, vkCmdPushConstants);

  LOAD_VK_SUB_DEVICE_SYMBOL(
      m_device, m_command_list_context, vkCmdBindPipeline);
  LOAD_VK_SUB_DEVICE_SYMBOL(
      m_device, m_command_list_context, vkCmdBindVertexBuffers);
  LOAD_VK_SUB_DEVICE_SYMBOL(
      m_device, m_command_list_context, vkCmdBindIndexBuffer);

  LOAD_VK_SUB_DEVICE_SYMBOL(m_device, m_command_list_context, vkCmdSetViewport);
  LOAD_VK_SUB_DEVICE_SYMBOL(m_device, m_command_list_context, vkCmdSetScissor);

#ifdef TRACY_ENABLE
  LOAD_VK_SUB_DEVICE_SYMBOL(
      m_device, m_queue_profiler_context, vkBeginCommandBuffer);
  LOAD_VK_SUB_DEVICE_SYMBOL(
      m_device, m_queue_profiler_context, vkResetCommandBuffer);
  LOAD_VK_SUB_DEVICE_SYMBOL(
      m_device, m_queue_profiler_context, vkAllocateCommandBuffers);
  LOAD_VK_SUB_DEVICE_SYMBOL(
      m_device, m_queue_profiler_context, vkFreeCommandBuffers);
  LOAD_VK_SUB_DEVICE_SYMBOL(
      m_device, m_queue_profiler_context, vkCmdResetQueryPool);
  LOAD_VK_SUB_DEVICE_SYMBOL(
      m_device, m_queue_profiler_context, vkCmdWriteTimestamp);
  LOAD_VK_SUB_DEVICE_SYMBOL(
      m_device, m_queue_profiler_context, vkCreateQueryPool);
  LOAD_VK_SUB_DEVICE_SYMBOL(
      m_device, m_queue_profiler_context, vkDestroyQueryPool);
  LOAD_VK_SUB_DEVICE_SYMBOL(
      m_device, m_queue_profiler_context, vkEndCommandBuffer);
  LOAD_VK_SUB_DEVICE_SYMBOL(
      m_device, m_queue_profiler_context, vkGetQueryPoolResults);
  LOAD_VK_SUB_DEVICE_SYMBOL(m_device, m_queue_profiler_context, vkQueueSubmit);
  LOAD_VK_SUB_DEVICE_SYMBOL(
      m_device, m_queue_profiler_context, vkQueueWaitIdle);
  LOAD_VK_SUB_DEVICE_SYMBOL(
      m_device, m_queue_profiler_context, vkCreateCommandPool);
  LOAD_VK_SUB_DEVICE_SYMBOL(
      m_device, m_queue_profiler_context, vkDestroyCommandPool);

  LOAD_VK_SUB_DEVICE_SYMBOL(m_device, m_queue_profiler_context, vkCreateFence);
  LOAD_VK_SUB_DEVICE_SYMBOL(m_device, m_queue_profiler_context, vkDestroyFence);
  LOAD_VK_SUB_DEVICE_SYMBOL(
      m_device, m_queue_profiler_context, vkGetFenceStatus);
  LOAD_VK_SUB_DEVICE_SYMBOL(
      m_device, m_queue_profiler_context, vkWaitForFences);
  LOAD_VK_SUB_DEVICE_SYMBOL(m_device, m_queue_profiler_context, vkResetFences);

  // Optional

  TRY_LOAD_VK_SUB_DEVICE_SYMBOL(
      m_device, m_queue_profiler_context, vkGetCalibratedTimestampsEXT);

  m_queue_profiler_context.vkGetPhysicalDeviceCalibrateableTimeDomainsEXT =
      reinterpret_cast<PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT>(
          _context->vkGetInstanceProcAddr(_context->instance,
              "vkGetPhysicalDeviceCalibrateableTimeDomainsEXT"));
  m_queue_profiler_context.physical_device = _phys_dev;
#else
  (void)_phys_dev;
#endif

  m_command_list_context.m_device = this;

  VkQueue queue;
  vkGetDeviceQueue(m_device, _graphics_and_device_queue, 0, &queue);
  m_queue.exchange(queue);

  return setup_arena_properties();
}

uint32_t vulkan_device::get_memory_type_index(
    uint32_t _types, VkFlags _properties) const {
  uint32_t i = 0;
  do {
    if (0 != (_types & 0x1)) {
      const VkMemoryType& current_memory_types =
          m_physical_device_memory_properties->memoryTypes[i];

      if ((_properties & current_memory_types.propertyFlags) == _properties) {
        return i;
      }
    }
    ++i;
  } while (_types >>= 1);

  return uint32_t(-1);
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
    ptr->initialize(this, m_allocator, &m_queue_context, q, m_queue_index);
  }

  return ptr;
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
  VkFence& data = get_data(_fence);

  VkResult hr = vkCreateFence(m_device, &s_create_fence, nullptr, &data);
  (void)hr;
  WN_DEBUG_ASSERT(hr == VK_SUCCESS, "Cannot create fence");
}

void vulkan_device::destroy_fence(fence* _fence) {
  VkFence& data = get_data(_fence);
  vkDestroyFence(m_device, data, nullptr);
}

void vulkan_device::wait_fence(const fence* _fence) const {
  const VkFence& data = get_data(_fence);
  vkWaitForFences(
      m_device, 1, &data, false, static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF));
}

void vulkan_device::reset_fence(fence* _fence) {
  ::VkFence& data = get_data(_fence);
  vkResetFences(m_device, 1, &data);
}

void vulkan_device::initialize_signal(signal* _signal) {
  VkSemaphoreCreateInfo create_info = {
      VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,  // sType
      nullptr,                                  // nullptr
      0,                                        // flags
  };
  ::VkSemaphore& data = get_data(_signal);
  vkCreateSemaphore(m_device, &create_info, nullptr, &data);
}

void vulkan_device::destroy_signal(signal* _signal) {
  ::VkSemaphore& data = get_data(_signal);
  vkDestroySemaphore(m_device, data, nullptr);
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

void vulkan_device::reset_command_allocator(command_allocator* _alloc) {
  VkCommandPool& pool = _alloc->data_as<VkCommandPool>();
  vkResetCommandPool(m_device, pool, 0);
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
    ptr->initialize(m_allocator, buffer, pool, &m_command_list_context);
  }

  return ptr;
}

void vulkan_device::initialize_image(
    const image_create_info& _info, const clear_value&, image* _image) {
  ::VkImage& img = get_data(_image);

  VkImageCreateInfo create_info{
      VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,            // sType
      nullptr,                                        // pNext
      0,                                              // flags
      VK_IMAGE_TYPE_2D,                               // imageType
      image_format_to_vulkan_format(_info.m_format),  // format
      VkExtent3D{static_cast<uint32_t>(_info.m_width),
          static_cast<uint32_t>(_info.m_height), 1},  // extent
      _info.m_mip_levels,                             // mipLevels
      1,                                              // arrayLayers
      VK_SAMPLE_COUNT_1_BIT,                          // samples
      VK_IMAGE_TILING_OPTIMAL,                        // tiling
      resources_states_to_usage_bits(_info.m_valid_resource_states),  // usage
      VK_SHARING_MODE_EXCLUSIVE,  // sharingMode
      0,                          // queueFamilyIndexCount
      nullptr,                    // pQueueFamilyIndices
      VK_IMAGE_LAYOUT_UNDEFINED   // initialLayout
  };

  if (VK_SUCCESS != vkCreateImage(m_device, &create_info, nullptr, &img)) {
    m_log->log_error("Could not create image.");
  }

  _image->m_resource_info =
      containers::dynamic_array<image::image_buffer_resource_info>(
          m_allocator, _info.m_mip_levels);

  for (size_t i = 0; i < _info.m_mip_levels; ++i) {
    _image->m_resource_info[i].depth = 1;
    _image->m_resource_info[i].height =
        _info.m_height / (static_cast<uint64_t>(1) << static_cast<uint64_t>(i));
    _image->m_resource_info[i].height = _image->m_resource_info[i].height
                                            ? _image->m_resource_info[i].height
                                            : 1;
    _image->m_resource_info[i].width =
        _info.m_width / (static_cast<uint64_t>(1) << static_cast<uint64_t>(i));
    _image->m_resource_info[i].width =
        _image->m_resource_info[i].width ? _image->m_resource_info[i].width : 1;

    size_t width_in_blocks = (_image->m_resource_info[i].width +
                                 (format_block_width(_info.m_format) - 1)) /
                             format_block_width(_info.m_format);
    size_t height_in_blocks = (_image->m_resource_info[i].height +
                                  (format_block_height(_info.m_format) - 1)) /
                              format_block_height(_info.m_format);

    _image->m_resource_info[i].offset_in_bytes = 0;
    _image->m_resource_info[i].row_pitch_in_bytes =
        width_in_blocks *
        format_block_size(
            _info.m_format);  // TODO(awoloszyn): Optimize this, and make it
                              // dependent on image type

    _image->m_resource_info[i].total_memory_required =
        height_in_blocks * width_in_blocks * format_block_size(_info.m_format);
    _image->m_resource_info[i].format = _info.m_format;

    _image->m_resource_info[i].block_height =
        format_block_height(_info.m_format);
    _image->m_resource_info[i].block_width = format_block_width(_info.m_format);
    _image->m_resource_info[i].block_size = format_block_size(_info.m_format);
  }
}

void vulkan_device::bind_image_memory(
    image* _image, arena* _arena, uint64_t _offset) {
  ::VkImage image = get_data(_image);
  arena_data& arena = get_data(_arena);

  if (VK_SUCCESS != vkBindImageMemory(m_device, image, arena.memory, _offset)) {
    m_log->log_error("Can not bind image memory");
  }
}

VkFilter sampler_filter_to_vulkan(sampler_filter _filter) {
  switch (_filter) {
    case sampler_filter::nearest:
      return VK_FILTER_NEAREST;
    case sampler_filter::linear:
      return VK_FILTER_LINEAR;
  }
  return VK_FILTER_NEAREST;
}

VkSamplerMipmapMode sampler_filter_to_vulkan_mip(sampler_filter _filter) {
  switch (_filter) {
    case sampler_filter::nearest:
      return VK_SAMPLER_MIPMAP_MODE_NEAREST;
    case sampler_filter::linear:
      return VK_SAMPLER_MIPMAP_MODE_LINEAR;
  }
  return VK_SAMPLER_MIPMAP_MODE_LINEAR;
}

VkSamplerAddressMode sampler_addressing_to_vulkan(sampler_addressing _address) {
  switch (_address) {
    case sampler_addressing::repeat:
      return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    case sampler_addressing::mirrored_repeat:
      return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    case sampler_addressing::clamp:
      return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    case sampler_addressing::border:
      return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    case sampler_addressing::mirror_clamp:
      return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
  }
  return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
}

VkBorderColor border_color_to_vulkan(border_color _color) {
  switch (_color) {
    case border_color::black_transparent_f32:
      return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    case border_color::black_transparent_uint:
      return VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
    case border_color::black_opaque_f32:
      return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
    case border_color::black_opaque_uint:
      return VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    case border_color::white_opaque_f32:
      return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    case border_color::white_opaque_uint:
      return VK_BORDER_COLOR_INT_OPAQUE_WHITE;
  }
  return VK_BORDER_COLOR_INT_OPAQUE_WHITE;
}

void vulkan_device::initialize_sampler(
    const sampler_create_info& _info, sampler* _sampler) {
  VkSamplerCreateInfo create_info = {
      VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,    // sType
      nullptr,                                  // pNext
      0,                                        // flags
      sampler_filter_to_vulkan(_info.mag),      // magFilter
      sampler_filter_to_vulkan(_info.min),      // minFilter
      sampler_filter_to_vulkan_mip(_info.mip),  // mipFilter
      sampler_addressing_to_vulkan(_info.u),    // addressModeU
      sampler_addressing_to_vulkan(_info.v),    // addressModeV
      sampler_addressing_to_vulkan(_info.w),    // addressMoveW
      _info.lod_bias,                           // mipLodBias
      _info.enable_anisotropy,                  // anisotropyEnable
      _info.max_anisotropy,                     // maxAnisotropy
      _info.enable_comparison,                  // compareEnable
      compare_to_vulkan(_info.comparison),      // compareOp
      _info.min_lod,                            // minLod
      _info.max_lod,                            // maxLod
      border_color_to_vulkan(_info.border),     // borderColor
      VK_FALSE,                                 // unnormalizedCoordinates
  };

  auto& sampler = get_data(_sampler);
  if (VK_SUCCESS !=
      vkCreateSampler(m_device, &create_info, nullptr, &sampler)) {
    m_log->log_error("Could not create sampler");
  }
}

void vulkan_device::destroy_sampler(sampler* _sampler) {
  auto& sampler = get_data(_sampler);
  vkDestroySampler(m_device, sampler, nullptr);
}

image_memory_requirements vulkan_device::get_image_memory_requirements(
    const image* _image) {
  ::VkImage image = get_data(_image);
  VkMemoryRequirements requirements;
  vkGetImageMemoryRequirements(m_device, image, &requirements);
  return image_memory_requirements{static_cast<uint32_t>(requirements.size),
      static_cast<uint32_t>(requirements.alignment)};
}

buffer_memory_requirements vulkan_device::get_buffer_memory_requirements(
    const buffer* _buffer, const resource_states _usage) {
  const memory::unique_ptr<const buffer_info>& buffer = get_data(_buffer);
  VkMemoryRequirements requirements;
  vkGetBufferMemoryRequirements(m_device, buffer->buffer, &requirements);
  if (_usage & (resource_state::host_write | resource_state::host_read)) {
    requirements.alignment =
        requirements.alignment > m_device_limits->nonCoherentAtomSize
            ? requirements.alignment
            : m_device_limits->nonCoherentAtomSize;
  }
  return buffer_memory_requirements{static_cast<uint32_t>(requirements.size),
      static_cast<uint32_t>(requirements.alignment)};
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
  ::VkImage& img = get_data(_image);
  vkDestroyImage(m_device, img, nullptr);
}

swapchain_ptr vulkan_device::create_swapchain(
    surface& _surface, const swapchain_create_info& _info, queue*) {
  VkSurfaceKHR surface = _surface.data_as<VkSurfaceKHR>();
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
      WN_DEBUG_ASSERT(false, "Should never get here");
      mode = VK_PRESENT_MODE_FIFO_RELAXED_KHR;
  }

  surface_capabilities capabilities;
  graphics_error err;
  if (err = _surface.get_surface_capabilities(&capabilities),
      err != graphics_error::ok) {
    m_log->log_error("Could not get swapchain size");
    return nullptr;
  }

  VkSwapchainCreateInfoKHR create_info = {
      VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,  // sType
      nullptr,                                      // pNext
      0,                                            // flags
      surface,                                      // surface
      _info.num_buffers,                            // minImageCount
      image_format_to_vulkan_format(_info.format),  // imageFormat
      VK_COLORSPACE_SRGB_NONLINEAR_KHR,             // imageColorSpace
      {
          // imageExtent
          static_cast<uint32_t>(capabilities.width),   // width
          static_cast<uint32_t>(capabilities.height),  // height
      },
      1,  // imageArrayLayers
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
          VK_IMAGE_USAGE_TRANSFER_DST_BIT,    // imageUsage
      VK_SHARING_MODE_EXCLUSIVE,              // imageSharingMode
      0,                                      // queueFamilyIndexCount
      nullptr,                                // pQueueFamilyIndices
      VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,  // preTransform
      VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,      // compositeAlpha
      mode,                                   // presentMode
      VK_FALSE,                               // clipped
      VK_NULL_HANDLE                          // oldSwapchain
  };

  VkSwapchainKHR swapchain;
  if (VK_SUCCESS !=
      vkCreateSwapchainKHR(m_device, &create_info, nullptr, &swapchain)) {
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
  swp->initialize(m_allocator, this, static_cast<uint32_t>(capabilities.width),
      static_cast<uint32_t>(capabilities.height), info, swapchain);
  return swp;
}

swapchain_ptr vulkan_device::recreate_swapchain(surface& _surface,
    swapchain_ptr _old_swapchain, const swapchain_create_info& _info, queue*) {
  swapchain_create_info* last_info = &_old_swapchain->m_create_info;

  WN_RELEASE_ASSERT(_info.discard == last_info->discard,
      "Discard mode must match when recreating a swapchain");
  WN_RELEASE_ASSERT(_info.mode == last_info->mode,
      "Flip mode must match when recreating a swapchain");

  VkSurfaceKHR surface = _surface.data_as<VkSurfaceKHR>();
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
      WN_DEBUG_ASSERT(false, "Should never get here");
      mode = VK_PRESENT_MODE_FIFO_RELAXED_KHR;
  }

  vulkan_swapchain_constructable* old_swp =
      static_cast<vulkan_swapchain_constructable*>(_old_swapchain.get());
  old_swp->m_device = nullptr;
  surface_capabilities capabilities;
  graphics_error err;
  if (err = _surface.get_surface_capabilities(&capabilities),
      err != graphics_error::ok) {
    m_log->log_error("Could not get swapchain size");
    return nullptr;
  }

  VkSwapchainCreateInfoKHR create_info = {
      VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,  // sType
      nullptr,                                      // pNext
      0,                                            // flags
      surface,                                      // surface
      _info.num_buffers,                            // minImageCount
      image_format_to_vulkan_format(_info.format),  // imageFormat
      VK_COLORSPACE_SRGB_NONLINEAR_KHR,             // imageColorSpace
      {
          // imageExtent
          static_cast<uint32_t>(capabilities.width),   // width
          static_cast<uint32_t>(capabilities.height),  // height
      },
      1,  // imageArrayLayers
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
          VK_IMAGE_USAGE_TRANSFER_DST_BIT,    // imageUsage
      VK_SHARING_MODE_EXCLUSIVE,              // imageSharingMode
      0,                                      // queueFamilyIndexCount
      nullptr,                                // pQueueFamilyIndices
      VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,  // preTransform
      VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,      // compositeAlpha
      mode,                                   // presentMode
      VK_FALSE,                               // clipped
      old_swp->m_swapchain                    // oldSwapchain
  };

  VkSwapchainKHR swapchain;
  VkResult res;
  if (res = vkCreateSwapchainKHR(m_device, &create_info, nullptr, &swapchain),
      res != VK_SUCCESS) {
    m_log->log_error("Could not create VkSwapchain: ", res);
    return nullptr;
  }
  vkDestroySwapchainKHR(m_device, old_swp->m_swapchain, nullptr);
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
  swp->initialize(m_allocator, this, static_cast<uint32_t>(capabilities.width),
      static_cast<uint32_t>(capabilities.height), info, swapchain);
  return swp;
}

void vulkan_device::initialize_shader_module(shader_module* s,
    const containers::contiguous_range<const uint8_t>& bytes) {
  ::VkShaderModule& m = get_data(s);
  VkShaderModuleCreateInfo create_info = {
      VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,  // sType
      nullptr,                                      // pNext
      0,                                            // flags
      bytes.size(),                                 // codeSize
      reinterpret_cast<const uint32_t*>(bytes.data())};

  if (VK_SUCCESS != vkCreateShaderModule(m_device, &create_info, nullptr, &m)) {
    m_log->log_error("Error creating shader module.");
  }
}

void vulkan_device::destroy_shader_module(shader_module* s) {
  ::VkShaderModule& m = get_data(s);
  vkDestroyShaderModule(m_device, m, nullptr);
}

void vulkan_device::initialize_descriptor_set_layout(
    descriptor_set_layout* _layout,
    const containers::contiguous_range<const descriptor_binding_info>&
        _binding_infos) {
  ::VkDescriptorSetLayout& set_layout = get_data(_layout);

  containers::dynamic_array<VkDescriptorSetLayoutBinding> bindings(
      m_allocator, _binding_infos.size());
  VkDescriptorSetLayoutCreateInfo create_info = {
      VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,  // sType
      nullptr,                                              // pNext
      // TODO(awoloszyn): Handle push_constants here.
      0,                                             // flags
      static_cast<uint32_t>(_binding_infos.size()),  // bindingCount
      bindings.data()                                // pBindings
  };

  for (size_t i = 0; i < _binding_infos.size(); ++i) {
    const descriptor_binding_info& info = _binding_infos[i];
    VkDescriptorSetLayoutBinding& binding = bindings[i];
    binding.binding = static_cast<uint32_t>(info.binding);
    binding.descriptorCount = static_cast<uint32_t>(info.array_size);
    switch (info.type) {
      case descriptor_type::sampler:
        binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
        break;
      case descriptor_type::read_only_buffer:
        binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        break;
      case descriptor_type::read_only_image_buffer:
        binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        break;
      case descriptor_type::read_only_sampled_buffer:
        binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
        break;
      case descriptor_type::read_write_buffer:
        binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        break;
      case descriptor_type::read_write_image_buffer:
        binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        break;
      case descriptor_type::read_write_sampled_buffer:
        binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
        break;
      case descriptor_type::sampled_image:
        binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        break;
      default:
        WN_RELEASE_ASSERT(false, "You cannot reach here");
    }

    if (info.shader_stages & static_cast<uint32_t>(shader_stage::vertex)) {
      binding.stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
    }
    if (info.shader_stages & static_cast<uint32_t>(shader_stage::pixel)) {
      binding.stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
    }
    if (info.shader_stages &
        static_cast<uint32_t>(shader_stage::tessellation_control)) {
      binding.stageFlags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    }
    if (info.shader_stages &
        static_cast<uint32_t>(shader_stage::tessellation_evaluation)) {
      binding.stageFlags |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    }
    if (info.shader_stages & static_cast<uint32_t>(shader_stage::geometry)) {
      binding.stageFlags |= VK_SHADER_STAGE_GEOMETRY_BIT;
    }
    if (info.shader_stages & static_cast<uint32_t>(shader_stage::compute)) {
      binding.stageFlags |= VK_SHADER_STAGE_COMPUTE_BIT;
    }
  }

  if (VK_SUCCESS != vkCreateDescriptorSetLayout(
                        m_device, &create_info, nullptr, &set_layout)) {
    m_log->log_error("Could not create descriptor set layout");
  }
}

void vulkan_device::destroy_descriptor_set_layout(
    descriptor_set_layout* _layout) {
  ::VkDescriptorSetLayout& set = get_data(_layout);
  vkDestroyDescriptorSetLayout(m_device, set, nullptr);
}

VkDescriptorType descriptor_type_to_vulkan(descriptor_type _type) {
  switch (_type) {
    case descriptor_type::sampler:
      return VK_DESCRIPTOR_TYPE_SAMPLER;
      break;
    case descriptor_type::read_only_buffer:
      return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      break;
    case descriptor_type::read_only_image_buffer:
      return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
      break;
    case descriptor_type::read_only_sampled_buffer:
      return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
      break;
    case descriptor_type::read_write_buffer:
      return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
      break;
    case descriptor_type::read_write_image_buffer:
      return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
      break;
    case descriptor_type::read_write_sampled_buffer:
      return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
      break;
    case descriptor_type::sampled_image:
      return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
      break;
    default:
      WN_RELEASE_ASSERT(false, "You should never reach here");
      return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
  }
}

void vulkan_device::initialize_descriptor_pool(descriptor_pool* _pool,
    const containers::contiguous_range<const descriptor_pool_create_info>&
        _pool_data) {
  ::VkDescriptorPool& pool = get_data(_pool);

  containers::dynamic_array<VkDescriptorPoolSize> sizes(
      m_allocator, _pool_data.size());
  VkDescriptorPoolCreateInfo create_info = {
      VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,      // sType
      nullptr,                                            // pNext
      VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,  // flags
      0,                                                  // maxSets
      static_cast<uint32_t>(_pool_data.size()),           // poolSizeCount
      sizes.data()};
  size_t max_sets = 0;
  for (size_t i = 0; i < _pool_data.size(); ++i) {
    VkDescriptorPoolSize& size = sizes[i];
    const descriptor_pool_create_info& create = _pool_data[i];
    size.type = descriptor_type_to_vulkan(create.type);
    size.descriptorCount = static_cast<uint32_t>(create.max_descriptors);
    max_sets += create.max_descriptors;
  }
  create_info.maxSets = static_cast<uint32_t>(max_sets);
  if (VK_SUCCESS !=
      vkCreateDescriptorPool(m_device, &create_info, nullptr, &pool)) {
    m_log->log_error("Could not create descriptor pool");
  }
}
void vulkan_device::destroy_descriptor_pool(descriptor_pool* _pool) {
  ::VkDescriptorPool& pool = get_data(_pool);

  vkDestroyDescriptorPool(m_device, pool, nullptr);
}

void vulkan_device::initialize_descriptor_set(descriptor_set* _set,
    descriptor_pool* _pool, const descriptor_set_layout* _pool_data) {
  descriptor_set_data& set = get_data(_set);
  ::VkDescriptorPool& pool = get_data(_pool);
  const ::VkDescriptorSetLayout& layout = get_data(_pool_data);

  VkDescriptorSetAllocateInfo allocate_info = {
      VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,  // sType
      nullptr,                                         // pNext
      pool,                                            // descriptorPool
      1,                                               // descriptorSetCount
      &layout,                                         // pSetLayouts
  };

  if (VK_SUCCESS !=
      vkAllocateDescriptorSets(m_device, &allocate_info, &set.set)) {
    m_log->log_error("Could not allocate descriptor sets");
  }
  set.pool = pool;
}

void vulkan_device::destroy_descriptor_set(descriptor_set* _set) {
  descriptor_set_data& set = get_data(_set);
  vkFreeDescriptorSets(m_device, set.pool, 1, &set.set);
}

void vulkan_device::update_descriptors(descriptor_set* _set,
    const containers::contiguous_range<buffer_descriptor>& _buffer_updates,
    const containers::contiguous_range<image_descriptor>& _image_updates,
    const containers::contiguous_range<sampler_descriptor>& _sampler_updates) {
  descriptor_set_data& set = get_data(_set);
  containers::deque<VkDescriptorImageInfo> images(m_allocator);
  containers::deque<VkDescriptorBufferInfo> buffers(m_allocator);
  containers::dynamic_array<VkWriteDescriptorSet> descriptor_set_writes(
      m_allocator);
  descriptor_set_writes.reserve(images.size() + buffers.size());

  for (auto& b : _buffer_updates) {
    memory::unique_ptr<buffer_info>& bdata = get_data(b.resource);
    buffers.push_back(VkDescriptorBufferInfo{
        bdata->buffer,                          // buffer
        b.offset_in_elements * b.element_size,  // offset
        b.element_size * b.num_elements         // range
    });

    descriptor_set_writes.push_back(VkWriteDescriptorSet{
        VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,  // sType
        nullptr,                                 // pNext
        set.set,                                 // dstSet
        b.binding,                               // dstBinding
        b.array_offset,                          // dstArrayElement
        1,                                       // descriptorCount
        descriptor_type_to_vulkan(b.type),       // descriptorType
        nullptr,                                 // pImageInfo
        &buffers.back(),                         // pBufferInfo
        nullptr,                                 // pTexelBufferView
    });
  }

  for (auto& i : _image_updates) {
    ::VkImageView& view = get_data(i.resource);
    images.push_back({
        VkSampler(VK_NULL_HANDLE),                // sampler
        view,                                     // imageView
        resource_state_to_vulkan_layout(i.state)  // imageLayout // range
    });

    descriptor_set_writes.push_back(VkWriteDescriptorSet{
        VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,  // sType
        nullptr,                                 // pNext
        set.set,                                 // dstSet
        i.binding,                               // dstBinding
        i.array_offset,                          // dstArrayElement
        1,                                       // descriptorCount
        descriptor_type_to_vulkan(i.type),       // descriptorType
        &images.back(),                          // pImageInfo
        nullptr,                                 // pBufferInfo
        nullptr,                                 // pTexelBufferView
    });
  }

  for (auto& s : _sampler_updates) {
    ::VkSampler& sampler = get_data(s.resource);
    images.push_back({
        sampler,                      // sampler
        VkImageView(VK_NULL_HANDLE),  // imageView
        VK_IMAGE_LAYOUT_GENERAL       // imageLayout unused
    });

    descriptor_set_writes.push_back(VkWriteDescriptorSet{
        VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,  // sType
        nullptr,                                 // pNext
        set.set,                                 // dstSet
        s.binding,                               // dstBinding
        s.array_offset,                          // dstArrayElement
        1,                                       // descriptorCount
        VK_DESCRIPTOR_TYPE_SAMPLER,              // descriptorType
        &images.back(),                          // pImageInfo
        nullptr,                                 // pBufferInfo
        nullptr,                                 // pTexelBufferView
    });
  }

  vkUpdateDescriptorSets(m_device,
      static_cast<uint32_t>(descriptor_set_writes.size()),
      descriptor_set_writes.data(), 0, nullptr);
}

void vulkan_device::initialize_pipeline_layout(pipeline_layout* _layout,
    const containers::contiguous_range<const descriptor_set_layout*>&
        _descriptor_sets,
    const containers::contiguous_range<const push_constant_range>& ranges) {
  memory::unique_ptr<pipeline_layout_data>& layout = get_data(_layout);
  layout = memory::make_unique<pipeline_layout_data>(m_allocator);
  layout->push_constants =
      containers::dynamic_array<pipeline_layout_data::push_constant>(
          m_allocator);

  containers::dynamic_array<VkPushConstantRange> push_constant_ranges(
      m_allocator);
  push_constant_ranges.reserve(ranges.size());

  for (auto& p : ranges) {
    layout->push_constants.push_back({p.offset_in_uint32s * 4, p.valid_stages});
    push_constant_ranges.push_back(
        VkPushConstantRange{shader_stages_to_vulkan(p.valid_stages),
            p.offset_in_uint32s * 4, p.num_uint32_values * 4});
  }

  containers::dynamic_array<::VkDescriptorSetLayout> descriptor_set_layouts(
      m_allocator, _descriptor_sets.size());
  VkPipelineLayoutCreateInfo create_info = {
      VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,   // sType
      nullptr,                                         // pNext
      0,                                               // flags
      static_cast<uint32_t>(_descriptor_sets.size()),  // sets
      descriptor_set_layouts.data(),                   // pSetLayouts
      static_cast<uint32_t>(
          push_constant_ranges.size()),  // pushConstantRangeCount
      push_constant_ranges.data(),       // pPushConstantRanges
  };

  for (size_t i = 0; i < _descriptor_sets.size(); ++i) {
    const descriptor_set_layout* set_layout = _descriptor_sets[i];
    const ::VkDescriptorSetLayout& l = get_data(set_layout);
    descriptor_set_layouts[i] = l;
  }

  if (VK_SUCCESS != vkCreatePipelineLayout(
                        m_device, &create_info, nullptr, &layout->layout)) {
    m_log->log_error("Could not create pipeline layout");
  }
}

void vulkan_device::destroy_pipeline_layout(pipeline_layout* _layout) {
  memory::unique_ptr<pipeline_layout_data> layout =
      core::move(get_data(_layout));
  vkDestroyPipelineLayout(m_device, layout->layout, nullptr);
}

void vulkan_device::initialize_render_pass(render_pass* _pass,
    const containers::contiguous_range<const render_pass_attachment>&
        _attachments,
    const containers::contiguous_range<const subpass_description>& _subpasses,
    const containers::contiguous_range<const subpass_dependency>& _deps) {
  ::VkRenderPass& pass_data = get_data(_pass);

  containers::dynamic_array<VkAttachmentDescription> attachments(
      m_allocator, _attachments.size());
  containers::dynamic_array<VkSubpassDescription> subpasses(
      m_allocator, _subpasses.size());
  containers::dynamic_array<VkSubpassDependency> dependencies(
      m_allocator, _deps.size());

  VkRenderPassCreateInfo create_info = {
      VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,   // sType
      nullptr,                                     // pNext
      0,                                           // flags
      static_cast<uint32_t>(attachments.size()),   // attachmentCount
      attachments.data(),                          // pAttachments
      static_cast<uint32_t>(subpasses.size()),     // subpassCount
      subpasses.data(),                            // pSubpasses
      static_cast<uint32_t>(dependencies.size()),  // dependencyCount
      dependencies.data()                          // pDependencies
  };

  for (size_t i = 0; i < attachments.size(); ++i) {
    // TODO: Figure out how to handle the MayAlias bit.
    const render_pass_attachment& att = _attachments[i];
    attachments[i] = {0,                                     // Flags
        image_format_to_vulkan_format(att.format),           // format
        multi_sampled_to_vulkan(att.num_samples),            // samples
        load_op_to_vulkan(att.attachment_load_op),           // loadOp
        store_op_to_vulkan(att.attachment_store_op),         // storeOp
        load_op_to_vulkan(att.stencil_load_op),              // stencilLoadOp
        store_op_to_vulkan(att.stencil_store_op),            // stencilStore
        resource_state_to_vulkan_layout(att.initial_state),  // initialLayout
        resource_state_to_vulkan_layout(att.final_state)};
  }

  containers::deque<containers::dynamic_array<VkAttachmentReference>>
      attachment_references(m_allocator);
  containers::deque<containers::dynamic_array<uint32_t>> preserve_references(
      m_allocator);
  for (size_t i = 0; i < subpasses.size(); ++i) {
    const subpass_description& desc = _subpasses[i];
    attachment_references.emplace_back(
        m_allocator, desc.input_attachments.size());
    containers::dynamic_array<VkAttachmentReference>& inputs =
        attachment_references.back();
    attachment_references.emplace_back(
        m_allocator, desc.color_attachments.size());
    containers::dynamic_array<VkAttachmentReference>& colors =
        attachment_references.back();
    attachment_references.emplace_back(
        m_allocator, desc.resolve_attachments.size());
    containers::dynamic_array<VkAttachmentReference>& resolves =
        attachment_references.back();
    preserve_references.emplace_back(
        m_allocator, desc.preserve_attachments.size());
    containers::dynamic_array<uint32_t>& preserve = preserve_references.back();

    for (size_t j = 0; j < desc.input_attachments.size(); ++j) {
      inputs[j] = {desc.input_attachments[j].attachment,
          resource_state_to_vulkan_layout(desc.input_attachments[j].state)};
    }

    for (size_t j = 0; j < desc.color_attachments.size(); ++j) {
      colors[j] = {desc.color_attachments[j].attachment,
          resource_state_to_vulkan_layout(desc.color_attachments[j].state)};
    }

    for (size_t j = 0; j < desc.resolve_attachments.size(); ++j) {
      resolves[j] = {desc.resolve_attachments[j].attachment,
          resource_state_to_vulkan_layout(desc.resolve_attachments[j].state)};
    }

    for (size_t j = 0; j < desc.preserve_attachments.size(); ++j) {
      preserve[j] = desc.preserve_attachments[j].attachment;
    }

    VkAttachmentReference ref;
    VkAttachmentReference* depth_ref = nullptr;
    if (desc.depth_stencil) {
      ref = {desc.depth_stencil->attachment,
          resource_state_to_vulkan_layout(desc.depth_stencil->state)};
      depth_ref = &ref;
    }

    subpasses[i] = {
        0,                                       // flags
        VK_PIPELINE_BIND_POINT_GRAPHICS,         // pipelineBindPoint
        static_cast<uint32_t>(inputs.size()),    // inputAttachmentCount
        inputs.data(),                           // pInputAttachmentCount
        static_cast<uint32_t>(colors.size()),    // colorAttachmentCount
        colors.data(),                           // pColorAttachments
        resolves.data(),                         // pResolveAttachments
        depth_ref,                               // pDepthStencilAttachments
        static_cast<uint32_t>(preserve.size()),  // preserveAttachmentCount
        preserve.data()                          // pPreserveAttachments
    };
  }

  if (VK_SUCCESS !=
      vkCreateRenderPass(m_device, &create_info, nullptr, &pass_data)) {
    m_log->log_error("Could not create render pass");
  }
}

void vulkan_device::destroy_render_pass(render_pass* _pass) {
  ::VkRenderPass& pass_data = get_data(_pass);
  vkDestroyRenderPass(m_device, pass_data, nullptr);
}

// TODO: figure out how to deal with stencil here
void vulkan_device::initialize_image_view(image_view* _view,
    const image* _image, uint32_t _base_mip_level, uint32_t _num_mip_levels) {
  ::VkImageView& view = get_data(_view);
  const ::VkImage& img = get_data(_image);
  // TODO(awoloszyn): Update this for multi-dimensional views
  // and mip-maps.
  VkImageViewCreateInfo create_info = {
      VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,  // sType
      nullptr,                                   // pNext
      0,                                         // flags
      img,                                       // image
      VK_IMAGE_VIEW_TYPE_2D,                     // viewType
      image_format_to_vulkan_format(
          _image->get_buffer_requirements(_base_mip_level).format),  // format
      {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
          VK_COMPONENT_SWIZZLE_IDENTITY,
          VK_COMPONENT_SWIZZLE_IDENTITY},  // components
      {image_components_to_aspect(_view->get_components()), _base_mip_level,
          _num_mip_levels, 0, 1},  // subresourceRange
  };

  if (VK_SUCCESS != vkCreateImageView(m_device, &create_info, nullptr, &view)) {
    m_log->log_error("Could not create image view");
  }
}

void vulkan_device::destroy_image_view(image_view* _view) {
  ::VkImageView& view = get_data(_view);
  vkDestroyImageView(m_device, view, nullptr);
}

containers::contiguous_range<const arena_properties>
vulkan_device::get_arena_properties() const {
  return containers::contiguous_range<const arena_properties>(
      m_arena_properties.data(), m_arena_properties.size());
}

bool vulkan_device::setup_arena_properties() {
  // get buffer heaps
  const VkBufferUsageFlags usage =
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
      VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
      VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT |
      VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT |
      VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
  const VkBufferCreateInfo buffer_create_info = {
      VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,  // sType
      nullptr,                               // pNext
      0,                                     // flags
      1,                                     // size
      usage,                                 // usage
      VK_SHARING_MODE_EXCLUSIVE,             // sharingMode
      0,                                     // queueFamilyIndexCount
      0                                      // pQueueFamilyIndices
  };
  VkBuffer test_buffer;

  if (vkCreateBuffer(m_device, &buffer_create_info, nullptr, &test_buffer) !=
      VK_SUCCESS) {
    return false;
  }

  VkMemoryRequirements requirements;

  vkGetBufferMemoryRequirements(m_device, test_buffer, &requirements);

  const uint32_t buffer_heaps = requirements.memoryTypeBits;

  vkDestroyBuffer(m_device, test_buffer, nullptr);

  // get image heaps
  const VkImageCreateInfo image_create_info = {
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
      VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
          VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT |
          VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
          VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,  // usage
      VK_SHARING_MODE_EXCLUSIVE,                // sharingMode
      0,                                        // queueFamilyIndexCount
      nullptr,                                  // pQueueFamilyIndices
      VK_IMAGE_LAYOUT_UNDEFINED                 // initialLayout
  };
  VkImage test_image;

  if (vkCreateImage(m_device, &image_create_info, nullptr, &test_image) !=
      VK_SUCCESS) {
    return false;
  }

  VkMemoryRequirements image_requirements;

  vkGetImageMemoryRequirements(m_device, test_image, &image_requirements);

  const uint32_t image_heaps = image_requirements.memoryTypeBits;

  vkDestroyImage(m_device, test_image, nullptr);

  // create arena properties
  const uint32_t memory_type_count =
      m_physical_device_memory_properties->memoryTypeCount;

  m_arena_properties.reserve(memory_type_count);
  m_arena_to_vulkan_index.reserve(memory_type_count);
  struct {
    VkFormat format;
    bool supported;
    uint32_t format_heaps;
  } format_checks[] = {
      {VK_FORMAT_D16_UNORM, m_supported_formats->has_d16_unorm, 0},
      {VK_FORMAT_X8_D24_UNORM_PACK32, m_supported_formats->has_d24_unorm, 0},
      {VK_FORMAT_D32_SFLOAT, m_supported_formats->has_d32_float, 0},
      {VK_FORMAT_S8_UINT, m_supported_formats->has_s8_unorm},
      {VK_FORMAT_D16_UNORM_S8_UINT, m_supported_formats->has_d16_unorm_s8_unorm,
          0},
      {VK_FORMAT_D24_UNORM_S8_UINT, m_supported_formats->has_d24_unorm_s8_unorm,
          0},
      {VK_FORMAT_D32_SFLOAT_S8_UINT,
          m_supported_formats->has_d32_float_s8_unorm, 0}};

  for (auto& format : format_checks) {
    if (!format.supported) {
      continue;
    }

    const VkImageCreateInfo depth_image_create_info = {
        VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,  // sType
        nullptr,                              // pNext
        0,                                    // flags
        VK_IMAGE_TYPE_2D,                     // imageType
        format.format,                        // format
        VkExtent3D{1, 1, 1},                  // extent
        1,                                    // mipLevels
        1,                                    // arrayLayers
        VK_SAMPLE_COUNT_1_BIT,                // samples
        VK_IMAGE_TILING_OPTIMAL,              // tiling
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
            VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,  // usage
        VK_SHARING_MODE_EXCLUSIVE,                // sharingMode
        0,                                        // queueFamilyIndexCount
        nullptr,                                  // pQueueFamilyIndices
        VK_IMAGE_LAYOUT_UNDEFINED                 // initialLayout
    };
    VkImage depth_test_image;

    if (vkCreateImage(m_device, &depth_image_create_info, nullptr,
            &depth_test_image) != VK_SUCCESS) {
      return false;
    }

    VkMemoryRequirements depth_image_requirements;

    vkGetImageMemoryRequirements(
        m_device, depth_test_image, &depth_image_requirements);

    vkDestroyImage(m_device, depth_test_image, nullptr);

    format.format_heaps = depth_image_requirements.memoryTypeBits;
  }

  for (uint32_t i = 0; i < memory_type_count; ++i) {
    size_t heap_bit = static_cast<size_t>(1) << i;
    const VkMemoryType& current_memory_types =
        m_physical_device_memory_properties->memoryTypes[i];
    const VkMemoryPropertyFlags& property_flags =
        current_memory_types.propertyFlags;
    uint32_t valid_bits = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT |
                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
                          VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
    if ((property_flags & valid_bits) != property_flags) {
      // This memory has some flags that we don't really want.
      // E.g. Protected, lazily allocated.
      // If we want to support those types, we must do so
      // explicitly.
      continue;
    }

    const bool allows_image_and_render_targets =
        ((heap_bit & image_heaps) != 0);
    const arena_properties new_arena_properties = {
        ((property_flags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0),
        ((property_flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0),
        ((property_flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0),
        ((property_flags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) != 0),
        (((1 << i) & buffer_heaps) != 0), allows_image_and_render_targets,
        allows_image_and_render_targets,
        ((heap_bit & format_checks[0].format_heaps) != 0),
        ((heap_bit & format_checks[1].format_heaps) != 0),
        ((heap_bit & format_checks[2].format_heaps) != 0),
        ((heap_bit & format_checks[3].format_heaps) != 0),
        ((heap_bit & format_checks[4].format_heaps) != 0),
        ((heap_bit & format_checks[5].format_heaps) != 0),
        ((heap_bit & format_checks[6].format_heaps) != 0)};

    m_arena_properties.push_back(new_arena_properties);
    m_arena_to_vulkan_index.push_back(static_cast<uint32_t>(i));
  }

  return true;
}

bool vulkan_device::initialize_arena(arena* _arena, const size_t _index,
    const uint64_t _size, const bool /*_multisampled*/) {
  WN_DEBUG_ASSERT(
      m_arena_properties.size() > _index, "arena property index out of range");
  WN_DEBUG_ASSERT(_size > 0, "arena should be non-zero size");

  const uint32_t vulkan_index = m_arena_to_vulkan_index[_index];
  VkDeviceMemory new_memory;
  const VkMemoryAllocateInfo allocate_info = {
      VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,  // sType
      nullptr,                                 // pNext
      _size,                                   // allocationSize
      vulkan_index                             // memoryTypeIndex
  };

  if (vkAllocateMemory(m_device, &allocate_info, nullptr, &new_memory) !=
      VK_SUCCESS) {
    m_log->log_error("Could not successfully allocate device memory of size ",
        _size, " bytes");

    return false;
  }

  void* pointer = nullptr;

  if (m_arena_properties[_index].host_visible) {
    if (vkMapMemory(m_device, new_memory, 0, _size, 0, &pointer) !=
        VK_SUCCESS) {
      return false;
    }
  }

  arena_data& adata = get_data(_arena);

  adata.memory = core::move(new_memory);
  adata.root = pointer;
  _arena->m_size = _size;

  return true;
}

void vulkan_device::destroy_arena(arena* _arena) {
  arena_data& adata = get_data(_arena);

  vkFreeMemory(m_device, adata.memory, nullptr);
}

void vulkan_device::initialize_framebuffer(
    framebuffer* _framebuffer, const framebuffer_create_info& _info) {
  ::VkRenderPass pass = get_data(_info.pass);
  ::VkFramebuffer& framebuffer = get_data(_framebuffer);
  containers::dynamic_array<::VkImageView> image_views(
      m_allocator, _info.image_views.size());
  for (size_t i = 0; i < _info.image_views.size(); ++i) {
    const image_view* view = _info.image_views[i];
    image_views[i] = get_data(view);
  }

  VkFramebufferCreateInfo create_info = {
      VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,        // sType
      nullptr,                                          // pNExt
      0,                                                // flags
      pass,                                             // renderPass
      static_cast<uint32_t>(_info.image_views.size()),  // attachmentCount
      image_views.data(),                               // pAttachments
      _info.width,                                      // width
      _info.height,                                     // height
      _info.depth,                                      // depth
  };

  if (VK_SUCCESS !=
      vkCreateFramebuffer(m_device, &create_info, nullptr, &framebuffer)) {
    m_log->log_error("Could not create framebuffer, something went wrong");
  }
}

void vulkan_device::destroy_framebuffer(framebuffer* _framebuffer) {
  ::VkFramebuffer& fb = get_data(_framebuffer);
  vkDestroyFramebuffer(m_device, fb, nullptr);
}

void vulkan_device::initialize_graphics_pipeline(graphics_pipeline* _pipeline,
    const graphics_pipeline_description& _create_info,
    const pipeline_layout* _layout, const render_pass* _renderpass,
    uint32_t _subpass) {
  uint32_t num_used_shader_stages = 0;
  containers::array<VkPipelineShaderStageCreateInfo, 5> shader_stages;
  bool has_rasterization = false;
  for (size_t i = 0; i < 5; ++i) {
    if (!_create_info.m_shader_stages[i].entry_point.empty()) {
      const auto& stage = _create_info.m_shader_stages[i];
      const shader_module* mod = stage.module;
      ::VkShaderModule module = get_data(mod);
      shader_stages[num_used_shader_stages++] = {
          VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,        // sType
          nullptr,                                                    // pNext
          0,                                                          // flags
          shader_stage_to_vulkan(static_cast<shader_stage>(1 << i)),  // stage
          module,
          stage.entry_point.c_str(),  // pName
          nullptr,                    // pSpecializationInfo
      };
      if (static_cast<shader_stage>(1 << i) == shader_stage::pixel) {
        has_rasterization = true;
      }
    }
  }
  has_rasterization |= _create_info.m_depth_enabled;

  containers::dynamic_array<VkVertexInputBindingDescription> vertex_bindings(
      m_allocator, _create_info.m_vertex_streams.size());
  for (size_t i = 0; i < vertex_bindings.size(); ++i) {
    const auto& binding = _create_info.m_vertex_streams[i];
    VkVertexInputRate rate = binding.frequency == stream_frequency::per_instance
                                 ? VK_VERTEX_INPUT_RATE_INSTANCE
                                 : VK_VERTEX_INPUT_RATE_VERTEX;
    vertex_bindings[i] = {binding.index, binding.stride, rate};
  }

  containers::dynamic_array<VkVertexInputAttributeDescription>
      vertex_attributes(m_allocator, _create_info.m_vertex_attributes.size());
  for (size_t i = 0; i < vertex_attributes.size(); ++i) {
    const auto& attribute = _create_info.m_vertex_attributes[i];
    vertex_attributes[i] = {attribute.attribute_index, attribute.stream_index,
        image_format_to_vulkan_format(attribute.format), attribute.offset};
  }

  VkPipelineVertexInputStateCreateInfo vertex_input_state_info = {
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,  // sType
      nullptr,                                                    // pNext
      0,                                                          // flags
      static_cast<uint32_t>(
          vertex_bindings.size()),  // vertexBindingDescriptionCount
      vertex_bindings.data(),       // pVertexBindingDescriptions
      static_cast<uint32_t>(
          vertex_attributes.size()),  // vertexAttributeDescriptionCount
      vertex_attributes.data()        // pVertexAttributeDescriptions
  };

  VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info = {
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,  // sType
      nullptr,                                                      // pNext
      0,                                                            // flags
      primitive_topology_to_vulkan(_create_info.m_topology),        // topology
      _create_info.m_index_restart
          ? VkBool32(VK_TRUE)
          : VkBool32(VK_FALSE)  // primitiveRestartEnabled
  };

  VkPipelineTessellationStateCreateInfo* tessellation_create_info = nullptr;
  VkPipelineTessellationStateCreateInfo tess_info;
  if (_create_info.m_topology == topology::patch_list) {
    tess_info = {
        VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,  // sType
        nullptr,                                                    // pNext
        0,                                                          // flags
        _create_info.m_num_patch_control_points,  // patchControlPoints
    };
    tessellation_create_info = &tess_info;
  }

  VkPipelineViewportStateCreateInfo* viewport_create_info = nullptr;
  VkPipelineViewportStateCreateInfo viewport_info;
  containers::dynamic_array<VkViewport> viewports(
      m_allocator, _create_info.m_static_viewports.size());
  containers::dynamic_array<VkRect2D> scissors(
      m_allocator, _create_info.m_static_scissors.size());
  bool has_dynamic_viewport = true;
  bool has_dynamic_scissor = true;
  if (has_rasterization) {
    viewport_create_info = &viewport_info;
    for (size_t i = 0; i < viewports.size(); ++i) {
      has_dynamic_viewport = false;
      const auto& viewport = _create_info.m_static_viewports[i];

      viewports[i] = VkViewport{viewport.x, viewport.y, viewport.width,
          viewport.height, viewport.min_depth, viewport.max_depth};
    }

    for (size_t i = 0; i < scissors.size(); ++i) {
      has_dynamic_scissor = false;
      const auto& scissor = _create_info.m_static_scissors[i];
      scissors[i] =
          VkRect2D{{scissor.x, scissor.y}, {scissor.width, scissor.height}};
    }

    viewport_info = VkPipelineViewportStateCreateInfo{
        VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,  // sType
        nullptr,                                                // pNext
        0,                                                      // flags
        _create_info.m_num_viewports,                           // viewportCount
        has_dynamic_viewport ? nullptr : viewports.data(),      // pViewports
        _create_info.m_num_scissors,                            // scissorCount
        has_dynamic_scissor ? nullptr : scissors.data(),        // pScissors
    };
  }

  VkPipelineRasterizationStateCreateInfo rasterization_create_info = {
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,  // sType
      nullptr,                                                     // pNext
      0,                                                           // flags
      false,  // TODO(awoloszyn): set up depth clamp
      false,  // TODO(awoloszyn): Set up rasterizer Discard
      fill_mode_to_vulkan(_create_info.m_fill_mode),  // polygonMode
      cull_mode_to_vulkan(_create_info.m_cull_mode),  // cullMode
      winding_to_vulkan(_create_info.m_winding),      // frontFace
      _create_info.m_depth_bias != 0.f,               // depthBiasEnable
      _create_info.m_depth_bias,                      // depthBiasConstantFactor
      0,     // depthBiasClamp TODO(awolosyn): Expose this
      0,     // depthBiasSlope TODO(awoloszyn): Expose this
      1.0f,  // lineWidth TODO(awoloszyn): Expose this
  };

  VkPipelineMultisampleStateCreateInfo* multisample_create_info = nullptr;
  VkPipelineMultisampleStateCreateInfo ms_info;

  VkSampleMask sample_masks[2] = {
      uint32_t(_create_info.m_sample_mask & static_cast<uint64_t>(0xFFFFFFFF)),
      uint32_t((_create_info.m_sample_mask >> 32) &
               static_cast<uint64_t>(0xFFFFFFFF))};
  if (has_rasterization) {
    multisample_create_info = &ms_info;

    ms_info = VkPipelineMultisampleStateCreateInfo{
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,  // sType
        nullptr,                                                   // pNext
        0,                                                         // flags
        samples_to_vulkan(_create_info.m_num_samples),             // samples
        false,                             // sampleShadingEnable
        0,                                 // minSampleShading
        sample_masks,                      // pSampleMask,
        _create_info.m_alpha_to_coverage,  // alphaToCoverageEnable
        false,                             // alphaToOneEnable
    };
  }

  VkPipelineDepthStencilStateCreateInfo* depth_create_info = nullptr;
  VkPipelineDepthStencilStateCreateInfo depth_info;
  bool has_stencil_reference = false;
  if (has_rasterization && _create_info.m_depth_enabled) {
    depth_create_info = &depth_info;

    depth_info = VkPipelineDepthStencilStateCreateInfo{
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,  // sType
        nullptr,                                                     // pNext
        0,                                                           // flags
        _create_info.m_depth_test_enabled,                   // depthTestEnable
        _create_info.m_depth_write_enabled,                  // depthWriteEnable
        compare_to_vulkan(_create_info.m_depth_comparison),  // depthCompareOp
        false,                           // depthBoundsTestEnable
        _create_info.m_stencil_enabled,  // stencilTestEnable
        // In D3D, read/write/stencil must be the same for both front and back
        VkStencilOpState{
            stencil_op_to_vulkan(
                _create_info.m_stencil_front_face.fail),  // failOp
            stencil_op_to_vulkan(
                _create_info.m_stencil_front_face.pass),  // passOp
            stencil_op_to_vulkan(
                _create_info.m_stencil_front_face.depth_fail),  // depthFail
            compare_to_vulkan(
                _create_info.m_stencil_front_face.compare),  // compareOp
            _create_info.m_stencil_read_mask,                // compareMask
            _create_info.m_stencil_write_mask,               // writeMask
            _create_info.m_static_stencil_reference,         // reference
        },
        VkStencilOpState{
            stencil_op_to_vulkan(
                _create_info.m_stencil_back_face.fail),  // failOp
            stencil_op_to_vulkan(
                _create_info.m_stencil_back_face.pass),  // passOp
            stencil_op_to_vulkan(
                _create_info.m_stencil_back_face.depth_fail),  // depthFail
            compare_to_vulkan(
                _create_info.m_stencil_back_face.compare),  // compareOp
            _create_info.m_stencil_read_mask,               // compareMask
            _create_info.m_stencil_write_mask,              // writeMask
            _create_info.m_static_stencil_reference,        // reference
        },
        0.0f,  // minDepthBounds
        1.0f,  // maxDepthBounds
    };
    if (_create_info.m_stencil_enabled) {
      has_stencil_reference = true;
    }
  }
  VkPipelineColorBlendStateCreateInfo* color_blend_info = nullptr;
  VkPipelineColorBlendStateCreateInfo color_info;
  containers::dynamic_array<VkPipelineColorBlendAttachmentState> attachments(
      m_allocator, _create_info.m_color_attachments.size());
  bool has_blend_constants = false;
  if (has_rasterization && !_create_info.m_color_attachments.empty()) {
    color_blend_info = &color_info;

    for (size_t i = 0; i < _create_info.m_color_attachments.size(); ++i) {
      const auto& attachment = _create_info.m_color_attachments[i];

      attachments[i] = VkPipelineColorBlendAttachmentState{
          attachment.blend_enabled,                       // blendEnable
          blend_factor_to_vulkan(attachment.src_blend),   // srcColorBlendFactor
          blend_factor_to_vulkan(attachment.dst_blend),   // dstColorBlendFactor
          blend_op_to_vulkan(attachment.color_blend_op),  // colorBlendOp
          blend_factor_to_vulkan(
              attachment.src_blend_alpha),  // srcAlphaBlendFactor
          blend_factor_to_vulkan(
              attachment.dst_blend_alpha),                // dstAlphaBlendFactor
          blend_op_to_vulkan(attachment.alpha_blend_op),  // alphaBlendop
          write_components_to_vulkan(attachment.write_mask),  // colorWriteMask
      };
      if (attachment.src_blend == blend_factor::constant_color ||
          attachment.dst_blend == blend_factor::constant_color) {
        has_blend_constants = true;
      }
    }

    color_info = VkPipelineColorBlendStateCreateInfo{
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,  // sType
        nullptr,                                                   // pNext
        0,                                                         // flags
        _create_info.m_logic_op != logic_op::disabled,  // logicOpEnable
        logic_op_to_vulkan(_create_info.m_logic_op),    // logicOp
        static_cast<uint32_t>(attachments.size()),      // attachmentCount
        attachments.data(),                             // pAttachments
    };
    memory::memory_copy(&color_info.blendConstants[0],
        &_create_info.m_static_blend_constants[0], 4);
  }

  containers::dynamic_array<VkDynamicState> dynamic_states(m_allocator);
  dynamic_states.reserve(8);
  if (_create_info.m_static_viewports.empty() &&
      _create_info.m_num_viewports > 0) {
    dynamic_states.push_back(VK_DYNAMIC_STATE_VIEWPORT);
  }
  if (_create_info.m_static_scissors.empty() &&
      _create_info.m_num_scissors > 0) {
    dynamic_states.push_back(VK_DYNAMIC_STATE_SCISSOR);
  }
  if (!_create_info.m_has_static_blend_constants && has_blend_constants) {
    dynamic_states.push_back(VK_DYNAMIC_STATE_BLEND_CONSTANTS);
  }
  if (has_stencil_reference && !_create_info.m_has_static_stencil_reference) {
    dynamic_states.push_back(VK_DYNAMIC_STATE_STENCIL_REFERENCE);
  }

  VkPipelineDynamicStateCreateInfo* dynamic_state_info = nullptr;
  VkPipelineDynamicStateCreateInfo dynamic_state = {
      VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,  // sType
      nullptr,                                               // pNext
      0,                                                     // flags
      static_cast<uint32_t>(dynamic_states.size()), dynamic_states.data()};
  if (!dynamic_states.empty()) {
    dynamic_state_info = &dynamic_state;
  }

  const memory::unique_ptr<pipeline_layout_data>& layout = get_data(_layout);
  ::VkRenderPass renderpass = get_data(_renderpass);

  VkGraphicsPipelineCreateInfo pipeline_create_info = {
      VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,  // sType
      nullptr,                                          // pNext
      0,                            // flags TODO(awoloszyn): Maybe expose this?
      num_used_shader_stages,       // stageCount
      shader_stages.data(),         // pStages
      &vertex_input_state_info,     // pVertexInputState
      &input_assembly_create_info,  // pInputAssemblyState
      tessellation_create_info,     // pTessellationState
      viewport_create_info,         // pViewportState
      has_rasterization ? &rasterization_create_info
                        : nullptr,  // pRasterizationState
      multisample_create_info,      // pMultisampleState
      depth_create_info,            // pDepthStencilState
      color_blend_info,             // pColorBlendState
      dynamic_state_info,           // pDynamicState
      layout->layout,               // layout
      renderpass,                   // renderPass
      _subpass,                     // subpass
      VK_NULL_HANDLE,               // basePipelineHandle
      -1,                           // basePipelineIndex
  };

  graphics_pipeline_data& pipeline = get_data(_pipeline);
  if (VK_SUCCESS != vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1,
                        &pipeline_create_info, nullptr, &pipeline.pipeline)) {
    m_log->log_error("Could not create graphics pipeline");
  }
}

void vulkan_device::destroy_graphics_pipeline(graphics_pipeline* _pipeline) {
  graphics_pipeline_data& pipeline = get_data(_pipeline);
  vkDestroyPipeline(m_device, pipeline.pipeline, nullptr);
}

// buffer methods
bool vulkan_device::initialize_buffer(
    buffer* _buffer, const size_t _size, const resource_states _usage) {
  VkBufferUsageFlags usage = 0;

  if ((_usage & static_cast<resource_states>(resource_state::index_buffer))) {
    usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
  }

  if ((_usage & static_cast<resource_states>(resource_state::vertex_buffer)) !=
      0) {
    usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  }

  if ((_usage & static_cast<resource_states>(
                    resource_state::read_only_buffer)) != 0) {
    usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
  }

  if ((_usage & static_cast<resource_states>(
                    resource_state::read_write_buffer)) != 0) {
    usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
  }

  if ((_usage & static_cast<resource_states>(resource_state::copy_source)) !=
      0) {
    usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  }

  if ((_usage & static_cast<resource_states>(resource_state::copy_dest)) != 0) {
    usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
  }
  const VkBufferCreateInfo create_info = {
      VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,  // sType
      nullptr,                               // pNext
      0,                                     // flags
      _size,                                 // size
      usage,                                 // usage
      VK_SHARING_MODE_EXCLUSIVE,             // sharingMode
      0,                                     // queueFamilyIndexCount
      0                                      // pQueueFamilyIndices
  };
  VkBuffer new_buffer;

  if (vkCreateBuffer(m_device, &create_info, nullptr, &new_buffer) !=
      VK_SUCCESS) {
    return false;
  }

  memory::unique_ptr<buffer_info>& bdata = get_data(_buffer);

  bdata = memory::make_unique<buffer_info>(m_allocator);
  bdata->buffer = core::move(new_buffer);

  return true;
}

bool vulkan_device::bind_buffer(
    buffer* _buffer, arena* _arena, const uint64_t _offset) {
  memory::unique_ptr<buffer_info>& bdata = get_data(_buffer);
  const arena_data& adata = get_data(_arena);

  if (vkBindBufferMemory(m_device, bdata->buffer, adata.memory, _offset) !=
      VK_SUCCESS) {
    return false;
  }

  bdata->bound_arena = adata.memory;
  bdata->offset = _offset;
  bdata->arena_root = adata.root;

  return true;
}

void* vulkan_device::map_buffer(buffer* _buffer) {
  memory::unique_ptr<buffer_info>& bdata = get_data(_buffer);
  const VkMappedMemoryRange memory_range = {
      VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,  // sType
      nullptr,                                // pNext
      bdata->bound_arena,                     // memory
      bdata->offset,                          // offset
      VK_WHOLE_SIZE                           // size
  };

  if (vkInvalidateMappedMemoryRanges(m_device, 1, &memory_range) !=
      VK_SUCCESS) {
    m_log->log_warning("failed to invalidate device memory");

    return nullptr;
  }

  void* pointer = nullptr;

  if (bdata->arena_root) {
    pointer = static_cast<uint8_t*>(bdata->arena_root) + bdata->offset;
  }

  return pointer;
}

void vulkan_device::unmap_buffer(buffer* _buffer) {
  const memory::unique_ptr<buffer_info>& bdata = get_data(_buffer);

  const VkMappedMemoryRange memory_range = {
      VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,  // sType
      nullptr,                                // pNext
      bdata->bound_arena,                     // memory
      bdata->offset,                          // offset
      VK_WHOLE_SIZE                           // size
  };

  if (vkFlushMappedMemoryRanges(m_device, 1, &memory_range) != VK_SUCCESS) {
    m_log->log_warning("failed to flush device memory");
  }
}

void vulkan_device::destroy_buffer(buffer* _buffer) {
  memory::unique_ptr<buffer_info>& bdata = get_data(_buffer);

  vkDestroyBuffer(m_device, bdata->buffer, nullptr);

  bdata.reset();
}

queue_profiler_ptr vulkan_device::create_queue_profiler(
    queue* _queue, containers::string_view _name) {
  (void)_queue;
  (void)_name;
#ifdef TRACY_ENABLE
  vulkan_queue* queue = reinterpret_cast<vulkan_queue*>(_queue);

  auto profiler = memory::make_unique<vulkan_queue_profiler>(m_allocator);
  profiler->initialize(m_allocator, _name, this, queue, m_timestamp_period,
      &m_queue_profiler_context);
  return profiler;
#else
  return nullptr;
#endif
}

void vulkan_device::destroy_queue_profiler(queue_profiler* _profiler) {
  (void)_profiler;
#ifdef TRACY_ENABLE
  // DO this
#else
  WN_RELEASE_ASSERT(false, "Should never get here");
#endif
}

#undef get_data

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn
