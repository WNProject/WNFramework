// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_RUNTIME_GRAPHICS_DEVICE_H__
#define __WN_RUNTIME_GRAPHICS_DEVICE_H__

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNArenaProperties.h"
#include "WNGraphics/inc/WNDescriptorData.h"
#include "WNGraphics/inc/WNFramebufferData.h"
#include "WNGraphics/inc/WNGraphicsEnums.h"
#include "WNGraphics/inc/WNGraphicsPipelineDescription.h"
#include "WNGraphics/inc/WNGraphicsTypes.h"
#include "WNGraphics/inc/WNRenderPassTypes.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMath/inc/WNMatrix.h"
#include "WNMath/inc/WNVector.h"
#include "WNMemory/inc/unique_ptr.h"

#ifdef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/Internal/WNDeviceIncludes.h"
#else
#include "core/inc/utilities.h"
#endif

WN_GRAPHICS_FORWARD(device);
WN_GRAPHICS_FORWARD(queue);
WN_GRAPHICS_FORWARD(adapter);
WN_GRAPHICS_FORWARD(image);
WN_GRAPHICS_FORWARD(sampler);
WN_GRAPHICS_FORWARD(surface);
WN_GRAPHICS_FORWARD(swapchain);

namespace wn {
namespace runtime {
namespace window {

class window;

}  // namespace window

namespace graphics {
namespace internal {

#ifdef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
using device_base = _WN_GRAPHICS_DEVICE_TYPE::WN_GRAPHICS_PREFIXED_TYPE(device);
#else
using device_base = core::non_copyable;
#endif

}  // namespace internal

class arena;
class buffer;
class image;
class command_allocator;
class command_list;
class fence;
class framebuffer;
class image_view;
class shader_module;
class descriptor_set_layout;
class descriptor_pool;
class descriptor_set;
class pipeline_layout;
class render_pass;
class sampler;
class signal;
class graphics_pipeline;
class graphics_pipeline_description;

struct buffer_memory_requirements;
struct image_memory_requirements;
struct image_create_info;
struct swapchain_create_info;
struct sampler_create_info;

template <typename HeapTraits>
class heap;

class queue;
class swapchain;

using command_list_ptr = memory::unique_ptr<command_list>;
using queue_ptr = memory::unique_ptr<queue>;
using swapchain_ptr = memory::unique_ptr<swapchain>;

class device : public internal::device_base {
public:
  inline device() : internal::device_base() {}

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
  virtual ~device() = default;

  virtual containers::contiguous_range<const arena_properties>
  get_arena_properties() const = 0;

  // It is only valid to have a single queue active at a time.
  virtual queue_ptr create_queue() = 0;

  virtual swapchain_ptr create_swapchain(
      surface& _surface, const swapchain_create_info& _info, queue* queue) = 0;

  virtual swapchain_ptr recreate_swapchain(surface& _surface,
      swapchain_ptr _old_swapchain, const swapchain_create_info& _info,
      queue* queue) = 0;
  virtual size_t get_image_upload_buffer_alignment() = 0;
  virtual size_t get_buffer_upload_buffer_alignment() = 0;
#endif

  command_allocator create_command_allocator();
  fence create_fence();

  signal create_signal();

  arena create_arena(const size_t _index, const uint64_t _size);
  arena create_arena(
      const size_t _index, const uint64_t _size, const bool _multisampled);

  // The contiguous range must be 32-bit aligned for
  // compatibility.
  shader_module create_shader_module(
      const containers::contiguous_range<const uint8_t>& bytes);

  image create_image(
      const image_create_info& _info, const clear_value& _optimized_clear);

  sampler create_sampler(const sampler_create_info& _info);

  descriptor_set_layout create_descriptor_set_layout(
      const containers::contiguous_range<const descriptor_binding_info>&
          _layout);

  descriptor_pool create_descriptor_pool(
      const containers::contiguous_range<const descriptor_pool_create_info>&
          _pool_data);

  pipeline_layout create_pipeline_layout(
      const containers::contiguous_range<const descriptor_set_layout*>&
          _descriptor_sets,
      const containers::contiguous_range<const push_constant_range>&
          _push_constants);

  render_pass create_render_pass(
      const containers::contiguous_range<const render_pass_attachment>&
          _attachments,
      const containers::contiguous_range<const subpass_description>& _subpasses,
      const containers::contiguous_range<const subpass_dependency>& _deps);

  image_view create_image_view(const image* _image, uint32_t _base_mip_level,
      uint32_t _num_mip_levels, image_components _components);

  framebuffer create_framebuffer(const framebuffer_create_info& create_info);

  // TODO(awoloszyn): Plumb the pipeline cache through here
  graphics_pipeline create_graphics_pipeline(
      const graphics_pipeline_description& create_info,
      const pipeline_layout* _layout, const render_pass* _renderpass,
      uint32_t _subpass);

  buffer create_buffer(const size_t _size, const resource_states _usage);

protected:
  friend class command_allocator;
  friend class fence;
  friend class image;
  friend class arena;
  friend class buffer;
  friend class sampler;
  friend class signal;

  template <typename HeapTraits>
  friend class heap;

  WN_GRAPHICS_ADD_FRIENDS(queue);
  WN_GRAPHICS_ADD_FRIENDS(adapter);
  WN_GRAPHICS_ADD_FRIENDS(swapchain);

