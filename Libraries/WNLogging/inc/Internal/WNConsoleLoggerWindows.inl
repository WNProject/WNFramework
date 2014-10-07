////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WN_CONSOLE_LOGGER_WINDOWS_INL__
#define __WN_CONSOLE_LOGGER_WINDOWS_INL__
#include "WNLogging/inc/WNConsoleLogger.h"


#define __WN_FOREGROUND_WHITE FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN
const static WN_UINT16 LogColors[WNLogging::eLogMax] = {
    __WN_FOREGROUND_WHITE,
    FOREGROUND_RED | FOREGROUND_INTENSITY,
    FOREGROUND_RED,
    FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    FOREGROUND_RED | FOREGROUND_GREEN,
    FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    FOREGROUND_GREEN | FOREGROUND_INTENSITY
};

static DWORD s_StandardHandles[] = {
    STD_OUTPUT_HANDLE,
    STD_ERROR_HANDLE
};


template<WNLogging::WNConsoleLocation T_Level>
WNLogging::WNConsoleLogger<T_Level>::WNConsoleLogger() : mConsoleCreated(WN_FALSE), mConsoleHandle(INVALID_HANDLE_VALUE), mInitialized(WN_FALSE) {
}

template<WNLogging::WNConsoleLocation T_Level>
WNLogging::WNConsoleLogger<T_Level>::~WNConsoleLogger() {
    if(mConsoleCreated)
    {
        FreeConsole();
    }
    if(mConsoleHandle != INVALID_HANDLE_VALUE)
    {
        SetConsoleTextAttribute(mConsoleHandle, __WN_FOREGROUND_WHITE);
    }
}


WN_INLINE WN_VOID WriteSectionToConsole(HANDLE handle, const WN_CHAR* buffer, WN_SIZE_T amount) {
    if(amount == 0){
        return;
    }
    DWORD written = 0;
    DWORD toWrite = static_cast<DWORD>(amount);
    do {
        if(!WriteConsoleA(handle, buffer, static_cast<DWORD>(amount), &written, 0)) {
            return;
        } else {
            toWrite -= written;
        }
    } while(toWrite > 0);
}

template<WNLogging::WNConsoleLocation T_Level>
WN_VOID WNLogging::WNConsoleLogger<T_Level>::FlushBuffer(const WN_CHAR* _buffer, WN_SIZE_T _bufferSize, const std::vector<WNLogging::WNLogColorElement>& _colors) {
    if(mConsoleHandle == INVALID_HANDLE_VALUE && !mInitialized) {
        mConsoleCreated = AllocConsole(); //If we cannot create a new console, we must already have one.. hopefully
        mConsoleHandle = GetStdHandle(s_StandardHandles[T_Level]);
        mInitialized = true;
    }
    if(mConsoleHandle != INVALID_HANDLE_VALUE) {
        if(_colors.size() > 0) {
            {
                WN_SIZE_T len =  _colors.front().mPosition - _buffer;
                WriteSectionToConsole(mConsoleHandle, _buffer, len);
            }
                
            for(WN_SIZE_T i = 0; i < _colors.size(); ++i) {
                    
                SetConsoleTextAttribute(mConsoleHandle, LogColors[(_colors)[i].mLevel]);
                const WN_CHAR* endColor = ((_colors).size() == i+1)? _buffer + _bufferSize: (_colors)[i+1].mPosition;
                    
                WN_SIZE_T len =  endColor - (_colors)[i].mPosition;
                WriteSectionToConsole(mConsoleHandle, (_colors)[i].mPosition, len);
            }
        } else {
            WN_SIZE_T len =  _bufferSize;
            WriteSectionToConsole(mConsoleHandle, _buffer, len);
        }
    }
}

#endif//__WN_CONSOLE_LOGGER_WINDOWS_INL__
