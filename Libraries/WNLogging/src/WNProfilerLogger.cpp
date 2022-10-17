// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNLogging/inc/WNProfilerLogger.h"
#include "profiling/inc/profiling.h"

namespace wn {
namespace logging {
// 0xRRGGBB
const static uint32_t log_colors[static_cast<size_t>(log_level::max)] = {
    0xFFFFFF, 0xFF0000, 0xEF0000, 0xFFFF00, 0xEFEF00, 0x0000FF, 0x00FF00,
    0x00FFFF};

void profiler_logger::flush_buffer(const char* _buffer, size_t _buffer_size,
    const color_element* _colors, size_t _num_colors) {
  if (profiling::is_profiling_enabled()) {
    (void)log_colors;
    size_t color = 0;
    for (size_t i = 0; i < _buffer_size; ++i) {
      if (color < _num_colors &&
          static_cast<size_t>(_colors[color].m_position - _buffer) == i &&
          _colors[color].m_level != log_level::none) {
        m_last_level = _colors[color].m_level;
        ++color;
      }
      if (_buffer[i] == '\n' || m_temp_buffer_offset > 1023) {
        PROFILE_MESSAGE(log_colors[static_cast<size_t>(m_last_level)],
            m_temp_buffer, m_temp_buffer_offset);
        m_temp_buffer_offset = 0;
        continue;
      }
      m_temp_buffer[m_temp_buffer_offset++] = _buffer[i];
    }
  }
}
}  // namespace logging
}  // namespace wn
