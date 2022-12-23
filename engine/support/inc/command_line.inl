// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/type_manager.h"
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"
#include "support/inc/command_line.h"

namespace wn {
namespace {

template <typename T>
struct type_name {};

template <>
struct type_name<int32_t> {
  static const char* name() {
    return "IntParam";
  }
  static const char* text_name() {
    return "int";
  }
  static const char* function() {
    return "_get_int_param";
  }
};
template <>
struct type_name<const char*> {
  static const char* name() {
    return "StringParam";
  }
  static const char* text_name() {
    return "string";
  }
  static const char* function() {
    return "_get_string_param";
  }
};
template <>
struct type_name<bool> {
  static const char* name() {
    return "BoolParam";
  }
  static const char* function() {
    return "_get_bool_param";
  }
  static const char* text_name() {
    return "bool";
  }
};
}  // namespace
namespace scripting {

template <typename T>
struct exported_script_type<wn::support::command_line_param<T>> {
  static containers::string_view exported_name() {
    return type_name<T>::name();
  }

  static void export_type(
      wn::scripting::exporter<support::command_line_param<T>>* _exporter) {
    _exporter->template register_nonvirtual_function<
        decltype(&support::command_line_param<T>::get_param),
        &support::command_line_param<T>::get_param>("get_param");
    _exporter->template register_nonvirtual_function<
        decltype(&support::command_line_param<T>::get_param_i),
        &support::command_line_param<T>::get_param_i>("get_param");
    _exporter->template register_nonvirtual_function<
        decltype(&support::command_line_param<T>::get_num_params),
        &support::command_line_param<T>::get_num_params>("get_num_params");
    _exporter->template register_nonvirtual_function<
        decltype(&support::command_line_param<T>::get_all_params),
        &support::command_line_param<T>::get_all_params>("get_all_params");
  }
};

}  // namespace scripting

namespace support {

template <>
inline bool command_line_param<int32_t>::parse(const char* v) {
  size_t it = containers::string_view(v).find_first_of("=");
  if (it == containers::string_view::npos) {
    return false;
  }
  char* endptr;
  int32_t val = ::strtoul(v + it + 1, &endptr, 0);
  if (endptr == v) {
    return false;
  }
  param.push_back(val);
  return true;
}

template <>
inline bool command_line_param<const char*>::parse(const char* v) {
  size_t it = containers::string_view(v).find_first_of("=");
  if (it == containers::string_view::npos) {
    return false;
  }
  param.push_back(v + it + 1);
  return true;
}

template <>
inline bool command_line_param<bool>::parse(const char* v) {
  param.push_back(!containers::string_view(v).starts_with("--no-"));
  return true;
}

template <typename T>
inline const char* command_line_param<T>::get_name_text() {
  return type_name<T>::text_name();
}

template <typename T>
command_line_resource_manager<T>::command_line_resource_manager(
    memory::allocator* _allocator, command_line_manager* _manager)
  : scripting::resource_manager(
        containers::string(_allocator, type_name<T>::name())),
    m_allocator(_allocator),
    m_manager(_manager),
    m_params(_allocator) {}

template <typename T>
bool command_line_resource_manager<T>::convert_to_function(
    containers::string_view _resource_name,
    containers::string_view _resource_data, logging::log* _log,
    containers::string* _dat, core::optional<uintptr_t>* _user_data) {
  (void)_resource_data;

  containers::string param_name(m_allocator);
  containers::string param_short(m_allocator);
  containers::string param_description(m_allocator);
  bool is_required = false;
  containers::dynamic_array<containers::dynamic_array<const char*>> verb(
      m_allocator);

  rapidjson::Document doc;
  doc.Parse(_resource_name.data(), _resource_name.size());
  (void)doc;
  if (doc.GetParseError() != rapidjson::kParseErrorNone) {
    _log->log_error("Error Parsing JSON for ", type_name<T>::name(), ": ",
        rapidjson::GetParseError_En(doc.GetParseError()));
    return false;
  }
  if (!doc.IsObject()) {
    _log->log_error(
        "JSON error for ", type_name<T>::name(), ": Root is not an object");
    return false;
  }

  for (auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it) {
    if (containers::string_view(it->name.GetString()) == "name") {
      if (!it->value.IsString()) {
        _log->log_error("'name' must be a string value");
        return false;
      }
      if (!param_name.empty()) {
        _log->log_error("More than one parameter name specified");
        return false;
      }
      param_name = it->value.GetString();
    } else if (containers::string_view(it->name.GetString()) == "short") {
      if (!it->value.IsString()) {
        _log->log_error("'short' must be a string value");
        return false;
      }
      if (!param_short.empty()) {
        _log->log_error("More than one parameter short_name specified");
        return false;
      }
      param_short = it->value.GetString();
    } else if (containers::string_view(it->name.GetString()) == "description") {
      if (!it->value.IsString()) {
        _log->log_error("'description' must be a string value");
        return false;
      }
      if (!param_description.empty()) {
        _log->log_error("More than one parameter description specified");
        return false;
      }
      param_description = it->value.GetString();
    } else if (containers::string_view(it->name.GetString()) == "required") {
      if (!it->value.IsBool()) {
        _log->log_error("'required' must be a boolean value");
        return false;
      }
      is_required = it->value.GetBool();
    } else if (containers::string_view(it->name.GetString()) == "verb") {
      if (!it->value.IsArray()) {
        _log->log_error("'verb' must be an array value");
        return false;
      }
      auto arr = it->value.GetArray();
      if (it->value.GetArray().Size() < 1) {
        _log->log_error("'verb' array size must be > 0");
        return false;
      }
      verb.push_back(containers::dynamic_array<const char*>(m_allocator));
      for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
        if (!arr[i].IsString()) {
          _log->log_error("'verb' array contents must be strings");
          return false;
        }

        verb.back().push_back(arr[i].GetString());
      }
    } else {
      _log->log_error(
          "Unknown command-line parameter value: ", it->name.GetString());
      return false;
    }
  }

