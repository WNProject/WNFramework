// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "ui/inc/resource_manager.h"

#include "WNFileSystem/inc/WNMapping.h"
#include "WNScripting/inc/WNEngine.h"
#include "WNScripting/inc/forward.h"
#include "ui/inc/ui_data.h"
#include "ui/inc/ui_scripting_parser.h"

namespace wn {
namespace engine {
namespace ui {

namespace {
struct ui_resource_userdata {
  memory::allocator* m_allocator;
  containers::string m_ui_name;
  scripting::engine* m_engine;
};
}  // namespace

void resource_manager::register_scripting(
    memory::allocator* _allocator, scripting::engine* _engine) {
  _engine->register_resource<scripting::shared_script_pointer<ui_data>>(
      memory::make_unique<resource_manager>(_allocator, _engine, _allocator));
}

bool resource_manager::resolve_scripting(scripting::engine*) {
  return true;
}

resource_manager::resource_manager(
    scripting::engine* _engine, memory::allocator* _allocator)
  : scripting::resource_manager(containers::string(_allocator, "UIResource")),
    m_allocator(_allocator),
    m_engine(_engine),
    m_ui_names(_allocator) {}

bool resource_manager::convert_to_function(
    containers::string_view _resource_name, containers::string* _dat) {
  auto it = m_ui_names.find(_resource_name.to_string(m_allocator));
  if (it == m_ui_names.end()) {
    return false;
  }
  _dat->append("getNew").append(it->second);
  return true;
}

bool resource_manager::get_include_for_resource(
    containers::string_view _res, containers::string* _out) {
  *_out = _res;
  return true;
}

scripting::convert_type resource_manager::convert_file(logging::log* _log,
    file_system::mapping* _mapping, containers::string_view _file_name,
    containers::string* _out_string) {
  containers::string _ui_name(m_allocator);
  ui_scripting_parser parser(_log, m_allocator);
  if (!parser.parse_ui(_mapping, _file_name, _out_string, &_ui_name)) {
    return scripting::convert_type::failed;
  }
  m_ui_names[_file_name.to_string(m_allocator)] = core::move(_ui_name);

  return scripting::convert_type::success;
}

}  // namespace ui
}  // namespace engine
}  // namespace wn