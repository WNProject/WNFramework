// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "ui/inc/resource.h"

#include "WNFileSystem/inc/WNMapping.h"
#include "WNScripting/inc/WNEngine.h"

namespace wn {
namespace engine {
namespace ui {

int32_t get_resource(void* user_data) {
  (void)user_data;
  return 42;
}

void resource::register_scripting(memory::allocator* _allocator,
    scripting::engine* _engine, file_system::mapping* _mapping) {
  _engine->register_resource<decltype(&get_resource), &get_resource>(
      memory::make_unique<resource>(_allocator, _allocator, _mapping));
}

bool resource::resolve_scripting(scripting::engine*) {
  return true;
}

resource::resource(
    memory::allocator* _allocator, file_system::mapping* _mapping)
  : scripting::resource(containers::string(_allocator, "UIResource")),
    m_allocator(_allocator),
    m_mapping(_mapping) {}

bool resource::convert_to_user_data(
    containers::string_view _resource_name, void** _dat) {
  (void)_resource_name;
  (void)_dat;
  return true;
}

bool resource::get_include_for_resource(
    containers::string_view _res, containers::string* _out) {
  *_out = _res.substr(1, _res.size() - 2);
  return true;
}

scripting::convert_type resource::convert_file(
    containers::string_view _file_name, containers::string* _out_string) {
  file_system::result res;
  file_system::file_ptr file = m_mapping->open_file(_file_name, res);
  if (!file) {
    return scripting::convert_type::failed;
  }
  (void)_out_string;

  return scripting::convert_type::success;
}

}  // namespace ui
}  // namespace engine
}  // namespace wn