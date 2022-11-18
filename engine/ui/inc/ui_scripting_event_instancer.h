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
  event_instancer(scripting::engine* _engine, memory::allocator* _allocator,
      logging::log* _log);
  Rocket::Core::EventListener* InstanceEventListener(
      const Rocket::Core::String& value,
      Rocket::Core::Element* element) override;

  void Release() override;

  static void register_scripting(
      memory::allocator*, scripting::engine* _engine);

  void add_element_context(Rocket::Core::ElementDocument* doc,
      scripting::script_actor_pointer<ui_data> data) {
    m_contexts[doc] = core::move(data);
  }

  void register_currently_loading_doc(
      scripting::script_actor_pointer<ui_data> doc) {
    m_currently_loading_doc = core::move(doc);
  }

  scripting::script_actor_pointer<ui_data> release_currently_loading_doc() {
    return core::move(m_currently_loading_doc);
  }

  scripting::script_pointer<ui_data> get_element_context(
      Rocket::Core::ElementDocument* doc) {
    return m_contexts[doc].get();
  }

  containers::hash_set<Rocket::Core::ElementDocument*>& dirty_documents() {
    return m_dirty_documents;
  }

private:
  scripting::engine* m_engine;
  logging::log* m_log;
  containers::hash_map<Rocket::Core::ElementDocument*,
      scripting::script_actor_pointer<ui_data>>
      m_contexts;
  scripting::script_actor_pointer<ui_data> m_currently_loading_doc;
  containers::hash_set<Rocket::Core::ElementDocument*> m_dirty_documents;
};

class event_listener : public Rocket::Core::EventListener {
public:
  event_listener(scripting::engine* _engine, logging::log* _log,
      scripting::script_actor_pointer<ui_data> ui_dat,
      event_instancer* _instancer, const Rocket::Core::String& code,
      Rocket::Core::Element* element);

  ~event_listener();

  /// Process the incoming Event
  void ProcessEvent(Rocket::Core::Event& event) override;

private:
  scripting::engine* m_engine;
  logging::log* m_log;
  scripting::script_actor_pointer<ui_data> m_ui_data;
  event_instancer* m_instancer;
  wn::scripting::script_function<void, const char*> m_callee;
  wn::scripting::script_function<void, int32_t, void*, const char*>
      m_member_callee;
};

}  // namespace ui
}  // namespace engine
}  // namespace wn
#endif