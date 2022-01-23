// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_LOGGING_FILE_LOGGER_H__
#define __WN_LOGGING_FILE_LOGGER_H__

#include "WNLogging/inc/WNLog.h"

namespace wn {
namespace logging {

inline const char* default_function_name() {
  return (nullptr);
}

template <const char* (*T_FileFunction)() = default_function_name>
class file_logger : public logger {
public:
  file_logger() {
    const char* log_name = T_FileFunction();
    if (log_name) {
      m_out_file = fopen(log_name, "w+");
    } else {
      m_out_file = nullptr;
    }
  }

  ~file_logger() {
    if (m_out_file) {
      fclose(m_out_file);
      m_out_file = nullptr;
    }
  }
  void flush_buffer(
      const char* _buffer, size_t _buffer_size, const color_element*, size_t) {
    if (m_out_file) {
      size_t written = 0;
      size_t to_write = _buffer_size;
      do {
        written = fwrite(_buffer + written, 1, to_write, m_out_file);
        to_write -= written;
        if (written == 0) {
          to_write = 0;
        }
      } while (to_write > 0);
    } else {
      printf("%.*s", static_cast<int32_t>(_buffer_size), _buffer);
    }
  }

private:
  FILE* m_out_file;
};

}  // namespace logging
}  // namespace wn

#endif  // __WN_LOGGING_FILE_LOGGER_H__
