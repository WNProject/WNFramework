// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/inc/ui.h"

#include "RmlUi/Core.h"
#include "RmlUi/Core/Element.h"
#include "RmlUi/Debugger/Debugger.h"
#include "WNGraphics/inc/WNCommandList.h"
#include "WNScripting/inc/WNEngine.h"
#include "WNWindow/inc/WNWindow.h"
#include "engine_base/inc/context.h"
#include "profiling/inc/profiling.h"
#include "ui/inc/ui_data.h"
#include "ui/inc/ui_rocket_interop.h"
#include "ui/inc/ui_rocket_renderer.h"
#include "window/inc/window.h"

using namespace wn::engine;
using namespace wn::runtime::graphics;

namespace {

int get_key_modifier_state(wn::runtime::window::window* _window) {
  bool ctrl =
      _window->get_key_state(wn::runtime::window::key_code::key_lctrl) ||
      _window->get_key_state(wn::runtime::window::key_code::key_rctrl) ||
      _window->get_key_state(wn::runtime::window::key_code::key_any_ctrl);

  bool alt = _window->get_key_state(wn::runtime::window::key_code::key_lalt) ||
             _window->get_key_state(wn::runtime::window::key_code::key_ralt) ||
             _window->get_key_state(wn::runtime::window::key_code::key_any_alt);

  bool shift =
      _window->get_key_state(wn::runtime::window::key_code::key_lshift) ||
      _window->get_key_state(wn::runtime::window::key_code::key_rshift) ||
      _window->get_key_state(wn::runtime::window::key_code::key_any_shift);

  return Rml::Input::KeyModifier::KM_CTRL * ctrl +
         Rml::Input::KeyModifier::KM_ALT * alt +
         Rml::Input::KeyModifier::KM_SHIFT * shift;
}

Rml::Input::KeyIdentifier key_code_to_key_index(
    wn::runtime::window::key_code _key_code) {
  if (_key_code >= wn::runtime::window::key_code::key_0 &&
      _key_code <= wn::runtime::window::key_code::key_9) {
    return static_cast<Rml::Input::KeyIdentifier>(
        Rml::Input::KeyIdentifier::KI_0 + static_cast<int>(_key_code) -
        static_cast<int>(wn::runtime::window::key_code::key_0));
  }
  if (_key_code >= wn::runtime::window::key_code::key_a &&
      _key_code <= wn::runtime::window::key_code::key_z) {
    return static_cast<Rml::Input::KeyIdentifier>(
        Rml::Input::KeyIdentifier::KI_A + static_cast<int>(_key_code) -
        static_cast<int>(wn::runtime::window::key_code::key_a));
  }
  if (_key_code >= wn::runtime::window::key_code::key_num_0 &&
      _key_code <= wn::runtime::window::key_code::key_num_9) {
    return static_cast<Rml::Input::KeyIdentifier>(
        Rml::Input::KeyIdentifier::KI_NUMPAD0 + static_cast<int>(_key_code) -
        static_cast<int>(wn::runtime::window::key_code::key_num_0));
  }

  switch (_key_code) {
    case wn::runtime::window::key_code::key_space:
      return Rml::Input::KeyIdentifier::KI_SPACE;
    case wn::runtime::window::key_code::key_lshift:
      return Rml::Input::KeyIdentifier::KI_LSHIFT;
    case wn::runtime::window::key_code::key_rshift:
      return Rml::Input::KeyIdentifier::KI_RSHIFT;
    case wn::runtime::window::key_code::key_lctrl:
      return Rml::Input::KeyIdentifier::KI_LCONTROL;
    case wn::runtime::window::key_code::key_rctrl:
      return Rml::Input::KeyIdentifier::KI_RCONTROL;
    case wn::runtime::window::key_code::key_lalt:
      return Rml::Input::KeyIdentifier::KI_LMENU;
    case wn::runtime::window::key_code::key_ralt:
      return Rml::Input::KeyIdentifier::KI_RMENU;
    case wn::runtime::window::key_code::key_backspace:
      return Rml::Input::KeyIdentifier::KI_BACK;
    case wn::runtime::window::key_code::key_del:
      return Rml::Input::KeyIdentifier::KI_DELETE;
    case wn::runtime::window::key_code::key_left:
      return Rml::Input::KeyIdentifier::KI_LEFT;
    case wn::runtime::window::key_code::key_right:
      return Rml::Input::KeyIdentifier::KI_RIGHT;
    case wn::runtime::window::key_code::key_up:
      return Rml::Input::KeyIdentifier::KI_UP;
    case wn::runtime::window::key_code::key_down:
      return Rml::Input::KeyIdentifier::KI_DOWN;
    case wn::runtime::window::key_code::key_tab:
      return Rml::Input::KeyIdentifier::KI_TAB;
    default:
      return Rml::Input::KeyIdentifier::KI_UNKNOWN;
  }
}

int mouse_button_to_index(wn::runtime::window::mouse_button _button) {
  switch (_button) {
    case wn::runtime::window::mouse_button::mouse_l:
      return 0;
    case wn::runtime::window::mouse_button::mouse_r:
      return 1;
    case wn::runtime::window::mouse_button::mouse_m:
      return 2;
    default:
      return -1;
  }
}

}  // namespace
namespace wn {
namespace engine {
namespace ui {
ui::~ui() {}

void ui::initialize_for_renderpass(renderer::render_context* _renderer,
    renderer::render_pass* _render_pass,
    runtime::graphics::command_list* _setup_list) {
  PROFILE_REGION(UIInitialize);

  WN_RELEASE_ASSERT(
      !m_rocket_context, "Cannot add ui to more than one renderpass");

  auto allocator = m_context->m_ui_allocator;

  m_renderer =
      memory::make_unique<rocket_renderer>(allocator, allocator, _renderer,
          _render_pass, m_context->m_file_mapping, m_engine, m_context->m_log);
  m_renderer->set_setup_command_list(_setup_list);
  m_file_interface = memory::make_unique<rocket_file_interface>(
      allocator, m_context->m_log, m_context->m_file_mapping);
  m_system_interface = memory::make_unique<rocket_system_interface>(
      allocator, m_context->m_log, _renderer->get_window()->underlying());
  m_window = _renderer->get_window()->underlying();
  m_instancer = memory::make_unique<event_instancer>(
      allocator, m_engine, allocator, m_context->m_log);
  m_input_context = m_window->get_input_context();
  m_width = static_cast<int>(_render_pass->get_width());
  m_height = static_cast<int>(_render_pass->get_height());

  Rml::SetGlobalContextForBlock ctxblk(&m_global_context);
  auto setter =
      m_context->m_application_data->default_job_pool->add_fiber_swap_function(
          functional::function<void()>(allocator, [this]() {
            Rml::GlobalContext::SetGlobalContextForThread(&m_global_context);
          }));
  (void)setter;
  Rml::SetRenderInterface(m_renderer.get());
  Rml::SetSystemInterface(m_system_interface.get());
  Rml::SetFileInterface(m_file_interface.get());

  Rml::Initialise();
  Rml::Factory::RegisterEventListenerInstancer(m_instancer.get());

  m_document_context =
      Rml::CreateContext("main", Rml::Vector2i(m_width, m_height));
  const float dpi = static_cast<float>(m_window->get_dpi());

  m_document_context->SetDensityIndependentPixelRatio(dpi / 96.0f);

  Rml::LoadFontFace("assets/fonts/FiraCode-Regular.ttf", false,
      Rml::Style::FontWeight::Normal);

  Rml::Debugger::Initialise(m_document_context);

  for (auto& doc : m_documents) {
    add_doc(doc.get());
  }
  m_documents.clear();
}

void ui::update_render_data(size_t _frame_parity, command_list* _cmd_list) {
  Rml::SetGlobalContextForBlock ctxblk(&m_global_context);
  auto allocator = m_context->m_ui_allocator;
  auto setter =
      m_context->m_application_data->default_job_pool->add_fiber_swap_function(
          functional::function<void()>(allocator, [this]() {
            Rml::GlobalContext::SetGlobalContextForThread(&m_global_context);
          }));
  (void)setter;
  {
    PROFILE_REGION(UIDocumentUpdate);
    if (!m_instancer->dirty_documents().empty()) {
      auto d = core::move(m_instancer->dirty_documents());
      m_instancer->dirty_documents() =
          containers::hash_set<Rml::ElementDocument*>(
              m_context->m_ui_allocator);
      for (auto& it : d) {
        scripting::script_pointer<ui_data> dat =
            m_instancer->get_element_context(it);
        dat.invoke(&ui_data::update, it);
      }
    }
  }
  PROFILE_REGION(UIUpdateRenderData);
  wn::runtime::window::input_event evt;
  int key_modifier_state = get_key_modifier_state(m_window);
  while (m_input_context->get_event(&evt)) {
    switch (evt.type()) {
      case wn::runtime::window::event_type::key_down:
        m_document_context->ProcessKeyDown(
            key_code_to_key_index(evt.get_keycode()), key_modifier_state);
        if (evt.get_keycode() == runtime::window::key_code::key_l &&
            m_window->get_key_state(runtime::window::key_code::key_any_ctrl)) {
          m_debugger_visible ^= true;
          Rml::Debugger::SetVisible(m_debugger_visible);
        }
        break;
      case wn::runtime::window::event_type::key_up:
        m_document_context->ProcessKeyUp(
            key_code_to_key_index(evt.get_keycode()), key_modifier_state);
        break;
      case wn::runtime::window::event_type::mouse_down:
        m_document_context->ProcessMouseButtonDown(
            mouse_button_to_index(evt.get_mouse_button()), key_modifier_state);
        break;
      case wn::runtime::window::event_type::mouse_up:
        m_document_context->ProcessMouseButtonUp(
            mouse_button_to_index(evt.get_mouse_button()), key_modifier_state);
        break;
      case wn::runtime::window::event_type::mouse_move:
        m_document_context->ProcessMouseMove(
            static_cast<int>(evt.get_mouse_x()),
            static_cast<int>(evt.get_mouse_y()), key_modifier_state);
        break;
      case wn::runtime::window::event_type::mouse_wheel:
        m_document_context->ProcessMouseWheel(
            static_cast<float>(evt.get_mouse_wheel()), key_modifier_state);
        break;
      case wn::runtime::window::event_type::text_input:
        m_document_context->ProcessTextInput(
            static_cast<Rml::Character>(evt.get_character()));
        break;
    }
  }

  m_renderer->set_setup_command_list(_cmd_list);
  m_renderer->set_render_command_list(nullptr);
  m_renderer->start_frame(static_cast<size_t>(1) << _frame_parity);
  if (static_cast<size_t>(m_width) != m_renderer->get_width() ||
      static_cast<size_t>(m_height) != m_renderer->get_height()) {
    m_width = static_cast<int>(m_renderer->get_width());
    m_height = static_cast<int>(m_renderer->get_height());

    m_document_context->SetDimensions(Rml::Vector2i(m_width, m_height));
    const float dpi = static_cast<float>(m_window->get_dpi());
    m_document_context->SetDensityIndependentPixelRatio(dpi / 96.0f);
  }
  {
    PROFILE_REGION(UIUpdateContext);
    m_document_context->Update();
  }
  // Just make sure this doesnt get kept there
  m_renderer->set_setup_command_list(nullptr);
  m_renderer->set_render_command_list(nullptr);
}

// TODO:Split out render and the rest of it!
void ui::render(renderer::render_pass* _render_pass,
    command_list* _setup_cmd_list, command_list* _cmd_list,
    const descriptor_set* _pass_set) {
  Rml::SetGlobalContextForBlock ctxblk(&m_global_context);
  auto allocator = m_context->m_ui_allocator;
  auto setter =
      m_context->m_application_data->default_job_pool->add_fiber_swap_function(
          functional::function<void()>(allocator, [this]() {
            Rml::GlobalContext::SetGlobalContextForThread(&m_global_context);
          }));
  (void)setter;
  PROFILE_REGION(UIRender);

  (void)_render_pass;
  m_renderer->set_setup_command_list(_setup_cmd_list);
  m_renderer->set_render_command_list(_cmd_list);
  m_renderer->set_renderpass_descriptor_set(_pass_set);
  m_document_context->Render();
  // Just make sure this doesnt get kept there
  m_renderer->set_setup_command_list(nullptr);
  m_renderer->set_render_command_list(nullptr);
  m_renderer->set_renderpass_descriptor_set(nullptr);
}

int32_t ui::add_document(
    scripting::script_actor_pointer<ui_data> _data, int32_t _x, int32_t _y) {
  Rml::SetGlobalContextForBlock ctxblk(&m_global_context);
  auto allocator = m_context->m_ui_allocator;
  auto setter =
      m_context->m_application_data->default_job_pool->add_fiber_swap_function(
          functional::function<void()>(allocator, [this]() {
            Rml::GlobalContext::SetGlobalContextForThread(&m_global_context);
          }));
  (void)setter;
  if (m_document_context) {
    document_to_add a{_x, _y, core::move(_data), ++m_document_idx};
    add_doc(&a);
    return m_document_idx;
  }
  auto doc = memory::make_unique<document_to_add>(m_context->m_ui_allocator);
  doc->_x = _x;
  doc->_y = _y;
  doc->_data = core::move(_data);
  doc->_idx = core::move(++m_document_idx);
  m_documents.push_back(core::move(doc));
  return m_document_idx;
}

void ui::add_doc(document_to_add* _doc) {
  Rml::SetGlobalContextForBlock ctxblk(&m_global_context);
  auto allocator = m_context->m_ui_allocator;
  auto setter =
      m_context->m_application_data->default_job_pool->add_fiber_swap_function(
          functional::function<void()>(allocator, [this]() {
            Rml::GlobalContext::SetGlobalContextForThread(&m_global_context);
          }));
  (void)setter;
  const char* element_name = _doc->_data.invoke(&ui_data::get_ui_name);
  m_instancer->register_currently_loading_doc(core::move(_doc->_data));
  auto document = m_document_context->LoadDocument(element_name);
  m_instancer->add_element_context(
      document, m_instancer->release_currently_loading_doc());

  document->SetProperty(
      Rml::PropertyId::Left, Rml::Property(_doc->_x, Rml::Property::PX));
  document->SetProperty(
      Rml::PropertyId::Top, Rml::Property(_doc->_y, Rml::Property::PX));
  WN_RELEASE_ASSERT(document != NULL,
      "This should not be null, since we ALREADY parsed this once before for "
      "correctness");
  document->Show();
  m_rocket_documents[_doc->_idx] = document;
}

}  // namespace ui
}  // namespace engine
}  // namespace wn
