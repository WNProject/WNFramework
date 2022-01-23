// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_VULKAN_RESOURCE_STATES_H__
#define __WN_GRAPHICS_INTERNAL_VULKAN_RESOURCE_STATES_H__

#include "WNGraphics/inc/WNGraphicsEnums.h"
#include "WNMath/inc/WNBasic.h"

namespace wn {
namespace runtime {
namespace graphics {
namespace internal {
namespace vulkan {

static inline VkFlags resource_state_to_vulkan_access_flags(
    resource_state _state) {
  static const VkFlags states[] = {
      0,                                    // initial
      VK_ACCESS_HOST_WRITE_BIT,             // host_write
      VK_ACCESS_HOST_READ_BIT,              // host_read
      VK_ACCESS_TRANSFER_READ_BIT,          // copy_source
      VK_ACCESS_TRANSFER_WRITE_BIT,         // copy_dest
      VK_ACCESS_TRANSFER_READ_BIT,          // blit_source
      VK_ACCESS_TRANSFER_WRITE_BIT,         // blit_dest
      VK_ACCESS_INDEX_READ_BIT,             // index_buffer
      VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,  // vertex_buffer
      VK_ACCESS_UNIFORM_READ_BIT,           // read_only_buffer
      VK_ACCESS_SHADER_READ_BIT |
          VK_ACCESS_SHADER_WRITE_BIT,        // read_write_buffer
      VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,  // render_target
      VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
          VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,  // depth_target
      VK_ACCESS_SHADER_READ_BIT,                        // texture
      VK_ACCESS_MEMORY_READ_BIT                         // present
  };

  static_assert((1 << (sizeof(states) / sizeof(states[0]) - 2)) + 1 ==
                    static_cast<uint32_t>(resource_state::max),
      "Expected the number of VKImageUsage and resources_states to match");
  WN_DEBUG_ASSERT(_state < resource_state::max, "Resource state out of bounds");
  if (_state == resource_state::initial) {
    return states[0];
  }

  return states[math::trailing_zeros(static_cast<uint32_t>(_state)) + 1];
}

static inline VkImageLayout resource_state_to_vulkan_layout(
    resource_state _state) {
  static const VkImageLayout layouts[] = {
      VK_IMAGE_LAYOUT_UNDEFINED,                         // initial
      VK_IMAGE_LAYOUT_GENERAL,                           // host_write
      VK_IMAGE_LAYOUT_GENERAL,                           // host_read
      VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,              // copy_source
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,              // copy_dest
      VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,              // blit_source
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,              // blit_dest
      VK_IMAGE_LAYOUT_UNDEFINED,                         // index_buffer
      VK_IMAGE_LAYOUT_UNDEFINED,                         // vertex_buffer
      VK_IMAGE_LAYOUT_UNDEFINED,                         // read_only_buffer
      VK_IMAGE_LAYOUT_UNDEFINED,                         // read_write_buffer
      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,          // render_target
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,  // depth_target
      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,          // texture
      VK_IMAGE_LAYOUT_PRESENT_SRC_KHR                    // present
  };

  static_assert((1 << (sizeof(layouts) / sizeof(layouts[0]) - 2)) + 1 ==
                    static_cast<uint32_t>(resource_state::max),
      "Expected the number of VkImageLayouts and resources_states to match");
  WN_DEBUG_ASSERT(_state < resource_state::max, "Resource state out of bounds");
  if (_state == resource_state::initial) {
    return layouts[0];
  }

  return layouts[math::trailing_zeros(static_cast<uint32_t>(_state)) + 1];
}

static inline VkPipelineStageFlags flags_to_vulkan_stage(
    pipeline_stages _state) {
  VkPipelineStageFlags flags = 0;

  if (_state & static_cast<uint32_t>(pipeline_stage::top_of_pipe)) {
    flags |= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
  }
  if (_state & static_cast<uint32_t>(pipeline_stage::draw_indirect)) {
    flags |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
  }
  if (_state & static_cast<uint32_t>(pipeline_stage::vertex_input)) {
    flags |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
  }
  if (_state & static_cast<uint32_t>(pipeline_stage::vertex_shader)) {
    flags |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
  }
  if (_state & static_cast<uint32_t>(pipeline_stage::hull_shader)) {
    flags |= VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
  }
  if (_state & static_cast<uint32_t>(pipeline_stage::domain_shader)) {
    flags |= VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
  }
  if (_state & static_cast<uint32_t>(pipeline_stage::geometry_shader)) {
    flags |= VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
  }
  if (_state & static_cast<uint32_t>(pipeline_stage::fragment_shader)) {
    flags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  }
  if (_state & static_cast<uint32_t>(pipeline_stage::early_fragment_tests)) {
    flags |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  }
  if (_state & static_cast<uint32_t>(pipeline_stage::late_fragment_tests)) {
    flags |= VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
  }
  if (_state & static_cast<uint32_t>(pipeline_stage::color_attachment_output)) {
    flags |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  }
  if (_state & static_cast<uint32_t>(pipeline_stage::compute_shader)) {
    flags |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
  }
  if (_state & static_cast<uint32_t>(pipeline_stage::transfer)) {
    flags |= VK_PIPELINE_STAGE_TRANSFER_BIT;
  }
  if (_state & static_cast<uint32_t>(pipeline_stage::bottom_of_pipe)) {
    flags |= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  }
  if (_state & static_cast<uint32_t>(pipeline_stage::host)) {
    flags |= VK_PIPELINE_STAGE_HOST_BIT;
  }
  if (_state & static_cast<uint32_t>(pipeline_stage::all_graphics)) {
    flags |= VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
  }
  if (_state & static_cast<uint32_t>(pipeline_stage::all_commands)) {
    flags |= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
  }
  return flags;
}

static inline VkFlags resource_state_to_vulkan_pipeline_stage(
    resource_state _state) {
  static const VkFlags states[] = {
      VK_PIPELINE_STAGE_HOST_BIT,                     // initial
      VK_PIPELINE_STAGE_HOST_BIT,                     // host_write
      VK_PIPELINE_STAGE_HOST_BIT,                     // host_read
      VK_PIPELINE_STAGE_TRANSFER_BIT,                 // copy_source
      VK_PIPELINE_STAGE_TRANSFER_BIT,                 // copy_dest
      VK_PIPELINE_STAGE_TRANSFER_BIT,                 // blit_source
      VK_PIPELINE_STAGE_TRANSFER_BIT,                 // blit_dest
      VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,             // index_buffer
      VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,             // vertex_buffer
      VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,             // read_only_buffer
      VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,             // read_write_buffer
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // render_target
      VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,     // depth_target
      VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,             // texture
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT   // present
  };

  static_assert((1 << (sizeof(states) / sizeof(states[0]) - 2)) + 1 ==
                    static_cast<uint32_t>(resource_state::max),
      "Expected the number of VkFlags and resources_states to match");
  WN_DEBUG_ASSERT(_state < resource_state::max, "Resource state out of bounds");
  if (_state == resource_state::initial) {
    return states[0];
  }

  return states[math::trailing_zeros(static_cast<uint32_t>(_state)) + 1];
}

static inline VkImageUsageFlags resources_states_to_usage_bits(
    resource_states _state) {
  uint32_t usage = 0;
  if (_state & static_cast<uint32_t>(resource_state::copy_dest)) {
    usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  }
  if (_state & static_cast<uint32_t>(resource_state::copy_source)) {
    usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
  }
  if (_state & static_cast<uint32_t>(resource_state::render_target)) {
    usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  }
  if (_state & static_cast<uint32_t>(resource_state::depth_target)) {
    usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
  }
  if (_state & static_cast<uint32_t>(resource_state::texture)) {
    usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
  }
  if (_state & static_cast<uint32_t>(resource_state::blit_source)) {
    usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
  }
  if (_state & static_cast<uint32_t>(resource_state::blit_dest)) {
    usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  }
  return VkImageUsageFlags(usage);
}

static inline VkShaderStageFlagBits shader_stage_to_vulkan(shader_stage stage) {
  switch (stage) {
    case shader_stage::vertex:
      return VK_SHADER_STAGE_VERTEX_BIT;
    case shader_stage::pixel:
      return VK_SHADER_STAGE_FRAGMENT_BIT;
    case shader_stage::tessellation_control:
      return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    case shader_stage::tessellation_evaluation:
      return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    case shader_stage::geometry:
      return VK_SHADER_STAGE_GEOMETRY_BIT;
    case shader_stage::compute:
      return VK_SHADER_STAGE_COMPUTE_BIT;
  }
  WN_DEBUG_ASSERT(false, "We should never end up here");
  return VkShaderStageFlagBits(0);
}

static inline uint32_t shader_stages_to_vulkan(uint32_t stages) {
  uint32_t output = 0;

  if (stages & static_cast<uint32_t>(shader_stage::vertex)) {
    output |= VK_SHADER_STAGE_VERTEX_BIT;
  }
  if (stages & static_cast<uint32_t>(shader_stage::pixel)) {
    output |= VK_SHADER_STAGE_FRAGMENT_BIT;
  }
  if (stages & static_cast<uint32_t>(shader_stage::tessellation_control)) {
    output |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
  }
  if (stages & static_cast<uint32_t>(shader_stage::tessellation_evaluation)) {
    output |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
  }
  if (stages & static_cast<uint32_t>(shader_stage::geometry)) {
    output |= VK_SHADER_STAGE_GEOMETRY_BIT;
  }
  if (stages & static_cast<uint32_t>(shader_stage::compute)) {
    output |= VK_SHADER_STAGE_COMPUTE_BIT;
  }

  return output;
}

static inline VkPrimitiveTopology primitive_topology_to_vulkan(topology t) {
  switch (t) {
    case topology::undefined:
      return VkPrimitiveTopology(0);
    case topology::point_list:
      return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    case topology::line_list:
      return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    case topology::line_strip:
      return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
    case topology::triangle_list:
      return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    case topology::triangle_strip:
      return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    case topology::line_list_with_adjacency:
      return VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;
    case topology::line_strip_with_adjacency:
      return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
    case topology::triangle_list_with_adjacency:
      return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
    case topology::triangle_strip_with_adjacency:
      return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
    case topology::patch_list:
      return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
  }
  WN_DEBUG_ASSERT(false, "We should never end up here");
  return VkPrimitiveTopology(0);
}

static inline VkPolygonMode fill_mode_to_vulkan(fill_mode _mode) {
  switch (_mode) {
    case fill_mode::fill:
      return VK_POLYGON_MODE_FILL;
    case fill_mode::line:
      return VK_POLYGON_MODE_LINE;
    case fill_mode::point:
      return VK_POLYGON_MODE_POINT;
  }
  WN_DEBUG_ASSERT(false, "We should never get here");
  return VkPolygonMode(0);
}

static inline VkCullModeFlags cull_mode_to_vulkan(cull_mode _mode) {
  switch (_mode) {
    case cull_mode::back:
      return VK_CULL_MODE_BACK_BIT;
    case cull_mode::front:
      return VK_CULL_MODE_FRONT_BIT;
    case cull_mode::none:
      return VK_CULL_MODE_NONE;
  }
  WN_DEBUG_ASSERT(false, "We should never get here");
  return VkCullModeFlags(0);
}

static inline VkFrontFace winding_to_vulkan(winding _mode) {
  switch (_mode) {
    case winding::clockwise:
      return VK_FRONT_FACE_CLOCKWISE;
    case winding::counter_clockwise:
      return VK_FRONT_FACE_COUNTER_CLOCKWISE;
  }
  WN_DEBUG_ASSERT(false, "We should never get here");
  return VkFrontFace(0);
}

static inline VkSampleCountFlagBits samples_to_vulkan(
    multisample_count _count) {
  switch (_count) {
    case multisample_count::disabled:
      return VkSampleCountFlagBits(0);
    case multisample_count::samples_1:
      return VK_SAMPLE_COUNT_1_BIT;
    case multisample_count::samples_2:
      return VK_SAMPLE_COUNT_2_BIT;
    case multisample_count::samples_4:
      return VK_SAMPLE_COUNT_4_BIT;
    case multisample_count::samples_8:
      return VK_SAMPLE_COUNT_8_BIT;
    case multisample_count::samples_16:
      return VK_SAMPLE_COUNT_16_BIT;
    case multisample_count::samples_32:
      return VK_SAMPLE_COUNT_32_BIT;
    case multisample_count::samples_64:
      return VK_SAMPLE_COUNT_64_BIT;
  }
  WN_DEBUG_ASSERT(false, "We should never get here");
  return VkSampleCountFlagBits(0);
}

static inline VkCompareOp compare_to_vulkan(comparison_op _compare) {
  switch (_compare) {
    case comparison_op::always:
      return VK_COMPARE_OP_ALWAYS;
    case comparison_op::equal:
      return VK_COMPARE_OP_EQUAL;
    case comparison_op::greater:
      return VK_COMPARE_OP_GREATER;
    case comparison_op::greater_than_or_equal:
      return VK_COMPARE_OP_GREATER_OR_EQUAL;
    case comparison_op::less:
      return VK_COMPARE_OP_LESS;
    case comparison_op::less_than_or_equal:
      return VK_COMPARE_OP_LESS_OR_EQUAL;
    case comparison_op::never:
      return VK_COMPARE_OP_NEVER;
    case comparison_op::not_equal:
      return VK_COMPARE_OP_NOT_EQUAL;
  }
  WN_DEBUG_ASSERT(false, "We should never get here");
  return VkCompareOp(0);
}

static inline VkStencilOp stencil_op_to_vulkan(stencil_op _stencil) {
  switch (_stencil) {
    case stencil_op::keep:
      return VK_STENCIL_OP_KEEP;
    case stencil_op::zero:
      return VK_STENCIL_OP_ZERO;
    case stencil_op::replace:
      return VK_STENCIL_OP_REPLACE;
    case stencil_op::invert:
      return VK_STENCIL_OP_INVERT;
    case stencil_op::increment_wrap:
      return VK_STENCIL_OP_INCREMENT_AND_WRAP;
    case stencil_op::decrement_wrap:
      return VK_STENCIL_OP_DECREMENT_AND_WRAP;
    case stencil_op::increment_saturate:
      return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
    case stencil_op::decrement_saturate:
      return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
  }
  WN_DEBUG_ASSERT(false, "We should never get here");
  return VkStencilOp(0);
}

static inline VkBlendFactor blend_factor_to_vulkan(blend_factor _factor) {
  switch (_factor) {
    case blend_factor::zero:
      return VK_BLEND_FACTOR_ZERO;
    case blend_factor::one:
      return VK_BLEND_FACTOR_ONE;
    case blend_factor::src_color:
      return VK_BLEND_FACTOR_SRC_COLOR;
    case blend_factor::inv_src_color:
      return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
    case blend_factor::dst_color:
      return VK_BLEND_FACTOR_DST_COLOR;
    case blend_factor::inv_dst_color:
      return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
    case blend_factor::src_alpha:
      return VK_BLEND_FACTOR_SRC_ALPHA;
    case blend_factor::inv_src_alpha:
      return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    case blend_factor::dst_alpha:
      return VK_BLEND_FACTOR_DST_ALPHA;
    case blend_factor::inv_dst_alpha:
      return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
    case blend_factor::constant_color:
      return VK_BLEND_FACTOR_CONSTANT_COLOR;
    case blend_factor::inv_constant_color:
      return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
    case blend_factor::src_alpha_saturate:
      return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
    case blend_factor::src1_color:
      return VK_BLEND_FACTOR_SRC1_COLOR;
    case blend_factor::inv_src1_color:
      return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
    case blend_factor::src1_alpha:
      return VK_BLEND_FACTOR_SRC1_ALPHA;
    case blend_factor::inv_src1_alpha:
      return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
  }
  WN_DEBUG_ASSERT(false, "We should never get here");
  return VkBlendFactor(0);
}

static inline VkBlendOp blend_op_to_vulkan(blend_op _op) {
  switch (_op) {
    case blend_op::add:
      return VK_BLEND_OP_ADD;
    case blend_op::sub:
      return VK_BLEND_OP_SUBTRACT;
    case blend_op::inv_sub:
      return VK_BLEND_OP_REVERSE_SUBTRACT;
    case blend_op::min:
      return VK_BLEND_OP_MIN;
    case blend_op::max:
      return VK_BLEND_OP_MAX;
  }
  WN_DEBUG_ASSERT(false, "We should never get here");
  return VkBlendOp(0);
}

static inline VkLogicOp logic_op_to_vulkan(logic_op _op) {
  switch (_op) {
    case logic_op::disabled:
      return VkLogicOp(0);
    case logic_op::op_clear:
      return VK_LOGIC_OP_CLEAR;
    case logic_op::op_set:
      return VK_LOGIC_OP_SET;
    case logic_op::op_nop:
      return VK_LOGIC_OP_NO_OP;
    case logic_op::op_copy:
      return VK_LOGIC_OP_COPY;
    case logic_op::op_copy_reverse:
      return VK_LOGIC_OP_COPY_INVERTED;
    case logic_op::op_invert:
      return VK_LOGIC_OP_INVERT;
    case logic_op::op_and:
      return VK_LOGIC_OP_AND;
    case logic_op::op_and_reverse:
      return VK_LOGIC_OP_AND_REVERSE;
    case logic_op::op_and_invert:
      return VK_LOGIC_OP_AND_INVERTED;
    case logic_op::op_nand:
      return VK_LOGIC_OP_NAND;
    case logic_op::op_or:
      return VK_LOGIC_OP_OR;
    case logic_op::op_or_reverse:
      return VK_LOGIC_OP_OR_REVERSE;
    case logic_op::op_or_invert:
      return VK_LOGIC_OP_OR_INVERTED;
    case logic_op::op_xor:
      return VK_LOGIC_OP_XOR;
    case logic_op::op_nor:
      return VK_LOGIC_OP_NOR;
    case logic_op::op_nxor:
      return VK_LOGIC_OP_EQUIVALENT;
  }
  WN_DEBUG_ASSERT(false, "We should never get here");
  return VkLogicOp(0);
}

static inline VkColorComponentFlags write_components_to_vulkan(
    write_components _components) {
  VkColorComponentFlags flags = 0;
  if (_components & static_cast<uint8_t>(write_component::r)) {
    flags |= VK_COLOR_COMPONENT_R_BIT;
  }
  if (_components & static_cast<uint8_t>(write_component::g)) {
    flags |= VK_COLOR_COMPONENT_G_BIT;
  }
  if (_components & static_cast<uint8_t>(write_component::b)) {
    flags |= VK_COLOR_COMPONENT_B_BIT;
  }
  if (_components & static_cast<uint8_t>(write_component::a)) {
    flags |= VK_COLOR_COMPONENT_A_BIT;
  }
  return VkColorComponentFlags(flags);
}

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_VULKAN_RESOURCE_STATES_H__
