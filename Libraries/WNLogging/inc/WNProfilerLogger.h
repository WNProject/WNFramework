// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_LOGGING_PROFILER_LOGGER_H__
#define __WN_LOGGING_PROFILER_LOGGER_H__
#include "WNLogging/inc/WNLog.h"

namespace wn {
namespace logging {

class profiler_logger : public logger {
public:
  void flush_buffer(const char* _buffer, size_t _buffer_size,
      const color_element* _colors, size_t _num_colors);

private:
  log_level m_last_level = log_level::none;
  char m_temp_buffer[1024];
  size_t m_temp_buffer_offset = 0;
};

}  // namespace logging
}  // namespace wn

#endif  // __WN_LOGGING_PROFILER_LOGGER_H__
