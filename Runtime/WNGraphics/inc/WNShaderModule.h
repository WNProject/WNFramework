// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_SHADER_MODULE_H__
#define __WN_GRAPHICS_SHADER_MODULE_H__

#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNGraphics/inc/WNGraphicsEnums.h"
#include "WNGraphics/inc/WNGraphicsObjectBase.h"
#include "WNMemory/inc/manipulation.h"

WN_GRAPHICS_FORWARD(queue)
WN_GRAPHICS_FORWARD(device)

namespace wn {
namespace runtime {
namespace graphics {

class shader_module final : public base_object<2> {
public:
  shader_module() : m_device(nullptr) {}

  inline shader_module(shader_module&& _other) : m_device(_other.m_device) {
    _other.m_device = nullptr;

    memory::memcpy(&m_data, &_other.m_data, sizeof(opaque_data));
    memory::memzero(&_other.m_data, sizeof(opaque_data));
  }

  inline ~shader_module() {
    if (is_valid()) {
      m_device->destroy_shader_module(this);
    }
  }

  inline bool is_valid() const {
    return (m_device != nullptr);
  }

  shader_module& operator=(shader_module&& _other) {
    if (is_valid()) {
      m_device->destroy_shader_module(this);
    }
    m_device = _other.m_device;
    _other.m_device = nullptr;

    memory::memcpy(&m_data, &_other.m_data, sizeof(opaque_data));
    memory::memzero(&_other.m_data, sizeof(opaque_data));
    return *this;
  }

private:
  WN_GRAPHICS_ADD_FRIENDS(device);

  inline shader_module(device* _device) : m_device(_device) {}

  device* m_device;
};

}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_SHADER_MODULE_H__
