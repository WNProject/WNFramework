// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_LOG_H__
#define __WN_LOG_H__
#include "WNCore/inc/WNAssert.h"
#include "WNCore/inc/WNTypes.h"
#include "WNMemory/inc/WNStringUtility.h"
#include <vector>


#define LTM(N) typename T##N
#define LVM(N) T##N v##N
namespace WNLogging {
    enum WNLogLevel { //starting at 1 so that we can turn off with 0
        eNone = 0,
        eCritical,
        eError,
        eWarning,
        eIssue,
        eInfo,
        eDebug,
        eLogMax
    };

    static const char* LogMessages[WNLogging::eLogMax] = { //-1 here because log levels start at 0
        "None:",
        "Critial:",
        "Error:",
        "Warning:",
        "Issue:",
        "Info:",
        "Debug:"
    };

    enum WNLogFlags {
        eWNNoNewLine = 1<<0,
        eWNNoHeader = 1<<1
    };


    struct WNLogColorElement {
        WNLogLevel mLevel;
        wn_char* mPosition;
    };

    class WNLogger {
    public:
        virtual wn_void FlushBuffer(const wn_char* _buffer, wn_size_t bufferSize, const std::vector<WNLogColorElement>& mColors) = 0;
    };

    #define LOGGER_ENCODING_HELPER(type, formatter) \
    template<typename T> \
    struct _Enc##type { static const wn_char* GetVal(){return formatter;} }; \
    template<> struct _Enc##type<wn_wchar> { static const wn_wchar* GetVal(){return L##formatter;} };

    LOGGER_ENCODING_HELPER(Ptr, "%p");
    template<typename T, typename BuffType>
    struct LogTypeHelper {
        WN_FORCE_INLINE static wn_bool DoLog(const T& _0, BuffType* _buffer, wn_size_t& _bufferLeft) {
            int printed = wn::memory::snprintf(_buffer, _bufferLeft, _EncPtr<BuffType>::GetVal(), _0);
            if(printed < 0 || static_cast<wn_size_t>(printed) >= _bufferLeft) {
                return(wn_false);
            }
            _bufferLeft -= printed;
            return(wn_true);
        }
    };

    template<typename T, typename BuffType>
    WN_FORCE_INLINE wn_bool LogType(const T&_0, BuffType* _buffer, wn_size_t& _bufferLeft) {
        return(LogTypeHelper<T, BuffType>::DoLog(_0, _buffer, _bufferLeft));
    }

    class WNLogElem {
        friend class WNLog;
        enum WNLogAmount {
            eWNFALSE,
            eWNTRUE
        };
    };

    class WNLog {
    public:
     WNLog(WNLogger* logger, WNLogLevel lvl = eError, wn_size_t size = 1024,
           wn_bool flush = wn_false)
         : mLogger(logger) {
            mBufferSize = size;
            mBufferLeft = mBufferSize;
            mCurrentLogLevel = lvl;
            mLogBuffer = static_cast<wn_char*>(malloc(mBufferSize));
            mFlushAfterMessage = flush;
        }

        ~WNLog() { Flush(); }
        WN_FORCE_INLINE wn_void SetLogLevel(WNLogLevel _level) { mCurrentLogLevel = _level; }
        WN_FORCE_INLINE wn_void Flush();
        WN_FORCE_INLINE wn_void FlushExternal(const wn_char* _buffer, wn_size_t _bufferSize, const std::vector<WNLogColorElement>& mColors);

        template<LTM(0)>
        WN_FORCE_INLINE wn_void Log(WNLogLevel, wn_size_t, LVM(0));
        template<LTM(0), LTM(1)>
        WN_FORCE_INLINE wn_void Log(WNLogLevel, wn_size_t, LVM(0), LVM(1));
        template<LTM(0), LTM(1), LTM(2)>
        WN_FORCE_INLINE wn_void Log(WNLogLevel, wn_size_t, LVM(0), LVM(1), LVM(2));
        template<LTM(0), LTM(1), LTM(2), LTM(3)>
        WN_FORCE_INLINE wn_void Log(WNLogLevel, wn_size_t, LVM(0), LVM(1), LVM(2), LVM(3));
        template<LTM(0), LTM(1), LTM(2), LTM(3), LTM(4)>
        WN_FORCE_INLINE wn_void Log(WNLogLevel, wn_size_t, LVM(0), LVM(1), LVM(2), LVM(3), LVM(4));
        template<LTM(0), LTM(1), LTM(2), LTM(3), LTM(4), LTM(5)>
        WN_FORCE_INLINE wn_void Log(WNLogLevel, wn_size_t, LVM(0), LVM(1), LVM(2), LVM(3), LVM(4), LVM(5));
        template<LTM(0), LTM(1), LTM(2), LTM(3), LTM(4), LTM(5), LTM(6)>
        WN_FORCE_INLINE wn_void Log(WNLogLevel, wn_size_t, LVM(0), LVM(1), LVM(2), LVM(3), LVM(4), LVM(5), LVM(6));
        template<LTM(0), LTM(1), LTM(2), LTM(3), LTM(4), LTM(5), LTM(6), LTM(7)>
        WN_FORCE_INLINE wn_void Log(WNLogLevel, wn_size_t, LVM(0), LVM(1), LVM(2), LVM(3), LVM(4), LVM(5), LVM(6), LVM(7));
        template<LTM(0), LTM(1), LTM(2), LTM(3), LTM(4), LTM(5), LTM(6), LTM(7), LTM(8)>
        WN_FORCE_INLINE wn_void Log(WNLogLevel, wn_size_t, LVM(0), LVM(1), LVM(2), LVM(3), LVM(4), LVM(5), LVM(6), LVM(7), LVM(8));
        template<LTM(0), LTM(1), LTM(2), LTM(3), LTM(4), LTM(5), LTM(6), LTM(7), LTM(8), LTM(9)>
        WN_FORCE_INLINE wn_void Log(WNLogLevel, wn_size_t, LVM(0), LVM(1), LVM(2), LVM(3), LVM(4), LVM(5), LVM(6), LVM(7), LVM(8), LVM(9));
    private:
        WNLog& operator=(const WNLog& _other); // dont want an assignment
        WN_FORCE_INLINE wn_void LogHeader(WNLogLevel);
        WN_FORCE_INLINE wn_void LogNewline();
        template<typename T0> wn_void LogParam(const T0& _val);

        WNLogger* mLogger;
        wn_char* mLogBuffer;
        wn_size_t mBufferLeft;
        wn_size_t mBufferSize;
        std::vector<WNLogColorElement> mColorElements;
        wn_size_t mCurrentLogLevel;
        wn_bool mFlushAfterMessage;
    };

    WN_FORCE_INLINE WNLog* get_null_logger() {
      static WNLog m_null_logger(wn_nullptr, eNone, 0, wn_false);
      return &m_null_logger;
    }
}
#include "WNLogging/inc/WNLog.inl"
#include "WNLogging/inc/WNDefaultLogTypes.inl"
#endif//__WN_LOG_H__
