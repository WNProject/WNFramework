// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "renderer/inc/render_data.h"

#include "WNScripting/inc/WNEngine.h"

using namespace wn::engine;
using namespace wn;

namespace wn {
namespace engine {
namespace renderer {
void render_data::register_scripting(scripting::engine* _engine) {
  _engine->export_script_type<render_data>();
}

bool render_data::resolve_scripting(scripting::engine* _engine) {
  return _engine->resolve_script_type<render_data>();
}

void render_target_usage::register_scripting(scripting::engine* _engine) {
  _engine->export_script_type<render_target_usage>();
}

bool render_target_usage::resolve_scripting(scripting::engine* _engine) {
  return _engine->resolve_script_type<render_target_usage>();
}

void pass_data::register_scripting(scripting::engine* _engine) {
  _engine->export_script_type<pass_data>();
}

bool pass_data::resolve_scripting(scripting::engine* _engine) {
  return _engine->resolve_script_type<pass_data>();
}

void render_dependency::register_scripting(scripting::engine* _engine) {
  _engine->export_script_type<render_dependency>();
}

bool render_dependency::resolve_scripting(scripting::engine* _engine) {
  return _engine->resolve_script_type<render_dependency>();
}

}  // namespace renderer
}  // namespace engine
}  // namespace wn
