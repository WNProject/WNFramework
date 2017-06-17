// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_SHADER_MODULE_H__
#define __WN_GRAPHICS_SHADER_MODULE_H__

#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNGraphics/inc/WNGraphicsEnums.h"
#include "WNGraphics/inc/WNGraphicsObjectBase.h"
#include "WNMemory/inc/WNBasic.h"

WN_GRAPHICS_FORWARD(queue)
WN_GRAPHICS_FORWARD(device)

namespace wn {
namespace runtime {
namespace graphics {

class shader_module WN_FINAL : public base_object<2> {
public:
  shader_module() = delete;

  WN_FORCE_INLINE shader_module(shader_module&& _other)
    : m_device(_other.m_device) {
    _other.m_device = nullptr;

    memory::memcpy(&m_data, &_other.m_data, sizeof(opaque_data));
    memory::memzero(&_other.m_data, sizeof(opaque_data));
  }

  WN_FORCE_INLINE ~shader_module() {
    if (is_valid()) {
      m_device->destroy_shader_module(this);
    }
  }

  WN_FORCE_INLINE bool is_valid() const {
    return (m_device != nullptr);
  }

private:
  WN_GRAPHICS_ADD_FRIENDS(device);

  WN_FORCE_INLINE shader_module(device* _device) : m_device(_device) {}

  device* m_device;
};

}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_SHADER_MODULE_H__
