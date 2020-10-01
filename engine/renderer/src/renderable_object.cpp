// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "renderer/inc/renderable_object.h"
#include "WNScripting/inc/WNEngine.h"

using namespace wn::engine;

namespace wn {
namespace scripting {
template <>
struct exported_script_type<renderer::renderable_object> {
  static containers::string_view exported_name() {
    return "RenderableObject";
  }

  template <typename T>
  static void export_type(T*) {}
};
}  // namespace scripting
namespace engine {
namespace renderer {

void renderable_object::register_scripting(scripting::engine* _engine) {
  _engine->register_cpp_type<renderable_object>();
}

bool renderable_object::resolve_scripting(scripting::engine*) {
  return true;
}

}  // namespace renderer
}  // namespace engine
}  // namespace wn