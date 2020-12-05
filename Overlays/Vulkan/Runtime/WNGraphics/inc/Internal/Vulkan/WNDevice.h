// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_VULKAN_DEVICE_H__
#define __WN_GRAPHICS_INTERNAL_VULKAN_DEVICE_H__

#include "WNContainers/inc/WNDynamicArray.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNGraphics/inc/Internal/Vulkan/WNVulkanCommandListContext.h"
#include "WNGraphics/inc/Internal/Vulkan/WNVulkanContext.h"
#include "WNGraphics/inc/Internal/Vulkan/WNVulkanInclude.h"
#include "WNGraphics/inc/Internal/Vulkan/WNVulkanQueueContext.h"
#include "WNGraphics/inc/Internal/Vulkan/WNVulkanSurfaceHelper.h"
#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNAdapterFeatures.h"
#include "WNGraphics/inc/WNArenaProperties.h"
#include "WNGraphics/inc/WNDescriptorData.h"
#include "WNGraphics/inc/WNFramebufferData.h"
#include "WNGraphics/inc/WNGraphicsTypes.h"
#include "WNGraphics/inc/WNRenderPassTypes.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMath/inc/WNMatrix.h"
#include "WNMath/inc/WNVector.h"
#include "WNMemory/inc/unique_ptr.h"

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/WNDevice.h"
#else
#include "WNCore/inc/utilities.h"
#endif

#include <atomic>

namespace wn {
namespace memory {

class allocator;

}  // namespace memory

namespace runtime {
namespace graphics {

class arena;
class buffer;
class command_allocator;
class command_list;
class descriptor_set;
class descriptor_set_layout;
class descriptor_pool;
class pipeline_layout;
class fence;
class queue;
struct image_create_info;
class image;
class image_view;
class sampler;
struct sampler_create_info;
class signal;
class swapchain;
class shader_module;
class surface;
class render_pass;
class framebuffer;
class graphics_pipeline;
class graphics_pipeline_description;
struct swapchain_create_info;
struct descriptor_binding_info;
struct descriptor_pool_create_info;
struct image_memory_requirements;
struct buffer_memory_requirements;

using queue_ptr = memory::unique_ptr<queue>;
using command_list_ptr = memory::unique_ptr<command_list>;
using swapchain_ptr = memory::unique_ptr<swapchain>;

namespace internal {
namespace vulkan {

class vulkan_command_list;
class vulkan_adapter;

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
using vulkan_device_base = device;
#else
using vulkan_device_base = core::non_copyable;
#endif

template <typename T>
struct data_type;

class vulkan_device WN_GRAPHICS_FINAL : public vulkan_device_base {
public:
  ~vulkan_device() WN_GRAPHICS_OVERRIDE_FINAL;

  containers::contiguous_range<const arena_properties> get_arena_properties()
      const WN_GRAPHICS_OVERRIDE_FINAL;

  queue_ptr create_queue() WN_GRAPHICS_OVERRIDE_FINAL;

  size_t get_image_upload_buffer_alignment() WN_GRAPHICS_OVERRIDE_FINAL;
  size_t get_buffer_upload_buffer_alignment() WN_GRAPHICS_OVERRIDE_FINAL;

  swapchain_ptr create_swapchain(surface& _surface,
      const swapchain_create_info& _info,
      queue* queue) WN_GRAPHICS_OVERRIDE_FINAL;

  swapchain_ptr recreate_swapchain(surface& _surface,
      swapchain_ptr _old_swapchain, const swapchain_create_info& _info,
      queue* queue) WN_GRAPHICS_OVERRIDE_FINAL;

  math::mat44f get_perspective_fixup_matrix() WN_GRAPHICS_OVERRIDE_FINAL {
    return math::mat44f({         //
        1.0f, 0.0f, 0.0f, 0.0f,   //
        0.0f, -1.0f, 0.0f, 0.0f,  //
        0.0f, 0.0f, 1.0f, 0.0f,   //
        0.0, 0.0f, 0.0f, 1.0f});
  }

  containers::string_view get_shader_suffix() WN_GRAPHICS_OVERRIDE_FINAL {
    return containers::string_view(".spv");
  };

  math::vec2f get_2d_transform_scale() WN_GRAPHICS_OVERRIDE_FINAL {
    return math::vec2f({1.0f, -1.0f});
  }

protected:
  friend class graphics::arena;
  friend class graphics::buffer;
  friend class graphics::fence;
  friend class graphics::queue;
  friend class vulkan_queue;
  friend class vulkan_swapchain;
  friend class vulkan_adapter;
  friend class vulkan_command_list;

  inline vulkan_device()
    : vulkan_device_base(),
      m_device(VK_NULL_HANDLE),
      m_queue(VK_NULL_HANDLE),
      m_allocator(nullptr),
      m_log(nullptr) {}

  bool initialize(memory::allocator* _allocator, logging::log* _log,
      VkDevice _device, PFN_vkDestroyDevice _destroy_device,
      const VkPhysicalDeviceMemoryProperties* _memory_properties,
      const adapter_formats* _depth_formats, vulkan_context* _context,
      uint32_t _graphics_and_device_queue);

