// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_LOGGING_CONSOLE_LOGGER_H__
#define __WN_LOGGING_CONSOLE_LOGGER_H__

#include "WNLogging/inc/WNLog.h"
#include "core/inc/base.h"

namespace wn {
namespace logging {

enum console_location { std_out, std_err };
template <console_location T_Location = std_out>
class console_logger : public logger {
public:
  console_logger();
  ~console_logger();
  virtual void flush_buffer(
      const char* _buffer, size_t bufferSize, const color_element*, size_t);

private:
#if defined(_WN_WINDOWS)
  HANDLE m_console_handle;
  BOOL m_console_created;
  BOOL m_initialized;
#elif defined(_WN_ANDROID)
  int32_t m_log_priority;
#endif
};

}  // namespace logging
}  // namespace wn

#ifdef _WN_WINDOWS
#include "WNLogging/inc/Internal/WNConsoleLoggerWindows.inl"
#elif defined _WN_ANDROID
#include "WNLogging/inc/Internal/WNConsoleLoggerAndroid.inl"
#else
#include "WNLogging/inc/Internal/WNConsoleLoggerLinux.inl"
#endif

#endif  // __WN_LOGGING_CONSOLE_LOGGER_H__
