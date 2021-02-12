// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_RUNTIME_GRAPHICS_ARENA_H__
#define __WN_RUNTIME_GRAPHICS_ARENA_H__

#include "WNGraphics/inc/WNDevice.h"
#include "WNGraphics/inc/WNGraphicsObjectBase.h"
#include "WNMemory/inc/string_utility.h"
#include "core/inc/types.h"

WN_GRAPHICS_FORWARD(device);

namespace wn {
namespace runtime {
namespace graphics {

class arena final : public base_object<2> {
private:
  using base = base_object<2>;

public:
  inline arena(arena&& _other)
    : m_device(core::move(_other.m_device)), m_size(core::move(_other.m_size)) {
    _other.m_device = nullptr;
    _other.m_size = 0;

    memory::memory_copy(&m_data, &_other.m_data);
    memory::memory_zero(&_other.m_data);
  }

  inline ~arena() {
    if (is_valid()) {
      m_device->destroy_arena(this);
    }
  }

  inline bool is_valid() const {
    return (m_device != nullptr && size() != 0);
  }

  inline uint64_t size() const {
    return m_size;
  }

private:
  WN_GRAPHICS_ADD_FRIENDS(device);

  inline arena() : base(), m_device(nullptr), m_size(0) {}

  inline arena(device* _device) : base(), m_device(_device), m_size(0) {}

  device* m_device;
  uint64_t m_size;
};

}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_RUNTIME_GRAPHICS_ARENA_H__
