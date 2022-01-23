// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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

void render_target::setup_target(render_context* _renderer,
    wn::engine::renderer::render_target::render_target_data* _target) {
  _target->m_layout = runtime::graphics::resource_state::initial;
  wn::runtime::graphics::clear_value clear{};

  runtime::graphics::resource_states states =
      runtime::graphics::resource_state::copy_source |
      runtime::graphics::resource_state::render_target;
  if (m_is_depth) {
    states = static_cast<runtime::graphics::resource_states>(
        runtime::graphics::resource_state::depth_target);
  }
  _target->m_image = m_device->create_image(
      runtime::graphics::image_create_info{static_cast<size_t>(m_width),
          static_cast<size_t>(m_height), m_format, states, 1},
      clear);
  // Clear the allocation before making another one.
  _target->m_allocation = gpu_allocation();

  auto reqs = _target->m_image.get_memory_requirements();
  gpu_allocation alloc =
      _renderer->get_allocation_for_render_target(reqs.size, reqs.alignment);
  if (!alloc.is_valid()) {
    m_log->log_error("Could not allocate memory for rendertarget");
    m_log->flush();
    m_is_valid = false;
    m_targets.clear();
    return;
  }

  _target->m_image.bind_memory(alloc.arena(), alloc.offset());
  _target->m_allocation = core::move(alloc);
  _target->m_image_view =
      m_device->create_image_view(&_target->m_image, 0, 1, m_components);
  _target->m_out_of_date = false;
}

render_target::render_target(memory::allocator* _allocator, logging::log* _log,
    render_context* _renderer, runtime::graphics::device* _device,
    wn::scripting::shared_script_pointer<rt_description> desc,
    uint32_t _num_backings)
  : m_device(_device), m_targets(_allocator), m_log(_log) {
  m_description = desc;
  int32_t format = desc.invoke(&rt_description::get_format);
  int32_t width = desc.invoke(&rt_description::get_width);
  int32_t height = desc.invoke(&rt_description::get_height);
  m_width = static_cast<size_t>(width);
  m_height = static_cast<size_t>(height);

  m_is_depth = desc.invoke(&rt_description::is_depth);
  m_format = static_cast<runtime::graphics::data_format>(format);
  if (m_is_depth) {
    // TODO(awoloszyn): Stencil
    m_components = static_cast<runtime::graphics::image_components>(
        runtime::graphics::image_component::depth);
  }
  WN_DEBUG_ASSERT(
      width > 0 && height > 0, "The image dimensions must be positive");
  m_targets.reserve(_num_backings);

  for (size_t i = 0; i < _num_backings; ++i) {
    m_targets.emplace_back();
    setup_target(_renderer, &m_targets.back());
  }
}

void render_target::transition_layout(size_t i,
    runtime::graphics::resource_state _state,
    runtime::graphics::command_list* _list) {
  auto& target = m_targets[i];
  if (target.m_layout != _state) {
    _list->transition_resource(
        target.m_image, m_components, 0, 1, target.m_layout, _state);
    target.m_layout = _state;
  }
}

runtime::graphics::image_view* render_target::get_image_view_for_index(
    size_t i) {
  WN_DEBUG_ASSERT(!m_targets[i].m_out_of_date,
      "setup_for_frame was not called for this frame");
  return &m_targets[i].m_image_view;
}

runtime::graphics::image* render_target::get_image_for_index(size_t i) {
  WN_DEBUG_ASSERT(!m_targets[i].m_out_of_date,
      "setup_for_frame was not called for this frame");
  return &m_targets[i].m_image;
}

void render_target::setup_for_frame(
    render_context* _renderer, uint64_t _frame, bool _check_dimensions) {
  size_t index = get_index_for_frame(_frame);
  if (_check_dimensions) {
    size_t new_width =
        static_cast<size_t>(m_description.invoke(&rt_description::get_width));
    size_t new_height =
        static_cast<size_t>(m_description.invoke(&rt_description::get_height));
    if (new_width != m_width || new_height != m_height) {
      ++m_generation;
      m_width = new_width;
      m_height = new_height;
      for (auto& target : m_targets) {
        target.m_out_of_date = true;
      }
    }
  }
  if (m_targets[index].m_out_of_date) {
    setup_target(_renderer, &m_targets[index]);
  }
  return;
}

}  // namespace renderer
}  // namespace engine
}  // namespace wn
