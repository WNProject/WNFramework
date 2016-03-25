// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_COMMAND_LIST_H__
#define __WN_GRAPHICS_COMMAND_LIST_H__

#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNHeapTraits.h"
#include "WNMemory/inc/WNUniquePtr.h"

#ifdef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#ifdef _WN_GRAPHICS_VULKAN_DEVICE_TYPE_AVAILABLE
#include "WNGraphics/inc/Internal/Vulkan/WNCommandList.h"
#elif defined _WN_GRAPHICS_D3D12_DEVICE_TYPE_AVAILABLE
#include "WNGraphics/inc/Internal/D3D12/WNCommandList.h"
#endif
#else
#include "WNCore/inc/WNUtility.h"
#endif

namespace wn {
namespace graphics {
namespace internal {

#ifdef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#ifdef _WN_GRAPHICS_VULKAN_DEVICE_TYPE_AVAILABLE
using command_list_base = vulkan::vulkan_command_list;
#elif defined _WN_GRAPHICS_D3D12_DEVICE_TYPE_AVAILABLE
using command_list_base = d3d12::d3d12_command_list;
#endif
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

  template <typename T>
  WN_FORCE_INLINE void enqueue_buffer_copy(
      const upload_heap_buffer<T>& _upload_buffer,
      const download_heap_buffer<T>& _download_buffer) {
    WN_DEBUG_ASSERT_DESC(
        _download_buffer.range().size() >= _upload_buffer.range().size(),
        "The destionation is smaller than the source");

    enqueue_copy(*(_upload_buffer.m_heap), _upload_buffer.m_offset,
        *(_download_buffer.m_heap), _download_buffer.m_offset,
        sizeof(T) * _upload_buffer.range().size());
  }

protected:
#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
  virtual void enqueue_upload_barrier(const upload_heap& upload_heap,
      size_t _offset_in_bytes, size_t _size) = 0;

  virtual void enqueue_download_barrier(const download_heap& download_heap,
      size_t _offset_in_bytes, size_t _size) = 0;

  virtual void enqueue_copy(const upload_heap& _upload_heap,
      size_t _upload_offset_in_bytes, const download_heap& _download_heap,
      size_t _download_offset_in_bytes, size_t _upload_size) = 0;
#endif
};

using command_list_ptr = memory::unique_ptr<command_list>;

}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_COMMAND_LIST_H__
