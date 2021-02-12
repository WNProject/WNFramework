// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "support/inc/command_line.h"

namespace wn {
namespace support {

template <typename T>
command_line_param<T>* get_param(scripting::wn_size_t st) {
  static_assert(sizeof(size_t) == sizeof(uintptr_t), "Invalid regex type");
  return reinterpret_cast<command_line_param<T>*>(
      static_cast<uintptr_t>(st.val));
}

void command_line_manager::register_scripting(
    memory::allocator* _allocator, scripting::engine* _engine) {
  _engine->register_cpp_type<command_line_param<int32_t>>();
  _engine->register_cpp_type<command_line_param<const char*>>();
  _engine->register_cpp_type<command_line_param<bool>>();

  _engine
      ->register_function<decltype(&get_param<int32_t>), &get_param<int32_t>>(
          type_name<int32_t>::function());
  _engine->register_function<decltype(&get_param<const char*>),
      &get_param<const char*>>(type_name<const char*>::function());
  _engine->register_function<decltype(&get_param<bool>), &get_param<bool>>(
      type_name<bool>::function());

  _engine->register_resource<command_line_param<int32_t>*>(
      memory::make_unique<command_line_resource_manager<int32_t>>(
          _allocator, _allocator, this));
  _engine->register_resource<command_line_param<const char*>*>(
      memory::make_unique<command_line_resource_manager<const char*>>(
          _allocator, _allocator, this));
  _engine->register_resource<command_line_param<bool>*>(
      memory::make_unique<command_line_resource_manager<bool>>(
          _allocator, _allocator, this));
  _engine->register_resource<void*>(
      memory::make_unique<verb_manager>(_allocator, _allocator, m_log, this));
}

command_line_manager::command_line_manager(
    memory::allocator* _allocator, logging::log* _log)
  : m_allocator(_allocator),
    m_verbs(_allocator),
    m_parameters(_allocator),
    m_log(_log) {}

bool verb::resolve(scripting::engine* _engine, logging::log* _log) {
  _engine->get_function(m_function_name, &m_function);
  if (!m_function) {
    _log->log_error(
        "Could not find function ", m_function_name, " for verb ", m_verb_name);
    return false;
  }
  for (auto& it : m_verbs) {
    if (!it.second->resolve(_engine, _log)) {
      return false;
    }
  }
  return true;
}

bool verb::add_verb(logging::log* _log, memory::unique_ptr<verb> _verb,
    containers::contiguous_range<const char*> _parent) {
  if (!_parent.empty()) {
    auto it = m_verbs.find(_parent[0]);
    if (it == m_verbs.end()) {
      _log->log_error(
          "Cannot find parent ", _parent[0], " for verb ", _verb->m_verb_name);
      return false;
    }
    _parent.remove_prefix(1);
    return it->second->add_verb(_log, core::move(_verb), _parent);
  }
  if (m_verbs.find(_verb->m_verb_name.to_string_view()) != m_verbs.end()) {
    _log->log_error("Verb: ", _verb->m_verb_name, " specified more than once");
    return false;
  }
  m_verbs[_verb->m_verb_name.to_string_view()] = core::move(_verb);
  return true;
}

bool command_line_manager::add_verb(memory::unique_ptr<verb> _verb,
    containers::contiguous_range<const char*> _parent) {
  if (!_parent.empty()) {
    auto it = m_verbs.find(_parent[0]);
    if (it == m_verbs.end()) {
      m_log->log_error(
          "Cannot find parent ", _parent[0], " for verb ", _verb->m_verb_name);
      return false;
    }
    _parent.remove_prefix(1);
    return it->second->add_verb(m_log, core::move(_verb), _parent);
  }
  if (m_verbs.find(_verb->m_verb_name.to_string_view()) != m_verbs.end()) {
    m_log->log_error("Verb: ", _verb->m_verb_name, " specified more than once");
    return false;
  }
  m_verbs[_verb->m_verb_name.to_string_view()] = core::move(_verb);
  return true;
}

void command_line_manager::print_help(bool _full) {
#define outln(...)                                                             \
  m_log->log_critical_flags(                                                   \
      static_cast<size_t>(logging::log_flags::no_header), __VA_ARGS__)
#define out(...)                                                               \
  m_log->log_critical_flags(                                                   \
      static_cast<size_t>(logging::log_flags::no_header) |                     \
          static_cast<size_t>(logging::log_flags::no_newline),                 \
      __VA_ARGS__)

  outln(m_program_name, ": Usage");
  outln(" --help, -h:    Print the help message");
  outln(" --fullhelp:    Print the full help message (including hidden flags)");
  outln("");

  if (!m_current_verb && m_verbs.size()) {
    outln("  Verbs:");
    for (auto& v : m_verbs) {
      outln("    ", v.second->m_verb_name, ":  ", v.second->m_description);
    }
  }

  outln("  Args:");
  for (auto& it : m_parameters) {
    if (_full || !it.second->description.starts_with("_")) {
      out("    --", it.second->name, "<", it.second->get_name_text(),
          ">: ", it.second->description);
      outln("");
    }
  }

  if (m_current_verb) {
    m_current_verb->print_help(m_allocator, m_log, 1, _full);
  }
#undef outln
#undef out
}

void verb::print_help(memory::allocator* _allocator, logging::log* _log,
    size_t _depth, bool _full) {
#define outln(...)                                                             \
  _log->log_critical_flags(                                                    \
      static_cast<size_t>(logging::log_flags::no_header), __VA_ARGS__)
#define out(...)                                                               \
  _log->log_critical_flags(                                                    \
      static_cast<size_t>(logging::log_flags::no_header) |                     \
          static_cast<size_t>(logging::log_flags::no_newline),                 \
      __VA_ARGS__)

  containers::string str(_allocator);
  for (size_t i = 0; i < _depth; ++i) {
    str += "  ";
  }
  outln(str, m_verb_name, ":  ", m_description);
  if (!m_current_child_verb && m_verbs.size()) {
    outln(str, "  Sub-Verbs:");
    for (auto& v : m_verbs) {
      outln(str, "    ", v.second->m_verb_name, ":  ", v.second->m_description);
    }
  }

  outln(str, "  Args:");
  for (auto& it : m_parameters) {
    if (_full || !it.second->description.starts_with("_")) {
      out(str, "    --", it.second->name, "<", it.second->get_name_text(),
          ">: ", it.second->description);
      outln("");
    }
  }

  if (m_current_child_verb) {
    m_current_child_verb->print_help(_allocator, _log, _depth + 1, _full);
  }

#undef outln
#undef out
}

void command_line_manager::set_global_program_name(const char* param) {
  m_program_name = param;
}

bool verb::handle_command_line_param(
    memory::allocator* _allocator, const char* param, logging::log* _log) {
  containers::string_view p(param);

  if (!p.starts_with("-")) {
    // Handle Verb
    if (m_verbs.find(p) != m_verbs.end()) {
      m_current_child_verb = m_verbs[p].get();
      return true;
    }
    return false;
  }

  p = p.substr(2);
  auto eq = p.find_first_of("=");
  if (eq != containers::string_view::npos) {
    p = p.substr(0, eq);
  }
  auto it = m_parameters.find(p.to_string(_allocator));
  if (it == m_parameters.end() && p.starts_with("no-")) {
    p = p.substr(3);
    it = m_parameters.find(p.to_string(_allocator));
  }
  if (it == m_parameters.end()) {
    _log->log_error("Unknown command-line parameter ", param);
    return false;
  }
  if (!it->second->parse(param)) {
    _log->log_error("Error parsing command-line parameter ", param, " as ",
        it->second->type);
    return false;
  }
  return true;
}

bool command_line_manager::insert_global_param(const char* param) {
  containers::string_view p(param);

  if (p == "--help" || p == "-h" || p == "-help" || p == "--h") {
    print_help(false);
    return false;
  }
  if (p == "--fullhelp" || p == "-fullhelp") {
    print_help(true);
    return false;
  }

  if (m_current_verb) {
    if (!m_current_verb->handle_command_line_param(m_allocator, param, m_log)) {
      print_help(false);
      return false;
    }
    return true;
  }

  if (!p.starts_with("-")) {
    // Handle Verb
    if (m_verbs.find(p) != m_verbs.end()) {
      m_current_verb = m_verbs[p].get();
      return true;
    }
    m_log->log_error("Could not find verb", p);
    print_help(false);
    return false;
  }

  p = p.substr(2);
  auto eq = p.find_first_of("=");
  if (eq != containers::string_view::npos) {
    p = p.substr(0, eq);
  }
  auto it = m_parameters.find(p.to_string(m_allocator));
  if (it == m_parameters.end() && p.starts_with("no-")) {
    p = p.substr(3);
    it = m_parameters.find(p.to_string(m_allocator));
  }
  if (it == m_parameters.end()) {
    m_log->log_error("Unknown command-line parameter ", param);
    print_help(false);
    return false;
  }
  if (!it->second->parse(param)) {
    m_log->log_error("Error parsing command-line parameter ", param, " as ",
        it->second->type);
    print_help(false);
    return false;
  }
  return true;
}

bool command_line_manager::resolve_scripting(scripting::engine* _engine) {
  for (auto& verb : m_verbs) {
    if (!verb.second->resolve(_engine, m_log)) {
      return false;
    }
  }
  _engine->get_function("main", &m_main);
  return true;
}

int32_t verb::run_application(
    scripting::engine* _engine, engine_base::context* _context) {
  if (m_current_child_verb) {
    return m_current_child_verb->run_application(_engine, _context);
  }
  return _engine->invoke(m_function, _context);
}

int32_t command_line_manager::run_application(
    scripting::engine* _engine, engine_base::context* _context) {
  if (m_current_verb) {
    return m_current_verb->run_application(_engine, _context);
  }
  if (!m_main) {
    m_log->log_error("Cannot find main function");
    return -1;
  }
  return _engine->invoke(m_main, _context);
}

verb_manager::verb_manager(memory::allocator* _allocator, logging::log* _log,
    command_line_manager* _manager)
  : scripting::resource_manager(containers::string(_allocator, "Verb")),
    m_allocator(_allocator),
    m_log(_log),
    m_command_line_manager(_manager) {
  m_can_be_instantiated = false;
  m_must_be_instantiated = false;
}

bool verb_manager::setup_resource(
    containers::string_view _res, containers::string*) {
  containers::string verb_name(m_allocator);
  containers::string description(m_allocator);
  containers::string function(m_allocator);

  rapidjson::Document doc;
  doc.Parse(_res.data(), _res.size());
  (void)doc;
  if (doc.GetParseError() != rapidjson::kParseErrorNone) {
    m_log->log_error("Error Parsing JSON for verb: ",
        rapidjson::GetParseError_En(doc.GetParseError()));
    return false;
  }
  if (!doc.IsObject()) {
    m_log->log_error("JSON error for verb: Root is not an object");
    return false;
  }
  containers::dynamic_array<const char*> parent_verb(m_allocator);
  for (auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it) {
    if (containers::string_view(it->name.GetString()) == "name") {
      if (!it->value.IsString()) {
        m_log->log_error("Verb: 'name' must be a string value");
        return false;
      }
      if (!verb_name.empty()) {
        m_log->log_error("Verb: More than one verb name specified");
        return false;
      }
      verb_name = it->value.GetString();
    } else if (containers::string_view(it->name.GetString()) == "description") {
      if (!it->value.IsString()) {
        m_log->log_error("Verb: 'description' must be a string value");
        return false;
      }
      if (!description.empty()) {
        m_log->log_error("Verb: More than one description specified");
        return false;
      }
      description = it->value.GetString();
    } else if (containers::string_view(it->name.GetString()) == "function") {
      if (!it->value.IsString()) {
        m_log->log_error("Verb: 'function' must be a string value");
        return false;
      }
      if (!function.empty()) {
        m_log->log_error("Verb: More than one function specified");
        return false;
      }
      function = it->value.GetString();
    } else if (containers::string_view(it->name.GetString()) == "verb") {
      if (!it->value.IsArray()) {
        m_log->log_error("'verb' must be an array value");
        return false;
      }
      auto arr = it->value.GetArray();
      if (it->value.GetArray().Size() < 1) {
        m_log->log_error("'verb' array size must be > 0");
        return false;
      }
      if (!parent_verb.empty()) {
        m_log->log_error("'verb' specified more than once for parent verb");
        return false;
      }

      for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
        if (!arr[i].IsString()) {
          m_log->log_error("'verb' array contents must be strings");
          return false;
        }

        parent_verb.push_back(arr[i].GetString());
      }
    } else {
      m_log->log_error(
          "Verb: Unknown command-line parameter value: ", it->name.GetString());
      return false;
    }
  }

  if (verb_name.empty()) {
    m_log->log_error("The verb must have a name");
    return false;
  }
  if (description.empty()) {
    m_log->log_error("The verb must have a description: ", verb_name);
    return false;
  }
  if (function.empty()) {
    m_log->log_error("The verb must have a function: ", verb_name);
    return false;
  }

  auto v = memory::make_unique<verb>(m_allocator);
  v->m_function_name = core::move(function);
  v->m_parameters = containers::hash_map<containers::string,
      memory::unique_ptr<command_line_param_base>>(m_allocator);
  v->m_verb_name = core::move(verb_name);
  v->m_description = core::move(description);
  v->m_verbs =
      containers::hash_map<containers::string_view, memory::unique_ptr<verb>>(
          m_allocator);
  return m_command_line_manager->add_verb(core::move(v), parent_verb);
}

}  // namespace support
}  // namespace wn
