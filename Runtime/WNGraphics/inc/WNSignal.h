// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_SIGNAL_H__
#define __WN_GRAPHICS_SIGNAL_H__

#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNMemory/inc/manipulation.h"

WN_GRAPHICS_FORWARD(queue)
WN_GRAPHICS_FORWARD(device)
WN_GRAPHICS_FORWARD(swapchain)

namespace wn {
namespace runtime {
namespace graphics {

class signal final : public core::non_copyable {
public:
  signal() = delete;

  inline signal(signal&& _other) : m_device(_other.m_device) {
    _other.m_device = nullptr;

    memory::memcpy(&m_data, &_other.m_data, sizeof(opaque_data));
    memory::memzero(&_other.m_data, sizeof(opaque_data));
  }

  inline ~signal() {
    if (is_valid()) {
      m_device->destroy_signal(this);
    }
  }

  inline bool is_valid() const {
    return (m_device != nullptr);
  }

private:
  WN_GRAPHICS_ADD_FRIENDS(queue)
  WN_GRAPHICS_ADD_FRIENDS(device)
  WN_GRAPHICS_ADD_FRIENDS(swapchain)

  inline signal(device* _device) : m_data({0}), m_device(_device) {}

  template <typename T>
  inline T& data_as() {
    static_assert(sizeof(opaque_data) >= sizeof(T),
        "invalid cast, target type size does not match opaque data size");

    return *reinterpret_cast<T*>(&m_data);
  }

  template <typename T>
  inline const T& data_as() const {
    static_assert(sizeof(opaque_data) >= sizeof(T),
        "invalid cast, target type size does not match opaque data size");

    return *reinterpret_cast<const T*>(&m_data);
  }

  // The opaque_data must be trivially copyable.
  // It also must be considered uninitialized when
  // memset to 0.
  struct opaque_data final {
    uint64_t _dummy[1];
  } m_data;

  device* m_device;
};

}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_SIGNAL_H__
