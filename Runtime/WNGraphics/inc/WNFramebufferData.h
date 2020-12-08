// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_FRAMEBUFFER_CREATE_INFO_H__
#define __WN_GRAPHICS_FRAMEBUFFER_CREATE_INFO_H__

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNGraphics/inc/WNGraphicsEnums.h"

namespace wn {
namespace runtime {
namespace graphics {

class render_pass;
class image_view;

// Every image, depth_buffer, and depth_stencil must have the
// same sizes.
// TODO(awoloszyn): Maybe export a feature-bit for the
//  Vulkan's ability to render to a subsection section of a
//  imageViews.
struct framebuffer_create_info {
  render_pass* pass;
  containers::contiguous_range<const image_view*> image_views;
  uint32_t width;   // This must match the width of every image_view
  uint32_t height;  // This must match the height of every image_view
  uint32_t depth;   // This must match the depth of every image_view
};

}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_FRAMEBUFFER_CREATE_INFO_H__
