// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_FENCE_H__
#define __WN_GRAPHICS_FENCE_H__

#include "WNGraphics/inc/WNDevice.h"
#include "WNMemory/inc/WNBasic.h"

namespace wn {
namespace graphics {
namespace internal {
namespace d3d12 {

class d3d12_queue;
class d3d12_device;

}  // namespace d3d12

namespace vulkan {

class vulkan_device;
class vulkan_queue;

}  // namespace vulkan
}  // namesapce internal

class queue;

class fence WN_FINAL : public core::non_copyable {
public:
  fence() = delete;

  WN_FORCE_INLINE fence(fence&& _other) : m_device(_other.m_device) {
    _other.m_device = nullptr;

    memory::memcpy(&m_data, &_other.m_data, sizeof(opaque_data));
    memory::memzero(&_other.m_data, sizeof(opaque_data));
  }

  WN_FORCE_INLINE ~fence() {
    if (is_valid()) {
      m_device->destroy_fence(this);
    }
  }

  WN_FORCE_INLINE bool is_valid() const {
    return (m_device != nullptr);
  }

  WN_FORCE_INLINE void wait() const {
    m_device->wait_fence(this);
  }

  WN_FORCE_INLINE void reset() {
    m_device->reset_fence(this);
  }

private:
  friend class internal::d3d12::d3d12_queue;
  friend class internal::d3d12::d3d12_device;
  friend class internal::vulkan::vulkan_queue;
  friend class internal::vulkan::vulkan_device;
  friend class device;
  friend class queue;

  WN_FORCE_INLINE fence(device* _device) : m_device(_device), m_data({0}) {}

  template <typename T>
  WN_FORCE_INLINE T& data_as() {
    static_assert(sizeof(opaque_data) >= sizeof(T),
        "invalid cast, target type size does not match opaque data size");

    return *reinterpret_cast<T*>(&m_data);
  }

  template <typename T>
  WN_FORCE_INLINE const T& data_as() const {
    static_assert(sizeof(opaque_data) >= sizeof(T),
        "invalid cast, target type size does not match opaque data size");

    return *reinterpret_cast<const T*>(&m_data);
  }

  // The opaque_data must be trivially copyable.
  // It also must be considered uninitialized when
  // memset to 0.
  struct opaque_data WN_FINAL {
    uint64_t _dummy[2];
  } m_data;

  device* m_device;
};

}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_HEAP_H__
