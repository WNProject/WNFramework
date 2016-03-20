// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_DEVICE_H__
#define __WN_GRAPHICS_DEVICE_H__

#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNCommandListForward.h"
#include "WNGraphics/inc/WNDeviceForward.h"
#include "WNGraphics/inc/WNHeapTraits.h"
#include "WNGraphics/inc/WNQueueForward.h"
#include "WNMemory/inc/WNUniquePtr.h"

namespace WNLogging {

class WNLog;

}  // namespace WNLogging

namespace wn {
namespace graphics {
class command_allocator;
class fence;
template <typename T>
class heap;
namespace internal {

class internal_device : public core::non_copyable {
public:
  WN_FORCE_INLINE internal_device(
      memory::allocator* _allocator, WNLogging::WNLog* _log)
    : m_allocator(_allocator), m_log(_log) {}

  // On success, returns an upload_heap object. This heap must be at least,
  // 1 byte in size. If creation fails a nullptr will be returned.
  virtual upload_heap create_upload_heap(size_t _num_bytes) = 0;
  virtual download_heap create_download_heap(size_t _num_bytes) = 0;

  // It is only valid to have a single queue active at a time.
  virtual queue_ptr create_queue() = 0;
  virtual fence create_fence() = 0;

  virtual command_allocator create_command_allocator() = 0;
protected:
  template <typename heap_type>
  friend class graphics::heap;
  friend class graphics::fence;
  friend class graphics::command_allocator;

  // Upload heap methods
  virtual uint8_t* acquire_range(
      upload_heap* _buffer, size_t _offset, size_t _num_bytes) = 0;
  virtual uint8_t* synchronize(
      upload_heap* _buffer, size_t _offset, size_t _num_bytes) = 0;
  virtual void release_range(
      upload_heap* _buffer, size_t _offset, size_t _num_bytes) = 0;
  virtual void destroy_heap(upload_heap* _heap) = 0;

  // Download heap methods
  virtual uint8_t* acquire_range(
      download_heap* _buffer, size_t _offset, size_t _num_bytes) = 0;
  virtual uint8_t* synchronize(
      download_heap* _buffer, size_t _offset, size_t _num_bytes) = 0;
  virtual void release_range(
      download_heap* _buffer, size_t _offset, size_t _num_bytes) = 0;
  virtual void destroy_heap(download_heap* _heap) = 0;

  // Destruction methods
  virtual void destroy_queue(graphics::queue* _queue) = 0;
  virtual void destroy_fence(fence* _fence) = 0;

  // Fence methods
  virtual void wait_fence(const fence* _fence) const = 0;
  virtual void reset_fence(fence* _fence) = 0;

  virtual void destroy_command_allocator(command_allocator*) = 0;
  virtual command_list_ptr create_command_list(command_allocator*) = 0;

#include "WNGraphics/inc/Internal/WNSetFriendQueues.h"
  memory::allocator* m_allocator;
  WNLogging::WNLog* m_log;
};

}  // namespace internal
}  // namespace graphics
}  // namespace wn

#if _WN_GRAPHICS_DEVICE_TYPES_AVAILABLE == 1
#if defined _WN_GRAPHICS_VULKAN_DEVICE_TYPE_AVAILABLE
#include "WNGraphics/inc/Internal/Vulkan/WNDevice.h"
#elif defined _WN_GRAPHICS_D3D12_DEVICE_TYPE_AVAILABLE
#include "WNGraphics/inc/Internal/D3D12/WNDevice.h"
#endif
#endif

#endif  // __WN_GRAPHICS_DEVICE_H__
