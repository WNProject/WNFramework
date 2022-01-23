// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_RENDER_PASS_TYPES_H__
#define __WN_GRAPHICS_RENDER_PASS_TYPES_H__

#include "WNGraphics/inc/WNGraphicsEnums.h"

namespace wn {
namespace runtime {
namespace graphics {

struct render_pass_attachment {
  data_format format = data_format::r8g8b8a8_unorm;
  multisample_count num_samples = multisample_count::samples_1;
  load_op attachment_load_op = load_op::load;
  store_op attachment_store_op = store_op::store;
  load_op stencil_load_op = load_op::load;
  store_op stencil_store_op = store_op::store;
  resource_state initial_state = resource_state::render_target;
  resource_state final_state = resource_state::render_target;
};

const uint32_t kUnusedAttachment = 0xFFFFFFFF;

struct attachment_reference {
  uint32_t attachment = 0;
  resource_state state = resource_state::render_target;
};

struct subpass_dependency {
  uint32_t src = 0;
  uint32_t dst = 0;
  resource_state src_state = resource_state::render_target;
  resource_state dst_state = resource_state::render_target;
  bool by_region = false;
};

struct subpass_description {
  // Must be size_0 if the device does not support input attachments.
  containers::contiguous_range<const attachment_reference> input_attachments;
  containers::contiguous_range<const attachment_reference> color_attachments;
  containers::contiguous_range<const attachment_reference> resolve_attachments;
  const attachment_reference* depth_stencil = nullptr;
  // resource_state is unused for preserve attachments.
  containers::contiguous_range<const attachment_reference> preserve_attachments;
};

}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_RENDER_PASS_TYPES_H__
