// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_LOGGING_BUFFER_LOGGER_INL__
#define __WN_LOGGING_BUFFER_LOGGER_INL__
#include "WNLogging/inc/WNBufferLogger.h"

namespace wn {
namespace logging {

template <void (*T_FlushCB)(
    void*, const char*, size_t, const color_element*, size_t)>
buffer_logger<T_FlushCB>::buffer_logger(void* _context)
  : m_context(_context) {}

template <void (*T_FlushCB)(
    void*, const char*, size_t, const color_element*, size_t)>
buffer_logger<T_FlushCB>::~buffer_logger() {}

template <void (*T_FlushCB)(
    void*, const char*, size_t, const color_element*, size_t)>
void buffer_logger<T_FlushCB>::flush_buffer(const char* _buffer,
    size_t _bufferSize, const color_element* _colors, size_t _num_colors) {
  T_FlushCB(m_context, _buffer, _bufferSize, _colors, _num_colors);
}

} // namespace logging
} // namespace wn

#endif  //__WN_LOGGING_BUFFER_LOGGER_INL__
