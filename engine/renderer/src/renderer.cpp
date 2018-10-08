// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "renderer/inc/renderer.h"
#include "engine_base/inc/context.h"

using namespace wn::engine;
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
    (void)_exporter;
  }
};
}  // namespace scripting
}  // namespace wn

namespace {
shared_cpp_pointer<engine::renderer::renderer> get_renderer(
    engine_base::context* _context) {
  _context->m_log->log_info("Renderer Created");
  return _context->m_engine->make_shared_cpp<engine::renderer::renderer>(
      _context->m_log);
}
}  // anonymous namespace

void engine::renderer::renderer::register_scripting(
    scripting::engine* _engine) {
  _engine->register_cpp_type<renderer>();
  _engine->register_function<decltype(&get_renderer), &get_renderer>(
      "getRenderer");
}
