// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_COMMAND_LIST_H__
#define __WN_GRAPHICS_COMMAND_LIST_H__

#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNCommandListForward.h"
#include "WNGraphics/inc/WNHeapTraits.h"
#include "WNMemory/inc/WNUniquePtr.h"

namespace wn {
namespace graphics {
namespace internal {

class internal_command_list : public core::non_copyable {
public:
  virtual ~internal_command_list() {}

  // Ensures that all CPU writes to this upload_heap have finished,
  // before the next command in the queue.
  template <typename T>
  void enqueue_barrier(const upload_heap_buffer<T>& _upload_buffer) {
    enqueue_upload_barrier(*_upload_buffer.m_heap, _upload_buffer.m_offset,
        sizeof(T) * _upload_buffer.range().size());
  }

  // Ensures that all GPU writes to this download_heap have finished,
  // before the next command in the queue.
  template <typename T>
  void enqueue_barrier(const download_heap_buffer<T>& _download_buffer) {
    enqueue_download_barrier(*_download_buffer.m_heap,
        _download_buffer.m_offset, sizeof(T) * _download_buffer.range().size());
  }

  template <typename T>
  void enqueue_buffer_copy(const upload_heap_buffer<T>& _upload_buffer,
      const download_heap_buffer<T>& _download_buffer) {
    WN_DEBUG_ASSERT_DESC(
        _download_buffer.range().size() >= _upload_buffer.range().size(),
        "The destionation is smaller than the source");
    enqueue_copy(*_upload_buffer.m_heap, _upload_buffer.m_offset,
        *_download_buffer.m_heap, _download_buffer.m_offset,
        sizeof(T) * _upload_buffer.range().size());
  }
  virtual void finalize() = 0;

protected:
  virtual void enqueue_upload_barrier(const upload_heap& upload_heap,
      size_t _offset_in_bytes, size_t _size) = 0;
  virtual void enqueue_download_barrier(const download_heap& download_heap,
      size_t _offset_in_bytes, size_t _size) = 0;
  virtual void enqueue_copy(const upload_heap& _upload_heap,
      size_t _upload_offset_in_bytes, const download_heap& _download_heap,
      size_t _download_offset_in_bytes, size_t _upload_size) = 0;
};

}  // namespace internal
}  // namespace graphics
}  // namespace wn

#if _WN_GRAPHICS_DEVICE_TYPES_AVAILABLE == 1
#if defined _WN_GRAPHICS_VULKAN_DEVICE_TYPE_AVAILABLE
#include "WNGraphics/inc/Internal/Vulkan/WNCommandList.h"
#elif defined _WN_GRAPHICS_D3D12_DEVICE_TYPE_AVAILABLE
#include "WNGraphics/inc/Internal/D3D12/WNCommandList.h"
#endif
#endif

#endif  // __WN_GRAPHICS_COMMAND_LIST_H__
