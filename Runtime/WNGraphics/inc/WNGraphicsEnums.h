// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_GRAPHICS_ENUMS_H__
#define __WN_GRAPHICS_GRAPHICS_ENUMS_H__

#include "WNCore/inc/WNBase.h"
#include "WNCore/inc/WNTypes.h"

namespace wn {
namespace graphics {

enum class data_format { r8g8b8a8_unorm, r32g32b32a32_sfloat, max };

enum class stream_frequency { per_vertex, per_instance };
enum class topology {
  undefined,
  point_list,
  line_list,
  line_strip,
  triangle_list,
  triangle_strip,
  line_list_with_adjacency,
  line_strip_with_adjacency,
  triangle_list_with_adjacency,
  triangle_strip_with_adjacency,
  patch_list  // This may not be supported by all hardware
};
enum class fill_mode { fill, line, point };

enum class index_type {
  none,
  u16,
  u32,
};

enum class winding { clockwise, counter_clockwise };

enum class cull_mode { back, front, none };

// If you re-order these make sure the logic in create_graphics_pipeline remains
// correct
enum class shader_stage : uint32_t {
  vertex = 1 << 0,
  tessellation_control = 1 << 1,     // Can only be used if the device supports
                                     // tessellation
  tessellation_evaluation = 1 << 2,  // Only if tessellation is supported.
  geometry = 1 << 3,  // Can only be used if the devive supports geometry
  pixel = 1 << 4,
  compute = 1 << 5
};

enum class blend_factor {
  zero,
  one,
  src_color,
  inv_src_color,
  dst_color,
  inv_dst_color,
  src_alpha,
  inv_src_alpha,
  dst_alpha,
  inv_dst_alpha,
  constant_color,
  inv_constant_color,
  src_alpha_saturate,
  src1_color,
  inv_src1_color,
  src1_alpha,
  inv_src1_alpha
};

enum class blend_op {  // s == source, d == dst, _c = component _f = factor
  add,                 // s_c * s_f + d_c * d_f
  sub,                 // s_c * s_f - d_c * d_f
  inv_sub,             // d_c * d_f - s_c * s_f
  min,                 // min(s_c, d_c)
  max,                 // max(s_c, d_c)
};

enum class write_component : uint8_t {
  r = 1 << 0,
  g = 1 << 1,
  b = 1 << 2,
  a = 1 << 3
};

static const uint8_t k_all_write_components =
    static_cast<uint8_t>(write_component::r) |
    static_cast<uint8_t>(write_component::g) |
    static_cast<uint8_t>(write_component::b) |
    static_cast<uint8_t>(write_component::a);
using write_components = uint8_t;

enum class logic_op {  // s == source, d == dest
  disabled,            // ~
  op_clear,            // 0
  op_set,              // ~0
  op_nop,              // d
  op_copy,             // s
  op_copy_reverse,     // ~s
  op_invert,           // ~d
  op_and,              // s & d
  op_and_reverse,      // s & ~d
  op_and_invert,       // ~s & d
  op_nand,             // ~(s & d)
  op_or,               // s | d
  op_or_reverse,       // s | ~d
  op_or_invert,        // ~s | d
  op_xor,              // s ^ d
  op_nor,              // ~(s | d)
  op_nxor,             // ~(s ^ d)
};

enum class comparison_op {
  never,
  always,
  equal,
  not_equal,
  less,
  less_than_or_equal,
  greater,
  greater_than_or_equal,
};

enum class stencil_op {
  keep,
  zero,
  replace,
  invert,
  increment_wrap,
  decrement_wrap,
  increment_saturate,
  decrement_saturate,
};

enum class multisample_count {
  disabled = 0,
  samples_1 = 1,
  samples_2 = 2,
  samples_4 = 4,
  samples_8 = 8,
  samples_16 = 16,
  samples_32 = 32,
  samples_64 = 64
};

enum class load_op { load, clear, dont_care };

enum class store_op { store, dont_care };

enum class resource_state : uint32_t {
  initial = 0,
  copy_source = 1 << 0,
  copy_dest = 1 << 1,
  render_target = 1 << 2,
  texture = 1 << 3,
  present = 1 << 4,
  max,
};

const static uint32_t k_all_resource_states =
    ((static_cast<uint32_t>(resource_state::max) - 1) << 1) - 1;
using resource_states = uint32_t;

enum class descriptor_type : uint16_t {
  sampler,
  sampled_image,
  read_only_buffer,
  read_only_image_buffer,
  read_only_sampled_buffer,
  read_write_buffer,
  read_write_image_buffer,
  read_write_sampled_buffer,
  descriptor_type_max
  // input_attachment << TODO(awoloszyn): Figure out if/what we will do here.
  // D3D does not have an equivalent, but we can easily emulate with other types
  // Combined image/sampler? maybe
};

enum class image_component : uint8_t {
  color = 1 << 0,
  depth = 1 << 1,
  stencil = 1 << 2
};

using image_components = uint8_t;

}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_GRAPHICS_ENUMS_H__
