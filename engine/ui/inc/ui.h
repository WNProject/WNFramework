// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_ENGINE_UI_H__
#define __WN_ENGINE_UI_H__

#include "RmlUi/Core.h"
#include "WNMultiTasking/inc/mutex.h"
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
  static scripting::shared_cpp_pointer<ui> get_renderer(
      engine_base::context* _context);
  ui(engine_base::context* _context, scripting::engine* _engine)
    : m_context(_context),
      m_engine(_engine),
      m_documents(_context->m_ui_allocator),
      m_rocket_documents(_context->m_ui_allocator) {}
  virtual ~ui();
  void initialize_for_renderpass(renderer::render_context* _renderer,
      renderer::render_pass* _renderpass,
      runtime::graphics::command_list* _setup_list) override;
  void render(renderer::render_pass* _render_pass,
      runtime::graphics::command_list* _setup_cmd_list,
      runtime::graphics::command_list* cmd_list,
      const runtime::graphics::descriptor_set* pass_set) override;
  void update_render_data(
      size_t _frame_parity, runtime::graphics::command_list* cmd_list) override;

  int32_t add_document(
      scripting::script_actor_pointer<ui_data> _data, int32_t _x, int32_t _y);

private:
  struct document_to_add {
    int32_t _x;
    int32_t _y;
    scripting::script_actor_pointer<ui_data> _data;
    int32_t _idx;
  };

  void add_doc(document_to_add* doc);

  engine_base::context* m_context;
  scripting::engine* m_engine;
  scripting::script_actor_pointer<ui_data> m_data;
  runtime::window::window* m_window = nullptr;
  runtime::window::input_context* m_input_context;
  memory::unique_ptr<rocket_renderer> m_renderer;
  memory::unique_ptr<rocket_file_interface> m_file_interface;
  memory::unique_ptr<rocket_system_interface> m_system_interface;

  containers::dynamic_array<memory::unique_ptr<document_to_add>> m_documents;
  containers::hash_map<int32_t, Rml::ElementDocument*> m_rocket_documents;
  int32_t m_document_idx = 0;

  // Rocket stuff
  memory::unique_ptr<Rml::Context> m_rocket_context;
  memory::unique_ptr<event_instancer> m_instancer;

  multi_tasking::mutex m_mutex;

  Rml::Context* m_document_context = nullptr;
  Rml::GlobalContext m_global_context;
  bool m_debugger_visible = false;
  int m_width = 0;
  int m_height = 0;
};

}  // namespace ui
}  // namespace engine
}  // namespace wn
#endif  // __WN_ENGINE_UI_H__
