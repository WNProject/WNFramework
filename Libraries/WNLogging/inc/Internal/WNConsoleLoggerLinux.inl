#ifndef __WN_CONSOLE_LOGGER_LINUX_INL__
#define __WN_CONSOLE_LOGGER_LINUX_INL__
#include "WNLogging/inc/WNConsoleLogger.h"

static const WN_CHAR* LogColors[WNLogging::eLogMax] = {
    "\e[0m",
    "\e[1;4;31m",
    "\e[31m",
    "\e[33m",
    "\e[36m",
    "\e[34m",
    "\e[32m"
};

static FILE* mFiles[] = {
    stdout,
    stderr
};

template<WNLogging::WNConsoleLocation T_Level>
WNLogging::WNConsoleLogger<T_Level>::WNConsoleLogger() {
}

template<WNLogging::WNConsoleLocation T_Level>
WNLogging::WNConsoleLogger<T_Level>::~WNConsoleLogger() {
    fprintf(mFiles[T_Level], "%s", LogColors[0]); 
}

template<WNLogging::WNConsoleLocation T_Level>
WN_VOID WNLogging::WNConsoleLogger<T_Level>::FlushBuffer(const WN_CHAR* _buffer, WN_SIZE_T _bufferSize, const  std::vector<WNLogging::WNLogColorElement>& _colors) {
    if(_colors.size() > 0) {
        {
            WN_SIZE_T len =  _colors.front().mPosition - _buffer;
            fprintf(mFiles[T_Level], "%.*s", static_cast<WN_INT32>(len), _buffer);
        }
            
        for(WN_SIZE_T i = 0; i < _colors.size(); ++i) {
            fprintf(mFiles[T_Level], "%s", LogColors[(_colors)[i].mLevel]);
            const WN_CHAR* endColor = ((_colors).size() == i+1)? _buffer + _bufferSize: (_colors)[i+1].mPosition;
            WN_SIZE_T len =  endColor - (_colors)[i].mPosition;
            fprintf(mFiles[T_Level], "%.*s", static_cast<WN_INT32>(len), (_colors)[i].mPosition);
        }
    } else {
        fprintf(mFiles[T_Level], "%.*s", static_cast<WN_INT32>(_bufferSize), _buffer);
    }
}

#endif//__WN_CONSOLE_LOGGER_LINUX_INL__