  // image methods
  void initialize_image(const image_create_info& _info,
      const clear_value& _optimized_clear,
      image* _image) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_image(image* _image) WN_GRAPHICS_OVERRIDE_FINAL;
  void bind_image_memory(image* _image, arena* _arena,
      uint64_t _offset) WN_GRAPHICS_OVERRIDE_FINAL;
  image_memory_requirements get_image_memory_requirements(
      const image* _image) WN_GRAPHICS_OVERRIDE_FINAL;

  // command allocator methods
  void initialize_command_allocator(
      command_allocator* _command_allocator) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_command_allocator(command_allocator*) WN_GRAPHICS_OVERRIDE_FINAL;
  void reset_command_allocator(
      command_allocator* _alloc) WN_GRAPHICS_OVERRIDE_FINAL;

  // fence methods
  void initialize_fence(fence* _fence) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_fence(fence* _fence) WN_GRAPHICS_OVERRIDE_FINAL;
  void wait_fence(const fence* _fence) const WN_GRAPHICS_OVERRIDE_FINAL;
  void reset_fence(fence* _fence) WN_GRAPHICS_OVERRIDE_FINAL;

  // signal methods
  void initialize_signal(signal* _signal) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_signal(signal* _signal) WN_GRAPHICS_OVERRIDE_FINAL;

  // queue methods
  void destroy_queue(queue* _queue) WN_GRAPHICS_OVERRIDE_FINAL;

  command_list_ptr create_command_list(
      command_allocator*) WN_GRAPHICS_OVERRIDE_FINAL;

  void initialize_shader_module(shader_module* s,
      const containers::contiguous_range<const uint8_t>& bytes)
      WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_shader_module(
      shader_module* shader_module) WN_GRAPHICS_OVERRIDE_FINAL;

  void initialize_descriptor_set_layout(descriptor_set_layout* _layout,
      const containers::contiguous_range<const descriptor_binding_info>&
          _binding_infos) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_descriptor_set_layout(
      descriptor_set_layout* _layout) WN_GRAPHICS_OVERRIDE_FINAL;

  void initialize_descriptor_pool(descriptor_pool* _pool,
      const containers::contiguous_range<const descriptor_pool_create_info>&
          _pool_data) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_descriptor_pool(
      descriptor_pool* _pool) WN_GRAPHICS_OVERRIDE_FINAL;

  void initialize_descriptor_set(descriptor_set* _set, descriptor_pool* _pool,
      const descriptor_set_layout* _pool_data) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_descriptor_set(descriptor_set* _set) WN_GRAPHICS_OVERRIDE_FINAL;

  void update_descriptors(descriptor_set* _set,
      const containers::contiguous_range<buffer_descriptor>& _buffer_updates,
      const containers::contiguous_range<image_descriptor>& _image_updates,
      const containers::contiguous_range<sampler_descriptor>& _sampler_updates)
      WN_GRAPHICS_OVERRIDE_FINAL;

  void initialize_pipeline_layout(pipeline_layout* _layout,
      const containers::contiguous_range<const descriptor_set_layout*>&
          _descriptor_sets,
      const containers::contiguous_range<const push_constant_range>&
          _push_constants) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_pipeline_layout(
      pipeline_layout* _layout) WN_GRAPHICS_OVERRIDE_FINAL;

  void initialize_render_pass(render_pass* _pass,
      const containers::contiguous_range<const render_pass_attachment>&
          _attachments,
      const containers::contiguous_range<const subpass_description>& _subpasses,
      const containers::contiguous_range<const subpass_dependency>& _deps)
      WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_render_pass(render_pass* _pass) WN_GRAPHICS_OVERRIDE_FINAL;

  void initialize_image_view(image_view* _view, const image* image,
      uint32_t _base_mip_level,
      uint32_t _num_mip_levels) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_image_view(image_view* _view) WN_GRAPHICS_OVERRIDE_FINAL;

  void initialize_sampler(const sampler_create_info& _info,
      sampler* _sampler) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_sampler(sampler* _sampler) WN_GRAPHICS_OVERRIDE_FINAL;

  // arena methods
  bool initialize_arena(arena* _arena, const size_t _index,
      const uint64_t _size,
      const bool _multisampled) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_arena(arena* _arena) WN_GRAPHICS_OVERRIDE_FINAL;

  void initialize_framebuffer(framebuffer* _framebuffer,
      const framebuffer_create_info& _info) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_framebuffer(
      framebuffer* _framebuffer) WN_GRAPHICS_OVERRIDE_FINAL;

  void initialize_graphics_pipeline(graphics_pipeline* _pipeline,
      const graphics_pipeline_description& _create_info,
      const pipeline_layout* _layout, const render_pass* _renderpass,
      uint32_t _subpass) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_graphics_pipeline(
      graphics_pipeline* _pipeline) WN_GRAPHICS_OVERRIDE_FINAL;

