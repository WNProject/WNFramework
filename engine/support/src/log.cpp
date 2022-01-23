// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNLogging/inc/WNLog.h"
#include "WNScripting/inc/WNFactory.h"
#include "support/inc/string.h"
namespace wn {

namespace {

struct log_flusher {};

log_flusher* _error() {
  scripting::get_scripting_tls()->_log->log_params(logging::log_level::error,
      static_cast<size_t>(logging::log_flags::no_newline));
  return reinterpret_cast<log_flusher*>(
      static_cast<uintptr_t>(logging::log_level::error));
}

log_flusher* _warning() {
  scripting::get_scripting_tls()->_log->log_params(logging::log_level::warning,
      static_cast<size_t>(logging::log_flags::no_newline));
  return reinterpret_cast<log_flusher*>(
      static_cast<uintptr_t>(logging::log_level::warning));
}
log_flusher* _info() {
  scripting::get_scripting_tls()->_log->log_params(logging::log_level::info,
      static_cast<size_t>(logging::log_flags::no_newline));
  return reinterpret_cast<log_flusher*>(
      static_cast<uintptr_t>(logging::log_level::info));
}

log_flusher* _critical() {
  scripting::get_scripting_tls()->_log->log_params(logging::log_level::critical,
      static_cast<size_t>(logging::log_flags::no_newline));
  return reinterpret_cast<log_flusher*>(
      static_cast<uintptr_t>(logging::log_level::critical));
}

log_flusher* _issue() {
  scripting::get_scripting_tls()->_log->log_params(logging::log_level::issue,
      static_cast<size_t>(logging::log_flags::no_newline));
  return reinterpret_cast<log_flusher*>(
      static_cast<uintptr_t>(logging::log_level::issue));
}

log_flusher* _debug() {
  scripting::get_scripting_tls()->_log->log_params(

      logging::log_level::debug,
      static_cast<size_t>(logging::log_flags::no_newline));
  return reinterpret_cast<log_flusher*>(
      static_cast<uintptr_t>(logging::log_level::debug));
}

log_flusher* _o_cstr(log_flusher* fl, const char* str) {
  logging::log_level l =
      static_cast<logging::log_level>(reinterpret_cast<uintptr_t>(fl));
  if (!str) {
    scripting::get_scripting_tls()->_log->log_params(l,
        static_cast<size_t>(logging::log_flags::no_newline) |
            static_cast<size_t>(logging::log_flags::no_header),
        "<nullptr>");
  }
  size_t s = ::strlen(str);
  for (size_t i = 0; i < s; i += 512) {
    scripting::get_scripting_tls()->_log->log_params(l,
        static_cast<size_t>(logging::log_flags::no_newline) |
            static_cast<size_t>(logging::log_flags::no_header),
        containers::string_view(str + i, (s - i > 512) ? 512 : s - i));
  }

  return fl;
}

log_flusher* _o_int(log_flusher* fl, int32_t val) {
  logging::log_level l =
      static_cast<logging::log_level>(reinterpret_cast<uintptr_t>(fl));
  scripting::get_scripting_tls()->_log->log_params(l,
      static_cast<size_t>(logging::log_flags::no_newline) |
          static_cast<size_t>(logging::log_flags::no_header),
      val);
  return fl;
}

log_flusher* _o_bool(log_flusher* fl, bool val) {
  logging::log_level l =
      static_cast<logging::log_level>(reinterpret_cast<uintptr_t>(fl));
  scripting::get_scripting_tls()->_log->log_params(l,
      static_cast<size_t>(logging::log_flags::no_newline) |
          static_cast<size_t>(logging::log_flags::no_header),
      val ? "true" : "false");
  return fl;
}

log_flusher* _o_float(log_flusher* fl, float val) {
  logging::log_level l =
      static_cast<logging::log_level>(reinterpret_cast<uintptr_t>(fl));
  scripting::get_scripting_tls()->_log->log_params(l,
      static_cast<size_t>(logging::log_flags::no_newline) |
          static_cast<size_t>(logging::log_flags::no_header),
      val);
  return fl;
}

void _end(log_flusher* fl) {
  logging::log_level l =
      static_cast<logging::log_level>(reinterpret_cast<uintptr_t>(fl));
  scripting::get_scripting_tls()->_log->log_params(
      l, static_cast<size_t>(logging::log_flags::no_header));
}

}  // namespace

namespace scripting {

template <>
struct exported_script_type<log_flusher> {
  static containers::string_view exported_name() {
    return "Log";
  }

  static void export_type(exporter<log_flusher>* _exporter) {
    _exporter->register_pseudo_function<decltype(&_o_cstr), &_o_cstr>("o");
    _exporter->register_pseudo_function<decltype(&_o_int), &_o_int>("o");
    _exporter->register_pseudo_function<decltype(&_o_float), &_o_float>("o");
    _exporter->register_pseudo_function<decltype(&_o_bool), &_o_bool>("o");
    _exporter->register_pseudo_function<decltype(&_end), &_end>("end");
  }
};
}  // namespace scripting

namespace support {
namespace log {
void register_scripting(memory::allocator*, scripting::engine* _engine) {
  _engine->register_cpp_type<log_flusher>();
  _engine->register_function<decltype(&_error), &_error>("log_error");
  _engine->register_function<decltype(&_info), &_info>("log_info");
  _engine->register_function<decltype(&_critical), &_critical>("log_critical");
  _engine->register_function<decltype(&_warning), &_warning>("log_warning");
  _engine->register_function<decltype(&_debug), &_debug>("log_debug");
  _engine->register_function<decltype(&_issue), &_issue>("log_issue");
}

bool resolve_scripting(scripting::engine*) {
  return true;
}
}  // namespace log
}  // namespace support
}  // namespace wn
