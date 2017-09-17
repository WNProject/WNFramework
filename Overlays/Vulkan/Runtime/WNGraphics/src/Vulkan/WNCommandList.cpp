// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/Internal/Vulkan/WNCommandList.h"
#include "WNGraphics/inc/Internal/Vulkan/WNBufferData.h"
#include "WNGraphics/inc/Internal/Vulkan/WNDataTypes.h"
#include "WNGraphics/inc/Internal/Vulkan/WNDevice.h"
#include "WNGraphics/inc/Internal/Vulkan/WNResourceStates.h"
#include "WNGraphics/inc/Internal/Vulkan/WNVulkanCommandListContext.h"
#include "WNGraphics/inc/WNBuffer.h"
#include "WNGraphics/inc/WNFramebuffer.h"
#include "WNGraphics/inc/WNImage.h"
#include "WNGraphics/inc/WNRenderPass.h"

namespace wn {
namespace runtime {
namespace graphics {
namespace internal {
namespace vulkan {

#define get_data(f)                                                            \
  (f)->data_as<                                                                \
      typename data_type<core::remove_pointer<decltype(f)>::type>::value>()

vulkan_command_list::~vulkan_command_list() {
  m_context->vkFreeCommandBuffers(
      m_context->m_device->m_device, m_command_pool, 1, &m_command_buffer);
}

void vulkan_command_list::finalize() {
  m_context->vkEndCommandBuffer(m_command_buffer);
}

void vulkan_command_list::transition_resource(
    const image& _image, resource_state _from, resource_state _to) {
  const VkImage& image_res = get_data((&_image));

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
      resource_state_to_vulkan_pipeline_stage(_to), 0, 0, nullptr, 0, nullptr,
      1, &barrier);
}

void vulkan_command_list::transition_resource(
    const buffer& _buffer, resource_state _from, resource_state _to) {
  const memory::unique_ptr<const buffer_info>& buffer = get_data(&_buffer);
  VkBufferMemoryBarrier barrier{
      VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,       // sType
      nullptr,                                       // pNext
      resource_state_to_vulkan_access_flags(_from),  // srcAccessMask
      resource_state_to_vulkan_access_flags(_to),    // dstAccessMask
      VK_QUEUE_FAMILY_IGNORED,                       // srcQueueFamilyIndex
      VK_QUEUE_FAMILY_IGNORED,                       // dstQueueFamilyIndex
      buffer->buffer,                                // buffer
      0,                                             // offset
      _buffer.size()                                 // size
  };
  m_context->vkCmdPipelineBarrier(m_command_buffer,
      resource_state_to_vulkan_pipeline_stage(_from),
      resource_state_to_vulkan_pipeline_stage(_to), 0, 0, nullptr, 1, &barrier,
      0, nullptr);
}

void vulkan_command_list::copy_buffer(const buffer& _src_buffer,
    size_t _src_offset, const buffer& _dst_buffer, size_t _dst_offset,
    size_t _size) {
  VkBufferCopy copy = {_src_offset, _dst_offset, _size};

  const memory::unique_ptr<const buffer_info>& srcbuffer =
      get_data(&_src_buffer);
  const memory::unique_ptr<const buffer_info>& dstbuffer =
      get_data(&_dst_buffer);
  m_context->vkCmdCopyBuffer(
      m_command_buffer, srcbuffer->buffer, dstbuffer->buffer, 1, &copy);
}

void vulkan_command_list::copy_buffer_to_image(const buffer& _src_buffer,
    size_t _src_offset_in_bytes, const image& _dst_image) {
  const VkImage& dst_image = get_data(&_dst_image);
  const memory::unique_ptr<const buffer_info>& srcbuffer =
      get_data(&_src_buffer);

  VkBufferImageCopy copy{
      _src_offset_in_bytes,                            // bufferOffset
      static_cast<uint32_t>(_dst_image.get_width()),   // bufferRowLength
      static_cast<uint32_t>(_dst_image.get_height()),  // bufferImageHeight
      VkImageSubresourceLayers{
          VK_IMAGE_ASPECT_COLOR_BIT,  // aspectMask
          0,                          // mipLevel
          0,                          // baseArrayLayer
          1,                          // layerCount
      },                              // imageSubresource
      VkOffset3D{0, 0, 0},            // imageOffset
      VkExtent3D{static_cast<uint32_t>(_dst_image.get_width()),
          static_cast<uint32_t>(_dst_image.get_height()), 1},  // imageExtent
  };

  m_context->vkCmdCopyBufferToImage(m_command_buffer, srcbuffer->buffer,
      dst_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
}

void vulkan_command_list::copy_image_to_buffer(const image& _image,
    const buffer& _dst_buffer, size_t _buffer_offset_in_bytes) {
  const VkImage& image = get_data(&_image);
  const memory::unique_ptr<const buffer_info>& dstbuffer =
      get_data(&_dst_buffer);

  VkBufferImageCopy copy{
      _buffer_offset_in_bytes,                     // bufferOffset
      static_cast<uint32_t>(_image.get_width()),   // bufferRowLength
      static_cast<uint32_t>(_image.get_height()),  // bufferImageHeight
      VkImageSubresourceLayers{
          VK_IMAGE_ASPECT_COLOR_BIT,  // aspectMask
          0,                          // mipLevel
          0,                          // baseArrayLayer
          1,                          // layerCount
      },                              // imageSubresource
      VkOffset3D{0, 0, 0},            // imageOffset
      VkExtent3D{static_cast<uint32_t>(_image.get_width()),
          static_cast<uint32_t>(_image.get_height()), 1},  // imageExtent
  };

  m_context->vkCmdCopyImageToBuffer(m_command_buffer, image,
      VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstbuffer->buffer, 1, &copy);
}

void vulkan_command_list::draw(uint32_t _vertex_count, uint32_t _instance_count,
    uint32_t _vertex_offset, uint32_t _instance_offset) {
  m_context->vkCmdDraw(m_command_buffer, _vertex_count, _instance_count,
      _vertex_offset, _instance_offset);
}

void vulkan_command_list::set_scissor(const scissor& _scissor) {
  VkRect2D scissor{{_scissor.x, _scissor.y}, {_scissor.width, _scissor.height}};
  m_context->vkCmdSetScissor(m_command_buffer, 0, 1, &scissor);
}

void vulkan_command_list::begin_render_pass(render_pass* _pass,
    framebuffer* _framebuffer, const render_area& _area,
    const containers::contiguous_range<clear_value>& _clears) {
  static_assert(sizeof(clear_value) == sizeof(VkClearValue),
      "You have to translate clear_value to VkClearValue");
  ::VkRenderPass render_pass = get_data(_pass);
  ::VkFramebuffer framebuffer = get_data(_framebuffer);
  VkRect2D render_area = {_area.x, _area.y, _area.width, _area.height};
  VkRenderPassBeginInfo begin_info = {
      VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,              // sType
      nullptr,                                               // pNext
      render_pass,                                           // renderPass
      framebuffer,                                           // framebuffer
      render_area,                                           // renderArea
      static_cast<uint32_t>(_clears.size()),                 // numClears
      reinterpret_cast<const VkClearValue*>(_clears.data())  // clears
  };
  m_context->vkCmdBeginRenderPass(
      m_command_buffer, &begin_info, VK_SUBPASS_CONTENTS_INLINE);
}

void vulkan_command_list::end_render_pass() {
  m_context->vkCmdEndRenderPass(m_command_buffer);
}

void vulkan_command_list::bind_graphics_descriptor_sets(
    const containers::contiguous_range<const descriptor_set*> _sets,
    uint32_t _base_index) {
  containers::dynamic_array<::VkDescriptorSet> sets(m_allocator, _sets.size());
  ::VkPipelineLayout layout = get_data(m_current_graphics_pipeline_layout);
  uint32_t i = 0;
  for (const descriptor_set* set : _sets) {
    sets[i++] = get_data(set).set;
  }

  m_context->vkCmdBindDescriptorSets(m_command_buffer,
      VK_PIPELINE_BIND_POINT_GRAPHICS, layout, _base_index,
      static_cast<uint32_t>(sets.size()), sets.data(), 0, nullptr);
}

void vulkan_command_list::bind_graphics_pipeline(graphics_pipeline* _pipeline) {
  graphics_pipeline_data& pipeline = get_data(_pipeline);
  m_context->vkCmdBindPipeline(
      m_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline);
}

void vulkan_command_list::bind_vertex_buffer(
    uint32_t stream, const buffer* _buffer) {
  const memory::unique_ptr<const buffer_info>& buffer = get_data(_buffer);
  const VkDeviceSize offset = 0;
  m_context->vkCmdBindVertexBuffers(
      m_command_buffer, stream, 1, &buffer->buffer, &offset);
}

#undef get_data

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn
