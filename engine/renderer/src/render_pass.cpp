// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "renderer/inc/render_pass.h"

#include "profiling/inc/profiling.h"
#include "renderer/inc/render_context.h"

namespace wn {
namespace engine {
namespace renderer {

render_pass::render_pass(memory::allocator* _allocator, logging::log* _log,
    runtime::graphics::device* _device, render_context* _context,
    containers::dynamic_array<runtime::graphics::render_pass_attachment>
        _attachments,
    containers::dynamic_array<render_target*> _render_targets,
    uint32_t _buffering_depth,
    const runtime::graphics::render_pass_attachment& _depth_attachment)
  : m_allocator(_allocator),
    m_device(_device),
    m_framebuffers(_allocator),
    m_buffers(_allocator),
    m_buffer_allocations(_allocator),
    m_buffer_sets(_allocator),
    m_render_targets(core::move(_render_targets)),
    m_renderables(_allocator),
    m_width(0),
    m_height(0),
    m_has_depth_target(false),
    m_buffering_depth(_buffering_depth),
    m_dirty_buffering(_buffering_depth) {
  m_has_depth_target =
      _depth_attachment.format != runtime::graphics::data_format::max;
  // TODO(awoloszyn): Instead of creating framebuffers on-demand,
  //    actually figure out what they should all be before-hand.
  m_width = m_render_targets[0]->get_width();
  m_height = m_render_targets[0]->get_height();

  WN_DEBUG_ASSERT(
      _attachments.size() + m_has_depth_target == m_render_targets.size(),
      "Attachments and render_targets must match");
  containers::dynamic_array<runtime::graphics::attachment_reference>
      color_attachments(_allocator);
  for (uint32_t i = 0; i < m_render_targets.size() - m_has_depth_target; ++i) {
    color_attachments.push_back(
        {i, runtime::graphics::resource_state::render_target});
    if (m_render_targets[i]->get_width() != m_width ||
        m_render_targets[i]->get_height() != m_height) {
      _log->log_error(
          "Renderpasses can only act on rendertargets of the same size");
    }
  }
  const wn::runtime::graphics::descriptor_pool_create_info pool_infos[] = {
      {_buffering_depth,
          wn::runtime::graphics::descriptor_type::read_only_buffer}};

  m_descriptor_pool = _device->create_descriptor_pool(pool_infos);
  const runtime::graphics::descriptor_binding_info infos[] = {{0,  // binding
      10,  // register_base
      1,   // array_size
      static_cast<uint32_t>(runtime::graphics::shader_stage::pixel) |
          static_cast<uint32_t>(runtime::graphics::shader_stage::vertex) |
          static_cast<uint32_t>(runtime::graphics::shader_stage::geometry) |
          static_cast<uint32_t>(
              runtime::graphics::shader_stage::tessellation_control) |
          static_cast<uint32_t>(
              runtime::graphics::shader_stage::tessellation_evaluation),
      runtime::graphics::descriptor_type::read_only_buffer}};
  m_buffer_layout = _device->create_descriptor_set_layout(infos);

  for (uint32_t i = 0; i < _buffering_depth; ++i) {
    m_buffers.push_back(_device->create_buffer(sizeof(float) * 4,
        static_cast<runtime::graphics::resource_states>(
            runtime::graphics::resource_state::read_only_buffer |
            runtime::graphics::resource_state::copy_dest)));

    runtime::graphics::buffer_memory_requirements reqs =
        m_buffers.back().get_memory_requirements();
    m_buffer_allocations.push_back(
        _context->get_allocation_for_buffer(reqs.size, reqs.alignment));
    m_buffers.back().bind_memory(m_buffer_allocations.back().arena(),
        m_buffer_allocations.back().offset());

    m_buffer_sets.push_back(
        m_descriptor_pool.create_descriptor_set(&m_buffer_layout));
    runtime::graphics::buffer_descriptor desc[] = {{
        0,                                                     // binding
        0,                                                     // array_offset
        runtime::graphics::descriptor_type::read_only_buffer,  // type
        &m_buffers.back(),                                     // resource
        0,                  // offset_in_elements
        sizeof(float) * 4,  // element_size
        1                   // num_elements
    }};
    m_buffer_sets.back().update_descriptors(desc, {}, {});
  }

  runtime::graphics::attachment_reference depth_attachment;

  if (m_has_depth_target) {
    _attachments.push_back(_depth_attachment);
    depth_attachment = {static_cast<uint32_t>(color_attachments.size()),
        runtime::graphics::resource_state::depth_target};
  }

  runtime::graphics::subpass_description desc = {};
  desc.color_attachments =
      containers::contiguous_range<runtime::graphics::attachment_reference>(
          color_attachments);
  desc.depth_stencil = m_has_depth_target ? &depth_attachment : nullptr;
  m_render_pass = memory::make_unique<runtime::graphics::render_pass>(
      _allocator,
      _device->create_render_pass(
          containers::contiguous_range<
              runtime::graphics::render_pass_attachment>(_attachments),
          containers::contiguous_range<runtime::graphics::subpass_description>(
              &desc, 1),
          {}));
}

void render_pass::render(render_context* _context, uint64_t _frame_idx,
    runtime::graphics::command_list* _setup,
    runtime::graphics::command_list* _render) {
  PROFILE_REGION(RenderpassRender);
  (void)_context;
  containers::dynamic_array<const runtime::graphics::image_view*> views(
      m_allocator);
  views.reserve(m_render_targets.size());

  for (auto& rt : m_render_targets) {
    auto idx = rt->get_index_for_frame(_frame_idx);
    views.push_back(rt->get_image_view_for_index(idx));
  }

  auto it = m_framebuffers.find(views);
  if (it == m_framebuffers.end()) {
    runtime::graphics::framebuffer_create_info inf = {
        m_render_pass.get(),
        containers::contiguous_range<const runtime::graphics::image_view*>(
            views.data(), views.size()),
        static_cast<uint32_t>(m_render_targets[0]->get_width()),
        static_cast<uint32_t>(m_render_targets[0]->get_height()),
        1,
    };

    runtime::graphics::framebuffer fb = m_device->create_framebuffer(inf);

    containers::dynamic_array<uint64_t> generations(m_allocator);
    for (auto& rt : m_render_targets) {
      generations.push_back(rt->get_generation());
    }
    bool success = false;
    std::tie(it, success) = m_framebuffers.insert(core::move(views),
        framebuffer_data{core::move(generations), core::move(fb)});
    WN_DEBUG_ASSERT(success, "Somehow insertion failed.");
  } else {
    bool recreate = false;
    for (size_t i = 0; i < m_render_targets.size(); ++i) {
      if (it->second.m_generations[i] !=
          m_render_targets[i]->get_generation()) {
        recreate = true;
        break;
      }
    }
    if (recreate) {
      m_dirty_buffering = m_buffering_depth;
      m_width = m_render_targets[0]->get_width();
      m_height = m_render_targets[0]->get_height();

      runtime::graphics::framebuffer_create_info inf = {
          m_render_pass.get(),
          containers::contiguous_range<const runtime::graphics::image_view*>(
              views.data(), views.size()),
          static_cast<uint32_t>(m_render_targets[0]->get_width()),
          static_cast<uint32_t>(m_render_targets[0]->get_height()),
          1,
      };

      runtime::graphics::framebuffer fb = m_device->create_framebuffer(inf);
      containers::dynamic_array<uint64_t> generations(m_allocator);
      for (size_t i = 0; i < m_render_targets.size(); ++i) {
        it->second.m_generations[i] = m_render_targets[i]->get_generation();
      }
      it->second.m_framebuffer = core::move(fb);
    }
  }
  auto buffer_idx = _frame_idx % m_buffering_depth;
  if (m_dirty_buffering) {
    auto buffer = _context->create_temporary_upload_buffer(sizeof(float) * 4);
    float* f = static_cast<float*>(buffer->map());
    f[0] = static_cast<float>(m_width);
    f[1] = static_cast<float>(m_height);
    f[2] = 0.0f;
    f[3] = 0.0f;
    buffer->unmap();

    _setup->transition_resource(m_buffers[buffer_idx],
        runtime::graphics::resource_state::initial,
        runtime::graphics::resource_state::copy_dest);

    _setup->transition_resource(m_buffers[buffer_idx],
        runtime::graphics::resource_state::host_write,
        runtime::graphics::resource_state::copy_source);

    _setup->copy_buffer(
        *buffer, 0, m_buffers[buffer_idx], 0, sizeof(float) * 4);
    _setup->transition_resource(m_buffers[buffer_idx],
        runtime::graphics::resource_state::copy_dest,
        runtime::graphics::resource_state::read_only_buffer);
  }

  containers::dynamic_array<runtime::graphics::clear_value> clears(m_allocator);
  clears.resize(m_render_targets.size());
  for (size_t i = 0; i < m_render_targets.size() - m_has_depth_target; ++i) {
    clears[i].color.float_vals[0] = 0.0f;
    clears[i].color.float_vals[1] = 0.0f;
    clears[i].color.float_vals[2] = 0.0f;
    clears[i].color.float_vals[3] = 1.0f;
  }

  if (m_has_depth_target) {
    clears.back().depth.depth = 0.0f;
    clears.back().depth.stencil = 0;
  }

  for (size_t i = 0; i < m_render_targets.size() - m_has_depth_target; ++i) {
    auto idx = m_render_targets[i]->get_index_for_frame(_frame_idx);
    m_render_targets[i]->transition_layout(
        idx, runtime::graphics::resource_state::render_target, _render);
  }
  if (m_has_depth_target) {
    auto idx = m_render_targets.back()->get_index_for_frame(_frame_idx);
    m_render_targets.back()->transition_layout(
        idx, runtime::graphics::resource_state::depth_target, _render);
  }

  _render->begin_render_pass(m_render_pass.get(), &it->second.m_framebuffer,
      {0, 0, static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height)},
      clears);
  for (auto& renderable : m_renderables) {
    renderable->render(this, _setup, _render, &m_buffer_sets[buffer_idx]);
  }

  _render->end_render_pass();
}

}  // namespace renderer
}  // namespace engine
}  // namespace wn
