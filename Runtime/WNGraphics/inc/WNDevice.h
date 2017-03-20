// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_DEVICE_H__
#define __WN_GRAPHICS_DEVICE_H__

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNDescriptorData.h"
#include "WNGraphics/inc/WNGraphicsEnums.h"
#include "WNGraphics/inc/WNHeapTraits.h"
#include "WNGraphics/inc/WNRenderPassTypes.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/WNUniquePtr.h"

#ifdef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/Internal/WNDeviceIncludes.h"
#else
#include "WNCore/inc/WNUtility.h"
#endif

WN_GRAPHICS_FORWARD(device);
WN_GRAPHICS_FORWARD(queue);
WN_GRAPHICS_FORWARD(adapter);
WN_GRAPHICS_FORWARD(image);
WN_GRAPHICS_FORWARD(swapchain);
namespace wn {
namespace runtime {
namespace window {
class window;
}  // namespace window
}  // namespace runtime

namespace graphics {
namespace internal {
#ifdef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
using device_base = _WN_GRAPHICS_DEVICE_TYPE::WN_GRAPHICS_PREFIXED_TYPE(device);
#else
using device_base = core::non_copyable;
#endif

}  // namespace internal

class command_allocator;
class command_list;
class fence;
class image_view;
class shader_module;
class descriptor_set_layout;
class descriptor_pool;
class descriptor_set;
class pipeline_layout;
class render_pass;

struct image_create_info;
struct swapchain_create_info;

template <typename HeapTraits>
class heap;

class queue;
class swapchain;

using command_list_ptr = memory::unique_ptr<command_list>;
using queue_ptr = memory::unique_ptr<queue>;
using swapchain_ptr = memory::unique_ptr<swapchain>;

class device : public internal::device_base {
public:
  WN_FORCE_INLINE device() : internal::device_base() {}

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
  virtual ~device() = default;

  // It is only valid to have a single queue active at a time.
  virtual queue_ptr create_queue() = 0;

  virtual swapchain_ptr create_swapchain(const swapchain_create_info& _info,
      queue* queue, runtime::window::window* window) = 0;
  virtual size_t get_image_upload_buffer_alignment() = 0;
  virtual size_t get_buffer_upload_buffer_alignment() = 0;

#endif

  upload_heap create_upload_heap(const size_t _num_bytes);
  download_heap create_download_heap(const size_t _num_bytes);

  command_allocator create_command_allocator();
  fence create_fence();

  // The contiguous range must be 32-bit aligned for
  // compatibility.
  shader_module create_shader_module(
      const containers::contiguous_range<const uint8_t>& bytes);

  image create_image(const image_create_info& _info);

  descriptor_set_layout create_descriptor_set_layout(
      const containers::contiguous_range<const descriptor_binding_info>&
          _layout);

  descriptor_pool create_descriptor_pool(
      const containers::contiguous_range<const descriptor_pool_create_info>&
          _pool_data);

  pipeline_layout create_pipeline_layout(
      const containers::contiguous_range<const descriptor_set_layout*>&
          _descriptor_sets);

  render_pass create_render_pass(
      const containers::contiguous_range<const render_pass_attachment>&
          _attachments,
      const containers::contiguous_range<const subpass_description>& _subpasses,
      const containers::contiguous_range<const subpass_dependency>& _deps);

  image_view create_image_view(const image* _image);

protected:
  friend class command_allocator;
  friend class fence;

  template <typename HeapTraits>
  friend class heap;

  friend class queue;
  WN_GRAPHICS_ADD_FRIENDS(queue);
  WN_GRAPHICS_ADD_FRIENDS(adapter);
  WN_GRAPHICS_ADD_FRIENDS(image);
  WN_GRAPHICS_ADD_FRIENDS(swapchain);

  friend class shader_module;
  friend class descriptor_set_layout;
  friend class descriptor_pool;
  friend class descriptor_set;
  friend class pipeline_layout;
  friend class render_pass;
  friend class image_view;

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
  // Upload heap methods
  virtual void initialize_upload_heap(
      upload_heap* _upload_heap, const size_t _num_bytes) = 0;
  virtual uint8_t* acquire_range(
      upload_heap* _buffer, size_t _offset, size_t _num_bytes) = 0;
  virtual uint8_t* synchronize(
      upload_heap* _buffer, size_t _offset, size_t _num_bytes) = 0;
  virtual void release_range(
      upload_heap* _buffer, size_t _offset, size_t _num_bytes) = 0;
  virtual void destroy_heap(upload_heap* _heap) = 0;

  // Download heap methods
  virtual void initialize_download_heap(
      download_heap* _download_heap, const size_t _num_bytes) = 0;
  virtual uint8_t* acquire_range(
      download_heap* _buffer, size_t _offset, size_t _num_bytes) = 0;
  virtual uint8_t* synchronize(
      download_heap* _buffer, size_t _offset, size_t _num_bytes) = 0;
  virtual void release_range(
      download_heap* _buffer, size_t _offset, size_t _num_bytes) = 0;
  virtual void destroy_heap(download_heap* _heap) = 0;

  // Destruction methods
  virtual void destroy_queue(queue* _queue) = 0;

  // command allocator methods
  virtual void initialize_command_allocator(
      command_allocator* _command_allocator) = 0;
  virtual void destroy_command_allocator(
      command_allocator* _command_allocator) = 0;

  // command list methods
  virtual command_list_ptr create_command_list(
      command_allocator* _command_allocator) = 0;

  // fence methods
  virtual void initialize_fence(fence* _fence) = 0;
  virtual void destroy_fence(fence* _fence) = 0;
  virtual void wait_fence(const fence* _fence) const = 0;
  virtual void reset_fence(fence* _fence) = 0;

  // Image methods
  virtual void initialize_image(
      const image_create_info& _info, image* _image) = 0;
  virtual void destroy_image(image* _image) = 0;

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

  // Pipeline layout
  virtual void initialize_pipeline_layout(pipeline_layout* _layout,
      const containers::contiguous_range<const descriptor_set_layout*>&
          _descriptor_sets) = 0;
  virtual void destroy_pipeline_layout(pipeline_layout* _layout) = 0;

  // Render pass
  virtual void initialize_render_pass(render_pass* _pass,
      const containers::contiguous_range<const render_pass_attachment>&
          _attachments,
      const containers::contiguous_range<const subpass_description>& _subpasses,
      const containers::contiguous_range<const subpass_dependency>& _deps) = 0;
  virtual void destroy_render_pass(render_pass* _pass) = 0;

  // Image View
  virtual void initialize_image_view(image_view* _view, const image* image) = 0;
  virtual void destroy_image_view(image_view* _view) = 0;
#endif
};

}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_DEVICE_H__
