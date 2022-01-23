// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "support/inc/string.h"
#include "WNScripting/inc/WNFactory.h"

namespace wn {
namespace scripting {
template <>
struct exported_script_type<support::string> {
  static containers::string_view exported_name() {
    return "String";
  }

  static void export_type(wn::scripting::exporter<support::string>* _exporter) {
    _exporter->register_nonvirtual_function<decltype(&support::string::c_str),
        &support::string::c_str>("c_str");
  }
};
}  // namespace scripting

namespace support {

scripting::shared_cpp_pointer<string> make_string(const char* _re) {
  return scripting::get_scripting_tls()->_engine->make_shared_cpp<string>(
      _re, scripting::get_scripting_tls()->_support_allocator);
}

void string::register_scripting(
    memory::allocator*, scripting::engine* _engine) {
  _engine->register_cpp_type<string>();
  _engine->register_function<decltype(&make_string), &make_string>(
      "make_string");
}

bool string::resolve_scripting(scripting::engine*) {
  return true;
}

string::string(const char* _re, memory::allocator* _alloc)
  : m_string(_alloc, _re) {}
string::~string() {}

}  // namespace support
}  // namespace wn
