// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_FENCE_H__
#define __WN_GRAPHICS_FENCE_H__

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNMemory/inc/WNBasic.h"

namespace wn {
namespace graphics {
namespace internal {
namespace d3d12 {

class queue;

} // namespace d3d12

namespace vulkan {

class queue;

} // namespace vulkan
} // namesapce internal

// The memory returned by this heap type may or may not be
// coherent. Make sure to flush any range before use. This will
// get optimized away if a flush was not strictly necessary.
class fence : public core::non_copyable {
public:
  ~fence() {
    if (m_device) {
      m_device->destroy_fence(this);
    }
  }

  bool WN_FORCE_INLINE is_valid() const {
    return m_device != nullptr;
  }

  WN_FORCE_INLINE fence(fence&& _other) {
    memory::memcpy(&m_data, &_other.m_data, sizeof(opaque_data));
    m_device = _other.m_device;
    memory::memzero(&_other.m_data, sizeof(opaque_data));
    _other.m_device = nullptr;
  }

  void WN_FORCE_INLINE wait() const {
    m_device->wait_fence(this);
  }

  void WN_FORCE_INLINE reset() {
    m_device->reset_fence(this);
  }

private:
  friend class internal::queue;
  WN_FORCE_INLINE fence(device* _device)
    : m_device(_device), m_data({0}) {}

  template <typename T>
  T& data_as() {
    static_assert(sizeof(opaque_data) >= sizeof(T), "Invalid cast");
    return *reinterpret_cast<T*>(&m_data);
  }

  template <typename T>
  const T& data_as() const {
    static_assert(sizeof(opaque_data) >= sizeof(T), "Invalid cast");
    return *reinterpret_cast<const T*>(&m_data);
  }

#include "WNGraphics/inc/Internal/WNSetFriendDevices.h"
#include "WNGraphics/inc/Internal/WNSetFriendQueues.h"

  // The opaque_data must be trivially copyable.
  // It also must be considered uninitialized when
  // memset to 0.
  struct opaque_data {
    uint64_t _dummy[2];
  } m_data;

  device* m_device;
};

}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_HEAP_H__