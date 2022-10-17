// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_CONSOLE_LOGGER_LINUX_INL__
#define __WN_CONSOLE_LOGGER_LINUX_INL__
#include "WNLogging/inc/WNConsoleLogger.h"

namespace wn {
namespace logging {

static const char* log_colors[static_cast<size_t>(log_level::max)] = {"\e[0m",
    "\e[1;4;31m", "\e[31m", "\e[33m", "\e[36m", "\e[34m", "\e[32m", "\e[35m"};

static FILE* s_files[] = {stdout, stderr};

template <console_location T_Location>
console_logger<T_Location>::console_logger() {}

template <console_location T_Location>
console_logger<T_Location>::~console_logger() {
  fprintf(s_files[static_cast<uint32_t>(T_Location)], "%s", log_colors[0]);
}

template <console_location T_Location>
void console_logger<T_Location>::flush_buffer(const char* _buffer,
    size_t _buffer_size, const color_element* _colors, size_t _num_colors) {
  if (_num_colors > 0) {
    {
      size_t len = _colors[0].m_position - _buffer;
      fprintf(s_files[static_cast<uint32_t>(T_Location)], "%.*s",
          static_cast<int32_t>(len), _buffer);
    }

    for (size_t i = 0; i < _num_colors; ++i) {
      fprintf(s_files[static_cast<uint32_t>(T_Location)], "%s",
          log_colors[static_cast<size_t>(_colors[i].m_level)]);
      const char* end_color = (_num_colors == i + 1)
                                  ? _buffer + _buffer_size
                                  : _colors[i + 1].m_position;
      size_t len = end_color - _colors[i].m_position;
      fprintf(s_files[static_cast<uint32_t>(T_Location)], "%.*s",
          static_cast<int32_t>(len), _colors[i].m_position);
    }
  } else {
    fprintf(s_files[static_cast<uint32_t>(T_Location)], "%.*s",
        static_cast<int32_t>(_buffer_size), _buffer);
  }
}

}  // namespace logging
}  // namespace wn

#endif  //__WN_CONSOLE_LOGGER_LINUX_INL__
