// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_LOGGING_MILTI_LOGGER_H__
#define __WN_LOGGING_MILTI_LOGGER_H__
#include "WNLogging/inc/WNLog.h"

namespace wn {
namespace logging {

template <typename T_Logger1, typename T_Logger2>
class multi_logger : public logger {
public:
  ~multi_logger();
  void flush_buffer(const char* _buffer, size_t _buffer_size,
      const color_element* _colors, size_t _num_colors);
  logger* l1() {
    return &m_logger_1;
  }
  logger* l2() {
    return &m_logger_2;
  }

private:
  T_Logger1 m_logger_1;
  T_Logger2 m_logger_2;
};

}  // namespace logging
}  // namespace wn

#include "WNLogging/inc/Internal/WNMultiLogger.inl"
#endif  // __WN_LOGGING_MILTI_LOGGER_H__
