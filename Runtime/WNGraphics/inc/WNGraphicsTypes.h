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

enum class sampler_filter : uint8_t { nearest, linear };
enum class sampler_addressing : uint8_t {
  repeat,
  mirrored_repeat,
  clamp,
  border,
  mirror_clamp
};
enum class border_color : uint8_t {
  black_transparent_f32,
  black_transparent_uint,
  black_opaque_f32,
  black_opaque_uint,
  white_opaque_f32,
  white_opaque_uint
};

struct sampler_create_info {
  sampler_filter min = sampler_filter::nearest;
  sampler_filter mag = sampler_filter::nearest;
  sampler_filter mip = sampler_filter::nearest;
  sampler_addressing u = sampler_addressing::repeat;
  sampler_addressing v = sampler_addressing::repeat;
  sampler_addressing w = sampler_addressing::repeat;
  float lod_bias = 0.0f;
  bool enable_anisotropy = false;
  float max_anisotropy = 1.0f;
  bool enable_comparison = false;
  comparison_op comparison = comparison_op::always;
  float min_lod = 0.0f;
  float max_lod = 0.0f;
  border_color border = border_color::black_opaque_f32;
};

inline bool operator==(
    const sampler_create_info& _a, const sampler_create_info& _b) {
  size_t anisotropy_match =
      (_a.enable_anisotropy == _b.enable_anisotropy) &&
      (!_a.enable_anisotropy || (_a.max_anisotropy == _b.max_anisotropy));

  size_t comparison_match =
      (_a.enable_comparison == _b.enable_comparison) &&
      (!_a.enable_comparison || (_a.comparison == _b.comparison));
  return anisotropy_match && comparison_match && _a.min == _b.min &&
         _a.mag == _b.mag && _a.mip == _b.mip && _a.u == _b.u && _a.v == _b.v &&
         _a.w == _b.w && _a.lod_bias == _b.lod_bias &&
         _a.min_lod == _b.min_lod && _a.max_lod == _b.max_lod &&
         _a.border == _b.border;
}

struct sampler_create_info_hasher {
  inline size_t operator()(const sampler_create_info& i) const {
    size_t anisotropy_hash =
        i.enable_anisotropy ? std::hash<float>{}(i.max_anisotropy) : 0;
    size_t comparison_hash =
        i.enable_comparison
            ? std::hash<uint32_t>{}(static_cast<uint32_t>(i.comparison))
            : 0;

    return std::hash<uint32_t>{}(static_cast<uint8_t>(i.min)) ^
           std::hash<uint32_t>{}(static_cast<uint8_t>(i.mag) << 4) ^
           std::hash<uint32_t>{}(static_cast<uint8_t>(i.mip) << 8) ^
           std::hash<uint32_t>{}(static_cast<uint8_t>(i.u) << 12) ^
           std::hash<uint32_t>{}(static_cast<uint8_t>(i.v) << 16) ^
           std::hash<uint32_t>{}(static_cast<uint8_t>(i.w) << 24) ^
           std::hash<float>{}(i.lod_bias) ^ anisotropy_hash ^ comparison_hash ^
           std::hash<float>{}(i.min_lod) << 8 ^
           std::hash<float>{}(i.min_lod) >> 8 ^
           std::hash<float>{}(i.max_lod) << 12 ^
           std::hash<float>{}(i.max_lod) >> 4 ^
           std::hash<uint32_t>{}(static_cast<uint8_t>(i.border) << 28);
  }
};

}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_GRAPHICS_TYPES_H__