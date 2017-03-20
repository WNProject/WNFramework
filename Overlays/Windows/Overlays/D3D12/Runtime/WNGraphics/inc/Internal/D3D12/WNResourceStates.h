// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_D3D12_RESOURCE_STATES_H__
#define __WN_GRAPHICS_INTERNAL_D3D12_RESOURCE_STATES_H__

#include "WNGraphics/inc/WNGraphicsEnums.h"

namespace wn {
namespace graphics {
namespace internal {
namespace d3d12 {

static WN_FORCE_INLINE D3D12_RESOURCE_STATES
resource_state_to_d3d12_resource_states(resource_state _state) {
  static const D3D12_RESOURCE_STATES states[] = {D3D12_RESOURCE_STATE_COMMON,
      D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COPY_DEST,
      D3D12_RESOURCE_STATE_RENDER_TARGET,
      D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE |
          D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
      D3D12_RESOURCE_STATE_PRESENT};

  static_assert((1 << (sizeof(states) / sizeof(states[0]) - 2)) + 1 ==
                    static_cast<uint32_t>(resource_state::max),
      "Expected the number of resource states and D3D12_RESOURCE_STATES to "
      "match");
  WN_DEBUG_ASSERT_DESC(
      _state < resource_state::max, "Resource state out of bounds");

  return states[static_cast<uint32_t>(_state)];
}

static WN_FORCE_INLINE D3D12_RESOURCE_FLAGS resources_states_to_resource_flags(
    resource_states _state) {
  D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;

  if (_state & static_cast<uint32_t>(resource_state::render_target)) {
    flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
  }
  return flags;
}

static WN_FORCE_INLINE D3D12_BLEND blend_to_d3d12(blend_factor _blend) {
  switch (_blend) {
    case blend_factor::zero:
      return D3D12_BLEND_ZERO;
    case blend_factor::one:
      return D3D12_BLEND_ONE;
    case blend_factor::src_color:
      return D3D12_BLEND_SRC_COLOR;
    case blend_factor::inv_src_color:
      return D3D12_BLEND_INV_SRC_COLOR;
    case blend_factor::dst_color:
      return D3D12_BLEND_DEST_COLOR;
    case blend_factor::inv_dst_color:
      return D3D12_BLEND_INV_DEST_COLOR;
    case blend_factor::src_alpha:
      return D3D12_BLEND_SRC_ALPHA;
    case blend_factor::inv_src_alpha:
      return D3D12_BLEND_INV_SRC_ALPHA;
    case blend_factor::dst_alpha:
      return D3D12_BLEND_DEST_ALPHA;
    case blend_factor::inv_dst_alpha:
      return D3D12_BLEND_INV_DEST_ALPHA;
    case blend_factor::constant_color:
      return D3D12_BLEND_BLEND_FACTOR;
    case blend_factor::inv_constant_color:
      return D3D12_BLEND_INV_BLEND_FACTOR;
    case blend_factor::src_alpha_saturate:
      return D3D12_BLEND_SRC_ALPHA_SAT;
    case blend_factor::src1_color:
      return D3D12_BLEND_SRC1_COLOR;
    case blend_factor::inv_src1_color:
      return D3D12_BLEND_INV_SRC1_COLOR;
    case blend_factor::src1_alpha:
      return D3D12_BLEND_SRC1_ALPHA;
    case blend_factor::inv_src1_alpha:
      return D3D12_BLEND_INV_SRC1_ALPHA;
  }

  WN_DEBUG_ASSERT_DESC(false, "You should never end up here");
  return D3D12_BLEND(0);
}

static WN_FORCE_INLINE D3D12_BLEND_OP blend_op_to_d3d12(blend_op _blend) {
  switch (_blend) {
    case blend_op::add:
      return D3D12_BLEND_OP_ADD;
    case blend_op::sub:
      return D3D12_BLEND_OP_SUBTRACT;
    case blend_op::inv_sub:
      return D3D12_BLEND_OP_REV_SUBTRACT;
    case blend_op::min:
      return D3D12_BLEND_OP_MIN;
    case blend_op::max:
      return D3D12_BLEND_OP_MAX;
  }

  WN_DEBUG_ASSERT_DESC(false, "You should never end up here");
  return D3D12_BLEND_OP(0);
}

static WN_FORCE_INLINE D3D12_LOGIC_OP logic_op_to_d3d12(logic_op _logic) {
  switch (_logic) {
    case logic_op::disabled:
      return D3D12_LOGIC_OP(0);
    case logic_op::op_clear:
      return D3D12_LOGIC_OP_CLEAR;
    case logic_op::op_set:
      return D3D12_LOGIC_OP_SET;
    case logic_op::op_nop:
      return D3D12_LOGIC_OP_NOOP;
    case logic_op::op_copy:
      return D3D12_LOGIC_OP_COPY;
    case logic_op::op_copy_reverse:
      return D3D12_LOGIC_OP_COPY_INVERTED;
    case logic_op::op_invert:
      return D3D12_LOGIC_OP_INVERT;
    case logic_op::op_and:
      return D3D12_LOGIC_OP_AND;
    case logic_op::op_and_reverse:
      return D3D12_LOGIC_OP_AND_REVERSE;
    case logic_op::op_and_invert:
      return D3D12_LOGIC_OP_AND_INVERTED;
    case logic_op::op_nand:
      return D3D12_LOGIC_OP_NAND;
    case logic_op::op_or:
      return D3D12_LOGIC_OP_OR;
    case logic_op::op_or_reverse:
      return D3D12_LOGIC_OP_OR_REVERSE;
    case logic_op::op_or_invert:
      return D3D12_LOGIC_OP_OR_INVERTED;
    case logic_op::op_xor:
      return D3D12_LOGIC_OP_XOR;
    case logic_op::op_nor:
      return D3D12_LOGIC_OP_NOR;
    case logic_op::op_nxor:
      return D3D12_LOGIC_OP_EQUIV;
  }

  WN_DEBUG_ASSERT_DESC(false, "You should never end up here");
  return D3D12_LOGIC_OP(0);
}

static WN_FORCE_INLINE UINT8 write_components_to_d3d12(
    write_components _write) {
  uint8_t write = 0;
  if (_write & static_cast<uint32_t>(write_component::r)) {
    write |= D3D12_COLOR_WRITE_ENABLE_RED;
  }
  if (_write & static_cast<uint32_t>(write_component::g)) {
    write |= D3D12_COLOR_WRITE_ENABLE_GREEN;
  }
  if (_write & static_cast<uint32_t>(write_component::b)) {
    write |= D3D12_COLOR_WRITE_ENABLE_BLUE;
  }
  if (_write & static_cast<uint32_t>(write_component::a)) {
    write |= D3D12_COLOR_WRITE_ENABLE_ALPHA;
  }
  return write;
}

static WN_FORCE_INLINE D3D12_FILL_MODE fill_to_d3d12(fill_mode _fill) {
  switch (_fill) {
    case fill_mode::fill:
      return D3D12_FILL_MODE_SOLID;
    case fill_mode::line:
      return D3D12_FILL_MODE_WIREFRAME;
    case fill_mode::point:
      WN_DEBUG_ASSERT_DESC(false, "D3D12 does not support line fill");
  }
  WN_DEBUG_ASSERT_DESC(false, "You should never end up here");
  return D3D12_FILL_MODE(0);
}

static WN_FORCE_INLINE D3D12_CULL_MODE cull_mode_to_d3d12(cull_mode _cull) {
  switch (_cull) {
    case cull_mode::back:
      return D3D12_CULL_MODE_BACK;
    case cull_mode::front:
      return D3D12_CULL_MODE_FRONT;
    case cull_mode::none:
      return D3D12_CULL_MODE_NONE;
  }
  WN_DEBUG_ASSERT_DESC(false, "You should never end up here");
  return D3D12_CULL_MODE(0);
}

static WN_FORCE_INLINE D3D12_COMPARISON_FUNC comparison_op_to_d3d12(
    comparison_op _compare) {
  switch (_compare) {
    case comparison_op::never:
      return D3D12_COMPARISON_FUNC_NEVER;
    case comparison_op::always:
      return D3D12_COMPARISON_FUNC_ALWAYS;
    case comparison_op::equal:
      return D3D12_COMPARISON_FUNC_EQUAL;
    case comparison_op::not_equal:
      return D3D12_COMPARISON_FUNC_NOT_EQUAL;
    case comparison_op::less:
      return D3D12_COMPARISON_FUNC_LESS;
    case comparison_op::less_than_or_equal:
      return D3D12_COMPARISON_FUNC_LESS_EQUAL;
    case comparison_op::greater:
      return D3D12_COMPARISON_FUNC_GREATER;
    case comparison_op::greater_than_or_equal:
      return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
  }
  WN_DEBUG_ASSERT_DESC(false, "You should never end up here");
  return D3D12_COMPARISON_FUNC(0);
}

static WN_FORCE_INLINE D3D12_STENCIL_OP stencil_op_to_d3d12(
    stencil_op _stencil) {
  switch (_stencil) {
    case stencil_op::keep:
      return D3D12_STENCIL_OP_KEEP;
    case stencil_op::zero:
      return D3D12_STENCIL_OP_ZERO;
    case stencil_op::replace:
      return D3D12_STENCIL_OP_REPLACE;
    case stencil_op::invert:
      return D3D12_STENCIL_OP_INVERT;
    case stencil_op::increment_wrap:
      return D3D12_STENCIL_OP_INCR;
    case stencil_op::decrement_wrap:
      return D3D12_STENCIL_OP_DECR;
    case stencil_op::increment_saturate:
      return D3D12_STENCIL_OP_INCR_SAT;
    case stencil_op::decrement_saturate:
      return D3D12_STENCIL_OP_DECR_SAT;
  }
  WN_DEBUG_ASSERT_DESC(false, "You should never end up here");
  return D3D12_STENCIL_OP(0);
}

static WN_FORCE_INLINE D3D12_PRIMITIVE_TOPOLOGY_TYPE
topology_to_d3d12_topology_type(topology _topology) {
  switch (_topology) {
    case topology::undefined:
      return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
    case topology::point_list:
      return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
    case topology::line_list:
    case topology::line_strip:
    case topology::line_list_with_adjacency:
    case topology::line_strip_with_adjacency:
      return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    case topology::triangle_list:
    case topology::triangle_strip:
    case topology::triangle_list_with_adjacency:
    case topology::triangle_strip_with_adjacency:
      return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    case topology::patch_list:
      return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
  }
  WN_DEBUG_ASSERT_DESC(false, "You should never end up here");
  return D3D12_PRIMITIVE_TOPOLOGY_TYPE(0);
}

static WN_FORCE_INLINE D3D_PRIMITIVE_TOPOLOGY topology_to_d3d12_topology(
    topology _topology) {
  switch (_topology) {
    case topology::undefined:
      return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    case topology::point_list:
      return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
    case topology::line_list:
      return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
    case topology::line_strip:
      return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
    case topology::line_list_with_adjacency:
      return D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
    case topology::line_strip_with_adjacency:
      return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
    case topology::triangle_list:
      return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    case topology::triangle_strip:
      return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
    case topology::triangle_list_with_adjacency:
      return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
    case topology::triangle_strip_with_adjacency:
      return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
    case topology::patch_list:
      return D3D_PRIMITIVE_TOPOLOGY(
          D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST - 1);
  }
  WN_DEBUG_ASSERT_DESC(false, "You should never end up here");
  return D3D_PRIMITIVE_TOPOLOGY(0);
}

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_D3D12_RESOURCE_STATES_H__