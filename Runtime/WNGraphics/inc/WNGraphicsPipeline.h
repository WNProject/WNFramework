// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_GRAPHICS_PIPELINE_H__
#define __WN_GRAPHICS_GRAPHICS_PIPELINE_H__

#include "WNContainers/inc/WNStringView.h"
#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNGraphics/inc/WNGraphicsObjectBase.h"
#include "WNMemory/inc/manipulation.h"

WN_GRAPHICS_FORWARD(command_list);

namespace wn {
namespace runtime {
namespace graphics {

class graphics_pipeline final : public base_object<2> {
public:
  WN_FORCE_INLINE graphics_pipeline(graphics_pipeline&& _other)
    : m_device(_other.m_device) {
    _other.m_device = nullptr;

    memory::memcpy(&m_data, &_other.m_data, sizeof(opaque_data));
    memory::memzero(&_other.m_data, sizeof(opaque_data));
  }

  ~graphics_pipeline() {
    if (m_device) {
      m_device->destroy_graphics_pipeline(this);
    }
  }

private:
  WN_GRAPHICS_ADD_FRIENDS(device)
  WN_GRAPHICS_ADD_FRIENDS(command_list);

  WN_FORCE_INLINE graphics_pipeline(device* _device) : m_device(_device) {}
  device* m_device;
  friend class descriptor_pool;
};

}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_GRAPHICS_PIPELINE_H__
