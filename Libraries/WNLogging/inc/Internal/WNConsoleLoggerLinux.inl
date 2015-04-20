#ifndef __WN_CONSOLE_LOGGER_LINUX_INL__
#define __WN_CONSOLE_LOGGER_LINUX_INL__
#include "WNLogging/inc/WNConsoleLogger.h"

static const wn_char* LogColors[WNLogging::eLogMax] = {
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
wn_void WNLogging::WNConsoleLogger<T_Level>::FlushBuffer(const wn_char* _buffer, wn_size_t _bufferSize, const  std::vector<WNLogging::WNLogColorElement>& _colors) {
    if(_colors.size() > 0) {
        {
            wn_size_t len =  _colors.front().mPosition - _buffer;
            fprintf(mFiles[T_Level], "%.*s", static_cast<wn_int32>(len), _buffer);
        }
            
        for(wn_size_t i = 0; i < _colors.size(); ++i) {
            fprintf(mFiles[T_Level], "%s", LogColors[(_colors)[i].mLevel]);
            const wn_char* endColor = ((_colors).size() == i+1)? _buffer + _bufferSize: (_colors)[i+1].mPosition;
            wn_size_t len =  endColor - (_colors)[i].mPosition;
            fprintf(mFiles[T_Level], "%.*s", static_cast<wn_int32>(len), (_colors)[i].mPosition);
        }
    } else {
        fprintf(mFiles[T_Level], "%.*s", static_cast<wn_int32>(_bufferSize), _buffer);
    }
}

#endif//__WN_CONSOLE_LOGGER_LINUX_INL__
