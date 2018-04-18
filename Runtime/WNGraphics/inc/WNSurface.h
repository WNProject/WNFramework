// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_SURFACE_H__
#define __WN_GRAPHICS_SURFACE_H__

#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNMemory/inc/manipulation.h"

WN_GRAPHICS_FORWARD(adapter)
WN_GRAPHICS_FORWARD(device)

namespace wn {
namespace runtime {
namespace graphics {
class adapter;
class device;

class surface final : public core::non_copyable {
public:
  surface() = delete;

  ~surface();

  WN_FORCE_INLINE surface(surface&& _other) {
    m_adapter = _other.m_adapter;
    _other.m_adapter = nullptr;
    m_width = _other.m_width;
    m_height = _other.m_height;
    memory::memcpy(&m_data, &_other.m_data, sizeof(opaque_data));
    memory::memzero(&_other.m_data, sizeof(opaque_data));
  }

  WN_FORCE_INLINE bool is_valid() const {
    return (m_adapter != nullptr);
  }

  uint32_t get_width() const {
    return m_width;
  }

  uint32_t get_height() const {
    return m_height;
  }

private:
  WN_FORCE_INLINE void set_invalid() {
    m_adapter = nullptr;
  }

  WN_GRAPHICS_ADD_FRIENDS(adapter)
  WN_GRAPHICS_ADD_FRIENDS(device);

  WN_FORCE_INLINE surface(
      adapter* _adapter, uint32_t _width = 0, uint32_t _height = 0)
    : m_data({0}), m_adapter(_adapter), m_width(_width), m_height(_height) {}

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
  struct opaque_data final {
    uint64_t _dummy[2];
  } m_data;

  adapter* m_adapter;
  uint32_t m_width;
  uint32_t m_height;
};

}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_SURFACE_H__
