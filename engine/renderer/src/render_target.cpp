// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "renderer/inc/render_target.h"

#include "WNGraphics/inc/WNGraphicsEnums.h"
#include "WNGraphics/inc/WNImage.h"
#include "renderer/inc/render_context.h"

using namespace wn::engine;
using wn::scripting::script_object_type;
using wn::scripting::shared_cpp_pointer;

using namespace wn;

namespace wn {
namespace engine {
namespace renderer {

void rt_description::register_scripting(scripting::engine* _engine) {
  _engine->export_script_type<rt_description>();
  _engine->register_named_constant<int32_t>("clear_mode_load",
      static_cast<int32_t>(runtime::graphics::load_op::load));
  _engine->register_named_constant<int32_t>("clear_mode_clear",
      static_cast<int32_t>(runtime::graphics::load_op::clear));
  _engine->register_named_constant<int32_t>("clear_mode_dont_care",
      static_cast<int32_t>(runtime::graphics::load_op::dont_care));
  _engine->register_named_constant<int32_t>("store_mode_store",
      static_cast<int32_t>(runtime::graphics::store_op::store));
  _engine->register_named_constant<int32_t>("store_mode_dont_care",
      static_cast<int32_t>(runtime::graphics::store_op::dont_care));
}

bool rt_description::resolve_scripting(scripting::engine* _engine) {
  return _engine->resolve_script_type<rt_description>();
}

render_target::render_target(memory::allocator* _allocator, logging::log* _log,
    render_context* _renderer, runtime::graphics::device* _device,
    wn::scripting::script_pointer<rt_description> desc, uint32_t _num_backings)
  : m_images(_allocator),
    m_image_views(_allocator),
    m_allocations(_allocator),
    m_layouts(_allocator) {
  wn::runtime::graphics::clear_value value{};
  int32_t format = desc.invoke(&rt_description::get_format);
  int32_t width = desc.invoke(&rt_description::get_width);
  int32_t height = desc.invoke(&rt_description::get_height);
  bool is_depth = desc.invoke(&rt_description::is_depth);
  if (is_depth) {
    // TODO(awoloszyn): Stencil
    m_components = static_cast<runtime::graphics::image_components>(
        runtime::graphics::image_component::depth);
  }
  m_layouts.reserve(_num_backings);
  WN_DEBUG_ASSERT(
      width > 0 && height > 0, "The image dimensions must be positive");

  for (size_t i = 0; i < _num_backings; ++i) {
    m_layouts[i] = runtime::graphics::resource_state::initial;

    runtime::graphics::resource_states states =
        runtime::graphics::resource_state::copy_source |
        runtime::graphics::resource_state::render_target;
    if (is_depth) {
      states = static_cast<runtime::graphics::resource_states>(
          runtime::graphics::resource_state::depth_target);
    }

    m_images.emplace_back(_device->create_image(
        runtime::graphics::image_create_info{static_cast<size_t>(width),
            static_cast<size_t>(height),
            static_cast<runtime::graphics::data_format>(format), states, 1},
        value));
    auto reqs = m_images.back().get_memory_requirements();
    gpu_allocation alloc =
        _renderer->get_allocation_for_render_target(reqs.size, reqs.alignment);
    if (!alloc.is_valid()) {
      _log->log_error("Could not allocate memory for rendertarget");
      m_is_valid = false;
      m_images.clear();
      m_allocations.clear();
      return;
    }

    m_images.back().bind_memory(alloc.arena(), alloc.offset());
    m_image_views.emplace_back(
        _device->create_image_view(&m_images.back(), 0, 1, m_components));

    m_allocations.emplace_back(core::move(alloc));
  }
}

void render_target::transition_layout(size_t i,
    runtime::graphics::resource_state _state,
    runtime::graphics::command_list* _list) {
  if (m_layouts[i] != _state) {
    _list->transition_resource(
        m_images[i], m_components, 0, 1, m_layouts[i], _state);
    m_layouts[i] = _state;
  }
}

}  // namespace renderer
}  // namespace engine
}  // namespace wn
