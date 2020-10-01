// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "ui/inc/ui.h"

#include "WNScripting/inc/WNEngine.h"
#include "ui/inc/resource.h"

using namespace wn::engine;
namespace wn {
namespace scripting {

template <>
struct exported_script_type<ui::ui> {
  using parent_type = renderer::renderable_object;
  static containers::string_view exported_name() {
    return "UI";
  }

  template <typename T>
  static void export_type(T*) {}
};
}  // namespace scripting

namespace engine {
namespace ui {

void ui::render() {}

void ui::register_scripting(memory::allocator* _allocator,
    scripting::engine* _engine, file_system::mapping* _mapping) {
  _engine->register_cpp_type<ui>();
  resource::register_scripting(_allocator, _engine, _mapping);
}

bool ui::resolve_scripting(scripting::engine* _engine) {
  return resource::resolve_scripting(_engine);
}

}  // namespace ui
}  // namespace engine
}  // namespace wn