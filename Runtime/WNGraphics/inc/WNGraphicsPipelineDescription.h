// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_GRAPHICS_PIPELINE_DESCRIPTION_H__
#define __WN_GRAPHICS_GRAPHICS_PIPELINE_DESCRIPTION_H__

#include "WNContainers/inc/WNDynamicArray.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNGraphicsObjectBase.h"
#include "WNGraphics/inc/WNGraphicsTypes.h"
#include "WNMath/inc/WNBasic.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNMemory/inc/WNBasic.h"

WN_GRAPHICS_FORWARD(device);

namespace wn {
namespace runtime {
namespace graphics {

class shader_module;

struct viewport {
  float x;
  float y;
  float width;
  float height;
  float min_depth;
  float max_depth;
};

struct stencil_desc {
  stencil_op fail = stencil_op::keep;
  stencil_op pass = stencil_op::keep;
  stencil_op depth_fail = stencil_op::keep;
  comparison_op compare = comparison_op::always;
};

class graphics_pipeline_description final {
public:
  graphics_pipeline_description(memory::allocator* _allocator)
    : m_allocator(_allocator),
      m_vertex_streams(_allocator),
      m_vertex_attributes(_allocator),
      m_static_scissors(_allocator),
      m_static_viewports(_allocator),
      m_color_attachments(_allocator) {}

  WN_FORCE_INLINE graphics_pipeline_description& set_shader(shader_stage _stage,
      shader_module* _module, const containers::string_view& _entry_point);

  // You must add at least one vertex stream
  WN_FORCE_INLINE graphics_pipeline_description& add_vertex_stream(
      uint32_t _stream_index, uint32_t _stride, stream_frequency _frequency);

  // You must add at least one vertex attribute
  WN_FORCE_INLINE graphics_pipeline_description& add_vertex_attribute(
      uint32_t _stream_index, uint32_t _attribute_index,
      const containers::string_view& _semantic, uint32_t _offset,
      data_format _format);

  // Sets the index_buffer type for this pipeline, defaults to none.
  // If index_type is none, then this pipeline can not be used for
  // indexed draws.
  WN_FORCE_INLINE graphics_pipeline_description& set_index_type(
      index_type _type);

  // This can be set to enable primitive restart
  // when index 0xFFFF or 0xFFFFFFFF are hit respectively.
  // Note this MUST be 0x00 to turn off, 0xFFFF for u16 indices
  // and 0xFFFFFFFF for u32 indices
  WN_FORCE_INLINE graphics_pipeline_description& set_index_restart(
      uint32_t _restart);

  // Topology defaults to triangle_list
  WN_FORCE_INLINE graphics_pipeline_description& set_topology(
      topology _topology);

  // If topology == patch_list you must set the number of control point.
  WN_FORCE_INLINE graphics_pipeline_description& set_patch_control_points(
      uint32_t _num_points);

  // Sets the winding of the front-face. Defaults to winding::clockwise.
  WN_FORCE_INLINE graphics_pipeline_description& set_winding(winding _winding);

  // Sets the cull mode for the rasterization. Defaults to cull_mode::back
  WN_FORCE_INLINE graphics_pipeline_description& set_cull_mode(cull_mode _mode);

  // Sets the number of samples for rendering. Defaults to 1 sample
  WN_FORCE_INLINE graphics_pipeline_description& set_num_samples(
      multisample_count _samples = multisample_count::samples_1);

  static const uint64_t k_all_samples = 0xFFFFFFFFFFFFFFFF;
  // The minimum number of samples per pixel to shade. If this is not
  // specified the GPU is free to do what it deems most optimal.
  WN_FORCE_INLINE graphics_pipeline_description& set_min_sample_count(
      multisample_count _samples = multisample_count::disabled);

  // Specifies the sample mask for the pipeline. Defaults to all samples
  WN_FORCE_INLINE graphics_pipeline_description& set_sample_mask(
      uint64_t sample_mask);

  // If you set a static scissor then all scissors are static for this
  // pipeline.
  // It is invalid to call command_list->set_scissor after binding this
  // pipeline and before binding another.
  WN_FORCE_INLINE graphics_pipeline_description& set_num_scissors(
      uint32_t count);

  WN_FORCE_INLINE graphics_pipeline_description& set_static_scissor(
      uint32_t index, const scissor& scissor);
  WN_FORCE_INLINE graphics_pipeline_description& clear_static_scissor();

  // If you set a static viewport, then all viewports are static for this
  // pipeline. It is invalid to call command_list->set_viewport after
  // binding
  // this pipeline and before binding another.
  // Not all hardware may support an index > 0
  WN_FORCE_INLINE graphics_pipeline_description& set_num_viewports(
      uint32_t count);

