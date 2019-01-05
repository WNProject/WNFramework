// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "renderer/inc/render_context.h"
#include "WNGraphics/inc/WNCommandList.h"
#include "WNGraphics/inc/WNGraphicsEnums.h"
#include "WNGraphics/inc/WNSwapchain.h"
#include "engine_base/inc/context.h"
#include "renderer/inc/render_data.h"
#include "renderer/inc/render_target.h"
#include "window/inc/window.h"

using namespace wn::engine;
using wn::scripting::script_object_type;
using wn::scripting::script_pointer;
using wn::scripting::shared_cpp_pointer;

using namespace wn;

// TODO(awoloszyn): Make this configurable.
const size_t kRendertargetHeapSize =
    1024 * 1024 * 128;  // 128MB rendertarget heap.
const uint32_t kNumDefaultBackings = 2;

// First let us export the renderer to scripting.
namespace wn {
namespace scripting {
template <>
struct exported_script_type<renderer::render_context> {
  static containers::string_view exported_name() {
    return "RenderContext";
  }

  template <typename T>
  static void export_type(T* _exporter) {
    _exporter->template register_nonvirtual_function<
        decltype(&renderer::render_context::width), &renderer::render_context::width>(
        "width");
    _exporter->template register_nonvirtual_function<
        decltype(&renderer::render_context::height), &renderer::render_context::height>(
        "height");
    _exporter->template register_nonvirtual_function<
        decltype(&renderer::render_context::render), &renderer::render_context::render>(
        "render");
    _exporter->template register_nonvirtual_function<
        decltype(&renderer::render_context::register_description),
        &renderer::render_context::register_description>("register_description");
  }
};
}  // namespace scripting
}  // namespace wn

namespace {

shared_cpp_pointer<engine::renderer::render_context> get_attachmentless_render_context(
    engine_base::context* _context) {
  return _context->m_engine->make_shared_cpp<engine::renderer::render_context>(
      _context->m_allocator, _context->m_log, nullptr, 0, 0);
}

shared_cpp_pointer<engine::renderer::render_context> get_render_context(
    engine_base::context* _context, int32_t _width, int32_t _height) {
  if (_width == 0 || _height == 0) {
    _context->m_log->log_error(
        "If you want to create a renderer without a default attachment"
        " do not specify a width or height");
  }
  return _context->m_engine->make_shared_cpp<engine::renderer::render_context>(
      _context->m_allocator, _context->m_log, nullptr, _width, _height);
}

shared_cpp_pointer<engine::renderer::render_context> get_render_context_with_window(
    engine_base::context* _context, window::window* _window) {
  return _context->m_engine->make_shared_cpp<engine::renderer::render_context>(
      _context->m_allocator, _context->m_log, _window, _window->width(),
      _window->height());
}

struct render_data : script_object_type {
  using parent_type = void;
  static wn::containers::string_view exported_name() {
    return "RenderData";
  }

  void export_type(
      scripting::engine::script_type_importer<render_data>* _importer) {
    _importer->register_function("register_data", &register_data);
  }

  wn::scripting::scripting_virtual_object_function<render_data, void,
      engine_base::context*>
      register_data;
};

}  // anonymous namespace

