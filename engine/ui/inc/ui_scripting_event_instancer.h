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

namespace wn {
namespace engine {
namespace ui {

class event_instancer : public Rocket::Core::EventListenerInstancer {
public:
  event_instancer(scripting::engine* _engine, logging::log* _log);
  Rocket::Core::EventListener* InstanceEventListener(
      const Rocket::Core::String& value,
      Rocket::Core::Element* element) override;

  void Release() override;

  static void register_scripting(
      memory::allocator*, scripting::engine* _engine);

private:
  scripting::engine* m_engine;
  logging::log* m_log;
};

class event_listener : public Rocket::Core::EventListener {
public:
  event_listener(scripting::engine* _engine, logging::log* _log,
      const Rocket::Core::String& code, Rocket::Core::Element* element);

  ~event_listener();

  /// Process the incoming Event
  void ProcessEvent(Rocket::Core::Event& event) override;

private:
  scripting::engine* m_engine;
  logging::log* m_log;
  wn::scripting::script_function<void, Rocket::Core::Element*> m_callee;
};

}  // namespace ui
}  // namespace engine
}  // namespace wn
#endif