  WN_FORCE_INLINE graphics_pipeline_description& set_static_viewport(
      uint32_t index, const viewport& viewport);
  WN_FORCE_INLINE graphics_pipeline_description& clear_static_viewport();

  // Sets the fill_mode of the polygon. Defaults to fill_mode::fill
  // This may not be supported by all hardware.
  WN_FORCE_INLINE graphics_pipeline_description& set_fill_mode(
      fill_mode _mode = fill_mode::fill);

  // Sets the depth vias of the pipeline. Defaults to 0.
  WN_FORCE_INLINE graphics_pipeline_description& set_depth_bias(
      float _bias = 0.f);

  // Sets alpha to coverage. Defaults to false.
  WN_FORCE_INLINE graphics_pipeline_description& set_alpha_to_coverage(
      bool _enable);

  // Adds a color output to the pipeline.
  // The defaults are set to the default values of the other
  // set_* functions on color_outputs
  // If a add_color_output has already been set with this _index,
  // this resets the values to their defaults.
  WN_FORCE_INLINE graphics_pipeline_description& add_color_output(
      uint32_t _index);

  // Enabled blending for the given color output
  WN_FORCE_INLINE graphics_pipeline_description& enable_blending(
      uint32_t _index, blend_factor _src_blend = blend_factor::one,
      blend_factor _dst_blend = blend_factor::zero,
      blend_op _blend_op = blend_op::add,
      blend_factor _src_blend_alpha = blend_factor::one,
      blend_factor _dst_blend_alpha = blend_factor::zero,
      blend_op _blend_op_alpha = blend_op::add);
  // Sets the write mask for the given color output
  WN_FORCE_INLINE graphics_pipeline_description& set_write_mask(
      uint32_t _index, write_components _write_mask = k_all_write_components);

  // Sets the logic op for the given color output.
  WN_FORCE_INLINE graphics_pipeline_description& set_logic_op(
      logic_op _logic_op = logic_op::disabled);
  // If blend_factory_constant was set, this sets a static set
  // of blend constants. Otherwise they are dynamic. For all
  WN_FORCE_INLINE graphics_pipeline_description& set_static_blend_constants(
      float _constants[4]);
  WN_FORCE_INLINE graphics_pipeline_description& clear_static_blend_constants();

  WN_FORCE_INLINE graphics_pipeline_description& set_depth_buffer_enabled(
      bool _enabled);

  // TODO(awoloszyn): Handle depth bounds tests
  WN_FORCE_INLINE graphics_pipeline_description& set_depth(
      bool _depth_test_enabled = false, bool _depth_write_enabled = false,
      comparison_op _comparison_op = comparison_op::less);
  WN_FORCE_INLINE graphics_pipeline_description& set_stencil(
      bool _stencil_enabled = false, uint32_t _read_mask = 0xFFFFFFFF,
      uint32_t _write_mask = 0xFFFFFFFF,
      const stencil_desc& _front_face = stencil_desc(),
      const stencil_desc& _back_face = stencil_desc());
  // Sets a static stencil reference value. If this is
  // not set, and a stencil reference is needed, then
  // it must be set on the command buffer.
  // If this is set, it is invalid to set the stencil dynamically
  // on the command buffer after this pipeline is bound.
  WN_FORCE_INLINE graphics_pipeline_description& set_static_stencil_ref(
      uint32_t _reference);
  WN_FORCE_INLINE graphics_pipeline_description& clear_static_stencil_ref();

private:
  WN_GRAPHICS_ADD_FRIENDS(device);
  memory::allocator* m_allocator;

  struct shader_info {
    shader_module* module = nullptr;
    containers::string entry_point;
  };
  containers::array<shader_info, 5> m_shader_stages;

  struct vertex_stream {
    uint32_t index;
    uint32_t stride;
    stream_frequency frequency;
  };
  containers::dynamic_array<vertex_stream> m_vertex_streams;

  struct vertex_attribute {
    uint32_t stream_index;
    uint32_t attribute_index;
    containers::string semantic;
    uint32_t offset;
    data_format format;
  };
  containers::dynamic_array<vertex_attribute> m_vertex_attributes;

  index_type m_index_type = index_type::none;
  uint32_t m_index_restart = 0x00;
  topology m_topology = topology::triangle_list;
  uint32_t m_num_patch_control_points = 0;
  winding m_winding = winding::clockwise;
  cull_mode m_cull_mode = cull_mode::back;
  multisample_count m_num_samples = multisample_count::samples_1;
  multisample_count m_min_shading_samples = multisample_count::disabled;
  uint64_t m_sample_mask = k_all_samples;

