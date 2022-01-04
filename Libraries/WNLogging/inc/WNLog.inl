// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_LOGGING_LOG_INL__
#define __WN_LOGGING_LOG_INL__
#include "WNLogging/inc/WNLog.h"
#include "core/inc/assert.h"
#include "core/inc/types.h"

namespace wn {
namespace logging {

template <log_level MAX_LOG_LEVEL>
inline void log_impl<MAX_LOG_LEVEL>::flush() {
  if (m_buffer_size != m_buffer_left) {
    m_logger->flush_buffer(m_log_buffer, m_buffer_size - m_buffer_left,
        m_color_elements, m_num_color_elements);
    m_num_color_elements = 0;
    m_buffer_left = m_buffer_size;
  }
}

template <log_level MAX_LOG_LEVEL>
inline void log_impl<MAX_LOG_LEVEL>::flush_external(const char* _buffer,
    size_t _bufferSize, const color_element* _colors, size_t _num_colors) {
  m_logger->flush_buffer(_buffer, _bufferSize, _colors, _num_colors);
}

template <log_level MAX_LOG_LEVEL>
template <typename Arg1, typename... Args>
void log_impl<MAX_LOG_LEVEL>::do_log_log(const Arg1& _0, const Args&... args) {
  log_param(_0);
  do_log_log<Args...>(args...);
}

template <log_level MAX_LOG_LEVEL>
void log_impl<MAX_LOG_LEVEL>::do_log_log() {}

template <log_level MAX_LOG_LEVEL>
template <typename Arg>
void log_impl<MAX_LOG_LEVEL>::do_log_log(const Arg& _0) {
  log_param(_0);
}

template <log_level MAX_LOG_LEVEL>
template <typename... Args>
inline void log_impl<MAX_LOG_LEVEL>::log_params(
    log_level _level, size_t _flags, const Args&... args) {
  if (static_cast<size_t>(_level) > static_cast<size_t>(MAX_LOG_LEVEL)) {
    return;
  }
  if (static_cast<size_t>(_level) == 0 ||
      static_cast<size_t>(_level) > static_cast<size_t>(m_current_log_level)) {
    return;
  }
  if ((_flags & static_cast<size_t>(log_flags::no_header)) == 0) {
    log_header(_level);
  }
  do_log_log(args...);

  if ((_flags & static_cast<size_t>(log_flags::no_newline)) == 0) {
    log_newline();
  }
  if (m_flush_after_message) {
    flush();
  }
}

template <log_level MAX_LOG_LEVEL>
template <typename T0>
inline void log_impl<MAX_LOG_LEVEL>::log_param(const T0& _val) {
  if (!log_type(_val, m_log_buffer + (m_buffer_size - m_buffer_left),
          m_buffer_left)) {
    flush();
    log_type(
        _val, m_log_buffer + (m_buffer_size - m_buffer_left), m_buffer_left);
  }
}

template <log_level MAX_LOG_LEVEL>
inline void log_impl<MAX_LOG_LEVEL>::log_header(log_level _level) {
  append_color(_level, m_log_buffer + (m_buffer_size - m_buffer_left));
  log_param(klog_messages[static_cast<size_t>(_level)]);
  append_color(log_level::none, m_log_buffer + (m_buffer_size - m_buffer_left));
}

template <log_level MAX_LOG_LEVEL>
inline void log_impl<MAX_LOG_LEVEL>::append_color(
    log_level _level, char* _position) {
  if (m_num_color_elements > 0 &&
      m_color_elements[m_num_color_elements - 1].m_position == _position) {
    m_color_elements[m_num_color_elements - 1].m_level = _level;
    return;
  }
  WN_DEBUG_ASSERT(m_num_color_elements < m_buffer_size,
      "It should not be possible to have more color transition than elements");
  m_num_color_elements += 1;
  m_color_elements[m_num_color_elements - 1].m_position = _position;
  m_color_elements[m_num_color_elements - 1].m_level = _level;
}

template <log_level MAX_LOG_LEVEL>
inline void log_impl<MAX_LOG_LEVEL>::log_newline() {
  static const char* new_line = "\n";
  log_param(new_line);
}

}  // namespace logging
}  // namespace wn

#endif  // __WN_LOGGING_LOG_INL__
