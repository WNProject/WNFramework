// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "engine_base/inc/context.h"
#include "ui/inc/resource_manager.h"
#include "ui/inc/ui.h"
#include "ui/inc/ui_scripting_event_instancer.h"

using namespace wn::engine;
using namespace wn::runtime::graphics;
namespace wn {
namespace scripting {

template <>
struct exported_script_type<ui::ui> {
  using parent_type = renderer::renderable_object;
  static containers::string_view exported_name() {
    return "UI";
  }

  template <typename T>
  static void export_type(T* _exporter) {
    _exporter->template register_nonvirtual_function<
        decltype(&ui::ui::add_document), &ui::ui::add_document>("add_document");
  }
};
}  // namespace scripting

namespace engine {
namespace ui {

void ui::register_scripting(
    memory::allocator* _allocator, scripting::engine* _engine) {
  ui_data::register_scripting(_engine);
  resource_manager::register_scripting(_allocator, _engine);
  event_instancer::register_scripting(_allocator, _engine);

  _engine->register_child_cpp_type<ui>();
  _engine->register_function<decltype(&ui::get_renderer), &ui::get_renderer>(
      "get_ui_renderer");
}

bool ui::resolve_scripting(scripting::engine* _engine) {
  return resource_manager::resolve_scripting(_engine) &&
         ui_data::resolve_scripting(_engine);
}

scripting::shared_cpp_pointer<ui> ui::get_renderer(
    engine_base::context* _context) {
  return _context->m_engine->make_shared_cpp<ui>(_context, _context->m_engine);
}

}  // namespace ui
}  // namespace engine
}  // namespace wn
