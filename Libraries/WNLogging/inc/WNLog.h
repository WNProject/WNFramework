// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_LOGGING_LOG_H__
#define __WN_LOGGING_LOG_H__

#include "WNCore/inc/assert.h"
#include "WNCore/inc/types.h"
#include "WNLogging/inc/Internal/WNConfig.h"
#include "WNLogging/inc/WNLogEnums.h"
#include "WNMemory/inc/string_utility.h"

namespace wn {
namespace logging {

static const char* klog_messages[static_cast<uint32_t>(log_level::max)] = {
    "None:   ", "Critial:", "Error:  ", "Warning:", "Issue:  ", "Info:   ",
    "Debug:  "};

enum class log_flags { no_newline = 1 << 0, no_header = 1 << 1 };

struct color_element {
  log_level m_level;
  char* m_position;
};

class logger {
public:
  virtual void flush_buffer(const char* _buffer, size_t _buffer_size,
      const color_element* _colors, size_t _num_colors) = 0;
};

#define LOGGER_ENCODING_HELPER(type, formatter)                                \
  template <typename T>                                                        \
  struct _enc##type {                                                          \
    static const char* get_val() {                                             \
      return formatter;                                                        \
    }                                                                          \
  };                                                                           \
  template <>                                                                  \
  struct _enc##type<wchar_t> {                                                 \
    static const wchar_t* get_val() {                                          \
      return L##formatter;                                                     \
    }                                                                          \
  };

LOGGER_ENCODING_HELPER(ptr, "0x%p");
template <typename T, typename BuffType>
struct log_type_helper {
  WN_FORCE_INLINE static bool do_log(
      const T& _0, BuffType* _buffer, size_t& _buffer_left) {
    int printed = wn::memory::snprintf(
        _buffer, _buffer_left, _encptr<BuffType>::get_val(), _0);
    if (printed < 0 || static_cast<size_t>(printed) >= _buffer_left) {
      return (false);
    }
    _buffer_left -= printed;
    return (true);
  }
};

template <typename T, typename BuffType>
WN_FORCE_INLINE bool log_type(
    const T& _0, BuffType* _buffer, size_t& _buffer_left) {
  return (log_type_helper<T, BuffType>::do_log(_0, _buffer, _buffer_left));
}

template <log_level MAX_LOG_LEVEL = internal::s_default_log_level>
class log_impl {
public:
  log_impl(logger* _logger, size_t _size, char* _write_buffer,
      color_element* _colors, log_level _lvl = log_level::error,
      bool _flush = false)
    : m_logger(_logger),
      m_log_buffer(_write_buffer),
      m_buffer_left(_size),
      m_buffer_size(_size),
      m_num_color_elements(0),
      m_color_elements(_colors),
      m_current_log_level(_lvl),
      m_flush_after_message(_flush) {}

  ~log_impl() {
    flush();
  }
  WN_FORCE_INLINE void set_log_level(log_level _level) {
    m_current_log_level = _level;
  }
  WN_FORCE_INLINE void flush();
  WN_FORCE_INLINE void flush_external(const char* _buffer, size_t _buffer_size,
      const color_element* _colors, size_t _num_colors);

  template <typename... Args>
  WN_FORCE_INLINE void log_params(log_level, size_t, const Args&... args);

  template <typename... Args>
  WN_FORCE_INLINE void log_critical_flags(size_t size, const Args&... args) {
    log_params(log_level::critical, size, args...);
    flush();
  }
  template <typename... Args>
  WN_FORCE_INLINE void log_error_flags(size_t size, const Args&... args) {
    log_params(log_level::error, size, args...);
  }
  template <typename... Args>
  WN_FORCE_INLINE void log_warning_flags(size_t size, const Args&... args) {
    log_params(log_level::warning, size, args...);
  }
  template <typename... Args>
  WN_FORCE_INLINE void log_issue_flags(size_t size, const Args&... args) {
    log_params(log_level::issue, size, args...);
  }
  template <typename... Args>
  WN_FORCE_INLINE void log_info_flags(size_t size, const Args&... args) {
    log_params(log_level::info, size, args...);
  }
  template <typename... Args>
  WN_FORCE_INLINE void log_debug_flags(size_t size, const Args&... args) {
    log_params(log_level::debug, size, args...);
  }

  template <typename... Args>
  WN_FORCE_INLINE void log_critical(const Args&... args) {
    log_critical_flags(0, args...);
  }
  template <typename... Args>
  WN_FORCE_INLINE void log_error(const Args&... args) {
    log_error_flags(0, args...);
  }
  template <typename... Args>
  WN_FORCE_INLINE void log_warning(const Args&... args) {
    log_warning_flags(0, args...);
  }
  template <typename... Args>
  WN_FORCE_INLINE void log_issue(const Args&... args) {
    log_issue_flags(0, args...);
  }
  template <typename... Args>
  WN_FORCE_INLINE void log_info(const Args&... args) {
    log_info_flags(0, args...);
  }
  template <typename... Args>
  WN_FORCE_INLINE void log_debug(const Args&... args) {
    log_debug_flags(0, args...);
  }

  template <typename Arg1, typename... Args>
  void do_log_log(const Arg1& _0, const Args&... args);

  template <typename Arg>
  void do_log_log(const Arg& _0);

  void do_log_log();

private:
  log_impl& operator=(const log_impl& _other) = delete;
  WN_FORCE_INLINE void log_header(log_level);
  WN_FORCE_INLINE void log_newline();
  WN_FORCE_INLINE void append_color(log_level, char*);
  template <typename T0>
  WN_FORCE_INLINE void log_param(const T0& _val);

  logger* m_logger;
  char* m_log_buffer;
  size_t m_buffer_left;
  size_t m_buffer_size;
  size_t m_num_color_elements;
  color_element* m_color_elements;
  log_level m_current_log_level;
  bool m_flush_after_message;
};
using log = log_impl<>;

WN_FORCE_INLINE log* get_null_logger() {
  static log m_null_logger(
      nullptr, 0, nullptr, nullptr, log_level::none, false);
  return &m_null_logger;
}

template <log_level MAX_LOG_LEVEL = internal::s_default_log_level,
    uint32_t BUFFER_SIZE = 1024>
class static_log {
  using log_type = log_impl<MAX_LOG_LEVEL>;

public:
  static_log(
      logger* _logger, log_level _lvl = log_level::error, bool _flush = false)
    : m_log(_logger, BUFFER_SIZE, m_buffer, m_color_elements, _lvl, _flush) {}
  log_type* log() {
    return &m_log;
  }

private:
  log_type m_log;
  char m_buffer[BUFFER_SIZE];
  color_element m_color_elements[BUFFER_SIZE];
};

#ifdef WN_ALLOW_TESTING_LOG
log* GetTestingLog();
#endif

}  // namespace logging
}  // namespace wn

#include "WNLogging/inc/WNDefaultLogTypes.inl"
#include "WNLogging/inc/WNLog.inl"

#endif  // __WN_LOGGING_LOG_H__
