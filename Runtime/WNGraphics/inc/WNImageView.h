// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_IMAGE_VIEW_H__
#define __WN_GRAPHICS_IMAGE_VIEW_H__

#include "WNGraphics/inc/WNGraphicsObjectBase.h"
#include "WNGraphics/inc/WNImage.h"

namespace wn {
namespace graphics {

class image_view : public base_object<2> {
public:
  WN_FORCE_INLINE image_view(image_view&& _other) : m_device(_other.m_device) {
    _other.m_device = nullptr;

    memory::memcpy(&m_data, &_other.m_data, sizeof(opaque_data));
    memory::memzero(&_other.m_data, sizeof(opaque_data));
  }

  ~image_view() {
    if (m_device) {
      m_device->destroy_image_view(this);
    }
  }

private:
  WN_GRAPHICS_ADD_FRIENDS(device);
  WN_GRAPHICS_ADD_FRIENDS(command_list);

  WN_FORCE_INLINE image_view(device* _device) : m_device(_device) {}

  device* m_device;
};

}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_IMAGE_VIEW_H__
