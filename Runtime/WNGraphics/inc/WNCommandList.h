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
#include "WNGraphics/inc/WNHeapTraits.h"
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

  virtual void enqueue_resource_transition(
      const image& _image, resource_state _from, resource_state _to) = 0;

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

  // Ensures that all CPU writes to this upload_heap have finished,
  // before the next command in the queue.
  template <typename T>
  WN_FORCE_INLINE void enqueue_barrier(
      const upload_heap_buffer<T>& _upload_buffer) {
    enqueue_upload_barrier(*(_upload_buffer.m_heap), _upload_buffer.m_offset,
        sizeof(T) * _upload_buffer.range().size());
  }

  // Ensures that all GPU writes to this download_heap have finished,
  // before the next command in the queue.
  template <typename T>
  WN_FORCE_INLINE void enqueue_barrier(
      const download_heap_buffer<T>& _download_buffer) {
    enqueue_download_barrier(*(_download_buffer.m_heap),
        _download_buffer.m_offset, sizeof(T) * _download_buffer.range().size());
  }

  // Copies bytes from the _upload_buffer to the _download_buffer.
  // Note: The size must be at least 1 byte.
  template <typename T>
  WN_FORCE_INLINE void enqueue_copy(const upload_heap_buffer<T>& _upload_buffer,
      const download_heap_buffer<T>& _download_buffer) {
    WN_DEBUG_ASSERT_DESC(
        _download_buffer.range().size() >= _upload_buffer.range().size(),
        "The destination is smaller than the source");
    WN_DEBUG_ASSERT_DESC(_upload_buffer.range().size() >= 1,
        "The copy must be at least one byte");

    enqueue_buffer_copy(*(_upload_buffer.m_heap), _upload_buffer.m_offset,
        *(_download_buffer.m_heap), _download_buffer.m_offset,
        sizeof(T) * _upload_buffer.range().size());
  }

  template <typename T>
  WN_FORCE_INLINE void enqueue_copy(
      const upload_heap_buffer<T>& _upload_buffer, const image& _image) {
    WN_DEBUG_ASSERT_DESC(_image.get_resource_info().total_memory_required <=
                             sizeof(T) * _upload_buffer.range().size(),
        "The image and the upload buffer do not match");
    enqueue_texture_upload(
        *(_upload_buffer.m_heap), _upload_buffer.m_offset, _image);
  }

  template <typename T>
  WN_FORCE_INLINE void enqueue_copy(
      const image& _image, const download_heap_buffer<T>& _download_buffer) {
    WN_DEBUG_ASSERT_DESC(_image.get_resource_info().total_memory_required <=
                             sizeof(T) * _download_buffer.range().size(),
        "The image and the download buffer do not match");
    enqueue_texture_download(
        _image, *(_download_buffer.m_heap), _download_buffer.m_offset);
  }

protected:
#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
  virtual void enqueue_upload_barrier(const upload_heap& upload_heap,
      size_t _offset_in_bytes, size_t _size) = 0;

  virtual void enqueue_download_barrier(const download_heap& download_heap,
      size_t _offset_in_bytes, size_t _size) = 0;

  virtual void enqueue_buffer_copy(const upload_heap& _upload_heap,
      size_t _upload_offset_in_bytes, const download_heap& _download_heap,
      size_t _download_offset_in_bytes, size_t _upload_size) = 0;

  virtual void enqueue_texture_upload(const upload_heap& upload_heap,
      size_t _upload_offset_in_bytes, const image& _image) = 0;
  virtual void enqueue_texture_download(const image& _image,
      const download_heap& _download_heap,
      size_t _download_offset_in_bytes) = 0;

#endif
};

using command_list_ptr = memory::unique_ptr<command_list>;

}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_COMMAND_LIST_H__
