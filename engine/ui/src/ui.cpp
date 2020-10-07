// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "ui/inc/ui.h"

#include "Rocket/Controls/Controls.h"
#include "Rocket/Core.h"
#include "Rocket/Debugger/Debugger.h"
#include "WNGraphics/inc/WNCommandList.h"
#include "WNScripting/inc/WNEngine.h"
#include "engine_base/inc/context.h"
#include "ui/inc/ui_data.h"
#include "ui/inc/ui_rocket_interop.h"
#include "ui/inc/ui_rocket_renderer.h"
#include "window/inc/window.h"

using namespace wn::engine;
using namespace wn::runtime::graphics;
namespace wn {
namespace engine {
namespace ui {
ui::~ui() {}

void ui::initialize_for_renderpass(
    renderer::render_context* _renderer, renderer::render_pass* _render_pass) {
  WN_RELEASE_ASSERT(
      !m_rocket_context, "Cannot add ui to more than one renderpass");

  auto allocator = m_context->m_allocator;
  m_rocket_context = memory::make_unique<Rocket::Core::Context>(allocator);

  m_renderer = memory::make_unique<rocket_renderer>(allocator, allocator,
      m_rocket_context.get(), _renderer, _render_pass,
      m_context->m_file_mapping);
  m_file_interface = memory::make_unique<rocket_file_interface>(
      allocator, m_context->m_log, m_context->m_file_mapping);
  m_system_interface = memory::make_unique<rocket_system_interface>(
      allocator, m_context->m_log, _renderer->get_window()->underlying());

  Rocket::Core::SetRenderInterface(m_rocket_context.get(), m_renderer.get());
  Rocket::Core::SetSystemInterface(
      m_rocket_context.get(), m_system_interface.get());
  Rocket::Core::SetFileInterface(
      m_rocket_context.get(), m_file_interface.get());

  Rocket::Core::Initialise(m_rocket_context.get());

  m_document_context =
      Rocket::Core::CreateDocumentContext(m_rocket_context.get(), "main",
          Rocket::Core::Vector2i(static_cast<int>(_render_pass->get_width()),
              static_cast<int>(_render_pass->get_height())));

  Rocket::Core::FontDatabase::LoadFontFace(m_rocket_context.get(),
      "assets/fonts/FiraCode-Regular.ttf", "Fira Code",
      Rocket::Core::Font::Style::STYLE_NORMAL,
      Rocket::Core::Font::Weight::WEIGHT_NORMAL);

  Rocket::Debugger::Initialise(m_rocket_context.get(), m_document_context);
  Rocket::Controls::Initialise(m_rocket_context.get());
  const char* element_name = m_data.invoke(&ui_data::get_ui_name);

  m_document = m_document_context->LoadDocument(element_name);
  WN_RELEASE_ASSERT(m_document != NULL,
      "This should not be null, since we ALREADY parsed this once before for "
      "correctness");
  m_document->Show();
  m_document->RemoveReference();
}

void ui::update_render_data(size_t _frame_parity, command_list* _cmd_list) {
  m_renderer->set_setup_command_list(_cmd_list);
  m_renderer->set_render_command_list(nullptr);
  m_renderer->start_frame(static_cast<size_t>(1) << _frame_parity);
  m_document_context->Update();
  // Just make sure this doesnt get kept there
  m_renderer->set_setup_command_list(nullptr);
  m_renderer->set_render_command_list(nullptr);
}

// TODO:Split out render and the rest of it!
void ui::render(renderer::render_pass* _render_pass,
    command_list* _setup_cmd_list, command_list* _cmd_list) {
  (void)_render_pass;
  m_renderer->set_setup_command_list(_setup_cmd_list);
  m_renderer->set_render_command_list(_cmd_list);
  m_document_context->Render();
  // Just make sure this doesnt get kept there
  m_renderer->set_setup_command_list(nullptr);
  m_renderer->set_render_command_list(nullptr);
}

}  // namespace ui
}  // namespace engine
}  // namespace wn