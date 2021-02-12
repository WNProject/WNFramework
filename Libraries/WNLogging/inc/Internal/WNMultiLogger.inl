// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_LOGGING_MULTI_LOGGER_H__
#define __WN_LOGGING_MULTI_LOGGER_H__
#include "WNLogging/inc/WNMultiLogger.h"

namespace wn {
namespace logging {

template <typename T_Logger1, typename T_Logger2>
logging::multi_logger<T_Logger1, T_Logger2>::~multi_logger() {}

template <typename T_Logger1, typename T_Logger2>
void logging::multi_logger<T_Logger1, T_Logger2>::flush_buffer(
    const char* _buffer, size_t _buffer_size, const color_element* _colors,
    size_t _num_colors) {
  m_logger_1.flush_buffer(_buffer, _buffer_size, _colors, _num_colors);
  m_logger_2.flush_buffer(_buffer, _buffer_size, _colors, _num_colors);
}

}  // namespace logging
}  // namespace wn

#endif  //__WN_LOGGING_MULTI_LOGGER_H__
