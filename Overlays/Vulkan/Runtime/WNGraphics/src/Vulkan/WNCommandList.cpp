// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/Internal/Vulkan/WNCommandList.h"
#include "WNGraphics/inc/Internal/Vulkan/WNBufferData.h"
#include "WNGraphics/inc/Internal/Vulkan/WNDevice.h"
#include "WNGraphics/inc/Internal/Vulkan/WNResourceStates.h"
#include "WNGraphics/inc/Internal/Vulkan/WNVulkanCommandListContext.h"
#include "WNGraphics/inc/WNHeap.h"
#include "WNGraphics/inc/WNImage.h"

namespace wn {
namespace graphics {
namespace internal {
namespace vulkan {

vulkan_command_list::~vulkan_command_list() {
  m_context->vkFreeCommandBuffers(
      m_context->m_device->m_device, m_command_pool, 1, &m_command_buffer);
}

void vulkan_command_list::enqueue_upload_barrier(
    const upload_heap& _upload_heap, size_t _offset_in_bytes, size_t _size) {
  const buffer_data& data = _upload_heap.data_as<buffer_data>();

  const VkBufferMemoryBarrier buffer_barrier{
      VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,  // sType
      nullptr,                                  // pNext
      VK_ACCESS_HOST_WRITE_BIT,                 // srcAccessMask
      VK_ACCESS_TRANSFER_READ_BIT,              // dstAccessMask
      0,                                        // srcQueueFamilyIndex
      0,                                        // dstQueueFamilyIndex
      data.buffer,                              // buffer
      _offset_in_bytes,                         // offset
      _size                                     // size
  };

  m_context->vkCmdPipelineBarrier(m_command_buffer, VK_PIPELINE_STAGE_HOST_BIT,
      VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, 0, 1, &buffer_barrier, 0, 0);
}

void vulkan_command_list::enqueue_download_barrier(
    const download_heap& _download_heap, size_t _offset_in_bytes,
    size_t _size) {
  const buffer_data& data = _download_heap.data_as<buffer_data>();

  const VkBufferMemoryBarrier buffer_barrier{
      VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,  // sType
      nullptr,                                  // pNext
      VK_ACCESS_TRANSFER_WRITE_BIT,             // srcAccessMask
      VK_ACCESS_HOST_READ_BIT,                  // dstAccessMask
      0,                                        // srcQueueFamilyIndex
      0,                                        // dstQueueFamilyIndex
      data.buffer,                              // buffer
      _offset_in_bytes,                         // offset
      _size                                     // size
  };

  m_context->vkCmdPipelineBarrier(m_command_buffer,
      VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_HOST_BIT, 0, 0, 0, 1,
      &buffer_barrier, 0, 0);
}

void vulkan_command_list::enqueue_buffer_copy(const upload_heap& _upload_heap,
    size_t _upload_offset_in_bytes, const download_heap& _download_heap,
    size_t _download_offset_in_bytes, size_t _upload_size) {
  const VkBufferCopy copy{
      _upload_offset_in_bytes,    // srcOffset
      _download_offset_in_bytes,  // dstOffset
      _upload_size,               // size
  };

  const buffer_data& upload_data = _upload_heap.data_as<buffer_data>();
  const buffer_data& download_data = _download_heap.data_as<buffer_data>();

  m_context->vkCmdCopyBuffer(
      m_command_buffer, upload_data.buffer, download_data.buffer, 1, &copy);
}

void vulkan_command_list::finalize() {
  m_context->vkEndCommandBuffer(m_command_buffer);
}

void vulkan_command_list::enqueue_resource_transition(
    const image& _image, resource_state _from, resource_state _to) {
  const VkImage& image_res = _image.data_as<VkImage>();

  VkImageMemoryBarrier barrier{
      VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,        // sType
      nullptr,                                       // pNext
      resource_state_to_vulkan_access_flags(_from),  // srcAccessMask
      resource_state_to_vulkan_access_flags(_to),    // dstAccessMask
      resource_state_to_vulkan_layout(_from),        // oldLayout
      resource_state_to_vulkan_layout(_to),          // newLayout
      VK_QUEUE_FAMILY_IGNORED,                       // srcQueueFamilyIndex
      VK_QUEUE_FAMILY_IGNORED,                       // dstQueueFamilyIndex
      image_res,                                     // image
      VkImageSubresourceRange{
          VK_IMAGE_ASPECT_COLOR_BIT,  // aspectMask
          0,                          // baseMipLevel
          1,                          // levelCount
          0,                          // baseArrayLayer
          1,                          // layerCount
      }                               // subresourceRange
  };

  m_context->vkCmdPipelineBarrier(m_command_buffer,
      resource_state_to_vulkan_pipeline_stage(_from),
      resource_state_to_vulkan_pipeline_stage(_to), 0, 0, 0, 0, 0, 1, &barrier);
}

void vulkan_command_list::enqueue_texture_upload(
    const upload_heap& _upload_heap, size_t _upload_offset_in_bytes,
    const image& _image) {
  const buffer_data& data = _upload_heap.data_as<buffer_data>();
  const VkImage& image_res = _image.data_as<VkImage>();

  VkBufferImageCopy copy{
      _upload_offset_in_bytes,  // bufferOffset
      0,                        // bufferRowLength
      0,                        // bufferImageHeight
      VkImageSubresourceLayers{
          VK_IMAGE_ASPECT_COLOR_BIT,  // aspectMask
          0,                          // mipLevel
          0,                          // baseArrayLayer
          1,                          // layerCount
      },                              // imageSubresource
      VkOffset3D{0, 0, 0},            // imageOffset
      VkExtent3D{static_cast<uint32_t>(_image.get_width()),
          static_cast<uint32_t>(_image.get_height()), 0},  // imageExtent
  };

  m_context->vkCmdCopyBufferToImage(m_command_buffer, data.buffer, image_res,
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
}

void vulkan_command_list::enqueue_texture_download(const image& _image,
    const download_heap& _download_heap, size_t _download_offset_in_bytes) {
  const buffer_data& data = _download_heap.data_as<buffer_data>();
  const VkImage& image_res = _image.data_as<VkImage>();

  VkBufferImageCopy copy{
      _download_offset_in_bytes,  // bufferOffset
      0,                          // bufferRowLength
      0,                          // bufferImageHeight
      VkImageSubresourceLayers{
          VK_IMAGE_ASPECT_COLOR_BIT,  // aspectMask
          0,                          // mipLevel
          0,                          // baseArrayLayer
          1,                          // layerCount
      },                              // imageSubresource
      VkOffset3D{0, 0, 0},            // imageOffset
      VkExtent3D{static_cast<uint32_t>(_image.get_width()),
          static_cast<uint32_t>(_image.get_height()), 0},  // imageExtent
  };

  m_context->vkCmdCopyImageToBuffer(m_command_buffer, image_res,
      VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, data.buffer, 1, &copy);
}

}  // namespace vulkan
}  // namesapce internal
}  // namespace graphics
}  // namespace wn
