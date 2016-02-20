// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_LOG_H__
#define __WN_LOG_H__
#include <vector>
#include "WNCore/inc/WNAssert.h"
#include "WNCore/inc/WNTypes.h"
#include "WNMemory/inc/WNStringUtility.h"

#define LTM(N) typename T##N
#define LVM(N) T##N v##N
namespace WNLogging {
enum WNLogLevel {  // starting at 1 so that we can turn off with 0
  eNone = 0,
  eCritical,
  eError,
  eWarning,
  eIssue,
  eInfo,
  eDebug,
  eLogMax
};

static const char* LogMessages[WNLogging::eLogMax] =
    {  //-1 here because log levels start at 0
        "None:", "Critial:", "Error:", "Warning:", "Issue:", "Info:", "Debug:"};

enum WNLogFlags { eWNNoNewLine = 1 << 0, eWNNoHeader = 1 << 1 };

struct WNLogColorElement {
  WNLogLevel mLevel;
  char* mPosition;
};

class WNLogger {
public:
  virtual void FlushBuffer(const char* _buffer, size_t bufferSize,
      const std::vector<WNLogColorElement>& mColors) = 0;
};

#define LOGGER_ENCODING_HELPER(type, formatter)                                \
  template <typename T>                                                        \
  struct _Enc##type {                                                          \
    static const char* GetVal() {                                              \
      return formatter;                                                        \
    }                                                                          \
  };                                                                           \
  template <>                                                                  \
  struct _Enc##type<wchar_t> {                                                 \
    static const wchar_t* GetVal() {                                           \
      return L##formatter;                                                     \
    }                                                                          \
  };

LOGGER_ENCODING_HELPER(Ptr, "%p");
template <typename T, typename BuffType>
struct LogTypeHelper {
  WN_FORCE_INLINE static bool DoLog(
      const T& _0, BuffType* _buffer, size_t& _bufferLeft) {
    int printed = wn::memory::snprintf(
        _buffer, _bufferLeft, _EncPtr<BuffType>::GetVal(), _0);
    if (printed < 0 || static_cast<size_t>(printed) >= _bufferLeft) {
      return (false);
    }
    _bufferLeft -= printed;
    return (true);
  }
};

template <typename T, typename BuffType>
WN_FORCE_INLINE bool LogType(
    const T& _0, BuffType* _buffer, size_t& _bufferLeft) {
  return (LogTypeHelper<T, BuffType>::DoLog(_0, _buffer, _bufferLeft));
}

class WNLogElem {
  friend class WNLog;
  enum WNLogAmount { eWNFALSE, eWNTRUE };
};

class WNLog {
public:
  WNLog(WNLogger* logger, WNLogLevel lvl = eError, size_t size = 1024,
      bool flush = false)
    : mLogger(logger) {
    mBufferSize = size;
    mBufferLeft = mBufferSize;
    mCurrentLogLevel = lvl;
    mLogBuffer = static_cast<char*>(malloc(mBufferSize));
    mFlushAfterMessage = flush;
  }

  ~WNLog() {
    Flush();
  }
  WN_FORCE_INLINE void SetLogLevel(WNLogLevel _level) {
    mCurrentLogLevel = _level;
  }
  WN_FORCE_INLINE void Flush();
  WN_FORCE_INLINE void FlushExternal(const char* _buffer, size_t _bufferSize,
      const std::vector<WNLogColorElement>& mColors);

  template <LTM(0)>
  WN_FORCE_INLINE void Log(WNLogLevel, size_t, LVM(0));
  template <LTM(0), LTM(1)>
  WN_FORCE_INLINE void Log(WNLogLevel, size_t, LVM(0), LVM(1));
  template <LTM(0), LTM(1), LTM(2)>
  WN_FORCE_INLINE void Log(WNLogLevel, size_t, LVM(0), LVM(1), LVM(2));
  template <LTM(0), LTM(1), LTM(2), LTM(3)>
  WN_FORCE_INLINE void Log(WNLogLevel, size_t, LVM(0), LVM(1), LVM(2), LVM(3));
  template <LTM(0), LTM(1), LTM(2), LTM(3), LTM(4)>
  WN_FORCE_INLINE void Log(
      WNLogLevel, size_t, LVM(0), LVM(1), LVM(2), LVM(3), LVM(4));
  template <LTM(0), LTM(1), LTM(2), LTM(3), LTM(4), LTM(5)>
  WN_FORCE_INLINE void Log(
      WNLogLevel, size_t, LVM(0), LVM(1), LVM(2), LVM(3), LVM(4), LVM(5));
  template <LTM(0), LTM(1), LTM(2), LTM(3), LTM(4), LTM(5), LTM(6)>
  WN_FORCE_INLINE void Log(WNLogLevel, size_t, LVM(0), LVM(1), LVM(2), LVM(3),
      LVM(4), LVM(5), LVM(6));
  template <LTM(0), LTM(1), LTM(2), LTM(3), LTM(4), LTM(5), LTM(6), LTM(7)>
  WN_FORCE_INLINE void Log(WNLogLevel, size_t, LVM(0), LVM(1), LVM(2), LVM(3),
      LVM(4), LVM(5), LVM(6), LVM(7));
  template <LTM(0), LTM(1), LTM(2), LTM(3), LTM(4), LTM(5), LTM(6), LTM(7),
      LTM(8)>
  WN_FORCE_INLINE void Log(WNLogLevel, size_t, LVM(0), LVM(1), LVM(2), LVM(3),
      LVM(4), LVM(5), LVM(6), LVM(7), LVM(8));
  template <LTM(0), LTM(1), LTM(2), LTM(3), LTM(4), LTM(5), LTM(6), LTM(7),
      LTM(8), LTM(9)>
  WN_FORCE_INLINE void Log(WNLogLevel, size_t, LVM(0), LVM(1), LVM(2), LVM(3),
      LVM(4), LVM(5), LVM(6), LVM(7), LVM(8), LVM(9));

private:
  WNLog& operator=(const WNLog& _other);  // dont want an assignment
  WN_FORCE_INLINE void LogHeader(WNLogLevel);
  WN_FORCE_INLINE void LogNewline();
  template <typename T0>
  void LogParam(const T0& _val);

  WNLogger* mLogger;
  char* mLogBuffer;
  size_t mBufferLeft;
  size_t mBufferSize;
  std::vector<WNLogColorElement> mColorElements;
  size_t mCurrentLogLevel;
  bool mFlushAfterMessage;
};

WN_FORCE_INLINE WNLog* get_null_logger() {
  static WNLog m_null_logger(nullptr, eNone, 0, false);
  return &m_null_logger;
}
}
#include "WNLogging/inc/WNDefaultLogTypes.inl"
#include "WNLogging/inc/WNLog.inl"
#endif  //__WN_LOG_H__
