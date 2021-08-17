// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_ENGINE_UI_H__
#define __WN_ENGINE_UI_H__

#include "Rocket/Core.h"
#include "WNWindow/inc/WNInputContext.h"
#include "renderer/inc/renderable_object.h"
#include "ui/inc/ui_data.h"
#include "ui/inc/ui_rocket_interop.h"
#include "ui/inc/ui_rocket_renderer.h"
#include "ui/inc/ui_scripting_event_instancer.h"

namespace wn {
namespace memory {
class allocator;
}
namespace scripting {
class engine;
}
namespace file_system {
class mapping;
}
namespace runtime {
namespace graphics {
class command_list;
}
}  // namespace runtime

namespace engine {
namespace ui {
class rocket_renderer;
class rocket_system_interface;
class rocket_file_interface;

class ui : public renderer::renderable_object {
public:
  static void register_scripting(
      memory::allocator* _allocator, scripting::engine* _engine);
  static bool resolve_scripting(scripting::engine* _engine);
  static scripting::shared_cpp_pointer<renderer::renderable_object>
  get_renderer(engine_base::context* _context,
      scripting::shared_script_pointer<ui_data> _data);
  ui(engine_base::context* _context, scripting::engine* _engine,
      scripting::shared_script_pointer<ui_data> _data)
    : m_context(_context), m_engine(_engine), m_data(_data) {}
  virtual ~ui();
  void initialize_for_renderpass(renderer::render_context* _renderer,
      renderer::render_pass* _renderpass,
      runtime::graphics::command_list* _setup_list) override;
  void render(renderer::render_pass* _render_pass,
      runtime::graphics::command_list* _setup_cmd_list,
      runtime::graphics::command_list* cmd_list) override;
  void update_render_data(
      size_t _frame_parity, runtime::graphics::command_list* cmd_list) override;

private:
  engine_base::context* m_context;
  scripting::engine* m_engine;
  scripting::shared_script_pointer<ui_data> m_data;
  runtime::window::window* m_window = nullptr;
  runtime::window::input_context* m_input_context;
  memory::unique_ptr<rocket_renderer> m_renderer;
  memory::unique_ptr<rocket_file_interface> m_file_interface;
  memory::unique_ptr<rocket_system_interface> m_system_interface;

  // Rocket stuff
  memory::unique_ptr<Rocket::Core::Context> m_rocket_context;
  memory::unique_ptr<event_instancer> m_instancer;

  Rocket::Core::DocumentContext* m_document_context;
  Rocket::Core::ElementDocument* m_document;
  bool m_debugger_visible = false;
  int m_width = 0;
  int m_height = 0;
};

}  // namespace ui
}  // namespace engine
}  // namespace wn
#endif  // __WN_ENGINE_UI_H__
