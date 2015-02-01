#ifndef __WN_CONSOLE_LOGGER_LINUX_INL__
#define __WN_CONSOLE_LOGGER_LINUX_INL__
#include "WNLogging/inc/WNConsoleLogger.h"
#include "WNUtils/inc/Android/WNLoggingData.h"
#include <android/log.h>
const WN_INT32 logLevels[] = {
    ANDROID_LOG_SILENT,
    ANDROID_LOG_FATAL,
    ANDROID_LOG_ERROR,
    ANDROID_LOG_WARN,
    ANDROID_LOG_WARN,
    ANDROID_LOG_INFO,
    ANDROID_LOG_DEBUG,
    ANDROID_LOG_DEFAULT
};

template<WNLogging::WNConsoleLocation T_Level>
WNLogging::WNConsoleLogger<T_Level>::WNConsoleLogger(): mLogPriority(ANDROID_LOG_INFO) {
}

template<WNLogging::WNConsoleLocation T_Level>
WNLogging::WNConsoleLogger<T_Level>::~WNConsoleLogger() {
}

template<WNLogging::WNConsoleLocation T_Level>
WN_VOID WNLogging::WNConsoleLogger<T_Level>::FlushBuffer(const WN_CHAR* _buffer, WN_SIZE_T _bufferSize, const  std::vector<WNLogging::WNLogColorElement>& _colors) {
    if(_colors.size() > 0) {
           
        for(WN_SIZE_T i = 0; i < _colors.size(); ++i) {
            if(_colors[i].mLevel > 0)
            {
                mLogPriority = _colors[i].mLevel;
            }
            else
            {
                const WN_CHAR* endColor = ((_colors).size() == i+1)? _buffer + _bufferSize: (_colors)[i+1].mPosition;
                WN_SIZE_T len =  endColor - (_colors)[i].mPosition;
                __android_log_print(mLogPriority, WNUtils::gAndroidLogTag, "%.*s", static_cast<WN_INT32>(len), (_colors)[i].mPosition);
            }
        }
    } else {
        __android_log_print(mLogPriority, WNUtils::gAndroidLogTag, "%.*s", (_bufferSize), _buffer);
    }
}

#endif//__WN_CONSOLE_LOGGER_LINUX_INL__
