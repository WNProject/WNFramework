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
    _exporter->register_nonvirtual_function<decltype(&support::string::split),
        &support::string::split>("split");
    _exporter->register_nonvirtual_function<
        decltype(&support::string::next_split), &support::string::next_split>(
        "next_split");
  }
};

template <>
struct exported_script_type<support::splits> {
  static containers::string_view exported_name() {
    return "Split";
  }

  static void export_type(wn::scripting::exporter<support::splits>* _exporter) {
    _exporter->register_nonvirtual_function<
        decltype(&support::splits::split_split), &support::splits::split_split>(
        "split_split");
    _exporter->register_nonvirtual_function<
        decltype(&support::splits::get_split_i), &support::splits::get_split_i>(
        "get_split");
    _exporter->register_nonvirtual_function<
        decltype(&support::splits::get_split_s), &support::splits::get_split_s>(
        "get_split");
    _exporter->register_nonvirtual_function<
        decltype(&support::splits::num_splits), &support::splits::num_splits>(
        "num_splits");
  }
};
}  // namespace scripting

namespace support {

scripting::slice<uint8_t> slice_split(
    scripting::slice<uint8_t> main, const char* split_chars) {
  auto data_as_u8 = main.begin();
  containers::string_view sv(
      reinterpret_cast<const char*>(data_as_u8), main.size());

  auto b = sv.find_first_not_of(split_chars);
  if (b == containers::string::npos) {
    return scripting::slice<uint8_t>(nullptr, {0});
  }

  auto n = sv.find_first_of(split_chars, b);
  if (n == containers::string::npos) {
    n = sv.size();
  }
  return scripting::slice<uint8_t>(data_as_u8 + b, {n - b});
}

scripting::slice<uint8_t> slice_next_split(scripting::slice<uint8_t> main,
    scripting::slice<uint8_t> first, const char* split_chars) {
  auto data_as_u8 = main.begin();
  if (first.begin() < data_as_u8 || first.end() > data_as_u8 + main.size()) {
    return scripting::slice<uint8_t>(nullptr, {0});
  }

  containers::string_view sv(
      reinterpret_cast<const char*>(data_as_u8), main.size());
  auto b = sv.find_first_not_of(split_chars, first.end() - data_as_u8);
  if (b == containers::string::npos) {
    return scripting::slice<uint8_t>(nullptr, {0});
  }

  auto n = sv.find_first_of(split_chars, b);
  if (n == containers::string::npos) {
    n = sv.size();
  }
  return scripting::slice<uint8_t>(data_as_u8 + b, {n - b});
}

scripting::shared_cpp_pointer<string> make_string(const char* _re) {
  return scripting::get_scripting_tls()->_engine->make_shared_cpp<string>(
      _re, scripting::get_scripting_tls()->_support_allocator);
}

scripting::shared_cpp_pointer<splits> split_string(
    scripting::shared_cpp_pointer<string> str, const char* split_chars) {
  auto s = scripting::get_scripting_tls()->_engine->make_shared_cpp<splits>(
      scripting::get_scripting_tls()->_support_allocator);
  auto splt = str->split(split_chars);
  while (splt.size() > 0) {
    s->m_splits.push_back(splt);
    splt = str->next_split(splt, split_chars);
  }
  s->m_base_string = str;
  return s;
}

splits::splits(memory::allocator* _alloc) : m_splits(_alloc) {}
splits::~splits() {}

scripting::shared_cpp_pointer<splits> splits::split_split(
    int32_t idx, const char* split_chars) {
  size_t i = static_cast<size_t>(idx);
  if (i > m_splits.size()) {
    return scripting::shared_cpp_pointer<splits>();
  }
  auto s = scripting::get_scripting_tls()->_engine->make_shared_cpp<splits>(
      scripting::get_scripting_tls()->_support_allocator);
  auto splt = slice_split(m_splits[i], split_chars);
  while (splt.size() > 0) {
    s->m_splits.push_back(splt);
    splt = slice_next_split(m_splits[i], splt, split_chars);
  }
  s->m_base_string = m_base_string;
  return s;
}

void string::register_scripting(
    memory::allocator*, scripting::engine* _engine) {
  _engine->register_cpp_type<string>();

  _engine->register_function<decltype(&make_string), &make_string>(
      "make_string");
  _engine->register_function<decltype(&slice_split), &slice_split>(
      "slice_split");
  _engine->register_function<decltype(&slice_next_split), &slice_next_split>(
      "slice_next_split");
  _engine->register_cpp_type<splits>();
  _engine->register_function<decltype(&split_string), &split_string>(
      "split_string");
}

bool string::resolve_scripting(scripting::engine*) {
  return true;
}

string::string(const char* _re, memory::allocator* _alloc)
  : m_string(_alloc, _re) {}
string::~string() {}

scripting::slice<uint8_t> string::split(const char* split_chars) {
  auto data_as_u8 = reinterpret_cast<uint8_t*>(m_string.data());
  auto b = m_string.find_first_not_of(split_chars);
  if (b == containers::string::npos) {
    return scripting::slice<uint8_t>(nullptr, {0});
  }

  auto n = m_string.find_first_of(split_chars, b);
  if (n == containers::string::npos) {
    n = m_string.size();
  }
  return scripting::slice<uint8_t>(data_as_u8 + b, {n - b});
}

scripting::slice<uint8_t> string::next_split(
    scripting::slice<uint8_t> first, const char* split_chars) {
  auto data_as_u8 = reinterpret_cast<uint8_t*>(m_string.data());
  if (first.begin() < data_as_u8 ||
      first.end() > data_as_u8 + m_string.size()) {
    return scripting::slice<uint8_t>(nullptr, {0});
  }

  auto b = m_string.find_first_not_of(split_chars, first.end() - data_as_u8);
  if (b == containers::string::npos) {
    return scripting::slice<uint8_t>(nullptr, {0});
  }

  auto n = m_string.find_first_of(split_chars, b);
  if (n == containers::string::npos) {
    n = m_string.size();
  }
  return scripting::slice<uint8_t>(data_as_u8 + b, {n - b});
}

}  // namespace support
}  // namespace wn
