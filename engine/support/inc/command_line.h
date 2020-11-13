// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_SUPPORT_COMMAND_LINE_H__
#define __WN_SUPPORT_COMMAND_LINE_H__

#include "WNContainers/inc/WNDeque.h"
#include "WNContainers/inc/WNHashMap.h"
#include "WNMemory/inc/unique_ptr.h"
#include "WNScripting/inc/WNEngine.h"
#include "WNScripting/inc/resource_manager.h"
#include "engine_base/inc/context.h"

#include <re2/re2.h>

namespace wn {
namespace support {

struct command_line_param_base {
  bool operator==(const command_line_param_base& _other) {
    return name == _other.name && short_name == _other.short_name &&
           description == _other.description && type == _other.type;
  }

  virtual ~command_line_param_base() = default;

  virtual bool parse(const char* v) = 0;
  virtual const char* get_name_text() = 0;
  containers::string name;
  containers::string short_name;
  containers::string description;
  const char* type;
};

template <typename T>
struct command_line_param : public command_line_param_base {
public:
  explicit command_line_param(memory::allocator* _allocator);

  T get_param_i(int32_t i) {
    return param[i];
  }
  int32_t get_num_params() {
    return static_cast<int32_t>(param.size());
  }
  bool parse(const char* v) override;
  const char* get_name_text() override;
  T get_param() {
    return param.size() > 0 ? param[0] : m_default;
  }

  T m_default = {};
  containers::deque<T> param;
};
class command_line_manager;

template <typename T>
struct command_line_resource_manager : public scripting::resource_manager {
  command_line_resource_manager(
      memory::allocator* _allocator, command_line_manager* _manager);

  bool convert_to_function(containers::string_view _resource_name,
      containers::string_view _resource_data, logging::log*,
      containers::string* _dat, core::optional<uintptr_t>* _user_data) override;

private:
  memory::allocator* m_allocator;
  command_line_manager* m_manager;
  containers::dynamic_array<command_line_param<T>> m_params;
};

struct verb {
  containers::hash_map<containers::string_view, memory::unique_ptr<verb>>
      m_verbs;
  containers::hash_map<containers::string,
      memory::unique_ptr<command_line_param_base>>
      m_parameters;
  containers::string m_function_name;
  containers::string m_verb_name;
  containers::string m_description;
  verb* m_current_child_verb = nullptr;
  wn::scripting::script_function<int32_t, engine_base::context*> m_function;
  bool resolve(scripting::engine* _engine, logging::log* _log);
  bool handle_command_line_param(
      memory::allocator* _allocator, const char* param, logging::log* _log);
  void print_help(memory::allocator* _allocator, logging::log* _log,
      size_t _depth, bool _full);
  command_line_param_base* add_command_line(memory::allocator* _allocator,
      logging::log* _log, containers::contiguous_range<const char*> verb,
      memory::unique_ptr<command_line_param_base> param);
  int32_t run_application(
      scripting::engine* _engine, engine_base::context* _context);
  bool add_verb(logging::log* _log, memory::unique_ptr<verb> _verb,
      containers::contiguous_range<const char*> _parent);
};

class command_line_manager {
public:
  command_line_manager(memory::allocator* _allocator, logging::log* _log);
  void register_scripting(
      memory::allocator* _allocator, scripting::engine* _engine);
  bool resolve_scripting(scripting::engine* _engine);
  void set_global_program_name(const char* prog_name);
  bool add_verb(memory::unique_ptr<verb> _verb,
      containers::contiguous_range<const char*> _parent);
  command_line_param_base* add_command_line(logging::log* _log,
      containers::contiguous_range<const char*> verb,
      memory::unique_ptr<command_line_param_base> param);

  int32_t run_application(
      scripting::engine* _engine, engine_base::context* _context);

  bool insert_global_param(const char* param);
  void print_help(bool _full);

private:
  memory::allocator* m_allocator;
  containers::hash_map<containers::string_view, memory::unique_ptr<verb>>
      m_verbs;
  containers::hash_map<containers::string,
      memory::unique_ptr<command_line_param_base>>
      m_parameters;
  wn::scripting::script_function<int32_t, engine_base::context*> m_main;
  logging::log* m_log;
  const char* m_program_name = "";
  verb* m_current_verb = nullptr;
};

class verb_manager : public scripting::resource_manager {
public:
  verb_manager(memory::allocator* _allocator, logging::log* _log,
      command_line_manager* _manager);

  virtual bool setup_resource(containers::string_view, containers::string*);

private:
  memory::allocator* m_allocator;
  logging::log* m_log;
  command_line_manager* m_command_line_manager;
};

}  // namespace support
}  // namespace wn
#include "support/inc/command_line.inl"
#endif  // __WN_SUPPORT_COMMAND_LINE_H__
