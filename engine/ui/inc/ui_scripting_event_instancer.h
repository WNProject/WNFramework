// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_ENGINE_UI_ROCKET_EVENT_INSTANCER_H__
#define __WN_ENGINE_UI_ROCKET_EVENT_INSTANCER_H__

#include "Rocket/Core/EventListener.h"
#include "Rocket/Core/EventListenerInstancer.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/allocator.h"
#include "WNScripting/inc/WNEngine.h"
#include "ui/inc/ui_data.h"

namespace wn {
namespace engine {
namespace ui {

class event_instancer : public Rocket::Core::EventListenerInstancer {
public:
  event_instancer(scripting::engine* _engine, memory::allocator* _allocator, logging::log* _log);
  Rocket::Core::EventListener* InstanceEventListener(
      const Rocket::Core::String& value,
      Rocket::Core::Element* element) override;

  void Release() override;

  static void register_scripting(
      memory::allocator*, scripting::engine* _engine);

  void add_element_context(Rocket::Core::ElementDocument* doc,
    scripting::shared_script_pointer<ui_data> data) {
    m_contexts[doc] = core::move(data);
  }

  void register_currently_loading_doc(
    scripting::shared_script_pointer<ui_data> doc) {
    m_currently_loading_doc = core::move(doc);
  }

  scripting::shared_script_pointer<ui_data> release_currently_loading_doc() {
    return core::move(m_currently_loading_doc);
  }

private:
  scripting::engine* m_engine;
  logging::log* m_log;
  containers::hash_map<Rocket::Core::ElementDocument*,
      scripting::shared_script_pointer<ui_data>>
      m_contexts;
  scripting::shared_script_pointer<ui_data> m_currently_loading_doc;
};

class event_listener : public Rocket::Core::EventListener {
public:
  event_listener(scripting::engine* _engine, logging::log* _log,
      scripting::shared_script_pointer<ui_data> ui_dat,
      const Rocket::Core::String& code, Rocket::Core::Element* element);

  ~event_listener();

  /// Process the incoming Event
  void ProcessEvent(Rocket::Core::Event& event) override;

private:
  scripting::engine* m_engine;
  logging::log* m_log;
  scripting::shared_script_pointer<ui_data> m_ui_data;
  wn::scripting::script_function<void, Rocket::Core::Element*> m_callee;
  wn::scripting::script_function<void, void*, Rocket::Core::Element*>
      m_member_callee;
};

}  // namespace ui
}  // namespace engine
}  // namespace wn
#endif