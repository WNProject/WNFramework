// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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

regex* get_regex(void* st) {
  return reinterpret_cast<regex*>(st);
}

void regex_manager::register_scripting(
    memory::allocator* _allocator, scripting::engine* _engine) {
  _engine->register_cpp_type<regex>();
  _engine->register_function<decltype(&get_regex), &get_regex>("_get_regex");
  _engine->register_resource<regex*>(
      memory::make_unique<regex_manager>(_allocator, _allocator));
}

bool regex_manager::resolve_scripting(scripting::engine*) {
  return true;
}

bool regex_manager::convert_to_function(containers::string_view _resource_data,
    containers::string_view, logging::log*, containers::string* _dat,
    core::optional<uintptr_t>* _user_data) {
  *_dat = "_get_regex";
  containers::string str = _resource_data.to_string(m_allocator);
  auto it = m_regexes.find(str);
  if (it == m_regexes.end()) {
    memory::unique_ptr<regex> up =
        memory::make_unique<regex>(m_allocator, str.c_str());
    if (!up->is_valid()) {
      return false;
    }
    auto cp =
        m_regexes.insert(core::make_pair(core::move(str), core::move(up)));
    it = cp.first;
    WN_DEBUG_ASSERT(cp.second, "Unexpected failure in hash map insertion");
  }
  _user_data->emplace(reinterpret_cast<uintptr_t>(it->second.get()));
  return true;
}

regex_manager::regex_manager(memory::allocator* _allocator)
  : scripting::resource_manager(containers::string(_allocator, "Regex")),
    m_allocator(_allocator),
    m_regexes(_allocator) {}

regex::regex(const char* _re) : m_regex(_re, RE2::Quiet) {}
regex::~regex() {}
bool regex::is_valid() const {
  return m_regex.ok();
}

}  // namespace support
}  // namespace wn
