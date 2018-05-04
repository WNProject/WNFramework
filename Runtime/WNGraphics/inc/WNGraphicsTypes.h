// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_GRAPHICS_TYPES_H__
#define __WN_GRAPHICS_GRAPHICS_TYPES_H__

#include "WNCore/inc/types.h"

namespace wn {
namespace runtime {
namespace graphics {

union clear_color {
  float float_vals[4];
  int32_t int_vals[4];
  uint32_t uint_vals[4];
};

struct clear_depth {
  float depth;
  uint32_t stencil;
};

union clear_value {
  clear_color color;
  clear_depth depth;
};

struct render_area {
  int32_t x;
  int32_t y;
  uint32_t width;
  uint32_t height;
};

struct image_create_info {
  static image_create_info default_texture(size_t _width, size_t _height) {
    return image_create_info{_width, _height, data_format::r8g8b8a8_unorm,
        static_cast<resource_states>(resource_state::texture), 1};
  }

  size_t m_width;
  size_t m_height;
  data_format m_format;
  resource_states m_valid_resource_states;  // Bit-flags of the image type.
  uint32_t m_mip_levels;

  // TODO(awoloszyn): Add mip-levels
  // TODO(awoloszyn): Add Depth/Array Size
  // TODO(awoloszyn): Add Multisample information
};

struct scissor {
  int32_t x;
  int32_t y;
  uint32_t width;
  uint32_t height;
};

struct push_constant_range {
  uint32_t valid_stages;
  uint32_t register_base;
  uint32_t num_uint32_values;
  uint32_t offset_in_uint32s;
};

}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_GRAPHICS_TYPES_H__