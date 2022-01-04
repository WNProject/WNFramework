// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_CONSOLE_LOGGER_WINDOWS_INL__
#define __WN_CONSOLE_LOGGER_WINDOWS_INL__
#include "WNLogging/inc/WNConsoleLogger.h"

namespace wn {
namespace logging {

#define __WN_FOREGROUND_WHITE                                                  \
  FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN
const static uint16_t log_colors[static_cast<size_t>(log_level::max)] = {
    __WN_FOREGROUND_WHITE, FOREGROUND_RED | FOREGROUND_INTENSITY,
    FOREGROUND_RED, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    FOREGROUND_RED | FOREGROUND_GREEN, FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    FOREGROUND_GREEN | FOREGROUND_INTENSITY};

static DWORD s_standard_handles[] = {STD_OUTPUT_HANDLE, STD_ERROR_HANDLE};

template <console_location T_Location>
console_logger<T_Location>::console_logger()
  : m_console_created(false),
    m_console_handle(INVALID_HANDLE_VALUE),
    m_initialized(false) {}

template <console_location T_Location>
console_logger<T_Location>::~console_logger() {
  if (m_console_created) {
    FreeConsole();
  }
  if (m_console_handle != INVALID_HANDLE_VALUE) {
    SetConsoleTextAttribute(m_console_handle, __WN_FOREGROUND_WHITE);
  }
}

inline void write_section_to_console(
    HANDLE _handle, const char* _buffer, size_t _amount) {
  if (_amount == 0) {
    return;
  }
  DWORD written = 0;
  DWORD to_write = static_cast<DWORD>(_amount);
  do {
    if (!WriteConsoleA(
            _handle, _buffer, static_cast<DWORD>(_amount), &written, 0)) {
      return;
    } else {
      to_write -= written;
    }
  } while (to_write > 0);
}

template <console_location T_Location>
void console_logger<T_Location>::flush_buffer(const char* _buffer,
    size_t _buffer_size, const color_element* _colors, size_t _num_colors) {
  if (m_console_handle == INVALID_HANDLE_VALUE && !m_initialized) {
    m_console_created = AllocConsole();  // If we cannot create a new console,
                                         // we must already have one.. hopefully
    m_console_handle = GetStdHandle(s_standard_handles[T_Location]);
    m_initialized = true;
  }
  if (m_console_handle != INVALID_HANDLE_VALUE) {
    if (_num_colors > 0) {
      {
        size_t len = _colors[0].m_position - _buffer;
        write_section_to_console(m_console_handle, _buffer, len);
      }

      for (size_t i = 0; i < _num_colors; ++i) {
        SetConsoleTextAttribute(m_console_handle,
            log_colors[static_cast<uint32_t>(_colors[i].m_level)]);
        const char* end_color = (_num_colors == i + 1)
                                    ? _buffer + _buffer_size
                                    : _colors[i + 1].m_position;

        size_t len = end_color - _colors[i].m_position;
        write_section_to_console(m_console_handle, _colors[i].m_position, len);
      }
    } else {
      size_t len = _buffer_size;
      write_section_to_console(m_console_handle, _buffer, len);
    }
  }
}

}  // namespace logging
}  // namespace wn

#endif  //__WN_CONSOLE_LOGGER_WINDOWS_INL__
