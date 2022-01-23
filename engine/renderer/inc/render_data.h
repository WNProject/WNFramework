// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_ENGINE_RENDER_DATA_H__
#define __WN_ENGINE_RENDER_DATA_H__

#include "WNScripting/inc/WNEngine.h"
#include "renderer/inc/render_target.h"

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

  static wn::containers::string_view required_script() {
    return "renderer/render_data.wns";
  }

  scripting::scripting_virtual_object_function<render_data, void> register_data;

  static void register_scripting(scripting::engine* _engine);
  static bool resolve_scripting(scripting::engine* _engine);
};

struct render_dependency : scripting::script_object_type {
  using parent_type = void;

  void export_type(
      scripting::engine::script_type_importer<render_dependency>* _importer) {
    _importer->register_function("get_render_target", &get_render_target);
    _importer->register_function("get_frame_offset", &get_frame_offset);
  }

  static wn::containers::string_view exported_name() {
    return "RenderDependency";
  }

  static void register_scripting(scripting::engine* _engine);
  static bool resolve_scripting(scripting::engine* _engine);

  static wn::containers::string_view required_script() {
    return "renderer/pass_data.wns";
  }

  scripting::scripting_object_function<render_dependency, int32_t>
      get_render_target;
  scripting::scripting_object_function<render_dependency, int32_t>
      get_frame_offset;
};

struct render_target_usage : scripting::script_object_type {
  using parent_type = void;
  void export_type(
      scripting::engine::script_type_importer<render_target_usage>* _importer) {
    _importer->register_function("get_render_target", &get_render_target);
    _importer->register_function("get_clear_mode", &get_clear_mode);
    _importer->register_function("get_store_mode", &get_store_mode);
  }

  static wn::containers::string_view exported_name() {
    return "RenderTargetUsage";
  }

  static void register_scripting(scripting::engine* _engine);
  static bool resolve_scripting(scripting::engine* _engine);

  static wn::containers::string_view required_script() {
    return "renderer/pass_data.wns";
  }

  scripting::scripting_object_function<render_target_usage, int32_t>
      get_render_target;
  scripting::scripting_object_function<render_target_usage, int32_t>
      get_clear_mode;
  scripting::scripting_object_function<render_target_usage, int32_t>
      get_store_mode;
};

struct pass_data : scripting::script_object_type {
  using parent_type = void;

  void export_type(
      scripting::engine::script_type_importer<pass_data>* _importer) {
    _importer->register_function("get_pass_name", &get_pass_name);
    _importer->register_function("get_render_targets", &get_render_targets);
    _importer->register_function("get_depth_target", &get_depth_target);
    _importer->register_function(
        "get_render_dependencies", &get_render_dependencies);
  }

  static wn::containers::string_view exported_name() {
    return "PassData";
  }

  static wn::containers::string_view required_script() {
    return "renderer/pass_data.wns";
  }

  scripting::scripting_object_function<pass_data, const char*> get_pass_name;
  scripting::scripting_object_function<pass_data,
      scripting::wn_array<scripting::script_pointer<render_target_usage>>>
      get_render_targets;
  scripting::scripting_object_function<pass_data,
      scripting::script_pointer<render_target_usage>>
      get_depth_target;
  scripting::scripting_object_function<pass_data,
      scripting::wn_array<scripting::script_pointer<render_dependency>>>
      get_render_dependencies;

  static void register_scripting(scripting::engine* _engine);
  static bool resolve_scripting(scripting::engine* _engine);
};

struct render_description : scripting::script_object_type {
  using parent_type = void;
  static wn::containers::string_view exported_name() {
    return "RenderDescription";
  }

  static wn::containers::string_view required_script() {
    return "renderer/render_description.wns";
  }

  void export_type(
      scripting::engine::script_type_importer<render_description>* _importer) {
    _importer->register_function("get_render_targets", &get_render_targets);
    _importer->register_function("get_passes", &get_passes);
    _importer->register_function("get_output_target", &get_output_target);
  }

  wn::scripting::scripting_object_function<render_description,
      scripting::wn_array<scripting::shared_script_pointer<rt_description>>>
      get_render_targets;
  wn::scripting::scripting_object_function<render_description,
      scripting::wn_array<scripting::script_pointer<pass_data>>>
      get_passes;
  wn::scripting::scripting_object_function<render_description, int32_t>
      get_output_target;
};

}  // namespace renderer
}  // namespace engine
}  // namespace wn

#endif  // __WN_ENGINE_RENDERER_RENDERER_H__
