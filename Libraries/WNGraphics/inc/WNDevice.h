// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_DEVICE_H__
#define __WN_GRAPHICS_DEVICE_H__

#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNHeapTraits.h"
#include "WNMemory/inc/WNUniquePtr.h"

#ifdef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#ifdef _WN_GRAPHICS_VULKAN_DEVICE_TYPE_AVAILABLE
#include "WNGraphics/inc/Internal/Vulkan/WNDevice.h"
#elif defined _WN_GRAPHICS_D3D12_DEVICE_TYPE_AVAILABLE
#include "WNGraphics/inc/Internal/D3D12/WNDevice.h"
#endif
#else
#include "WNCore/inc/WNUtility.h"
#endif

namespace WNLogging {

class WNLog;

}  // namespace WNLogging

namespace wn {
namespace graphics {
namespace internal {

#ifdef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#ifdef _WN_GRAPHICS_VULKAN_DEVICE_TYPE_AVAILABLE
using device_base = vulkan::vulkan_device;
#elif defined _WN_GRAPHICS_D3D12_DEVICE_TYPE_AVAILABLE
using device_base = d3d12::d3d12_device;
#endif
#else
using device_base = core::non_copyable;
#endif

}  // namespace internal

class command_allocator;
class command_list;
class fence;

template <typename HeapTraits>
class heap;

class queue;

using command_list_ptr = memory::unique_ptr<command_list>;
using queue_ptr = memory::unique_ptr<queue>;

class device : public internal::device_base {
public:
  WN_FORCE_INLINE device() : internal::device_base() {}

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
  virtual ~device() = default;

  // It is only valid to have a single queue active at a time.
  virtual queue_ptr create_queue() = 0;
#endif

  upload_heap create_upload_heap(const size_t _num_bytes);
  download_heap create_download_heap(const size_t _num_bytes);

  command_allocator create_command_allocator();

  fence create_fence();

protected:
  friend class command_allocator;
  friend class fence;

  template <typename HeapTraits>
  friend class heap;

  friend class queue;
  friend class vulkan_queue;
  friend class vulkan_adapter;

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
#endif
};

}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_DEVICE_H__
