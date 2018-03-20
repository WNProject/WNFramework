// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/Internal/Vulkan/WNSwapchain.h"
#include "WNGraphics/inc/Internal/Vulkan/WNDataTypes.h"
#include "WNGraphics/inc/Internal/Vulkan/WNDevice.h"
#include "WNGraphics/inc/Internal/Vulkan/WNQueue.h"
#include "WNGraphics/inc/WNFence.h"
#include "WNGraphics/inc/WNImage.h"
#include "WNGraphics/inc/WNSwapchain.h"

namespace wn {
namespace runtime {
namespace graphics {
namespace internal {
namespace vulkan {

#define get_data(f)                                                            \
  f->data_as<                                                                  \
      typename data_type<core::remove_pointer<decltype(f)>::type>::value>()

void vulkan_swapchain::initialize(memory::allocator* _allocator,
    vulkan_device* _device, uint32_t _width, uint32_t _height,
    const swapchain_create_info& _create_info, VkSwapchainKHR swapchain) {
  m_allocator = _allocator;
  m_device = _device;
  m_swapchain = swapchain;
  m_images = containers::dynamic_array<image>(m_allocator);
  uint32_t num_buffers = _create_info.num_buffers;
  m_images.reserve(_create_info.num_buffers);
  containers::dynamic_array<VkImage> images(
      m_allocator, _create_info.num_buffers);
  m_device->vkGetSwapchainImagesKHR(
      m_device->m_device, m_swapchain, &num_buffers, images.data());
  for (size_t i = 0; i < num_buffers; ++i) {
    m_images.push_back(image(reinterpret_cast<device*>(m_device)));
    image* image = &m_images.back();
    image->m_resource_info =
        containers::dynamic_array<image::image_buffer_resource_info>(
            _allocator, 1);
    image->m_resource_info[0].depth = 1;
    image->m_resource_info[0].height = _height;
    image->m_resource_info[0].width = _width;
    image->m_resource_info[0].offset_in_bytes = 0;
    image->m_resource_info[0].row_pitch_in_bytes = 0;
    image->m_resource_info[0].total_memory_required = 0;
    image->m_resource_info[0].format = _create_info.format;
    image->m_is_swapchain_image = true;
    ::VkImage& img = get_data(image);
    img = images[i];
  }
}

vulkan_swapchain::~vulkan_swapchain() {
  if (m_device) {
    m_device->vkDestroySwapchainKHR(m_device->m_device, m_swapchain, nullptr);
  }
}

uint32_t vulkan_swapchain::get_next_backbuffer_index(
    fence* fence, signal* _signal) const {
  uint32_t idx;
  ::VkFence f = fence ? get_data(fence) : VK_NULL_HANDLE;
  ::VkSemaphore s = _signal ? get_data(_signal) : VK_NULL_HANDLE;
  m_device->vkAcquireNextImageKHR(
      m_device->m_device, m_swapchain, 0xFFFFFFFFFFFFFFFF, s, f, &idx);
  return idx;
}

image* vulkan_swapchain::get_image_for_index(uint32_t index) {
  WN_DEBUG_ASSERT(index < m_images.size(), "Invalid image index");
  return &m_images[index];
}

void vulkan_swapchain::present_internal(queue* q, const swapchain_create_info&,
    signal* _signal, uint32_t index) const {
  ::VkSemaphore s = _signal ? get_data(_signal) : VK_NULL_HANDLE;
  vulkan_queue* queue = reinterpret_cast<vulkan_queue*>(q);
  queue->present(m_swapchain, index, s);
}

#undef get_data

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn
