// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "renderer/inc/render_context.h"

#include "WNGraphics/inc/WNCommandList.h"
#include "WNGraphics/inc/WNGraphicsEnums.h"
#include "WNGraphics/inc/WNSwapchain.h"
#include "engine_base/inc/context.h"
#include "profiling/inc/profiling.h"
#include "renderer/inc/render_data.h"
#include "renderer/inc/render_target.h"
#include "renderer/inc/renderable_object.h"
#include "renderer/inc/texture_manager.h"
#include "window/inc/window.h"

using namespace wn::engine;
using wn::scripting::script_object_type;
using wn::scripting::script_pointer;
using wn::scripting::shared_cpp_pointer;

using namespace wn;

// TODO(awoloszyn): Make this configurable, or resizable at least
const size_t kRendertargetHeapSize =
    1024 * 1024 * 256;  // 128MB rendertarget heap.
const size_t kGPUBufferHeapSize = 1024 * 1024 * 128;     // 128MB buffer heap;
const size_t kUploadBufferHeapSize = 1024 * 1024 * 128;  // 128MB upload heap;
const size_t kTextureHeapSize = 1024 * 1024 * 256;       // 256MB texture heap;
const size_t kTemporaryTextureHeapSize =
    1024 * 1024 * 32;  // 32MB for texture uploads;
const uint32_t kNumDefaultBackings = 2;

// First let us export the renderer to scripting.
namespace wn {
namespace scripting {
template <>
struct exported_script_type<renderer::render_context> {
  static containers::string_view exported_name() {
    return "RenderContext";
  }

  static void export_type(
      wn::scripting::exporter<renderer::render_context>* _exporter) {
    _exporter->register_virtual<int32_t>()
        .register_virtual<&renderer::render_context::width>("width");
    _exporter->register_virtual<int32_t>()
        .register_virtual<&renderer::render_context::height>("height");
    _exporter->register_virtual<bool>()
        .register_virtual<&renderer::render_context::render>("render");
    _exporter
        ->template register_virtual<void,
            shared_cpp_pointer<renderer::renderable_object>,
            slice<const char*>>()
        .register_virtual<&renderer::render_context::add_renderable_to_passes>(
            "add_renderable_to_passes");
    _exporter
        ->register_virtual<void,
            scripting::script_pointer<renderer::render_description>>()
        .register_virtual<&renderer::render_context::register_description>(
            "register_description");
  }
};
}  // namespace scripting
}  // namespace wn

namespace {

uint32_t select_adapter(engine_base::context* _context) {
  uint32_t adapter = 0;

  auto data = _context->m_application_data->executable_data;
  for (signed_t i = 0; i < data->argc; ++i) {
    if (data->argv[i] == containers::string_view("--adapter") &&
        i < data->argc - 1) {
      memory::readuint32(
          data->argv[i + 1], adapter, strnlen(data->argv[i + 1], 10));
    }
  }
  return adapter;
}

shared_cpp_pointer<engine::renderer::render_context>
get_attachmentless_render_context(engine_base::context* _context) {
  uint32_t adapter = select_adapter(_context);
  return _context->m_engine->make_shared_cpp<engine::renderer::render_context>(
      _context->m_allocator, _context->m_log, nullptr, 0, 0, adapter);
}

shared_cpp_pointer<engine::renderer::render_context> get_render_context(
    engine_base::context* _context, int32_t _width, int32_t _height) {
  uint32_t adapter = select_adapter(_context);
  if (_width == 0 || _height == 0) {
    _context->m_log->log_error(
        "If you want to create a renderer without a default attachment"
        " do not specify a width or height");
  }
  return _context->m_engine->make_shared_cpp<engine::renderer::render_context>(
      _context->m_allocator, _context->m_log, nullptr, _width, _height,
      adapter);
}

shared_cpp_pointer<engine::renderer::render_context>
get_render_context_with_window(
    engine_base::context* _context, window::window* _window) {
  uint32_t adapter = select_adapter(_context);
  return _context->m_engine->make_shared_cpp<engine::renderer::render_context>(
      _context->m_allocator, _context->m_log, _window, _window->width(),
      _window->height(), adapter);
}

struct render_data : script_object_type {
  using parent_type = void;
  static wn::containers::string_view exported_name() {
    return "RenderData";
  }

