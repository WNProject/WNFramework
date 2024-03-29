// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_ENGINE_RENDER_PASS_H__
#define __WN_ENGINE_RENDER_PASS_H__

#include "WNContainers/inc/WNHashMap.h"
#include "WNGraphics/inc/WNFramebuffer.h"
#include "WNGraphics/inc/WNImageView.h"
#include "WNGraphics/inc/WNRenderPass.h"
#include "renderer/inc/render_target.h"
#include "renderer/inc/renderable_object.h"

namespace wn {
namespace engine {
namespace renderer {
class render_context;
class render_pass {
public:
  render_pass(memory::allocator* _allocator, logging::log* _log,
      runtime::graphics::device* _device, render_context* _context,
      containers::dynamic_array<runtime::graphics::render_pass_attachment>
          _attachments,
      containers::dynamic_array<render_target*> _render_targets,
      uint32_t _buffering_depth,
      const runtime::graphics::render_pass_attachment& depth_attachment);
  void render(render_context* _context, uint64_t _frame_idx,
      runtime::graphics::command_list* _setup,
      runtime::graphics::command_list* _list);

  void add_renderable_object(
      scripting::shared_cpp_pointer<renderable_object> object) {
    m_renderables.push_back(object);
  }

  size_t get_width() {
    return m_width;
  }
  size_t get_height() {
    return m_height;
  }
  runtime::graphics::render_pass* get_render_pass() {
    return m_render_pass.get();
  }

  const runtime::graphics::descriptor_set_layout&
  get_layout_for_renderpass_descriptors() const {
    return m_buffer_layout;
  }

private:
  memory::allocator* m_allocator;
  runtime::graphics::device* m_device;
  struct framebuffer_data {
    containers::dynamic_array<uint64_t> m_generations;
    runtime::graphics::framebuffer m_framebuffer;
  };
  containers::hash_map<
      containers::dynamic_array<const runtime::graphics::image_view*>,
      framebuffer_data,
      containers::dynamic_array_hasher<const runtime::graphics::image_view*>,
      containers::dynamic_array_equality<const runtime::graphics::image_view*>>
      m_framebuffers;
  containers::dynamic_array<runtime::graphics::buffer> m_buffers;
  containers::dynamic_array<gpu_allocation> m_buffer_allocations;
  runtime::graphics::descriptor_set_layout m_buffer_layout;
  containers::dynamic_array<runtime::graphics::descriptor_set> m_buffer_sets;
  runtime::graphics::descriptor_pool m_descriptor_pool;
  memory::unique_ptr<runtime::graphics::render_pass> m_render_pass;
  containers::dynamic_array<render_target*> m_render_targets;
  containers::dynamic_array<scripting::shared_cpp_pointer<renderable_object>>
      m_renderables;

  size_t m_width;
  size_t m_height;
  bool m_has_depth_target;
  size_t m_buffering_depth;
  size_t m_dirty_buffering;
};

}  // namespace renderer
}  // namespace engine
}  // namespace wn

#endif  //__WN_ENGINE_RENDER_PASS_H__
