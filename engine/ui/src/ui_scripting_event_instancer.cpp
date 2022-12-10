// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "ui/inc/ui_scripting_event_instancer.h"
#include "WNScripting/inc/WNEngine.h"
#include "support/inc/string.h"

namespace wn {
namespace {
void ui_set_property(
    Rml::Element* _element, const char* property, const char* value) {
  _element->SetProperty(property, value);
};

scripting::shared_cpp_pointer<support::string> make_string(const char* _str) {
  return scripting::get_scripting_tls()
      ->_engine->make_shared_cpp<support::string>(
          _str, scripting::get_scripting_tls()->_support_allocator);
}

scripting::shared_cpp_pointer<support::string> ui_get_property(
    Rml::Element* _element, const char* _str) {
  auto elem = _element->GetProperty(_str);
  if (!elem) {
    return scripting::shared_cpp_pointer<support::string>();
  }
  return make_string(elem->Get<Rml::String>().c_str());
}

scripting::shared_cpp_pointer<support::string> ui_get_attribute(
    Rml::Element* _element, const char* _str) {
  auto elem = _element->GetAttribute(_str);
  if (!elem) {
    return scripting::shared_cpp_pointer<support::string>();
  }
  return make_string(elem->Get<Rml::String>().c_str());
}

int32_t ui_get_int_attribute(Rml::Element* _element, const char* _str) {
  auto elem = _element->GetAttribute(_str);
  if (!elem) {
    return -1;
  }
  return elem->Get<int>();
}

void ui_set_int_attribute(
    Rml::Element* _element, const char* _str, int32_t val) {
  _element->SetAttribute(_str, static_cast<int>(val));
}

void ui_set_attribute(
    Rml::Element* _element, const char* property, const char* value) {
  _element->SetAttribute(property, value);
};

scripting::shared_cpp_pointer<support::string> ui_get_rml(
    Rml::Element* _element) {
  auto str = _element->GetInnerRML();
  return make_string(str.c_str());
}

void ui_set_rml(Rml::Element* _element, const char* _str) {
  _element->SetInnerRML(_str);
}

Rml::Element* ui_parent(Rml::Element* _element) {
  return _element->GetParentNode();
}

Rml::Element* ui_next_sibling(Rml::Element* _element) {
  return _element->GetNextSibling();
}

int32_t ui_num_children(Rml::Element* _element) {
  return _element->GetNumChildren();
}

Rml::Element* ui_get_child(Rml::Element* _element, int32_t i) {
  return _element->GetChild(i);
}

Rml::Element* ui_get_first_child(Rml::Element* _element) {
  return _element->GetFirstChild();
}

Rml::Element* ui_get_owner(Rml::Element* _element) {
  return reinterpret_cast<Rml::Element*>(_element->GetOwnerDocument());
}

void ui_set_class(Rml::Element* _element, const char* _class, bool _activate) {
  return _element->SetClass(_class, _activate);
}

Rml::Element* ui_get_child_by_id(Rml::Element* _element, const char* id) {
  return _element->GetElementById(id);
}

}  // namespace

namespace scripting {
template <>
struct exported_script_type<Rml::Element> {
  static containers::string_view exported_name() {
    return "UiElement";
  }

  static void export_type(wn::scripting::exporter<Rml::Element>* _exporter) {
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
    _exporter->register_pseudo_function<decltype(&ui_get_child_by_id),
        &ui_get_child_by_id>("get_child_by_id");
  }
};

template <>
struct exported_script_type<Rml::ElementDocument> {
  using parent_type = Rml::Element;
  static containers::string_view exported_name() {
    return "UiDocument";
  }
  static void export_type(wn::scripting::exporter<Rml::ElementDocument>*) {}
};
}  // namespace scripting

namespace engine {
namespace ui {

void event_instancer::register_scripting(
    memory::allocator*, scripting::engine* _engine) {
  _engine->register_cpp_type<Rml::Element>();
  _engine->register_child_cpp_type<Rml::ElementDocument>();
}

event_instancer::event_instancer(scripting::engine* _engine,
    memory::allocator* _allocator, logging::log* _log)
  : m_engine(_engine),
    m_log(_log),
    m_contexts(_allocator),
    m_dirty_documents(_allocator) {}

Rml::EventListener* event_instancer::InstanceEventListener(
    const Rml::String& value, Rml::Element* element) {
  if (m_contexts.find(element->GetOwnerDocument()) == m_contexts.end()) {
    if (m_currently_loading_doc) {
      return new event_listener(
          m_engine, m_log, m_currently_loading_doc, this, value, element);
    } else {
      return nullptr;
    }
  }
  auto& r = m_contexts[element->GetOwnerDocument()];
  return new event_listener(m_engine, m_log, r, this, value, element);
}

event_listener::event_listener(scripting::engine* _engine, logging::log* _log,
    scripting::script_actor_pointer<ui_data> ui_dat,
    event_instancer* _instancer, const Rml::String& code, Rml::Element* element)
  : m_engine(_engine), m_log(_log), m_ui_data(ui_dat), m_instancer(_instancer) {
  (void)element;
  if (code.find("this.") == 0) {
    const char* nm = m_ui_data.invoke(&ui_data::get_class_name);
    _engine->get_named_actor_function(code.c_str() + 5, nm, &m_member_callee);
    if (!m_member_callee) {
      _log->log_error("Could not find member callback function ", code.c_str());
    }
  } else {
    _engine->get_function(code.c_str(), &m_callee);
    if (!m_callee) {
      _log->log_error("Could not find callback function ", code.c_str());
    }
  }
}

event_listener::~event_listener() {}

void event_listener::ProcessEvent(Rml::Event& event) {
  if (m_callee) {
    m_engine->invoke(m_callee, event.GetTargetElement()->GetId().c_str());
    return;
  } else if (m_member_callee) {
    m_engine->invoke(m_member_callee, 0, m_ui_data.unsafe_ptr(),
        event.GetTargetElement()->GetId().c_str());
    m_instancer->dirty_documents().insert(
        event.GetTargetElement()->GetOwnerDocument());
    return;
  }
  m_log->log_warning("Ignoring callback for event: ", event.GetType().c_str());
}

}  // namespace ui
}  // namespace engine
}  // namespace wn