// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "renderer/inc/render_target.h"
#include "WNGraphics/inc/WNGraphicsEnums.h"

using namespace wn::engine;
using wn::scripting::script_object_type;
using wn::scripting::shared_cpp_pointer;

using namespace wn;

namespace wn {
namespace engine {
namespace renderer {

void rt_description::register_scripting(scripting::engine* _engine) {
  _engine->export_script_type<rt_description>();
}

bool rt_description::resolve_scripting(scripting::engine* _engine) {
  return _engine->resolve_script_type<rt_description>();
}

}  // namespace renderer
}  // namespace engine
}  // namespace wn
