// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "ui/inc/ui_scripting_event_instancer.h"
#include "WNScripting/inc/WNEngine.h"

namespace {
void ui_set_property(
    Rocket::Core::Element* _element, const char* property, const char* value) {
  _element->SetProperty(property, value);
};
}  // namespace

namespace wn {
namespace scripting {
template <>
struct exported_script_type<Rocket::Core::Element> {
  static containers::string_view exported_name() {
    return "UiElement";
  }

  static void export_type(
      wn::scripting::exporter<Rocket::Core::Element>* _exporter) {
    _exporter->register_pseudo_function<decltype(&ui_set_property),
        &ui_set_property>("set_property");
  }
};
}  // namespace scripting

namespace engine {
namespace ui {

void event_instancer::register_scripting(
    memory::allocator*, scripting::engine* _engine) {
  _engine->register_cpp_type<Rocket::Core::Element>();
}

event_instancer::event_instancer(scripting::engine* _engine, logging::log* _log)
  : m_engine(_engine), m_log(_log) {}

Rocket::Core::EventListener* event_instancer::InstanceEventListener(
    const Rocket::Core::String& value, Rocket::Core::Element* element) {
  return new event_listener(m_engine, m_log, value, element);
}

void event_instancer::Release() {
  delete this;
}

event_listener::event_listener(scripting::engine* _engine, logging::log* _log,
    const Rocket::Core::String& code, Rocket::Core::Element* element)
  : m_engine(_engine), m_log(_log) {
  (void)element;
  _engine->get_function(code.CString(), &m_callee);
  if (!m_callee) {
    _log->log_error("Could not find callback function ", code.CString());
  }
}

event_listener::~event_listener() {}

void event_listener::ProcessEvent(Rocket::Core::Event& event) {
  if (m_callee) {
    m_engine->invoke(m_callee, event.GetTargetElement());
    return;
  }
  m_log->log_warning(
      "Ignoring callback for event: ", event.GetType().CString());
}

}  // namespace ui
}  // namespace engine
}  // namespace wn