  if (param_name.empty()) {
    _log->log_error("The parameter must have a name");
    return false;
  }
  if (param_description.empty()) {
    _log->log_error("The parameter must have a description: ", param_name);
    return false;
  }

  for (size_t i = 1; i < verb.size(); ++i) {
    auto m =
        memory::make_unique<command_line_param<T>>(m_allocator, m_allocator);
    m->name = containers::string(m_allocator, param_name);
    m->short_name = containers::string(m_allocator, core::move(param_short));
    m->description =
        containers::string(m_allocator, core::move(param_description));
    m->type = type_name<T>::name();
    m->m_required = is_required;
    _user_data->emplace(reinterpret_cast<uintptr_t>(m_manager->add_command_line(
        _log, containers::contiguous_range<const char*>(verb[i]),
        core::move(m))));
  }

  auto m = memory::make_unique<command_line_param<T>>(m_allocator, m_allocator);
  m->name = core::move(param_name);
  m->short_name = core::move(param_short);
  m->description = core::move(param_description);
  m->type = type_name<T>::name();
  m->m_required = is_required;
  containers::contiguous_range<const char*> v;
  if (verb.size()) {
    v = verb[0];
  }
  _user_data->emplace(reinterpret_cast<uintptr_t>(
      m_manager->add_command_line(_log, v, core::move(m))));

  *_dat = type_name<T>::function();

  return true;
}

template <typename T>
command_line_param<T>::command_line_param(memory::allocator* _allocator)
  : param(_allocator) {}

inline command_line_param_base* command_line_manager::add_command_line(
    logging::log* _log, containers::contiguous_range<const char*> _verb,
    memory::unique_ptr<command_line_param_base> param) {
  if (param->name == "help" || param->name == "h" || param->short_name == "h" ||
      param->short_name == "help" || param->name == "fullhelp" ||
      param->short_name == "fullhelp") {
    _log->log_error(
        "The paramter ", param->name, " will collide with the builtin help");
    return nullptr;
  }
  if (_verb.size() > 0) {
    auto it = m_verbs.find(_verb[0]);
    if (it == m_verbs.end()) {
      _log->log_error(
          "Could not find verb: ", _verb[0], " for parameter ", param->name);
      return nullptr;
    }
    _verb.remove_prefix(1);
    return it->second->add_command_line(
        m_allocator, _log, _verb, core::move(param));
  }
  auto it = m_parameters.find(param->name);
  if (it != m_parameters.end()) {
    if (!(*(it->second) == *param)) {
      _log->log_error("The paramter ", param->name,
          " was defined twice with different arguments");
      return nullptr;
    }
    return it->second.get();
  }
  auto i = param.get();
  m_parameters[containers::string(m_allocator, param->name.c_str())] =
      core::move(param);
  return i;
}

inline command_line_param_base* verb::add_command_line(
    memory::allocator* _allocator, logging::log* _log,
    containers::contiguous_range<const char*> _verb,
    memory::unique_ptr<command_line_param_base> param) {
  if (_verb.size() > 0) {
    auto it = m_verbs.find(_verb[0]);
    if (it == m_verbs.end()) {
      _log->log_error(
          "Could not find verb: ", _verb[0], " for parameter ", param->name);
      return nullptr;
    }
    _verb.remove_prefix(1);
    return it->second->add_command_line(
        _allocator, _log, _verb, core::move(param));
  }
  auto it = m_parameters.find(param->name);
  if (it != m_parameters.end()) {
    if (!(*(it->second) == *param)) {
      _log->log_error("The paramter ", param->name,
          " was defined twice with different arguments");
      return nullptr;
    }
    return it->second.get();
  }
  auto i = param.get();
  m_parameters[containers::string(_allocator, param->name.c_str())] =
      core::move(param);
  return i;
}

}  // namespace support
}  // namespace wn
