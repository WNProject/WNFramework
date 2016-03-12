// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_DEVICE_H__
#define __WN_GRAPHICS_DEVICE_H__

#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNMemory/inc/WNUniquePtr.h"

namespace WNLogging {
class WNLog;
}  // namespace WNLogging

namespace wn {
namespace graphics {
class upload_heap;

namespace internal {
namespace d3d12 {

class device;

}  // namespace d3d12

namespace vulkan {

class device;

}  // namespace vulkan

class device : public core::non_copyable {
public:
  WN_FORCE_INLINE device(memory::allocator* _allocator, WNLogging::WNLog* _log)
    : m_allocator(_allocator), m_log(_log) {}

  // On success, returns an upload_heap object. This heap must be at least,
  // 1 byte in size. If creation fails a nullptr will be returned.
  virtual upload_heap create_upload_heap(size_t _num_bytes) = 0;

protected:
  friend class upload_heap;
  // Upload heap methods
  virtual void flush_mapped_range(
      upload_heap* _buffer, size_t _offset, size_t _num_bytes) = 0;
  virtual void destroy_upload_heap(upload_heap* _heap) = 0;

  memory::allocator* m_allocator;
  WNLogging::WNLog* m_log;
};
}  // namespace internal

}  // namespace graphics
}  // namespace wn

#if _WN_GRAPHICS_DEVICE_TYPES_AVAILABLE > 1
namespace wn {
namespace graphics {
using device = internal::device;
}  // namespace graphics
}  // namespace wn
#elif defined _WN_GRAPHICS_VULKAN_DEVICE_TYPE_AVAILABLE
#include "WNGraphics/inc/Internal/Vulkan/WNDevice.h"
namespace wn {
namespace graphics {
using device = internal::vulkan::device;
}  // namespace graphics
}  // namespace wn
#elif defined _WN_GRAPHICS_D3D12_DEVICE_TYPE_AVAILABLE
#include "WNGraphics/inc/Internal/D3D12/WNDevice.h"
namespace wn {
namespace graphics {
using device = internal::d3d12::device;
}  // namespace graphics
}  // namespace wn
#endif

namespace wn {
namespace graphics {
using device_ptr = memory::unique_ptr<device>;
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_DEVICE_H__