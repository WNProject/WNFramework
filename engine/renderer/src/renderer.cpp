// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "renderer/inc/renderer.h"
#include "engine_base/inc/context.h"
#include "renderer/inc/render_data.h"

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
  }
};
}  // namespace scripting
}  // namespace wn

namespace {
shared_cpp_pointer<engine::renderer::renderer> get_renderer(
    engine_base::context* _context) {
  return _context->m_engine->make_shared_cpp<engine::renderer::renderer>(
      _context->m_log);
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

void renderer::register_scripting(scripting::engine* _engine) {
  render_data::register_scripting(_engine);
  pass_data::register_scripting(_engine);
  _engine->register_cpp_type<renderer>();
  _engine->register_function<decltype(&get_renderer), &get_renderer>(
      "get_renderer");
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

}  // namespace renderer
}  // namespace engine
}  // namespace wn
