// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_ENGINE_RENDER_TARGET_H__
#define __WN_ENGINE_RENDER_TARGET_H__

#include "WNScripting/inc/WNEngine.h"
namespace wn {
namespace engine {
namespace renderer {
struct rt_description : scripting::script_object_type {
  using parent_type = void;

  void export_type(
      scripting::engine::script_type_importer<rt_description>* _importer) {
    _importer->register_function("get_name", &get_name);
    _importer->register_function("get_width", &get_width);
    _importer->register_function("get_height", &get_height);
    _importer->register_function("get_format", &get_format);
  }

  static wn::containers::string_view exported_name() {
    return "RTDescription";
  }

  scripting::scripting_object_function<rt_description, const char*> get_name;
  scripting::scripting_object_function<rt_description, int32_t> get_width;
  scripting::scripting_object_function<rt_description, int32_t> get_height;
  scripting::scripting_object_function<rt_description, int32_t> get_format;

  static void register_scripting(scripting::engine* _engine);
  static bool resolve_scripting(scripting::engine* _engine);
};
}  // namespace renderer
}  // namespace engine
}  // namespace wn

#endif  // __WN_ENGINE_RENDER_TARGET_H__