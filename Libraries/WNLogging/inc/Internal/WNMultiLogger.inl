// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_LOGGING_MULTI_LOGGER_H__
#define __WN_LOGGING_MULTI_LOGGER_H__
#include "WNLogging/inc/WNMultiLogger.h"

namespace wn {
namespace logging {

template <typename T_Logger1, typename T_Logger2, int T_BufferSize>
logging::multi_logger<T_Logger1, T_Logger2, T_BufferSize>::~multi_logger() {
}

template <typename T_Logger1, typename T_Logger2, int T_BufferSize>
void logging::multi_logger<T_Logger1, T_Logger2,
    T_BufferSize>::flush_external(const char* _buffer, size_t _buffer_size,
    color_element* _colors, size_t num_color) {
  mLogger1.FlushExternal(_buffer, _buffer_size, _colors, num_colors);
  mLogger2.FlushExternal(_buffer, _buffer_size, _colors, num_colors);
}

} // namespace logging
} // namespace wn

#endif  //__WN_LOGGING_MULTI_LOGGER_H__
