// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_COMMAND_LIST_H__
#define __WN_GRAPHICS_COMMAND_LIST_H__

#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNDescriptors.h"
#include "WNGraphics/inc/WNFramebuffer.h"
#include "WNGraphics/inc/WNGraphicsPipeline.h"
#include "WNGraphics/inc/WNGraphicsTypes.h"
#include "WNGraphics/inc/WNImage.h"
#include "WNGraphics/inc/WNRenderPass.h"
#include "WNMemory/inc/WNUniquePtr.h"

#ifdef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/Internal/WNCommandListIncludes.h"
#else
#include "WNCore/inc/WNUtility.h"
#endif

namespace wn {
namespace graphics {
namespace internal {

#ifdef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
using command_list_base = _WN_GRAPHICS_DEVICE_TYPE::WN_GRAPHICS_PREFIXED_TYPE(
    command_list);
#else
using command_list_base = core::non_copyable;
#endif

}  // namespace internal

class command_list : public internal::command_list_base {
public:
  WN_FORCE_INLINE command_list() : internal::command_list_base() {}

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
  virtual ~command_list() = default;

  virtual void finalize() = 0;

  virtual void transition_resource(
      const image& _image, resource_state _from, resource_state _to) = 0;
  virtual void transition_resource(
      const buffer& _buffer, resource_state _from, resource_state _to) = 0;

  virtual void copy_buffer(const buffer& _src_buffer, size_t _src_offset,
      const buffer& _dst_buffer, size_t _dst_offset, size_t _size) = 0;

  virtual void copy_buffer_to_image(const buffer& _src_buffer,
      size_t _src_offset_in_bytes, const image& _dst_image) = 0;
  virtual void copy_image_to_buffer(const image& _image,
      const buffer& _dst_buffer, size_t _buffer_offset_in_bytes) = 0;

  virtual void draw(uint32_t _vertex_count, uint32_t _instance_count,
      uint32_t _vertex_offset, uint32_t _instance_offset) = 0;

  // TODO(awoloszyn): Expose another function (or a parameter)
  // to specify if this is allowed to execute sub-command-lists.
  // If this is the case, it can ONLY execute sub-command-lists.
  // You must specify one clear per attachment in the framebuffer,
  // even if it is not going to be used.
  // _area is to enable optimizations. It is a promise
  // to the graphics driver that you will not render outside
  // of this area. IT DOES NOT ENFORCE THIS. You must make sure that
  // this is in fact true.
  virtual void begin_render_pass(render_pass* _pass, framebuffer* _framebuffer,
      const render_area& _area,
      const containers::contiguous_range<clear_value>& _clears) = 0;

  virtual void end_render_pass() = 0;

  // Every time a DIFFERENT pipeline_layout is bound,
  // all existing bound descriptor sets are invalidated.
  virtual void bind_graphics_pipeline_layout(pipeline_layout* _layout) = 0;

  virtual void bind_graphics_descriptor_sets(
      const containers::contiguous_range<const descriptor_set*> _sets,
      uint32_t base_index) = 0;

  virtual void bind_graphics_pipeline(graphics_pipeline* _pipeline) = 0;
#endif

protected:
};

using command_list_ptr = memory::unique_ptr<command_list>;

}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_COMMAND_LIST_H__
