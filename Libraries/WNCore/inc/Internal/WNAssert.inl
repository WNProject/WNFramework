// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CORE_INTERNAL_ASSERT_INL__
#define __WN_CORE_INTERNAL_ASSERT_INL__

#ifndef __WN_CORE_ASSERT_H__
    #error "WNAssert.inl should never be included directly. Please include WNAssert.h instead"
#endif

#include "WNCore/inc/WNMacros.h"

#if defined _WN_WINDOWS && defined _WN_CORE_CONFIG_WINDOWS_ENABLE_ASSERT_POPUPS
    #include <windows.h>
    #include <tchar.h>
#else
    #include <stdio.h>
#endif

#include <assert.h>

namespace WNCore {
    namespace __WNInternal {
        WN_INLINE WN_VOID __WNAssert(WN_INT32 _argument, const WN_CHAR* _file, WN_UINT32 _line, const WN_CHAR* _message) {
            if (_argument == 0) {
                #if defined _WN_WINDOWS && defined _WN_CORE_CONFIG_WINDOWS_ENABLE_ASSERT_POPUPS
                    TCHAR display[1024];
                    TCHAR program[MAX_PATH];

                    ZeroMemory(display, 1024);
                    ZeroMemory(program, MAX_PATH);

                    #ifdef _WN_UNICODE
                        TCHAR file[MAX_PATH];
                        TCHAR message[512];

                        ZeroMemory(file, MAX_PATH);
                        ZeroMemory(message, 512);
                    #endif

                    GetModuleFileName(NULL, program, MAX_PATH);

                    #ifdef _WN_UNICODE
                        MultiByteToWideChar(CP_ACP, 0, _file, -1, file, MAX_PATH + 1);
                        MultiByteToWideChar(CP_ACP, 0, _message, -1,  message, 512);

                        swprintf_s(display, L"Assertion failed!\n\nProgram: %s\nFile: %s\nLine: %d\n\nExpression: %s", program, file, _line, message);
                    #else
                        sprintf_s(display, "Assertion failed!\n\nProgram: %s\nFile: %s\nLine: %d\n\nExpression: %s", program, _file, _line, _message);
                    #endif

                    MessageBox(NULL, display, _T("Assertion Failed!"), MB_OK | MB_ICONHAND | MB_SETFOREGROUND | MB_TASKMODAL);
                #else
                    WN_PRINTF("Assertion failed!\n\nFile: %s\nLine: %d\n\nExpression: %s", _file, _line, _message);
                #endif

                WN_DEBUG_BREAK();

                assert(WN_FALSE);
            }
        }
    }
}

#endif // __WN_CORE_INTERNAL_ASSERT_INL__