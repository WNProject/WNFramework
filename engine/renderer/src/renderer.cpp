// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "renderer/inc/renderer.h"
#include "WNGraphics/inc/WNGraphicsEnums.h"
#include "engine_base/inc/context.h"
#include "renderer/inc/render_data.h"
#include "window/inc/window.h"

using namespace wn::engine;
using wn::scripting::script_object_type;
using wn::scripting::shared_cpp_pointer;

using namespace wn;

// First let us export the renderer to scripting.
namespace wn {
namespace scripting {
template <>
struct exported_script_type<renderer::renderer> {
  static containers::string_view exported_name() {
    return "Renderer";
  }

  template <typename T>
  static void export_type(T* _exporter) {
    _exporter->template register_nonvirtual_function<
        decltype(&renderer::renderer::register_object),
        &renderer::renderer::register_object>("register_object");
    _exporter->template register_nonvirtual_function<
        decltype(&renderer::renderer::register_pass),
        &renderer::renderer::register_pass>("register_pass");
    _exporter->template register_nonvirtual_function<
        decltype(&renderer::renderer::register_attachment),
        &renderer::renderer::register_attachment>("register_attachment");
    _exporter->template register_nonvirtual_function<
        decltype(&renderer::renderer::width), &renderer::renderer::width>(
        "width");
    _exporter->template register_nonvirtual_function<
        decltype(&renderer::renderer::height), &renderer::renderer::height>(
        "height");
  }
};
}  // namespace scripting
}  // namespace wn

namespace {

shared_cpp_pointer<engine::renderer::renderer> get_attachmentless_renderer(
    engine_base::context* _context) {
  return _context->m_engine->make_shared_cpp<engine::renderer::renderer>(
      _context->m_allocator, _context->m_log, nullptr, 0, 0);
}

shared_cpp_pointer<engine::renderer::renderer> get_renderer(
    engine_base::context* _context, int32_t _width, int32_t _height) {
  if (_width == 0 || _height == 0) {
    _context->m_log->log_error(
        "IF you want to create a renderer without a default attachment"
        " do not specify a width or height");
  }
  return _context->m_engine->make_shared_cpp<engine::renderer::renderer>(
      _context->m_allocator, _context->m_log, nullptr, _width, _height);
}

shared_cpp_pointer<engine::renderer::renderer> get_renderer_with_window(
    engine_base::context* _context, window::window* _window) {
  return _context->m_engine->make_shared_cpp<engine::renderer::renderer>(
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
}
}  // namespace

void renderer::register_scripting(scripting::engine* _engine) {
  register_graphics_enums(_engine);
  render_data::register_scripting(_engine);
  pass_data::register_scripting(_engine);
  _engine->register_cpp_type<renderer>();
  _engine->register_function<decltype(&get_attachmentless_renderer),
      &get_attachmentless_renderer>("get_renderer");
  _engine->register_function<decltype(&get_renderer), &get_renderer>(
      "get_renderer");
  _engine->register_function<decltype(&get_renderer_with_window),
      &get_renderer_with_window>("get_renderer");
}

bool renderer::resolve_scripting(scripting::engine* _engine) {
  return render_data::resolve_scripting(_engine) &&
         pass_data::resolve_scripting(_engine);
}

void renderer::register_object(
    scripting::shared_script_pointer<render_data> _render_data) {
  _render_data.invoke(&render_data::register_data);
}

void renderer::register_pass(scripting::script_pointer<pass_data> _pass_data) {
  const char* pass_name = _pass_data.invoke(&pass_data::pass_name);
  scripting::wn_array_ptr<int32_t> passes =
      _pass_data.invoke(&pass_data::color_attachments);
  int32_t depth_attachment = _pass_data.invoke(&pass_data::depth_attachment);

  scripting::g_scripting_tls->_log->log_error("Register Pass: <", pass_name,
      ">, color_attachments: ", passes,
      ", depth_attachment: ", depth_attachment);
}

int32_t renderer::register_attachment(
    int32_t _format, int32_t _width, int32_t _height) {
  m_attachments.push_back(color_attachment{
      static_cast<runtime::graphics::data_format>(_format), _width, _height});
  size_t index = m_attachments.size() - 1;

  scripting::g_scripting_tls->_log->log_error("Register Attachment: <", index,
      ">, format: ", _format, ", size: ", _width, "x", _height);
  return static_cast<int32_t>(index);
}

}  // namespace renderer
}  // namespace engine
}  // namespace wn
