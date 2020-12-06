// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_LOGGING_BUFFER_LOGGER_H__
#define __WN_LOGGING_BUFFER_LOGGER_H__

#pragma once

#include "WNLogging/inc/WNLog.h"
#include "core/inc/base.h"

namespace wn {
namespace logging {

template <void (*FlushCB)(
    void*, const char*, size_t, const color_element*, size_t)>
class buffer_logger : public logger {
public:
  buffer_logger(void* _context) : m_context(_context) {}

  ~buffer_logger() = default;

  virtual void flush_buffer(const char* _buffer, size_t _buffer_size,
      const color_element* _colors, size_t _num_colors) override {
    FlushCB(m_context, _buffer, _buffer_size, _colors, _num_colors);
  }

private:
  void* m_context;
};

}  // namespace logging
}  // namespace wn

#endif  // __WN_LOGGING_BUFFER_LOGGER_H__
