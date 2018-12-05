// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_ENGINE_RENDER_PASS_H__
#define __WN_ENGINE_RENDER_PASS_H__

#include "WNContainers/inc/WNHashMap.h"
#include "WNGraphics/inc/WNFramebuffer.h"
#include "WNGraphics/inc/WNImageView.h"
#include "WNGraphics/inc/WNRenderPass.h"
#include "renderer/inc/render_target.h"

namespace wn {
namespace engine {
namespace renderer {
class render_pass {
public:
  render_pass(memory::allocator* _allocator, logging::log* _log,
      runtime::graphics::device* _device,
      containers::dynamic_array<runtime::graphics::render_pass_attachment>
          _attachments,
      containers::dynamic_array<render_target*> _render_targets,
      const runtime::graphics::render_pass_attachment& depth_attachment);
  void render(uint64_t _frame_idx, runtime::graphics::command_list* _list);

private:
  memory::allocator* m_allocator;
  runtime::graphics::device* m_device;

  containers::hash_map<
      containers::dynamic_array<const runtime::graphics::image_view*>,
      runtime::graphics::framebuffer,
      containers::dynamic_array_hasher<const runtime::graphics::image_view*>,
      containers::dynamic_array_equality<const runtime::graphics::image_view*>>
      m_framebuffers;
  memory::unique_ptr<runtime::graphics::render_pass> m_render_pass;
  containers::dynamic_array<render_target*> m_render_targets;
  containers::dynamic_array<runtime::graphics::render_pass_attachment> m_ops;
  size_t m_width;
  size_t m_height;
  bool m_has_depth_target;
};

}  // namespace renderer
}  // namespace engine
}  // namespace wn

#endif  //__WN_ENGINE_RENDER_PASS_H__