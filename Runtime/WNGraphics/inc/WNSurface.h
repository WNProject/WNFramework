// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_SURFACE_H__
#define __WN_GRAPHICS_SURFACE_H__

#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNGraphics/inc/WNErrors.h"
#include "WNMemory/inc/manipulation.h"

WN_GRAPHICS_FORWARD(adapter)
WN_GRAPHICS_FORWARD(device)

namespace wn {
namespace runtime {
namespace graphics {
class adapter;
class device;

struct surface_capabilities {
  uint32_t width;
  uint32_t height;
};

class surface final : public core::non_copyable {
public:
  surface() = delete;

  ~surface();

  inline surface(surface&& _other) {
    m_adapter = _other.m_adapter;
    _other.m_adapter = nullptr;
    m_width = _other.m_width;
    m_height = _other.m_height;
    m_valid_formats = core::move(_other.m_valid_formats);
    memory::memcpy(&m_data, &_other.m_data, sizeof(opaque_data));
    memory::memzero(&_other.m_data, sizeof(opaque_data));
  }

  inline bool is_valid() const {
    return (m_adapter != nullptr);
  }

  graphics_error get_surface_capabilities(surface_capabilities* _capabilities);

  const containers::dynamic_array<data_format>& valid_formats() const {
    return m_valid_formats;
  }

private:
  inline void set_invalid() {
    m_adapter = nullptr;
  }

  WN_GRAPHICS_ADD_FRIENDS(adapter)
  WN_GRAPHICS_ADD_FRIENDS(device);

  inline surface(memory::allocator* _allocator, adapter* _adapter)
    : m_data({0}), m_adapter(_adapter), m_valid_formats(_allocator) {}

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
    uint64_t _dummy[2];
  } m_data;

  adapter* m_adapter;
  uint32_t m_width = 0;
  uint32_t m_height = 0;
  containers::dynamic_array<data_format> m_valid_formats;
};

}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_SURFACE_H__
