// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "support/inc/regex.h"
#include "WNScripting/inc/WNFactory.h"

namespace wn {
namespace scripting {
template <>
struct exported_script_type<support::regex> {
  static containers::string_view exported_name() {
    return "Regex";
  }

  static void export_type(wn::scripting::exporter<support::regex>*) {}
};
}  // namespace scripting

namespace support {

scripting::shared_cpp_pointer<regex> make_regex(const char* _re) {
  return scripting::g_scripting_tls->_engine->make_shared_cpp<regex>(_re);
}

void regex::register_scripting(memory::allocator*, scripting::engine* _engine) {
  _engine->register_cpp_type<regex>();
  _engine->register_function<decltype(&make_regex), &make_regex>("make_regex");
}

bool regex::resolve_scripting(scripting::engine*) {
  return true;
}

regex::regex(const char* _re) : m_regex(_re) {}
regex::~regex() {}

}  // namespace support
}  // namespace wn
