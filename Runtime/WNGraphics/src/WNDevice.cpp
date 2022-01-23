// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNGraphics/inc/WNDevice.h"

#include "WNGraphics/inc/WNArena.h"
#include "WNGraphics/inc/WNBuffer.h"
#include "WNGraphics/inc/WNCommandAllocator.h"
#include "WNGraphics/inc/WNCommandList.h"
#include "WNGraphics/inc/WNDescriptors.h"
#include "WNGraphics/inc/WNFence.h"
#include "WNGraphics/inc/WNFramebuffer.h"
#include "WNGraphics/inc/WNGraphicsPipeline.h"
#include "WNGraphics/inc/WNGraphicsTypes.h"
#include "WNGraphics/inc/WNImage.h"
#include "WNGraphics/inc/WNImageView.h"
#include "WNGraphics/inc/WNRenderPass.h"
#include "WNGraphics/inc/WNSampler.h"
#include "WNGraphics/inc/WNShaderModule.h"
#include "WNGraphics/inc/WNSignal.h"
#include "WNGraphics/inc/WNSwapchain.h"

namespace wn {
namespace runtime {
namespace graphics {

command_allocator device::create_command_allocator() {
  command_allocator new_command_allocator(this);

  initialize_command_allocator(&new_command_allocator);

  return new_command_allocator;
}

fence device::create_fence() {
  fence new_fence(this);

  initialize_fence(&new_fence);

  return new_fence;
}

signal device::create_signal() {
  signal new_signal(this);
  initialize_signal(&new_signal);
  return new_signal;
}

image device::create_image(
    const image_create_info& _info, const clear_value& _optimized_clear) {
  image new_image(this);
  initialize_image(_info, _optimized_clear, &new_image);
  return new_image;
}

sampler device::create_sampler(const sampler_create_info& _info) {
  sampler new_sampler(this);
  initialize_sampler(_info, &new_sampler);
  return new_sampler;
}

shader_module device::create_shader_module(
    const containers::contiguous_range<const uint8_t>& _bytes) {
  shader_module s(this);
  initialize_shader_module(&s, _bytes);
  return s;
}

descriptor_set_layout device::create_descriptor_set_layout(
    const containers::contiguous_range<const descriptor_binding_info>&
        _layout) {
  descriptor_set_layout layout(this);
  initialize_descriptor_set_layout(&layout, _layout);
  return layout;
}

descriptor_pool device::create_descriptor_pool(
    const containers::contiguous_range<const descriptor_pool_create_info>&
        _pool_data) {
  static_assert(
      static_cast<uint32_t>(descriptor_type::descriptor_type_max) < 16,
      "This code needs to be updated if you increase the number of descriptor "
      "types");
#ifdef _WN_DEBUG
  uint8_t used_descriptors = 0;
  for (const auto& data : _pool_data) {
    WN_DEBUG_ASSERT(
        0 == (used_descriptors & (1 << static_cast<uint8_t>(data.type))),
        "Cannot define the same descriptor type more than once for a given "
        "pool");
    used_descriptors |= 1 << static_cast<uint8_t>(data.type);
  }
#endif
  descriptor_pool pool(this);
  initialize_descriptor_pool(&pool, _pool_data);
  return pool;
}

pipeline_layout device::create_pipeline_layout(
    const containers::contiguous_range<const descriptor_set_layout*>&
        _descriptor_sets,
    const containers::contiguous_range<const push_constant_range>&
        _push_constants) {
  pipeline_layout layout(this);
  initialize_pipeline_layout(&layout, _descriptor_sets, _push_constants);
  return layout;
}

render_pass device::create_render_pass(
    const containers::contiguous_range<const render_pass_attachment>&
        _attachments,
    const containers::contiguous_range<const subpass_description>& _subpasses,
    const containers::contiguous_range<const subpass_dependency>& _deps) {
  render_pass pass(this);
  initialize_render_pass(&pass, _attachments, _subpasses, _deps);
  return pass;
}

image_view device::create_image_view(const image* _image,
    uint32_t _base_mip_level, uint32_t _num_mip_levels,
    image_components _components) {
  image_view view(this, _components);
  initialize_image_view(&view, _image, _base_mip_level, _num_mip_levels);
  return view;
}

arena device::create_arena(const size_t _index, const uint64_t _size) {
  return create_arena(_index, _size, false);
}

arena device::create_arena(
    const size_t _index, const uint64_t _size, const bool _multisampled) {
  arena new_arena;

  if (initialize_arena(&new_arena, _index, _size, _multisampled)) {
    new_arena.m_device = this;
  }

  return new_arena;
}

framebuffer device::create_framebuffer(
    const framebuffer_create_info& _create_info) {
  framebuffer fb(this);
  initialize_framebuffer(&fb, _create_info);
  return fb;
}

graphics_pipeline device::create_graphics_pipeline(
    const graphics_pipeline_description& _create_info,
    const pipeline_layout* _layout, const render_pass* _renderpass,
    uint32_t _subpass) {
  graphics_pipeline pipeline(this);
  initialize_graphics_pipeline(
      &pipeline, _create_info, _layout, _renderpass, _subpass);
  return pipeline;
}

buffer device::create_buffer(const size_t _size, const resource_states _usage) {
  buffer new_buffer;

  if (initialize_buffer(&new_buffer, _size, _usage)) {
    new_buffer.m_size = _size;
    new_buffer.m_device = this;
    auto reqs = get_buffer_memory_requirements(&new_buffer, _usage);
    new_buffer.m_memory_alignment = reqs.alignment;
    new_buffer.m_memory_size = reqs.size;
  }

  return new_buffer;
}

}  // namespace graphics
}  // namespace runtime
}  // namespace wn