  // buffer methods
  bool initialize_buffer(buffer* _buffer, const size_t _size,
      const resource_states _usage) WN_GRAPHICS_OVERRIDE_FINAL;
  bool bind_buffer(buffer* _buffer, arena* _arena,
      const uint64_t _offset) WN_GRAPHICS_OVERRIDE_FINAL;
  void* map_buffer(buffer* _buffer) WN_GRAPHICS_OVERRIDE_FINAL;
  void unmap_buffer(buffer* _buffer) WN_GRAPHICS_OVERRIDE_FINAL;
  void destroy_buffer(buffer* _buffer) WN_GRAPHICS_OVERRIDE_FINAL;
  buffer_memory_requirements get_buffer_memory_requirements(
      const buffer* _buffer) WN_GRAPHICS_OVERRIDE_FINAL;

  uint32_t get_memory_type_index(uint32_t _types, VkFlags _properties) const;

  PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;
  PFN_vkGetDeviceQueue vkGetDeviceQueue;
  PFN_vkDestroyDevice vkDestroyDevice;

  PFN_vkAllocateMemory vkAllocateMemory;
  PFN_vkFreeMemory vkFreeMemory;

  // Images
  PFN_vkCreateImage vkCreateImage;
  PFN_vkDestroyImage vkDestroyImage;
  PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements;
  PFN_vkBindImageMemory vkBindImageMemory;

  // Samplers
  PFN_vkCreateSampler vkCreateSampler;
  PFN_vkDestroySampler vkDestroySampler;

  // Buffers
  PFN_vkCreateBuffer vkCreateBuffer;
  PFN_vkDestroyBuffer vkDestroyBuffer;
  PFN_vkBindBufferMemory vkBindBufferMemory;
  PFN_vkMapMemory vkMapMemory;
  PFN_vkUnmapMemory vkUnmapMemory;

  // Fences
  PFN_vkCreateFence vkCreateFence;
  PFN_vkDestroyFence vkDestroyFence;
  PFN_vkWaitForFences vkWaitForFences;
  PFN_vkResetFences vkResetFences;

  // Semaphores
  PFN_vkCreateSemaphore vkCreateSemaphore;
  PFN_vkDestroySemaphore vkDestroySemaphore;

  PFN_vkCreateCommandPool vkCreateCommandPool;
  PFN_vkDestroyCommandPool vkDestroyCommandPool;
  PFN_vkResetCommandPool vkResetCommandPool;

  PFN_vkFlushMappedMemoryRanges vkFlushMappedMemoryRanges;
  PFN_vkInvalidateMappedMemoryRanges vkInvalidateMappedMemoryRanges;

  PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers;
  PFN_vkFreeCommandBuffers vkFreeCommandBuffers;

  PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements;
  PFN_vkBeginCommandBuffer vkBeginCommandBuffer;

  // Swapchain
  PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR;
  PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR;
  PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR;
  PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR;

  // Shaders
  PFN_vkCreateShaderModule vkCreateShaderModule;
  PFN_vkDestroyShaderModule vkDestroyShaderModule;

  // Descriptor Pool
  PFN_vkCreateDescriptorPool vkCreateDescriptorPool;
  PFN_vkDestroyDescriptorPool vkDestroyDescriptorPool;

  // Descriptor Set
  PFN_vkAllocateDescriptorSets vkAllocateDescriptorSets;
  PFN_vkFreeDescriptorSets vkFreeDescriptorSets;
  PFN_vkUpdateDescriptorSets vkUpdateDescriptorSets;

  // Descriptor Set Layout
  PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout;
  PFN_vkDestroyDescriptorSetLayout vkDestroyDescriptorSetLayout;

  // Pipeline Layout
  PFN_vkCreatePipelineLayout vkCreatePipelineLayout;
  PFN_vkDestroyPipelineLayout vkDestroyPipelineLayout;

  // Render Pass
  PFN_vkCreateRenderPass vkCreateRenderPass;
  PFN_vkDestroyRenderPass vkDestroyRenderPass;

private:
  bool setup_arena_properties();

  containers::dynamic_array<arena_properties> m_arena_properties;

  // Image Views
  PFN_vkCreateImageView vkCreateImageView;
  PFN_vkDestroyImageView vkDestroyImageView;

  // Framebuffers
  PFN_vkCreateFramebuffer vkCreateFramebuffer;
  PFN_vkDestroyFramebuffer vkDestroyFramebuffer;

  // Graphics Pipeline
  PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines;
  PFN_vkDestroyPipeline vkDestroyPipeline;

  queue_context m_queue_context;
  command_list_context m_command_list_context;
  VkDevice m_device;
  std::atomic<VkQueue> m_queue;
  const VkPhysicalDeviceMemoryProperties* m_physical_device_memory_properties;
  const adapter_formats* m_supported_formats;
  memory::allocator* m_allocator;
  logging::log* m_log;
};

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_VULKAN_DEVICE_H__
