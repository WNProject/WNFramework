// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_ENGINE_UI_DATA_H__
#define __WN_ENGINE_UI_DATA_H__

#include "WNScripting/inc/WNEngine.h"
#include "Rocket/Core.h"

namespace wn {
namespace engine_base {
class context;
}

namespace engine {
namespace ui {
struct ui_data : scripting::script_actor_type {
  using parent_type = void;
  scripting::scripting_actor_function<ui_data, const char*> get_ui_name;
  scripting::scripting_actor_function<ui_data, const char*> get_class_name;
  scripting::scripting_virtual_actor_function<ui_data, void, int32_t, Rocket::Core::ElementDocument*>
      update;

  void export_type(
      scripting::engine::script_type_importer<ui_data>* _importer) {
    _importer->register_function("get_name", &get_ui_name);
    _importer->register_function("get_class_name", &get_class_name);
    _importer->register_function("update", &update);
  }

  static wn::containers::string_view exported_name() {
    return "RenderableUI";
  }

  static wn::containers::string_view required_script() {
    return "ui/renderable_ui.wns";
  }

  static void register_scripting(scripting::engine* _engine);
  static bool resolve_scripting(scripting::engine* _engine);
};
}  // namespace ui
}  // namespace engine
}  // namespace wn

#endif  // __WN_ENGINE_UI_DATA_H__