  uint32_t m_num_viewports = 1;
  uint32_t m_num_scissors = 1;
  containers::dynamic_array<scissor> m_static_scissors;
  containers::dynamic_array<viewport> m_static_viewports;

  fill_mode m_fill_mode = fill_mode::fill;

  float m_depth_bias = 0.f;
  bool m_alpha_to_coverage = false;

  struct color_attachment_info {
    bool blend_enabled = false;
    blend_factor src_blend = blend_factor::one;
    blend_factor dst_blend = blend_factor::zero;
    blend_op color_blend_op = blend_op::add;
    blend_factor src_blend_alpha = blend_factor::one;
    blend_factor dst_blend_alpha = blend_factor::zero;
    blend_op alpha_blend_op = blend_op::add;
    write_components write_mask = k_all_write_components;
  };
  containers::dynamic_array<color_attachment_info> m_color_attachments;

  logic_op m_logic_op = logic_op::disabled;
  bool m_has_static_blend_constants = false;
  float m_static_blend_constants[4] = {0.f, 0.f, 0.f, 0.f};

  bool m_depth_enabled = false;

  bool m_depth_test_enabled = false;
  bool m_depth_write_enabled = false;
  comparison_op m_depth_comparison = comparison_op::less;

  bool m_stencil_enabled = false;
  uint32_t m_stencil_read_mask = 0xFFFFFFFF;
  uint32_t m_stencil_write_mask = 0xFFFFFFFF;
  stencil_desc m_stencil_front_face = stencil_desc();
  stencil_desc m_stencil_back_face = stencil_desc();

