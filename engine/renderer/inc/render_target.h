// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_ENGINE_RENDER_TARGET_H__
#define __WN_ENGINE_RENDER_TARGET_H__

#include "WNGraphics/inc/WNImage.h"
#include "WNGraphics/inc/WNImageView.h"
#include "WNScripting/inc/WNEngine.h"
#include "renderer/inc/gpu_heap.h"

namespace wn {
namespace runtime {
namespace graphics {
class device;
}
}  // namespace runtime
namespace engine {
namespace renderer {
class render_context;

// rt_description is an exported type in scripting.
// It defines all of the properties needed to create
// a render target.
struct rt_description : scripting::script_object_type {
  using parent_type = void;

  void export_type(
      scripting::engine::script_type_importer<rt_description>* _importer) {
    _importer->register_function("get_name", &get_name);
    _importer->register_function("get_width", &get_width);
    _importer->register_function("get_height", &get_height);
    _importer->register_function("get_format", &get_format);
    _importer->register_function("is_depth", &is_depth);
  }

  static wn::containers::string_view exported_name() {
    return "RTDescription";
  }

  static wn::containers::string_view required_script() {
    return "renderer/rt_description.wns";
  }

  scripting::scripting_object_function<rt_description, const char*> get_name;
  scripting::scripting_virtual_object_function<rt_description, int32_t>
      get_width;
  scripting::scripting_virtual_object_function<rt_description, int32_t>
      get_height;
  scripting::scripting_object_function<rt_description, int32_t> get_format;
  scripting::scripting_object_function<rt_description, bool> is_depth;

  static void register_scripting(scripting::engine* _engine);
  static bool resolve_scripting(scripting::engine* _engine);
};

class render_target {
public:
  render_target(memory::allocator* _allocator, logging::log* _log,
      render_context* _renderer, runtime::graphics::device* _device,
      wn::scripting::shared_script_pointer<rt_description> desc,
      uint32_t _num_backings);
  bool is_valid() const {
    return m_is_valid;
  }
  void setup_for_frame(
      render_context* _renderer, uint64_t _frame, bool _check_dimensions);

  runtime::graphics::image_view* get_image_view_for_index(size_t i);
  runtime::graphics::image* get_image_for_index(size_t i);

  uint32_t get_index_for_frame(uint64_t _frame) const {
    return static_cast<uint32_t>(_frame % m_targets.size());
  }

  void transition_layout(size_t i, runtime::graphics::resource_state _state,
      runtime::graphics::command_list* _list);
  size_t get_width() {
    return m_width;
  }
  size_t get_height() {
    return m_height;
  }
  uint64_t get_generation() {
    return m_generation;
  }

private:
  struct render_target_data {
    render_target_data(render_target_data&& _other)
      : m_image(core::move(_other.m_image)),
        m_image_view(core::move(_other.m_image_view)),
        m_allocation(core::move(_other.m_allocation)),
        m_layout(core::move(_other.m_layout)),
        m_out_of_date(_other.m_out_of_date) {}
    render_target_data& operator=(render_target_data&& _other) {
      m_image = core::move(_other.m_image);
      m_image_view = core::move(_other.m_image_view);
      m_allocation = core::move(_other.m_allocation);
      m_layout = core::move(_other.m_layout);
      m_out_of_date = _other.m_out_of_date;
      return *this;
    }
    render_target_data() = default;
    runtime::graphics::image m_image;
    runtime::graphics::image_view m_image_view;
    gpu_allocation m_allocation;
    runtime::graphics::resource_state m_layout;
    bool m_out_of_date;
  };

  void setup_target(render_context* _renderer,
      wn::engine::renderer::render_target::render_target_data* _data);
  bool m_is_valid = true;
  runtime::graphics::device* m_device;
  runtime::graphics::image_components m_components =
      static_cast<runtime::graphics::image_components>(
          runtime::graphics::image_component::color);
  wn::scripting::shared_script_pointer<rt_description> m_description;
  wn::containers::dynamic_array<render_target_data> m_targets;
  logging::log* m_log;
  runtime::graphics::data_format m_format;
  size_t m_width;
  size_t m_height;
  bool m_is_depth;
  uint64_t m_generation = 0;
};

}  // namespace renderer
}  // namespace engine
}  // namespace wn

#endif  // __WN_ENGINE_RENDER_TARGET_H__
