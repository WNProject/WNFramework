// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_CONSOLE_LOGGER_WINDOWS_INL__
#define __WN_CONSOLE_LOGGER_WINDOWS_INL__
#include "WNLogging/inc/WNConsoleLogger.h"


#define __WN_FOREGROUND_WHITE FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN
const static wn_uint16 LogColors[WNLogging::eLogMax] = {
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
WNLogging::WNConsoleLogger<T_Level>::WNConsoleLogger() : mConsoleCreated(wn_false), mConsoleHandle(INVALID_HANDLE_VALUE), mInitialized(wn_false) {
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


WN_INLINE wn_void WriteSectionToConsole(HANDLE handle, const wn_char* buffer, wn_size_t amount) {
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
wn_void WNLogging::WNConsoleLogger<T_Level>::FlushBuffer(const wn_char* _buffer, wn_size_t _bufferSize, const std::vector<WNLogging::WNLogColorElement>& _colors) {
    if(mConsoleHandle == INVALID_HANDLE_VALUE && !mInitialized) {
        mConsoleCreated = AllocConsole(); //If we cannot create a new console, we must already have one.. hopefully
        mConsoleHandle = GetStdHandle(s_StandardHandles[T_Level]);
        mInitialized = true;
    }
    if(mConsoleHandle != INVALID_HANDLE_VALUE) {
        if(_colors.size() > 0) {
            {
                wn_size_t len =  _colors.front().mPosition - _buffer;
                WriteSectionToConsole(mConsoleHandle, _buffer, len);
            }

            for(wn_size_t i = 0; i < _colors.size(); ++i) {

                SetConsoleTextAttribute(mConsoleHandle, LogColors[(_colors)[i].mLevel]);
                const wn_char* endColor = ((_colors).size() == i+1)? _buffer + _bufferSize: (_colors)[i+1].mPosition;

                wn_size_t len =  endColor - (_colors)[i].mPosition;
                WriteSectionToConsole(mConsoleHandle, (_colors)[i].mPosition, len);
            }
        } else {
            wn_size_t len =  _bufferSize;
            WriteSectionToConsole(mConsoleHandle, _buffer, len);
        }
    }
}

#endif//__WN_CONSOLE_LOGGER_WINDOWS_INL__