  bool m_has_static_stencil_reference = false;
  uint32_t m_static_stencil_reference = 0;
};

graphics_pipeline_description& graphics_pipeline_description::set_shader(
    shader_stage _stage, shader_module* _module,
    const containers::string_view& _entry_point) {
  size_t array_index = math::trailing_zeros(static_cast<uint32_t>(_stage));
  m_shader_stages[array_index].module = _module;
  m_shader_stages[array_index].entry_point =
      _entry_point.to_string(m_allocator);
  return *this;
}

graphics_pipeline_description& graphics_pipeline_description::add_vertex_stream(
    uint32_t _stream_index, uint32_t _stride, stream_frequency _frequency) {
  m_vertex_streams.push_back(vertex_stream{_stream_index, _stride, _frequency});
  return *this;
}

graphics_pipeline_description&
graphics_pipeline_description::add_vertex_attribute(uint32_t _stream_index,
    uint32_t _attribute_index, const containers::string_view& _semantic,
    uint32_t _offset, data_format _format) {
  m_vertex_attributes.push_back(vertex_attribute{_stream_index,
      _attribute_index, _semantic.to_string(m_allocator), _offset, _format});
  return *this;
}

graphics_pipeline_description& graphics_pipeline_description::set_index_type(
    index_type _type) {
  m_index_type = _type;
  return *this;
}

graphics_pipeline_description& graphics_pipeline_description::set_index_restart(
    uint32_t _restart) {
  m_index_restart = _restart;
  return *this;
}

graphics_pipeline_description& graphics_pipeline_description::set_topology(
    topology _topology) {
  m_topology = _topology;
  return *this;
}

graphics_pipeline_description&
graphics_pipeline_description::set_patch_control_points(uint32_t _num_points) {
  m_num_patch_control_points = _num_points;
  return *this;
}

graphics_pipeline_description& graphics_pipeline_description::set_winding(
    winding _winding) {
  m_winding = _winding;
  return *this;
}

graphics_pipeline_description& graphics_pipeline_description::set_cull_mode(
    cull_mode _mode) {
  m_cull_mode = _mode;
  return *this;
}

graphics_pipeline_description& graphics_pipeline_description::set_num_samples(
    multisample_count _samples) {
  m_num_samples = _samples;
  return *this;
}

graphics_pipeline_description&
graphics_pipeline_description::set_min_sample_count(
    multisample_count _samples) {
  m_min_shading_samples = _samples;
  return *this;
};

graphics_pipeline_description& graphics_pipeline_description::set_sample_mask(
    uint64_t _sample_mask) {
  m_sample_mask = _sample_mask;
  return *this;
}

graphics_pipeline_description& graphics_pipeline_description::set_num_scissors(
    uint32_t _count) {
  m_num_scissors = _count;
  return *this;
}

graphics_pipeline_description&
graphics_pipeline_description::set_static_scissor(
    uint32_t index, const scissor& _scissor) {
  m_static_scissors.resize(
      math::max(m_static_scissors.size(), static_cast<size_t>(index + 1)));
  m_static_scissors[index] = _scissor;
  return *this;
}

graphics_pipeline_description&
graphics_pipeline_description::clear_static_scissor() {
  m_static_scissors.clear();
  return *this;
}

graphics_pipeline_description& graphics_pipeline_description::set_num_viewports(
    uint32_t _count) {
  m_num_viewports = _count;
  return *this;
}

graphics_pipeline_description&
graphics_pipeline_description::set_static_viewport(
    uint32_t index, const viewport& _viewport) {
  m_static_viewports.resize(
      math::max(m_static_viewports.size(), static_cast<size_t>(index + 1)));
  m_static_viewports[index] = _viewport;
  return *this;
}

graphics_pipeline_description&
graphics_pipeline_description::clear_static_viewport() {
  m_static_viewports.clear();
  return *this;
}

graphics_pipeline_description& graphics_pipeline_description::set_fill_mode(
    fill_mode _mode) {
  m_fill_mode = _mode;
  return *this;
}

graphics_pipeline_description& graphics_pipeline_description::set_depth_bias(
    float _bias) {
  m_depth_bias = _bias;
  return *this;
}

graphics_pipeline_description&
graphics_pipeline_description::set_alpha_to_coverage(bool _enable) {
  m_alpha_to_coverage = _enable;
  return *this;
}

graphics_pipeline_description& graphics_pipeline_description::add_color_output(
    uint32_t _index) {
  m_color_attachments.resize(
      math::max(m_color_attachments.size(), static_cast<size_t>(_index + 1)));
  m_color_attachments[_index].blend_enabled = false;
  m_color_attachments[_index].write_mask = k_all_write_components;
  return *this;
}

graphics_pipeline_description& graphics_pipeline_description::enable_blending(
    uint32_t _index, blend_factor _src_blend, blend_factor _dst_blend,
    blend_op _blend_op, blend_factor _src_blend_alpha,
    blend_factor _dst_blend_alpha, blend_op _blend_op_alpha) {
  m_color_attachments[_index].blend_enabled = true;
  m_color_attachments[_index].src_blend = _src_blend;
  m_color_attachments[_index].dst_blend = _dst_blend;
  m_color_attachments[_index].color_blend_op = _blend_op;
  m_color_attachments[_index].src_blend_alpha = _src_blend_alpha;
  m_color_attachments[_index].dst_blend_alpha = _dst_blend_alpha;
  m_color_attachments[_index].alpha_blend_op = _blend_op_alpha;
  return *this;
}

graphics_pipeline_description& graphics_pipeline_description::set_write_mask(
    uint32_t _index, write_components _write_mask) {
  m_color_attachments[_index].write_mask = _write_mask;
  return *this;
}

graphics_pipeline_description&
graphics_pipeline_description::set_static_blend_constants(float _constants[4]) {
  m_has_static_blend_constants = true;
  memory::memory_copy(&m_static_blend_constants[0], &_constants[0], 4);
  return *this;
}

graphics_pipeline_description&
graphics_pipeline_description::clear_static_blend_constants() {
  m_has_static_blend_constants = false;
  return *this;
}

graphics_pipeline_description& graphics_pipeline_description::set_logic_op(
    logic_op _logic_op) {
  m_logic_op = _logic_op;
  return *this;
}

graphics_pipeline_description&
graphics_pipeline_description::set_depth_buffer_enabled(bool _enabled) {
  m_depth_enabled = _enabled;
  return *this;
}

graphics_pipeline_description& graphics_pipeline_description::set_depth(
    bool _depth_test_enabled, bool _depth_write_enabled,
    comparison_op _comparison_op) {
  m_depth_test_enabled = _depth_test_enabled;
  m_depth_write_enabled = _depth_write_enabled;
  m_depth_comparison = _comparison_op;
  return *this;
}

graphics_pipeline_description& graphics_pipeline_description::set_stencil(
    bool _stencil_enabled, uint32_t _read_mask, uint32_t _write_mask,
    const stencil_desc& _front_face, const stencil_desc& _back_face) {
  m_stencil_enabled = _stencil_enabled;
  m_stencil_read_mask = _read_mask;
  m_stencil_write_mask = _write_mask;
  m_stencil_front_face = _front_face;
  m_stencil_back_face = _back_face;
  return *this;
}

graphics_pipeline_description&
graphics_pipeline_description::set_static_stencil_ref(uint32_t _reference) {
  m_has_static_stencil_reference = true;
  m_static_stencil_reference = _reference;
  return *this;
}

graphics_pipeline_description&
graphics_pipeline_description::clear_static_stencil_ref() {
  m_has_static_stencil_reference = false;
  return *this;
}

}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_GRAPHICS_PIPELINE_DESCRIPTION_H__
