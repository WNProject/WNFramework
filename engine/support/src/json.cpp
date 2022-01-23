// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "support/inc/json.h"
#include "WNFileSystem/inc/WNFile.h"
#include "WNFileSystem/inc/WNMapping.h"
#include "WNLogging/inc/WNLog.h"
#include "WNScripting/inc/WNFactory.h"
#include "engine_base/inc/context.h"
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"
#include "support/inc/string.h"

namespace wn {
namespace support {
scripting::shared_cpp_pointer<json_document> parse_file(
    engine_base::context* ctx, scripting::shared_cpp_pointer<string> str) {
  file_system::result result;
  file_system::file_ptr f =
      ctx->m_file_mapping->open_file(str->c_str(), result);
  if (result != file_system::result::ok) {
    ctx->m_log->log_error("Could not open file ", str->c_str());
    return scripting::shared_cpp_pointer<json_document>();
  }

  const char* dat = f->typed_data<const char>();
  size_t sz = f->size();
  rapidjson::Document doc;
  doc.Parse(dat, sz);
  if (doc.GetParseError() != rapidjson::kParseErrorNone) {
    ctx->m_log->log_error("Could not parse \"", str->c_str(),
        "\" :", rapidjson::GetParseError_En(doc.GetParseError()));
    return scripting::shared_cpp_pointer<json_document>();
  }
  scripting::shared_cpp_pointer<json_document> ret_doc =
      ctx->m_engine->make_shared_cpp<json_document>(core::move(doc));

  return ret_doc;
};

struct json_value : rapidjson::Value {
  json_value* get_array_value(int _idx) {
    return reinterpret_cast<json_value*>(&(*this)[_idx]);
  }

  json_value* get_member(const char* name, int _idx) {
    auto it = FindMember(name);
    while (_idx && it != MemberEnd()) {
      ++it;
      if (::strcmp(it->name.GetString(), name) == 0) {
        --_idx;
      }
    }
    return reinterpret_cast<json_value*>(&((it + _idx)->value));
  }

  int32_t get_num_members(const char* name) {
    int i = 0;
    auto it = FindMember(name);
    while (it != MemberEnd()) {
      if (!::strcmp(name, it->name.GetString())) {
        ++i;
      }
      ++it;
    }
    return i;
  }

  bool has_member(const char* name) {
    return HasMember(name);
  }

  const char* get_string() {
    return GetString();
  }

  int32_t get_int() {
    return GetInt();
  }

  bool get_bool() {
    return GetBool();
  }

  float get_float() {
    return GetFloat();
  }

  int32_t array_size() {
    return Size();
  }
};

json_value* json_document::value() {
  return reinterpret_cast<json_value*>(
      static_cast<rapidjson::Value*>(&m_document));
}
}  // namespace support
namespace scripting {

template <>
struct exported_script_type<support::json_value> {
  static containers::string_view exported_name() {
    return "JsonValue";
  }

  static void export_type(exporter<support::json_value>* _exporter) {
    _exporter->register_nonvirtual_function<
        decltype(&support::json_value::get_string),
        &support::json_value::get_string>("get_string");
    _exporter->register_nonvirtual_function<
        decltype(&support::json_value::get_int), &support::json_value::get_int>(
        "get_int");
    _exporter
        ->register_nonvirtual_function<decltype(&support::json_value::get_bool),
            &support::json_value::get_bool>("get_bool");
    _exporter->register_nonvirtual_function<
        decltype(&support::json_value::get_float),
        &support::json_value::get_float>("get_float");
    _exporter->register_nonvirtual_function<
        decltype(&support::json_value::array_size),
        &support::json_value::array_size>("array_size");
    _exporter->register_nonvirtual_function<
        decltype(&support::json_value::get_array_value),
        &support::json_value::get_array_value>("get_array_value");
    _exporter->register_nonvirtual_function<
        decltype(&support::json_value::get_member),
        &support::json_value::get_member>("get_member");
    _exporter->register_nonvirtual_function<
        decltype(&support::json_value::has_member),
        &support::json_value::has_member>("has_member");
    _exporter->register_nonvirtual_function<
        decltype(&support::json_value::get_num_members),
        &support::json_value::get_num_members>("get_num_members");
  }
};

template <>
struct exported_script_type<support::json_document> {
  static containers::string_view exported_name() {
    return "JsonDocument";
  }

  static void export_type(exporter<support::json_document>* _exporter) {
    _exporter
        ->register_nonvirtual_function<decltype(&support::json_document::value),
            &support::json_document::value>("value");
  }
};
}  // namespace scripting

namespace support {
void json_document::register_scripting(
    memory::allocator*, scripting::engine* _engine) {
  _engine->register_cpp_type<json_value>();
  _engine->register_cpp_type<json_document>();
  _engine->register_function<decltype(&parse_file), &parse_file>(
      "parse_json_file");
}

bool json_document::resolve_scripting(scripting::engine*) {
  return true;
}

}  // namespace support
}  // namespace wn