  friend class shader_module;
  friend class descriptor_set_layout;
  friend class descriptor_pool;
  friend class descriptor_set;
  friend class pipeline_layout;
  friend class render_pass;
  friend class image_view;
  friend class framebuffer;
  friend class graphics_pipeline;

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
public:
  virtual math::mat44f get_perspective_fixup_matrix() = 0;
  virtual containers::string_view get_shader_suffix() = 0;
  virtual math::vec2f get_2d_transform_scale() = 0;

private:
  // Destruction methods
  virtual void destroy_queue(queue* _queue) = 0;

  // command allocator methods
  virtual void initialize_command_allocator(
      command_allocator* _command_allocator) = 0;
  virtual void destroy_command_allocator(
      command_allocator* _command_allocator) = 0;
  virtual void reset_command_allocator(command_allocator* _alloc) = 0;

  // command list methods
  virtual command_list_ptr create_command_list(
      command_allocator* _command_allocator) = 0;

  // fence methods
  virtual void initialize_fence(fence* _fence) = 0;
  virtual void destroy_fence(fence* _fence) = 0;
  virtual void wait_fence(const fence* _fence) const = 0;
  virtual void reset_fence(fence* _fence) = 0;

  // signal methods
  virtual void initialize_signal(signal* _signal) = 0;
  virtual void destroy_signal(signal* _signal) = 0;

  // Image methods
  virtual void initialize_image(const image_create_info& _info,
      const clear_value& _optimized_clear, image* _image) = 0;
  virtual void destroy_image(image* _image) = 0;
  virtual image_memory_requirements get_image_memory_requirements(
      const image* _image) = 0;

  virtual void bind_image_memory(
      image* _image, arena* _arena, uint64_t _offset) = 0;

  virtual void initialize_sampler(
      const sampler_create_info& _info, sampler* _sampler) = 0;
  virtual void destroy_sampler(sampler* _sampler) = 0;

  // Shader methods
  virtual void initialize_shader_module(shader_module* s,
      const containers::contiguous_range<const uint8_t>& bytes) = 0;
  virtual void destroy_shader_module(shader_module* shader_module) = 0;

  // Descriptor set layout
  virtual void initialize_descriptor_set_layout(descriptor_set_layout* _layout,
      const containers::contiguous_range<const descriptor_binding_info>&
          _binding_infos) = 0;
  virtual void destroy_descriptor_set_layout(
      descriptor_set_layout* _layout) = 0;

  // Descriptor pool
  virtual void initialize_descriptor_pool(descriptor_pool* _pool,
      const containers::contiguous_range<const descriptor_pool_create_info>&
          _pool_data) = 0;
  virtual void destroy_descriptor_pool(descriptor_pool* _pool) = 0;

  // Descriptor set
  virtual void initialize_descriptor_set(descriptor_set* _set,
      descriptor_pool* _pool, const descriptor_set_layout* _pool_data) = 0;
  virtual void destroy_descriptor_set(descriptor_set* _set) = 0;

  virtual void update_descriptors(descriptor_set* _set,
      const containers::contiguous_range<buffer_descriptor>& _buffer_updates,
      const containers::contiguous_range<image_descriptor>& _image_updates,
      const containers::contiguous_range<sampler_descriptor>&
          _sampler_updates) = 0;

  // Pipeline layout
  virtual void initialize_pipeline_layout(pipeline_layout* _layout,
      const containers::contiguous_range<const descriptor_set_layout*>&
          _descriptor_sets,
      const containers::contiguous_range<const push_constant_range>&
          _push_constants) = 0;
  virtual void destroy_pipeline_layout(pipeline_layout* _layout) = 0;

  // Render pass
  virtual void initialize_render_pass(render_pass* _pass,
      const containers::contiguous_range<const render_pass_attachment>&
          _attachments,
      const containers::contiguous_range<const subpass_description>& _subpasses,
      const containers::contiguous_range<const subpass_dependency>& _deps) = 0;
  virtual void destroy_render_pass(render_pass* _pass) = 0;

  // Image View
  virtual void initialize_image_view(image_view* _view, const image* image,
      uint32_t _base_mip_level, uint32_t _num_mip_levels) = 0;
  virtual void destroy_image_view(image_view* _view) = 0;

  // arena methods
  virtual bool initialize_arena(arena* _arena, const size_t _index,
      const uint64_t _size, const bool _multisampled) = 0;
  virtual void destroy_arena(arena* _arena) = 0;

  // Framebuffer
  virtual void initialize_framebuffer(
      framebuffer* _framebuffer, const framebuffer_create_info& _info) = 0;
  virtual void destroy_framebuffer(framebuffer* _framebuffer) = 0;

  // Graphics pipeline
  // TODO(awoloszyn): Plumb the pipeline cache through here
  virtual void initialize_graphics_pipeline(graphics_pipeline* _pipeline,
      const graphics_pipeline_description& _create_info,
      const pipeline_layout* _layout, const render_pass* _renderpass,
      uint32_t _subpass) = 0;
  virtual void destroy_graphics_pipeline(graphics_pipeline* _pipeline) = 0;

  // buffer methods
  virtual bool initialize_buffer(
      buffer* _buffer, const size_t _size, const resource_states _usage) = 0;
  virtual bool bind_buffer(
      buffer* _buffer, arena* _arena, const uint64_t _offset) = 0;
  virtual void* map_buffer(buffer* _buffer) = 0;
  virtual void unmap_buffer(buffer* _buffer) = 0;
  virtual void destroy_buffer(buffer* _buffer) = 0;
  virtual buffer_memory_requirements get_buffer_memory_requirements(
      const buffer* _buffer) = 0;
#endif
};

}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_RUNTIME_GRAPHICS_DEVICE_H__