namespace wn {
namespace engine {
namespace renderer {
namespace {
void register_graphics_enums(scripting::engine* _engine) {
  // These should match the enums in WNGraphicsEnums.h
  _engine->register_named_constant<int32_t>("format_r8g8b8a8_unorm",
      static_cast<int32_t>(runtime::graphics::data_format::r8g8b8a8_unorm));
  _engine->register_named_constant<int32_t>("format_r32g32b32a32_sfloat",
      static_cast<int32_t>(
          runtime::graphics::data_format::r32g32b32a32_sfloat));
  _engine->register_named_constant<int32_t>("format_r32g32b32_sfloat",
      static_cast<int32_t>(runtime::graphics::data_format::r32g32b32_sfloat));
  _engine->register_named_constant<int32_t>("format_r32g32_sfloat",
      static_cast<int32_t>(runtime::graphics::data_format::r32g32_sfloat));
  _engine->register_named_constant<int32_t>("format_d16_unorm",
      static_cast<int32_t>(runtime::graphics::data_format::d16_unorm));
  _engine->register_named_constant<int32_t>("format_d24_unorm",
      static_cast<int32_t>(runtime::graphics::data_format::d24_unorm));
  _engine->register_named_constant<int32_t>("format_d32_float",
      static_cast<int32_t>(runtime::graphics::data_format::d32_float));
  _engine->register_named_constant<int32_t>("format_s8_unorm",
      static_cast<int32_t>(runtime::graphics::data_format::s8_unorm));
  _engine->register_named_constant<int32_t>("format_d16_unorm_s8_uint",
      static_cast<int32_t>(runtime::graphics::data_format::d16_unorm_s8_uint));
  _engine->register_named_constant<int32_t>("format_d24_unorm_s8_uint",
      static_cast<int32_t>(runtime::graphics::data_format::d24_unorm_s8_uint));
  _engine->register_named_constant<int32_t>("format_d32_float_s8_uint",
      static_cast<int32_t>(runtime::graphics::data_format::d32_float_s8_uint));
  _engine->register_named_constant<int32_t>("format_astc_4x4",
      static_cast<int32_t>(runtime::graphics::data_format::astc_4x4));
  _engine->register_named_constant<int32_t>("format_astc_5x4",
      static_cast<int32_t>(runtime::graphics::data_format::astc_5x4));
  _engine->register_named_constant<int32_t>("format_astc_5x5",
      static_cast<int32_t>(runtime::graphics::data_format::astc_5x5));
  _engine->register_named_constant<int32_t>("format_astc_6x5",
      static_cast<int32_t>(runtime::graphics::data_format::astc_6x5));
  _engine->register_named_constant<int32_t>("format_astc_6x6",
      static_cast<int32_t>(runtime::graphics::data_format::astc_6x6));
  _engine->register_named_constant<int32_t>("format_astc_8x5",
      static_cast<int32_t>(runtime::graphics::data_format::astc_8x5));
  _engine->register_named_constant<int32_t>("format_astc_8x6",
      static_cast<int32_t>(runtime::graphics::data_format::astc_8x6));
  _engine->register_named_constant<int32_t>("format_astc_8x8",
      static_cast<int32_t>(runtime::graphics::data_format::astc_8x8));
  _engine->register_named_constant<int32_t>("format_astc_10x5",
      static_cast<int32_t>(runtime::graphics::data_format::astc_10x5));
  _engine->register_named_constant<int32_t>("format_astc_10x6",
      static_cast<int32_t>(runtime::graphics::data_format::astc_10x6));
  _engine->register_named_constant<int32_t>("format_astc_10x8",
      static_cast<int32_t>(runtime::graphics::data_format::astc_10x8));
  _engine->register_named_constant<int32_t>("format_astc_10x10",
      static_cast<int32_t>(runtime::graphics::data_format::astc_10x10));
  _engine->register_named_constant<int32_t>("format_astc_12x10",
      static_cast<int32_t>(runtime::graphics::data_format::astc_12x10));
  _engine->register_named_constant<int32_t>("format_astc_12x12",
      static_cast<int32_t>(runtime::graphics::data_format::astc_12x12));
  _engine->register_named_constant<int32_t>("format_bc1_rgb",
      static_cast<int32_t>(runtime::graphics::data_format::bc1_rgb));
  _engine->register_named_constant<int32_t>("format_bc1_rgba",
      static_cast<int32_t>(runtime::graphics::data_format::bc1_rgba));
  _engine->register_named_constant<int32_t>(
      "format_bc2", static_cast<int32_t>(runtime::graphics::data_format::bc2));
  _engine->register_named_constant<int32_t>(
      "format_bc3", static_cast<int32_t>(runtime::graphics::data_format::bc3));
  _engine->register_named_constant<int32_t>(
      "format_max", static_cast<int32_t>(runtime::graphics::data_format::max));
}
}  // namespace

int32_t test() {
  return 0;
}

void render_context::register_scripting(scripting::engine* _engine) {
  register_graphics_enums(_engine);
  rt_description::register_scripting(_engine);
  render_dependency::register_scripting(_engine);
  render_target_usage::register_scripting(_engine);
  pass_data::register_scripting(_engine);
  render_data::register_scripting(_engine);
  _engine->export_script_type<render_description>();
  _engine->register_cpp_type<render_context>();
  _engine->register_function<decltype(&get_attachmentless_render_context),
      &get_attachmentless_render_context>("get_render_context");
  _engine
      ->register_function<decltype(&get_render_context), &get_render_context>(
      "get_render_context");
  _engine->register_function<decltype(&get_render_context_with_window),
      &get_render_context_with_window>("get_render_context");
  _engine->register_function<decltype(&test), &test>("test");
}

bool render_context::resolve_scripting(scripting::engine* _engine) {
  return rt_description::resolve_scripting(_engine) &&
         render_data::resolve_scripting(_engine) &&
         render_dependency::resolve_scripting(_engine) &&
         render_target_usage::resolve_scripting(_engine) &&
         pass_data::resolve_scripting(_engine) &&
         _engine->resolve_script_type<render_description>();
}

render_context::render_context(memory::allocator* _allocator, logging::log* _log,
    window::window* _window, int32_t _width, int32_t _height)
  : m_log(_log),
    m_window(_window),
    m_allocator(_allocator),
    m_width(_width),
    m_height(_height),
    m_factory(m_allocator, m_log),
    m_command_allocators(m_allocator),
    m_command_lists(m_allocator),
    m_frame_fences(m_allocator),
    m_swapchain_get_signals(m_allocator),
    m_swapchain_ready_signals(m_allocator),
    m_render_targets(_allocator),
    m_render_passes(_allocator) {
  if (_window) {
    m_log->log_info("Created Renderer With Window");
  } else {
    m_log->log_info("Created Renderer Without Window");
  }
  m_log->log_info("Renderer Size: ", _width, "x", _height);

  auto adapters = m_factory.query_adapters();
  m_log->log_info("Found ", adapters.size(), " adapters");

  if (adapters.size() < 1) {
    m_log->log_error("Could not find graphics adapter.");
    return;
  }
  auto& adapter = adapters[0];
  m_log->log_info("Selecting Adapter 0");
  m_device = adapter->make_device(
      m_allocator, m_log, wn::runtime::graphics::k_empty_adapter_features);
  m_queue = m_device->create_queue();

  m_arena_properties = m_device->get_arena_properties();
  for (uint32_t i = 0; i < m_arena_properties.size(); ++i) {
    if (m_arena_properties[i].allow_render_targets &&
        m_arena_properties[i].device_local) {
      m_render_target_heap = memory::make_unique<gpu_heap>(
          m_allocator, m_allocator, m_device.get(), i, kRendertargetHeapSize);
      break;
    }
  }
  if (!m_render_target_heap) {
    m_log->log_error("Selecting Adapter 0");
  }

  // TODO(awoloszyn): Figure out nil surface (or something),
  //    if we don't actually have a window to use.
  if (m_window) {
    auto surface = adapter->make_surface(m_window->underlying());
    if (surface.second != wn::runtime::graphics::graphics_error::ok) {
      m_log->log_error("Could not create surface");
      return;
    }
    m_log->log_info("Created surface");
    m_surface = memory::make_unique<runtime::graphics::surface>(
        m_allocator, core::move(surface.first));

    const runtime::graphics::swapchain_create_info create_info = {
        wn::runtime::graphics::data_format::r8g8b8a8_unorm, 2,
        wn::runtime::graphics::swap_mode::fifo,
        wn::runtime::graphics::discard_policy::discard};
    m_swapchain =
        m_device->create_swapchain(*m_surface, create_info, m_queue.get());
    m_log->log_info("Created swapchain");
  }

  for (size_t i = 0; i < kNumDefaultBackings; ++i) {
    m_command_allocators.push_back(m_device->create_command_allocator());
    m_command_lists.push_back(
        m_command_allocators.back().create_command_list());
    m_frame_fences.push_back(m_device->create_fence());
    m_swapchain_get_signals.push_back(m_device->create_signal());
    m_swapchain_ready_signals.push_back(m_device->create_signal());
  }
}

void render_context::register_description(
    scripting::script_pointer<render_description> _render_description) {
  m_output_rt = _render_description.invoke(&render_description::get_output_target);

  auto targets = _render_description.invoke(&render_description::get_render_targets);
  containers::dynamic_array<uint32_t> num_backings(m_allocator);
  num_backings.resize(targets.size());

  auto passes = _render_description.invoke(&render_description::get_passes);
  containers::dynamic_array<
      containers::dynamic_array<runtime::graphics::render_pass_attachment>>
      all_rp_descs(m_allocator);
  all_rp_descs.reserve(passes.size());

  containers::dynamic_array<runtime::graphics::render_pass_attachment>
      all_depth_descs(m_allocator);

  containers::dynamic_array<containers::dynamic_array<uint32_t>> all_rts(
      m_allocator);
  all_rts.reserve(passes.size());

  containers::dynamic_array<int32_t> all_dts(m_allocator);
  all_dts.reserve(passes.size());

  // TODO(awoloszyn): Figure out how to start merging passes so that
  // we can take advantage of subpasses.
  for (size_t i = 0; i < passes.size(); ++i) {
    const char* pass_name = passes[i].invoke(&pass_data::get_pass_name);
    scripting::wn_array<scripting::script_pointer<render_target_usage>>
        render_targets = passes[i].invoke(&pass_data::get_render_targets);
    containers::dynamic_array<runtime::graphics::render_pass_attachment>
        rp_attachments(m_allocator);
    rp_attachments.resize(render_targets.size());

    containers::dynamic_array<uint32_t> rts(m_allocator);
    rts.resize(render_targets.size());

    for (size_t j = 0; j < render_targets.size(); ++j) {
      int32_t rt =
          render_targets[j].invoke(&render_target_usage::get_render_target);
      if (static_cast<uint32_t>(rt) > targets.size()) {
        m_log->log_error(
            pass_name, " references non-existant RenderTarget", rt);
        return;
      }
      num_backings[rt] = math::max(num_backings[rt], kNumDefaultBackings);
      bool is_depth = targets[rt].invoke(&rt_description::is_depth);
      rp_attachments[j].format = static_cast<runtime::graphics::data_format>(
          targets[rt].invoke(&rt_description::get_format));

      rp_attachments[j].attachment_load_op =
          static_cast<runtime::graphics::load_op>(
              render_targets[j].invoke(&render_target_usage::get_clear_mode));
      rp_attachments[j].attachment_store_op =
          static_cast<runtime::graphics::store_op>(
              render_targets[j].invoke(&render_target_usage::get_store_mode));
      rp_attachments[j].num_samples =
          runtime::graphics::multisample_count::samples_1;
      rp_attachments[j].stencil_load_op = runtime::graphics::load_op::dont_care;
      rp_attachments[j].stencil_store_op =
          runtime::graphics::store_op::dont_care;

      // TODO(awoloszyn): The final state should be inferrable from usage.
      // i.e. If this pass outputs a RT to be used as an input to the next,
      //      we can use the final_state to get a (potentially) cheaper,
      //      transition.
      rp_attachments[j].initial_state =
          is_depth ? runtime::graphics::resource_state::depth_target
                   : runtime::graphics::resource_state::render_target;
      rp_attachments[j].final_state =
          is_depth ? runtime::graphics::resource_state::depth_target
                   : runtime::graphics::resource_state::render_target;
    }

    scripting::script_pointer<render_target_usage> depth_attachment =
        passes[i].invoke(&pass_data::get_depth_target);
    all_depth_descs.push_back(runtime::graphics::render_pass_attachment{});

    int32_t dt =
        depth_attachment.invoke(&render_target_usage::get_render_target);
    if (static_cast<uint32_t>(dt) > targets.size()) {
      m_log->log_error(pass_name, " references non-existant RenderTarget", dt);
      return;
    }
    all_dts.push_back(dt);
    if (dt >= 0) {
      num_backings[dt] = math::max(num_backings[dt], kNumDefaultBackings);
      all_depth_descs.back().format =
          static_cast<runtime::graphics::data_format>(
              targets[dt].invoke(&rt_description::get_format));
      all_depth_descs.back().attachment_load_op =
          static_cast<runtime::graphics::load_op>(
              depth_attachment.invoke(&render_target_usage::get_clear_mode));
      all_depth_descs.back().attachment_store_op =
          static_cast<runtime::graphics::store_op>(
              depth_attachment.invoke(&render_target_usage::get_store_mode));
      all_depth_descs.back().num_samples =
          runtime::graphics::multisample_count::samples_1;
      // TODO(awoloszyn): Plumb Stencil through here
      all_depth_descs.back().stencil_load_op =
          runtime::graphics::load_op::dont_care;
      all_depth_descs.back().stencil_store_op =
          runtime::graphics::store_op::dont_care;
      // TODO(awoloszyn): The final state should be inferrable from usage.
      // i.e. If this pass outputs a RT to be used as an input to the next,
      //      we can use the final_state to get a (potentially) cheaper,
      //      transition.
      all_depth_descs.back().initial_state =
          runtime::graphics::resource_state::render_target;
      all_depth_descs.back().final_state =
          runtime::graphics::resource_state::render_target;
    } else {
      all_depth_descs.back().format = runtime::graphics::data_format::max;
    }

    containers::dynamic_array<int32_t> deps(m_allocator);
    auto dependencies = passes[i].invoke(&pass_data::get_render_dependencies);
    for (size_t d = 0; d < dependencies.size(); ++d) {
      auto dep = dependencies[d];
      auto rt = dep.invoke(&render_dependency::get_render_target);
      if (rt != -1) {
        if (static_cast<uint32_t>(rt) > render_targets.size()) {
          m_log->log_error(pass_name, " does not have ", rt, " render targets");
          continue;
        }
        rt = render_targets[rt].invoke(&render_target_usage::get_render_target);
        auto rd = dep.invoke(&render_dependency::get_frame_offset);
        if (rd > 0) {
          m_log->log_error(
              "Cannot have a dependency forward in time: ", rt, "->", " rd");
          continue;
        }
        num_backings[rd] =
            math::max(num_backings[rd], static_cast<uint32_t>(-rt));
      }
    }

    all_rp_descs.push_back(core::move(rp_attachments));
    all_rts.push_back(core::move(rts));
  }

  for (size_t i = 0; i < num_backings.size(); ++i) {
    if (num_backings[i] == 0) {
      m_log->log_warning(
          "Unused RenderTarget ", targets[i].invoke(&rt_description::get_name));
    }
  }

  for (size_t i = 0; i < targets.size(); ++i) {
    m_render_targets.push_back(render_target(
        m_allocator, m_log, this, m_device.get(), targets[i], num_backings[i]));
  }

  for (size_t i = 0; i < all_rts.size(); ++i) {
    containers::dynamic_array<render_target*> rts(m_allocator);
    for (size_t j = 0; j < all_rts[i].size(); ++j) {
      rts.push_back(&m_render_targets[all_rts[i][j]]);
    }
    if (all_dts[i] != -1) {
      rts.push_back(&m_render_targets[all_dts[i]]);
    }

    m_render_passes.push_back(render_pass(m_allocator, m_log, m_device.get(),
        core::move(all_rp_descs[i]), core::move(rts), all_depth_descs[i]));
  }
}

gpu_allocation render_context::get_allocation_for_render_target(
    uint64_t _size, uint64_t _alignment) {
  return m_render_target_heap->allocate_memory(_size, _alignment);
}

bool render_context::render() {
  // TODO(awoloszyn): I don't like this tick, but it will do for now.
  size_t backing_idx = m_frame_num % kNumDefaultBackings;
  if (m_frame_num >= kNumDefaultBackings) {
    // Wait for the previous frame to be done:
    m_frame_fences[backing_idx].wait();
    m_frame_fences[backing_idx].reset();
  }

  m_command_lists[backing_idx].reset();
  m_command_allocators[backing_idx].reset();
  m_command_lists[backing_idx] =
      m_command_allocators[backing_idx].create_command_list();

  auto* cmd_list = m_command_lists[backing_idx].get();

  for (size_t i = 0; i < m_render_passes.size(); ++i) {
    m_render_passes[i].render(m_frame_num, cmd_list);
  }

  auto output_img_idx =
      m_render_targets[m_output_rt].get_index_for_frame(m_frame_num);
  auto output_img = m_render_targets[m_output_rt].get_image(output_img_idx);

  cmd_list->transition_resource(*output_img,
      static_cast<runtime::graphics::image_components>(
          runtime::graphics::image_component::color),
      0, 1, runtime::graphics::resource_state::render_target,
      runtime::graphics::resource_state::copy_source);

  auto swap_idx = m_swapchain->get_next_backbuffer_index(
      nullptr, &m_swapchain_get_signals[backing_idx]);
  auto swap_img = m_swapchain->get_image_for_index(swap_idx);
  cmd_list->transition_resource(*swap_img,
      static_cast<runtime::graphics::image_components>(
          runtime::graphics::image_component::color),
      0, 1, runtime::graphics::resource_state::present,
      runtime::graphics::resource_state::copy_dest);

  cmd_list->copy_image(*output_img, 0, *swap_img, 0);

  cmd_list->transition_resource(*swap_img,
      static_cast<runtime::graphics::image_components>(
          runtime::graphics::image_component::color),
      0, 1, runtime::graphics::resource_state::copy_dest,
      runtime::graphics::resource_state::present);

  cmd_list->transition_resource(*output_img,
      static_cast<runtime::graphics::image_components>(
          runtime::graphics::image_component::color),
      0, 1, runtime::graphics::resource_state::copy_source,
      runtime::graphics::resource_state::render_target);

  cmd_list->finalize();

  m_queue->enqueue_command_lists({cmd_list},
      {core::make_pair(
          static_cast<runtime::graphics::pipeline_stages>(
              runtime::graphics::pipeline_stage::color_attachment_output),
          &m_swapchain_get_signals[backing_idx])},
      &m_frame_fences[backing_idx], {&m_swapchain_ready_signals[backing_idx]});

  m_swapchain->present(
      m_queue.get(), &m_swapchain_ready_signals[backing_idx], swap_idx);

  m_frame_num++;
  return false;
}

}  // namespace renderer
}  // namespace engine
}  // namespace wn
