// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "window/inc/window.h"

#include "WNWindow/inc/WNWindow.h"
#include "WNWindow/inc/WNWindowFactory.h"
#include "engine_base/inc/context.h"
#include "renderer/inc/render_context.h"

using namespace wn::engine;
using wn::scripting::shared_cpp_pointer;
using namespace wn;

// First let us export the renderer to scripting.
namespace wn {
namespace scripting {
template <>
struct exported_script_type<window::window> {
  static containers::string_view exported_name() {
    return "Window";
  }

  template <typename T>
  static void export_type(T* _exporter) {
    (void)_exporter;
  }
};
}  // namespace scripting
}  // namespace wn

namespace {
inline shared_cpp_pointer<engine::window::window> get_window(
    engine_base::context* _context) {
  return _context->m_engine->make_shared_cpp<engine::window::window>(_context);
}

inline shared_cpp_pointer<engine::window::window> get_window_sized(
    engine_base::context* _context, int32_t _width, int32_t _height) {
  return _context->m_engine->make_shared_cpp<engine::window::window>(
      _context, _width, _height);
}
}  // anonymous namespace

void engine::window::window::register_scripting(scripting::engine* _engine) {
  _engine->register_cpp_type<window>();
  _engine->register_function<decltype(&get_window), &get_window>("get_window");
  _engine->register_function<decltype(&get_window_sized), &get_window_sized>(
      "get_window");
}

namespace wn {
namespace engine {
namespace window {

window::window(
    engine_base::context* _context, int32_t _width, int32_t _height) {
  _context->m_log->log_info("Created Window");
  m_log = _context->m_log;
  wn::multi_tasking::job_signal signal(0);
  m_window =
      runtime::window::window_factory(_context->m_allocator, _context->m_log)
          .create_window(runtime::window::window_type::system,
              _context->m_application_data->default_job_pool, &signal,
              _context->m_application_data, 100, 100, _width, _height);

  signal.wait_until(1);
  if (!m_window->is_valid()) {
    m_log->log_critical("Could not create window");
  }
}

window::~window() {
  m_log->log_info("Destroyed Window");
}

int32_t window::width() {
  return m_window->get_width();
}

int32_t window::height() {
  return m_window->get_height();
}

}  // namespace window
}  // namespace engine
}  // namespace wn