  static wn::containers::string_view required_script() {
    return "";
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

void render_context::register_scripting(
    memory::allocator* _allocator, scripting::engine* _engine) {
  register_graphics_enums(_engine);
  rt_description::register_scripting(_engine);
  render_dependency::register_scripting(_engine);
  render_target_usage::register_scripting(_engine);
  pass_data::register_scripting(_engine);
  render_data::register_scripting(_engine);
  renderable_object::register_scripting(_engine);
  texture_manager::register_scripting(_allocator, _engine);
  _engine->export_script_type<render_description>();
  _engine->register_cpp_type<render_context>();
  _engine->register_function<decltype(&get_attachmentless_render_context),
      &get_attachmentless_render_context>("get_render_context");
  _engine
      ->register_function<decltype(&get_render_context), &get_render_context>(
          "get_render_context");
  _engine->register_function<decltype(&get_render_context_with_window),
      &get_render_context_with_window>("get_render_context");
}

bool render_context::resolve_scripting(scripting::engine* _engine) {
  return rt_description::resolve_scripting(_engine) &&
         render_data::resolve_scripting(_engine) &&
         renderable_object::resolve_scripting(_engine) &&
         render_dependency::resolve_scripting(_engine) &&
         render_target_usage::resolve_scripting(_engine) &&
         pass_data::resolve_scripting(_engine) &&
         _engine->resolve_script_type<render_description>() &&
         texture_manager::resolve_scripting(_engine);
}

render_context::render_context(memory::allocator* _allocator,
    logging::log* _log, window::window* _window, int32_t _width,
    int32_t _height, uint32_t _forced_adapter)
  : m_log(_log),
    m_window(_window),
    m_allocator(_allocator),
    m_width(_width),
    m_height(_height),
    m_factory(m_allocator, m_log),
    m_command_allocators(m_allocator),
    m_command_lists(m_allocator),
    m_setup_command_allocators(m_allocator),
    m_setup_command_lists(m_allocator),
    m_frame_fences(m_allocator),
    m_swapchain_get_signals(m_allocator),
    m_swapchain_ready_signals(m_allocator),
    m_swapchain_image_initialized(m_allocator),
    m_render_targets(_allocator),
    m_render_passes(_allocator),
    m_render_pass_names(_allocator),
    m_renderables(_allocator),
    m_temporary_buffers(_allocator),
    m_temporary_images(_allocator),
    m_pending_renderables(_allocator) {
  if (_window) {
    m_log->log_info("Created Renderer With Window");
  } else {
    m_log->log_info("Created Renderer Without Window");
  }
  m_log->log_info("Renderer Size: ", _width, "x", _height);

  auto adapters = m_factory.query_adapters();
  m_log->log_info("Found ", adapters.size(), " adapters");

  if (adapters.size() < _forced_adapter + 1) {
    m_log->log_error("Could not find graphics adapter.");
    return;
  }
  auto& adapter = adapters[_forced_adapter];
  m_log->log_info("Selecting Adapter ", _forced_adapter);
  m_device = adapter->make_device(
      m_allocator, m_log, wn::runtime::graphics::k_empty_adapter_features);
  m_queue = m_device->create_queue();

  m_arena_properties = m_device->get_arena_properties();
  for (uint32_t i = 0; i < m_arena_properties.size(); ++i) {
    if (m_arena_properties[i].allow_render_targets &&
        m_arena_properties[i].device_local && !m_render_target_heap) {
      m_render_target_heap = memory::make_unique<gpu_heap>(
          m_allocator, m_allocator, m_device.get(), i, kRendertargetHeapSize);
    }
    if (m_arena_properties[i].allow_buffers &&
        m_arena_properties[i].device_local && !m_buffer_heap) {
      m_buffer_heap = memory::make_unique<gpu_heap>(
          m_allocator, m_allocator, m_device.get(), i, kGPUBufferHeapSize);
    }
    if (m_arena_properties[i].allow_buffers &&
        m_arena_properties[i].host_visible && !m_upload_heap) {
      m_upload_heap = memory::make_unique<gpu_heap>(
          m_allocator, m_allocator, m_device.get(), i, kUploadBufferHeapSize);
    }
    if (m_arena_properties[i].allow_images &&
        m_arena_properties[i].device_local && !m_texture_heap) {
      m_texture_heap = memory::make_unique<gpu_heap>(
          m_allocator, m_allocator, m_device.get(), i, kTextureHeapSize);
    }
    if (m_arena_properties[i].allow_images &&
        m_arena_properties[i].allow_render_targets &&
        !m_temporary_texture_heap) {
      m_temporary_texture_heap = memory::make_unique<gpu_heap>(m_allocator,
          m_allocator, m_device.get(), i, kTemporaryTextureHeapSize);
    }
  }
  if (!m_render_target_heap) {
    m_log->log_error("Could not create RenderTarget Heap", _forced_adapter);
    return;
  }

  if (!m_buffer_heap) {
    m_log->log_error("Could not create Buffer Heap", _forced_adapter);
    return;
  }
  if (!m_upload_heap) {
    m_log->log_error("Could not create Upload Heap", _forced_adapter);
    return;
  }

  // TODO(awoloszyn): Figure out nil surface (or something),
  //    if we don't actually have a window to use.
  if (m_window) {
    auto surface = adapter->make_surface(m_allocator, m_window->underlying());
    if (surface.second != wn::runtime::graphics::graphics_error::ok) {
      m_log->log_error("Could not create surface");
      return;
    }
    m_log->log_info("Created surface");
    m_surface = memory::make_unique<runtime::graphics::surface>(
        m_allocator, core::move(surface.first));

    runtime::graphics::data_format swapchain_format =
        m_surface->valid_formats()[0];
    const runtime::graphics::swapchain_create_info create_info = {
        swapchain_format, 2, wn::runtime::graphics::swap_mode::fifo,
        wn::runtime::graphics::discard_policy::discard};
    m_swapchain =
        m_device->create_swapchain(*m_surface, create_info, m_queue.get());
    m_log->log_info("Created swapchain");
  }

  auto inf = m_swapchain->info();
  m_swapchain_image_initialized.resize(inf.num_buffers);

  for (size_t i = 0; i < kNumDefaultBackings; ++i) {
    m_command_allocators.push_back(m_device->create_command_allocator());
    m_command_lists.push_back(runtime::graphics::command_list_ptr());
    m_setup_command_allocators.push_back(m_device->create_command_allocator());
    m_setup_command_lists.push_back(runtime::graphics::command_list_ptr());
    m_frame_fences.push_back(m_device->create_fence());
    m_swapchain_get_signals.push_back(m_device->create_signal());
    m_swapchain_ready_signals.push_back(m_device->create_signal());

    m_temporary_buffers.push_back(
        containers::list<temporary_buffer>(m_allocator));
    m_temporary_images.push_back(
        containers::list<temporary_image>(m_allocator));
  }
  m_last_up_to_date_width = m_width;
  m_last_up_to_date_height = m_height;
}

void render_context::register_description(
    scripting::script_pointer<render_description> _render_description) {
  m_output_rt =
      _render_description.invoke(&render_description::get_output_target);

  auto targets =
      _render_description.invoke(&render_description::get_render_targets);
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
    if (dt > static_cast<int32_t>(targets.size())) {
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
          runtime::graphics::resource_state::depth_target;
      all_depth_descs.back().final_state =
          runtime::graphics::resource_state::depth_target;
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
    m_render_pass_names.push_back(containers::string(m_allocator, pass_name));
    all_rp_descs.push_back(core::move(rp_attachments));
    all_rts.push_back(core::move(rts));
    m_pending_renderables.push_back(
        containers::list<scripting::shared_cpp_pointer<renderable_object>>(
            m_allocator));
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

gpu_allocation render_context::get_allocation_for_buffer(
    uint64_t _size, uint64_t _alignment) {
  return m_buffer_heap->allocate_memory(_size, _alignment);
}

gpu_allocation render_context::get_allocation_for_upload(
    uint64_t _size, uint64_t _alignment) {
  return m_upload_heap->allocate_memory(_size, _alignment);
}

gpu_allocation render_context::get_allocation_for_texture(
    uint64_t _size, uint64_t _alignment) {
  return m_texture_heap->allocate_memory(_size, _alignment);
}

void render_context::add_renderable_to_passes(
    scripting::shared_cpp_pointer<renderable_object> _obj,
    scripting::slice<const char*> _passes) {
  for (auto& i : _passes) {
    // TODO(awoloszyn): Can do much better here
    for (size_t rp = 0; rp < m_render_pass_names.size(); ++rp) {
      if (m_render_pass_names[rp] == i) {
        m_pending_renderables[rp].push_back(_obj);
        break;
      }
    }
  }
}

bool render_context::render() {
  size_t backing_idx = m_frame_num % kNumDefaultBackings;
  uint32_t swap_idx = 0;
  {
    static const char* render = "Render";
    profiling::FrameMarker marker(render);
    PROFILE_REGION(Rendering);

    // TODO(awoloszyn): I don't like this tick, but it will do for now.
    if (m_frame_num >= kNumDefaultBackings) {
      // Wait for the previous frame to be done:
      m_frame_fences[backing_idx].wait();
      m_frame_fences[backing_idx].reset();
      m_temporary_buffers[backing_idx].clear();
      m_temporary_images[backing_idx].clear();
    }
    int32_t new_width = m_window->width();
    int32_t new_height = m_window->height();
    if (new_width == m_last_up_to_date_width &&
        new_height == m_last_up_to_date_height) {
      m_last_up_to_date_frame = m_frame_num;
    }
    m_width = new_width;
    m_height = new_height;

    for (auto& rt : m_render_targets) {
      rt.setup_for_frame(this, m_frame_num, true);
    }

    // m_command_lists[backing_idx].reset();
    m_command_allocators[backing_idx].reset();
    m_command_lists[backing_idx] =
        m_command_allocators[backing_idx].create_command_list();

    // m_setup_command_lists[backing_idx].reset();
    m_setup_command_allocators[backing_idx].reset();
    m_setup_command_lists[backing_idx] =
        m_setup_command_allocators[backing_idx].create_command_list();

    auto* setup_list = m_setup_command_lists[backing_idx].get();
    {
      PROFILE_REGION(SetupRender);
      containers::hash_set<renderable_object*> m_added(m_allocator);
      for (size_t i = 0; i < m_pending_renderables.size(); ++i) {
        render_pass& pass = m_render_passes[i];
        for (wn::scripting::shared_cpp_pointer<
                 wn::engine::renderer::renderable_object>& obj :
            m_pending_renderables[i]) {
          obj->initialize_for_renderpass(this, &pass, setup_list);
          pass.add_renderable_object(obj);
          if (m_added.find(obj.get()) != m_added.end()) {
            continue;
          }
          m_added.insert(obj.get());
          m_renderables.push_back(core::move(obj));
        }
        m_pending_renderables[i].clear();
      }
    }
    {
      PROFILE_REGION(UpdateRenderData);
      for (auto& renderable : m_renderables) {
        renderable->update_render_data(backing_idx, setup_list);
      }
    }

    auto* cmd_list = m_command_lists[backing_idx].get();
    {
      PROFILE_REGION(RenderPasses);
      for (size_t i = 0; i < m_render_passes.size(); ++i) {
        m_render_passes[i].render(this, m_frame_num, setup_list, cmd_list);
      }
    }
    setup_list->finalize();
    m_queue->enqueue_command_lists({setup_list}, {}, nullptr, {});

    auto output_img_idx =
        m_render_targets[m_output_rt].get_index_for_frame(m_frame_num);
    auto output_img =
        m_render_targets[m_output_rt].get_image_for_index(output_img_idx);

    cmd_list->transition_resource(*output_img,
        static_cast<runtime::graphics::image_components>(
            runtime::graphics::image_component::color),
        0, 1, runtime::graphics::resource_state::render_target,
        runtime::graphics::resource_state::copy_source);

    swap_idx = m_swapchain->get_next_backbuffer_index(
        nullptr, &m_swapchain_get_signals[backing_idx]);
    while (m_frame_num > m_last_up_to_date_frame + 60) {
      m_log->log_warning("Swapchain out of date, resizing");

      runtime::graphics::data_format swapchain_format =
          m_surface->valid_formats()[0];
      const runtime::graphics::swapchain_create_info create_info = {
          swapchain_format, 2, wn::runtime::graphics::swap_mode::fifo,
          wn::runtime::graphics::discard_policy::discard};
      if (m_swapchain) {
        m_swapchain = m_device->recreate_swapchain(
            *m_surface, core::move(m_swapchain), create_info, m_queue.get());
      } else {
        m_swapchain =
            m_device->create_swapchain(*m_surface, create_info, m_queue.get());
      }
      if (!m_swapchain) {
        m_log->log_warning("Swapchain recreation failed, trying again");
        continue;
      }
      m_swapchain_image_initialized.resize(m_swapchain->info().num_buffers);
      for (auto& init : m_swapchain_image_initialized) {
        init = false;
      }

      swap_idx = m_swapchain->get_next_backbuffer_index(
          nullptr, &m_swapchain_get_signals[backing_idx]);
      WN_RELEASE_ASSERT(swap_idx >= 0, "Cannot recreate the swapchain");
      m_last_up_to_date_width = m_width;
      m_last_up_to_date_height = m_height;
      m_last_up_to_date_frame = m_frame_num;
    }

    auto swap_img = m_swapchain->get_image_for_index(swap_idx);
    if (m_swapchain_image_initialized[swap_idx]) {
      cmd_list->transition_resource(*swap_img,
          static_cast<runtime::graphics::image_components>(
              runtime::graphics::image_component::color),
          0, 1, runtime::graphics::resource_state::present,
          runtime::graphics::resource_state::blit_dest);

    } else {
      m_swapchain_image_initialized[swap_idx] = true;
      cmd_list->transition_resource(*swap_img,
          static_cast<runtime::graphics::image_components>(
              runtime::graphics::image_component::color),
          0, 1, runtime::graphics::resource_state::initial,
          runtime::graphics::resource_state::blit_dest);
    }

    // We use a blit image here in case the swapchain image format does
    // not match the render image format.
    cmd_list->blit_image(*output_img, 0, *swap_img, 0);

    cmd_list->transition_resource(*swap_img,
        static_cast<runtime::graphics::image_components>(
            runtime::graphics::image_component::color),
        0, 1, runtime::graphics::resource_state::blit_dest,
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
        &m_frame_fences[backing_idx],
        {&m_swapchain_ready_signals[backing_idx]});
  }
  m_swapchain->present(
      m_queue.get(), &m_swapchain_ready_signals[backing_idx], swap_idx);

  m_frame_num++;
  m_log->flush();

  return false;
}

runtime::graphics::image* render_context::create_temporary_image(
    runtime::graphics::image_create_info* _create_info) {
  runtime::graphics::clear_value c{};
  runtime::graphics::image img = m_device->create_image(*_create_info, c);
  runtime::graphics::image_memory_requirements reqs =
      img.get_memory_requirements();
  gpu_allocation alloc =
      m_temporary_texture_heap->allocate_memory(reqs.size, reqs.alignment);
  if (!alloc.is_valid()) {
    m_log->log_error("Could not allocate memory for temporary texture");
    return nullptr;
  }
  img.bind_memory(alloc.arena(), alloc.offset());

  size_t backing_idx = m_frame_num % kNumDefaultBackings;

  m_temporary_images[backing_idx].push_back(
      temporary_image(core::move(img), core::move(alloc)));

  return m_temporary_images[backing_idx].rbegin()->image();
}

runtime::graphics::buffer* render_context::create_temporary_upload_buffer(
    size_t _size) {
  runtime::graphics::buffer buff = m_device->create_buffer(
      _size, runtime::graphics::resource_state::copy_source |
                 runtime::graphics::resource_state::host_write);
  runtime::graphics::buffer_memory_requirements reqs =
      buff.get_memory_requirements();
  gpu_allocation alloc =
      m_upload_heap->allocate_memory(reqs.size, reqs.alignment);
  if (!alloc.is_valid()) {
    m_log->log_error("Could not allocate memory for temporary buffer");
    return nullptr;
  }
  buff.bind_memory(alloc.arena(), alloc.offset());

  size_t backing_idx = m_frame_num % kNumDefaultBackings;
  m_temporary_buffers[backing_idx].push_back(
      temporary_buffer(core::move(buff), core::move(alloc)));
  return m_temporary_buffers[backing_idx].rbegin()->buffer();
}

memory::unique_ptr<texture> render_context::load_texture(
    runtime::graphics::command_list* _setup_command_list,
    file_system::mapping* _mapping, scripting::script_pointer<texture_desc> tex,
    logging::log* _log) {
  return memory::make_unique<texture>(
      m_allocator, this, _mapping, _setup_command_list, tex, _log);
}

}  // namespace renderer
}  // namespace engine
}  // namespace wn
