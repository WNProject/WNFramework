// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "ui/inc/ui_scripting_event_instancer.h"
#include "WNScripting/inc/WNEngine.h"
#include "support/inc/string.h"

namespace wn {
namespace {
void ui_set_property(
    Rocket::Core::Element* _element, const char* property, const char* value) {
  _element->SetProperty(property, value);
};

scripting::shared_cpp_pointer<support::string> make_string(const char* _str) {
  return scripting::get_scripting_tls()
      ->_engine->make_shared_cpp<support::string>(
          _str, scripting::get_scripting_tls()->_support_allocator);
}

scripting::shared_cpp_pointer<support::string> ui_get_property(
    Rocket::Core::Element* _element, const char* _str) {
  auto elem = _element->GetProperty(_str);
  if (!elem) {
    return scripting::shared_cpp_pointer<support::string>();
  }
  return make_string(elem->Get<Rocket::Core::String>().CString());
}

scripting::shared_cpp_pointer<support::string> ui_get_attribute(
    Rocket::Core::Element* _element, const char* _str) {
  auto elem = _element->GetAttribute(_str);
  if (!elem) {
    return scripting::shared_cpp_pointer<support::string>();
  }
  return make_string(elem->Get<Rocket::Core::String>().CString());
}

int32_t ui_get_int_attribute(
    Rocket::Core::Element* _element, const char* _str) {
  auto elem = _element->GetAttribute(_str);
  if (!elem) {
    return -1;
  }
  return elem->Get<int>();
}

void ui_set_int_attribute(
    Rocket::Core::Element* _element, const char* _str, int32_t val) {
  _element->SetAttribute(_str, static_cast<int>(val));
}

void ui_set_attribute(
    Rocket::Core::Element* _element, const char* property, const char* value) {
  _element->SetAttribute(property, value);
};

scripting::shared_cpp_pointer<support::string> ui_get_rml(
    Rocket::Core::Element* _element) {
  auto str = _element->GetInnerRML();
  return make_string(str.CString());
}

void ui_set_rml(Rocket::Core::Element* _element, const char* _str) {
  _element->SetInnerRML(_str);
}

Rocket::Core::Element* ui_parent(Rocket::Core::Element* _element) {
  return _element->GetParentNode();
}

Rocket::Core::Element* ui_next_sibling(Rocket::Core::Element* _element) {
  return _element->GetNextSibling();
}

int32_t ui_num_children(Rocket::Core::Element* _element) {
  return _element->GetNumChildren();
}

Rocket::Core::Element* ui_get_child(
    Rocket::Core::Element* _element, int32_t i) {
  return _element->GetChild(i);
}

Rocket::Core::Element* ui_get_first_child(Rocket::Core::Element* _element) {
  return _element->GetFirstChild();
}

Rocket::Core::Element* ui_get_owner(Rocket::Core::Element* _element) {
  return reinterpret_cast<Rocket::Core::Element*>(_element->GetOwnerDocument());
}

void ui_set_class(
    Rocket::Core::Element* _element, const char* _class, bool _activate) {
  return _element->SetClass(_class, _activate);
}

}  // namespace

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
    _exporter->register_pseudo_function<decltype(&ui_get_property),
        &ui_get_property>("get_property");
    _exporter->register_pseudo_function<decltype(&ui_set_attribute),
        &ui_set_attribute>("set_attribute");
    _exporter->register_pseudo_function<decltype(&ui_set_int_attribute),
        &ui_set_int_attribute>("set_attribute");
    _exporter->register_pseudo_function<decltype(&ui_get_attribute),
        &ui_get_attribute>("get_attribute");
    _exporter->register_pseudo_function<decltype(&ui_get_int_attribute),
        &ui_get_int_attribute>("get_int_attribute");
    _exporter->register_pseudo_function<decltype(&ui_get_rml), &ui_get_rml>(
        "get_inner_rml");
    _exporter->register_pseudo_function<decltype(&ui_set_rml), &ui_set_rml>(
        "set_inner_rml");
    _exporter->register_pseudo_function<decltype(&ui_parent), &ui_parent>(
        "parent");
    _exporter->register_pseudo_function<decltype(&ui_next_sibling),
        &ui_next_sibling>("next_sibling");
    _exporter->register_pseudo_function<decltype(&ui_num_children),
        &ui_num_children>("num_children");
    _exporter->register_pseudo_function<decltype(&ui_get_child), &ui_get_child>(
        "get_child");
    _exporter->register_pseudo_function<decltype(&ui_get_first_child),
        &ui_get_first_child>("get_first_child");
    _exporter->register_pseudo_function<decltype(&ui_get_owner), &ui_get_owner>(
        "owner");
    _exporter->register_pseudo_function<decltype(&ui_set_class), &ui_set_class>(
        "set_class");
  }
};

template <>
struct exported_script_type<Rocket::Core::ElementDocument> {
  using parent_type = Rocket::Core::Element;
  static containers::string_view exported_name() {
    return "UiDocument";
  }
  static void export_type(
      wn::scripting::exporter<Rocket::Core::ElementDocument>*) {}
};
}  // namespace scripting

namespace engine {
namespace ui {

void event_instancer::register_scripting(
    memory::allocator*, scripting::engine* _engine) {
  _engine->register_cpp_type<Rocket::Core::Element>();
  _engine->register_child_cpp_type<Rocket::Core::ElementDocument>();
}

event_instancer::event_instancer(scripting::engine* _engine,
    memory::allocator* _allocator, logging::log* _log)
  : m_engine(_engine), m_log(_log), m_contexts(_allocator) {}

Rocket::Core::EventListener* event_instancer::InstanceEventListener(
    const Rocket::Core::String& value, Rocket::Core::Element* element) {
  if (m_contexts.find(element->GetOwnerDocument()) == m_contexts.end()) {
    return new event_listener(
        m_engine, m_log, m_currently_loading_doc, value, element);
  }
  auto& r = m_contexts[element->GetOwnerDocument()];
  return new event_listener(m_engine, m_log, r, value, element);
}

void event_instancer::Release() {
  delete this;
}

event_listener::event_listener(scripting::engine* _engine, logging::log* _log,
    scripting::script_actor_pointer<ui_data> ui_dat,
    const Rocket::Core::String& code, Rocket::Core::Element* element)
  : m_engine(_engine), m_log(_log), m_ui_data(ui_dat) {
  (void)element;
  if (code.Find("this.") == 0) {
    const char* nm = m_ui_data.invoke(&ui_data::get_class_name);
    _engine->get_named_member_function(
        code.CString() + 5, nm, &m_member_callee);
    if (!m_member_callee) {
      _log->log_error(
          "Could not find member callback function ", code.CString());
    }
  } else {
    _engine->get_function(code.CString(), &m_callee);
    if (!m_callee) {
      _log->log_error("Could not find callback function ", code.CString());
    }
  }
}

event_listener::~event_listener() {}

void event_listener::ProcessEvent(Rocket::Core::Event& event) {
  if (m_callee) {
    m_engine->invoke(m_callee, event.GetTargetElement());
    return;
  } else if (m_member_callee) {
    m_engine->invoke(
        m_member_callee, m_ui_data.unsafe_ptr(), event.GetTargetElement());
    return;
  }
  m_log->log_warning(
      "Ignoring callback for event: ", event.GetType().CString());
}

}  // namespace ui
}  // namespace engine
}  // namespace wn