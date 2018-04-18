// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_RENDER_PASS_H__
#define __WN_GRAPHICS_RENDER_PASS_H__

#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNCommandList.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNGraphics/inc/WNGraphicsEnums.h"
#include "WNGraphics/inc/WNRenderPassTypes.h"
#include "WNMemory/inc/manipulation.h"

WN_GRAPHICS_FORWARD(queue)
WN_GRAPHICS_FORWARD(device)
WN_GRAPHICS_FORWARD(command_list)

namespace wn {
namespace runtime {
namespace graphics {

class render_pass final : public base_object<2> {
public:
  render_pass() = delete;

  WN_FORCE_INLINE render_pass(render_pass&& _other)
    : m_device(_other.m_device) {
    _other.m_device = nullptr;

    memory::memcpy(&m_data, &_other.m_data, sizeof(opaque_data));
    memory::memzero(&_other.m_data, sizeof(opaque_data));
  }

  WN_FORCE_INLINE ~render_pass() {
    if (is_valid()) {
      m_device->destroy_render_pass(this);
    }
  }

  WN_FORCE_INLINE bool is_valid() const {
    return (m_device != nullptr);
  }

private:
  WN_GRAPHICS_ADD_FRIENDS(queue)
  WN_GRAPHICS_ADD_FRIENDS(device)
  WN_GRAPHICS_ADD_FRIENDS(command_list)

  WN_FORCE_INLINE render_pass(device* _device) : m_device(_device) {}

  device* m_device;
};

}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_RENDER_PASS_H__
