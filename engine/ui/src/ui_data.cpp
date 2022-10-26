// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/inc/ui_data.h"

#include "WNScripting/inc/WNEngine.h"

namespace wn {
namespace engine {
namespace ui {

void ui_data::register_scripting(scripting::engine* _engine) {
  _engine->export_script_actor_type<ui_data>();
}

bool ui_data::resolve_scripting(scripting::engine* _engine) {
  return _engine->resolve_script_actor_type<ui_data>();
}

}  // namespace ui
}  // namespace engine
}  // namespace wn
