// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_LOGGING_BUFFER_LOGGER_H__
#define __WN_LOGGING_BUFFER_LOGGER_H__

#include "WNCore/inc/WNBase.h"
#include "WNLogging/inc/WNLog.h"

namespace wn {
namespace logging {

template <void (*T_FlushCB)(
    void*, const char*, size_t, const color_element*, size_t)>
class buffer_logger : public logger {
public:
  buffer_logger(void* context);
  ~buffer_logger();
  virtual WN_INLINE void flush_buffer(
      const char* _buffer, size_t buffer_size, const color_element*, size_t);

private:
  void* m_context;
};

}  // namesapce logging
}  // namespace wn

#include "WNLogging/inc/Internal/WNBufferLogger.inl"
#endif  // __WN_LOGGING_BUFFER_LOGGER_H__
