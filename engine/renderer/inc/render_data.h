// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_ENGINE_RENDER_DATA_H__
#define __WN_ENGINE_RENDER_DATA_H__

#include "WNScripting/inc/WNEngine.h"

namespace wn {
namespace engine_base {
class context;
}
namespace engine {
namespace renderer {

struct render_data : scripting::script_object_type {
  using parent_type = void;

  void export_type(
      scripting::engine::script_type_importer<render_data>* _importer) {
    _importer->register_function("register_data", &register_data);
  }

  static wn::containers::string_view exported_name() {
    return "RenderData";
  }

  scripting::scripting_virtual_object_function<render_data, int32_t,
      engine_base::context*>
      register_data;

  static void register_scripting(scripting::engine* _engine);
  static void finalize_scripting(scripting::engine* _engine);
};

}  // namespace renderer
}  // namespace engine
}  // namespace wn

#endif  // __WN_ENGINE_RENDERER_RENDERER_H__