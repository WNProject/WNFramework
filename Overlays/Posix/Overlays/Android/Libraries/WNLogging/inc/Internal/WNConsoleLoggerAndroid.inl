// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_CONSOLE_LOGGER_LINUX_INL__
#define __WN_CONSOLE_LOGGER_LINUX_INL__

#include <android/log.h>

#include "WNLogging/inc/WNConsoleLogger.h"
#include "WNUtilities/inc/WNLoggingData.h"

namespace wn {
namespace logging {

const int32_t s_log_levels[] = {ANDROID_LOG_SILENT, ANDROID_LOG_FATAL,
    ANDROID_LOG_ERROR, ANDROID_LOG_WARN, ANDROID_LOG_WARN, ANDROID_LOG_INFO,
    ANDROID_LOG_DEBUG, ANDROID_LOG_VERBOSE, ANDROID_LOG_DEFAULT};

template <console_location T_Location>
console_logger<T_Location>::console_logger()
  : m_log_priority(ANDROID_LOG_INFO) {}

template <console_location T_Location>
console_logger<T_Location>::~console_logger() {}

template <console_location T_Location>
void console_logger<T_Location>::flush_buffer(const char* _buffer,
    size_t _buffer_size, const color_element* _colors, size_t _num_colors) {
  if (_num_colors) {
    for (size_t i = 0; i < _num_colors; ++i) {
      if (static_cast<uint32_t>(_colors[i].m_level) > 0) {
        m_log_priority = static_cast<uint32_t>(_colors[i].m_level);
      } else {
        const char* end_color = (_num_colors == i + 1)
                                    ? _buffer + _buffer_size
                                    : _colors[i + 1].m_position;
        size_t len = end_color - _colors[i].m_position;
        __android_log_print(s_log_levels[static_cast<uint32_t>(m_log_priority)],
            wn::utilities::gAndroidLogTag, "%.*s", static_cast<int32_t>(len),
            _colors[i].m_position);
      }
    }
  } else {
    __android_log_print(s_log_levels[static_cast<uint32_t>(m_log_priority)],
        wn::utilities::gAndroidLogTag, "%.*s", static_cast<int>(_buffer_size),
        _buffer);
  }
}

}  // namespace logging
}  // namespace wn

#endif  //__WN_CONSOLE_LOGGER_LINUX_INL__
