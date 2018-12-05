// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

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
class renderer;

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

  scripting::scripting_object_function<rt_description, const char*> get_name;
  scripting::scripting_object_function<rt_description, int32_t> get_width;
  scripting::scripting_object_function<rt_description, int32_t> get_height;
  scripting::scripting_object_function<rt_description, int32_t> get_format;
  scripting::scripting_object_function<rt_description, bool> is_depth;

  static void register_scripting(scripting::engine* _engine);
  static bool resolve_scripting(scripting::engine* _engine);
};

class render_target {
public:
  render_target(memory::allocator* _allocator, logging::log* _log,
      renderer* _renderer, runtime::graphics::device* _device,
      wn::scripting::script_pointer<rt_description> desc,
      uint32_t _num_backings);
  bool is_valid() const {
    return m_is_valid;
  }
  runtime::graphics::image_view* get_image_view(size_t i) {
    return &m_image_views[i];
  }
  runtime::graphics::image* get_image(size_t i) {
    return &m_images[i];
  }

  uint32_t get_index_for_frame(uint64_t _frame) const {
    return static_cast<uint32_t>(_frame % m_images.size());
  }

  void transition_layout(size_t i, runtime::graphics::resource_state _state,
      runtime::graphics::command_list* _list);

private:
  bool m_is_valid = true;
  runtime::graphics::image_components m_components =
      static_cast<runtime::graphics::image_components>(
          runtime::graphics::image_component::color);
  wn::containers::dynamic_array<wn::runtime::graphics::image> m_images;
  wn::containers::dynamic_array<wn::runtime::graphics::image_view>
      m_image_views;
  wn::containers::dynamic_array<gpu_allocation> m_allocations;
  wn::containers::dynamic_array<wn::runtime::graphics::resource_state>
      m_layouts;
};

}  // namespace renderer
}  // namespace engine
}  // namespace wn

#endif  // __WN_ENGINE_RENDER_TARGET_H__