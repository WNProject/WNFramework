// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "renderer/inc/texture_manager.h"
#include "WNFileSystem/inc/WNFile.h"
#include "WNFileSystem/inc/WNMapping.h"
#include "WNScripting/inc/WNEngine.h"
#include "WNScripting/inc/forward.h"
#include "renderer/inc/texture.h"

namespace wn {
namespace engine {
namespace renderer {

void texture_manager::register_scripting(
    memory::allocator* _allocator, scripting::engine* _engine) {
  _engine->export_script_type<texture_desc>();
  _engine->register_resource<scripting::shared_script_pointer<texture_desc>>(
      memory::make_unique<texture_manager>(_allocator, _engine, _allocator));
}

bool texture_manager::resolve_scripting(scripting::engine* _engine) {
  return _engine->resolve_script_type<texture_desc>();
}

texture_manager::texture_manager(
    scripting::engine* _engine, memory::allocator* _allocator)
  : scripting::resource_manager(containers::string(_allocator, "Texture")),
    m_allocator(_allocator),
    m_engine(_engine) {}

bool texture_manager::convert_to_function(
    containers::string_view _resource_name,
    containers::string_view _resource_data, logging::log*,
    containers::string* _dat, core::optional<uintptr_t>* _user_data) {
  (void)_resource_name;
  (void)_user_data;
  *_dat = "getNew";
  *_dat += _resource_data;
  return true;
}

bool texture_manager::setup_resource(
    containers::string_view _res, containers::string* _out) {
  *_out = _res;
  return true;
}

scripting::convert_type texture_manager::convert_file(logging::log* _log,
    file_system::mapping* _mapping, containers::string_view _file_name,
    containers::string* _out_data, containers::string* _out_string) {
  file_system::result res = file_system::result::ok;
  file_system::file_ptr file = _mapping->open_file(_file_name, res);
  if (res != file_system::result::ok) {
    return scripting::convert_type::failed;
  }

  if (0 == file->size()) {
    _log->log_error("File is empty");
    return scripting::convert_type::failed;
  }

  containers::string_view v(file->typed_data<char>(), file->typed_size<char>());
  containers::string_view texture_name;
  size_t line_begin = 0;
  size_t line_end = v.find_first_of("\r\n");
  while (line_begin != containers::string_view::npos) {
    containers::string_view line = v.substr(line_begin, line_end - line_begin);
    line = line.substr(line.find_first_not_of(" \t"));
    if (line.starts_with("//")) {
      line_begin = line_end;
      line_end = v.find_first_of("\r\n");
      continue;
    }
    texture_name = line.substr(0, line.find_first_of(" \t{"));
    break;
  }
  if (texture_name.empty()) {
    _log->log_error("Could not find texture name in texture file");
  }

  *_out_string = "include \"renderer/texture_description.wns\"\n\n";
  *_out_string += v.substr(0, texture_name.data() - v.data());
  *_out_string += "class ";
  *_out_string += texture_name;
  *_out_string += " : TextureDescription";
  *_out_string +=
      v.substr(texture_name.data() - v.data() + texture_name.size());
  *_out_string += "shared TextureDescription";
  *_out_string += " getNew";
  *_out_string += texture_name;
  *_out_string += "Shared() { shared TextureDescription";
  *_out_string += " x = shared ";
  *_out_string += texture_name;
  *_out_string += "();\n return x; }\n";

  *_out_string += texture_name;
  *_out_string += " getNew";
  *_out_string += texture_name;
  *_out_string += "() { ";
  *_out_string += texture_name;
  *_out_string += " x = ";
  *_out_string += texture_name;
  *_out_string += "();\n return x; }\n";

  *_out_data = texture_name;
  return scripting::convert_type::success;
}

}  // namespace renderer
}  // namespace engine
}  // namespace wn