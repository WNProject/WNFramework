// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_PIPELINE_H__
#define __WN_GRAPHICS_PIPELINE_H__

#include "WNContainers/inc/WNStringView.h"
#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNMemory/inc/WNBasic.h"

namespace wn {
namespace graphics {

struct viewport {
  float x;
  float y;
  float width;
  float height;
  float min_depth;
  float max_depth;
};

struct scissor {
  uint32_t x;
  uint32_t y;
  uint32_t width;
  uint32_t height;
};

struct stencil_desc {
  stencil_op fail = stencil_op::keep;
  stencil_op pass = stencil_op::keep;
  stencil_op depth_fail = stencil_op::keep;
  comparison_op compare = comparison_op::always;
};

class pipeline_description WN_FINAL {
public:
  pipeline_description(memory::allocator* _allocator)
    : m_allocator(_allocator) {}

  pipeline_description& set_shader(shader_stage _stage, shader_module* _module,
      const containers::string_view& _entry_point);

  // You must add at least one vertex stream
  pipeline_description& add_vertex_stream(
      uint32_t _stream_index, uint32_t _stride, stream_frequency _frequency);

  // You must add at least one vertex attribute
  pipeline_description& add_vertex_attribute(uint32_t _stream_index,
      uint32_t _attribute_index, const containers::string_view&,
      uint32_t _offset, image_format _format);

  // Sets the index_buffer type for this pipeline, defaults to none.
  // If index_type is none, then this pipeline can not be used for
  // indexed draws.
  pipeline_description& set_index_type(index_type _type);

  // If index_type is u16 or u32, this can be set to enable primitive restart
  // when index 0xFFFF or 0xFFFFFFFF are hit respectively.
  pipeline_description& set_index_restart(bool _restart);

  // You must set a topology
  pipeline_description& set_topology(topology _topology);

  // If topology == patch_list you must set the number of control point.
  pipeline_description& set_patch_control_points(uint32_t _num_points);

  // Sets the winding of the front-face. Defaults to winding::clockwise.
  pipeline_description& set_winding(winding _winding);

  // Sets the cull mode for the rasterization. Defaults to cull_mode::back
  pipeline_description& set_cull_mode(cull_mode _mode);

  // Sets the number of samples for rendering
  pipeline_description& set_num_samples(
      multisample_count _samples = multisample_count::samples_1);

  // The minimum number of samples per pixel to shade. If this is not
  // specified the GPU is free to do what it deems most optimal.
  pipeline_description& min_sample_count(
      multisample_count _samples = multisample_count::samples_1);

  // Specifies the sample mask for the pipeline.
  pipeline_description& sample_mask(uint32_t sample_mask);

  // If you set a static scissor then all scissors are static for this
  // pipeline.
  // It is invalid to call command_list->set_scissor after binding this
  // pipeline and before binding another.
  // If you set a static viewport, then all viewports are static for this
  // pipeline. It is invalid to call command_list->set_viewport after
  // binding
  // this pipeline and before binding another.
  // Not all hardware may support an index > 0
  pipeline_description& set_static_viewport(
      uint32_t index, const viewport& viewport);

  // Sets the fill_mode of the polygon. Defaults to fill_mode::fill
  // This may not be supported by all hardware.
  pipeline_description& set_fill_mode(fill_mode _mode);

  // Sets the depth vias of the pipeline. Defaults to 0.
  pipeline_description& set_depth_bias(float _bias);

  // Sets alpha to coverage. Defaults to false.
  pipeline_description& set_alpha_to_coverage(bool _enable);

  // Adds a color output to the pipeline.
  // The defaults are set to the default values of the other
  // set_* functions on color_outputs
  pipeline_description& add_color_output(uint32_t _index);

  // Enabled blending for the given color output
  pipeline_description& enable_blending(uint32_t _index,
      blend_factor _src_blend = blend_factor::one,
      blend_factor _dst_blend = blend_factor::zero,
      blend_op _blend_op = blend_op::one,
      blend_factor _src_blend_alpha = blend_factor::one,
      blend_factor _dst_blend_alpha = blend_factor::zero,
      blen_op _blend_op_alpha = blend_op::add);
  // Sets the write mask for the given color output
  pipeline_description& set_write_mask(uint32_t _index,
      wn::core::underlying_type_t<write_components> write_mask =
          write_components::r | write_components::g | write_components::b |
          write_components::a);

  // Sets the logic op for the given color output.
  pipeline_description& set_logic_op(uint32_t _index,
      bool _logic_op_enable = false, logic_op _logic_op = logic_op_noop);
  // If blend_factory_constant was set, this sets a static set
  // of blend constants. Otherwise they are dynamic.
  pipeline_description& set_static_blend_constants(
      uint32_t _index, float _constants[4]);

  // TODO(awoloszyn): Handle depth bounds tests
  pipeline_description& set_depth(bool _depth_test_enabled = false,
      bool _depth_write_enabled = false,
      comparison_op _comparison_op = comparison_op::less);
  pipeline_description& set_stencil(
      bool _stencil_enabled = false, uint32_t read_mask = 0xFFFFFFFF;
      uint32_t write_mask = 0xFFFFFFFF,
      const stencil_desc& front_face = stencil_desc(),
      const stencil_desc& back_face = stencil_desc());
  // Sets a static stencil reference value. If this is
  // not set, and a stencil reference is needed, then
  // it must be set on the command buffer.
  // If this is set, it is invalid to set the stencil dynamically
  // on the command buffer after this pipeline is bound.
  pipeline_description& set_static_stencil_ref(uint32_t reference);

private:
  memory::alocator m_allocator;
};

}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_PIPELINE_H__
