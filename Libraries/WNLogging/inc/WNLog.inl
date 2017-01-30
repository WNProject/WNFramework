// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_LOG_INL__
#define __WN_LOG_INL__
#include "WNCore/inc/WNAssert.h"
#include "WNCore/inc/WNTypes.h"
#include "WNLogging/inc/WNLog.h"

#define CHECK_LOG()                                                            \
  if (static_cast<size_t>(_level) == 0 ||                                      \
      static_cast<size_t>(_level) > mCurrentLogLevel) {                        \
    return;                                                                    \
  }                                                                            \
  if ((_flags & eWNNoHeader) == 0) {                                           \
    LogHeader(_level);                                                         \
  }
#define LOG_END()                                                              \
  if ((_flags & eWNNoNewLine) == 0) {                                          \
    LogNewline();                                                              \
  }                                                                            \
  if (mFlushAfterMessage) {                                                    \
    Flush();                                                                   \
  }

WN_FORCE_INLINE void WNLogging::WNLog::Flush() {
  if (mBufferSize != mBufferLeft) {
    mLogger->FlushBuffer(mLogBuffer, mBufferSize - mBufferLeft, mColorElements);
    mColorElements.clear();
    mBufferLeft = mBufferSize;
  }
}

WN_FORCE_INLINE void WNLogging::WNLog::FlushExternal(const char* _buffer,
    size_t _bufferSize, const std::vector<WNLogColorElement>& _colors) {
  mLogger->FlushBuffer(_buffer, _bufferSize, _colors);
}

template <LTM(0)>
WN_FORCE_INLINE void WNLogging::WNLog::Log(
    WNLogging::WNLogLevel _level, size_t _flags, LVM(0)) {
  CHECK_LOG();
  LogParam(v0);
  LOG_END();
}

template <LTM(0), LTM(1)>
WN_FORCE_INLINE void WNLogging::WNLog::Log(
    WNLogging::WNLogLevel _level, size_t _flags, LVM(0), LVM(1)) {
  CHECK_LOG();
  LogParam(v0);
  LogParam(v1);
  LOG_END();
}

template <LTM(0), LTM(1), LTM(2)>
WN_FORCE_INLINE void WNLogging::WNLog::Log(
    WNLogging::WNLogLevel _level, size_t _flags, LVM(0), LVM(1), LVM(2)) {
  CHECK_LOG();
  LogParam(v0);
  LogParam(v1);
  LogParam(v2);
  LOG_END();
}

template <LTM(0), LTM(1), LTM(2), LTM(3)>
WN_FORCE_INLINE void WNLogging::WNLog::Log(WNLogging::WNLogLevel _level,
    size_t _flags, LVM(0), LVM(1), LVM(2), LVM(3)) {
  CHECK_LOG();
  LogParam(v0);
  LogParam(v1);
  LogParam(v2);
  LogParam(v3);
  LOG_END();
}

template <LTM(0), LTM(1), LTM(2), LTM(3), LTM(4)>
WN_FORCE_INLINE void WNLogging::WNLog::Log(WNLogging::WNLogLevel _level,
    size_t _flags, LVM(0), LVM(1), LVM(2), LVM(3), LVM(4)) {
  CHECK_LOG();
  LogParam(v0);
  LogParam(v1);
  LogParam(v2);
  LogParam(v3);
  LogParam(v4);
  LOG_END();
}

template <LTM(0), LTM(1), LTM(2), LTM(3), LTM(4), LTM(5)>
WN_FORCE_INLINE void WNLogging::WNLog::Log(WNLogging::WNLogLevel _level,
    size_t _flags, LVM(0), LVM(1), LVM(2), LVM(3), LVM(4), LVM(5)) {
  CHECK_LOG();
  LogParam(v0);
  LogParam(v1);
  LogParam(v2);
  LogParam(v3);
  LogParam(v4);
  LogParam(v5);
  LOG_END();
}

template <LTM(0), LTM(1), LTM(2), LTM(3), LTM(4), LTM(5), LTM(6)>
WN_FORCE_INLINE void WNLogging::WNLog::Log(WNLogging::WNLogLevel _level,
    size_t _flags, LVM(0), LVM(1), LVM(2), LVM(3), LVM(4), LVM(5), LVM(6)) {
  CHECK_LOG();
  LogParam(v0);
  LogParam(v1);
  LogParam(v2);
  LogParam(v3);
  LogParam(v4);
  LogParam(v5);
  LogParam(v6);
  LOG_END();
}

template <LTM(0), LTM(1), LTM(2), LTM(3), LTM(4), LTM(5), LTM(6), LTM(7)>
WN_FORCE_INLINE void WNLogging::WNLog::Log(WNLogging::WNLogLevel _level,
    size_t _flags, LVM(0), LVM(1), LVM(2), LVM(3), LVM(4), LVM(5), LVM(6),
    LVM(7)) {
  CHECK_LOG();
  LogParam(v0);
  LogParam(v1);
  LogParam(v2);
  LogParam(v3);
  LogParam(v4);
  LogParam(v5);
  LogParam(v6);
  LogParam(v7);
  LOG_END();
}

template <LTM(0), LTM(1), LTM(2), LTM(3), LTM(4), LTM(5), LTM(6), LTM(7),
    LTM(8)>
WN_FORCE_INLINE void WNLogging::WNLog::Log(WNLogging::WNLogLevel _level,
    size_t _flags, LVM(0), LVM(1), LVM(2), LVM(3), LVM(4), LVM(5), LVM(6),
    LVM(7), LVM(8)) {
  CHECK_LOG();
  LogParam(v0);
  LogParam(v1);
  LogParam(v2);
  LogParam(v3);
  LogParam(v4);
  LogParam(v5);
  LogParam(v6);
  LogParam(v7);
  LogParam(v8);
  LOG_END();
}

template <LTM(0), LTM(1), LTM(2), LTM(3), LTM(4), LTM(5), LTM(6), LTM(7),
    LTM(8), LTM(9)>
WN_FORCE_INLINE void WNLogging::WNLog::Log(WNLogging::WNLogLevel _level,
    size_t _flags, LVM(0), LVM(1), LVM(2), LVM(3), LVM(4), LVM(5), LVM(6),
    LVM(7), LVM(8), LVM(9)) {
  CHECK_LOG();
  LogParam(v0);
  LogParam(v1);
  LogParam(v2);
  LogParam(v3);
  LogParam(v4);
  LogParam(v5);
  LogParam(v6);
  LogParam(v7);
  LogParam(v8);
  LogParam(v9);
  LOG_END();
}

template <typename T0>
void WNLogging::WNLog::LogParam(const T0& _val) {
  if (!LogType(_val, mLogBuffer + (mBufferSize - mBufferLeft), mBufferLeft)) {
    Flush();
    LogType(_val, mLogBuffer + (mBufferSize - mBufferLeft), mBufferLeft);
  }
}

WN_FORCE_INLINE void WNLogging::WNLog::LogHeader(WNLogLevel _level) {
  mColorElements.push_back(WNLogColorElement());
  mColorElements.back().mLevel = _level;
  mColorElements.back().mPosition = mLogBuffer + (mBufferSize - mBufferLeft);
  LogParam(LogMessages[_level]);
  mColorElements.push_back(WNLogColorElement());
  mColorElements.back().mLevel = WNLogging::eNone;
  mColorElements.back().mPosition = mLogBuffer + (mBufferSize - mBufferLeft);
}

WN_FORCE_INLINE void WNLogging::WNLog::LogNewline() {
  static const char* newLine = "\n";
  LogParam(newLine);
}

#endif  //__WN_LOG_INL__
