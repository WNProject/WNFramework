// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CORE_MACROS_H__
#define __WN_CORE_MACROS_H__

#include "WNCore/inc/WNBase.h"
#include "WNCore/inc/WNTypes.h"

#ifdef _WN_WINDOWS
    #include <windows.h>
#elif defined _WN_LINUX
    #include <signal.h>
#endif

#include <stdio.h>

#ifdef _WN_MSVC
    #pragma warning(disable: 4996) //snprintf

    #ifdef _WN_WINDOWS
        #define WN_UNUSED_ARG(_argument) UNREFERENCED_PARAMETER(_argument)
    #else
        #define WN_UNUSED_ARG(_argument) (_argument)
    #endif
    
    #define WN_SNPRINTF(dest,size, format, ...) _snprintf(dest, size, format, ##__VA_ARGS__)
#else
    #define WN_UNUSED_ARG(_argument) (static_cast<WN_VOID>(sizeof((_argument), 0)))
    #define WN_SNPRINTF(dest,size, format, ...) snprintf(dest, size, format, ##__VA_ARGS__)
#endif

#ifdef _WN_DEBUG
    #if defined _WN_LINUX
        #define WN_DEBUG_BREAK() raise(SIGTRAP)
    #else
        #ifdef _WN_MSVC
            #define WN_DEBUG_BREAK() __debugbreak()
        #elif defined _WN_GCC || defined _WN_CLANG
            #ifdef _WN_X86
                #define WN_DEBUG_BREAK() __asm__ __volatile__ ("int $3")
            #else
                #define WN_DEBUG_BREAK()
            #endif
        #endif
    #endif
#else
    #define WN_DEBUG_BREAK()
#endif

#ifdef _WN_WINDOWS
    #define WN_YIELD_PROCESSOR() YieldProcessor()
#else
    #if defined _WN_GCC || defined _WN_CLANG
        #ifdef _WN_X86
            #define WN_YIELD_PROCESSOR() __asm__ __volatile__ ("rep; nop" : : : "memory")
        #else
            #include <sched.h>
            #define WN_YIELD_PROCESSOR() sched_yield()
        #endif
    #endif
#endif

#define WN_ARRAY_LENGTH(_array) (sizeof(_array) / sizeof(_array[0]))

#define WN_STRINGERIZE(_value) #_value

#define WN_PRINTF printf
#define WN_SPRINTF sprintf
#define WN_WSPRINTF wsprintf

#define WN_BIT_MASK(_bit) (1LL << (_bit))
#define WN_BIT_SET(_value, _bit) ((_value) |= WN_BIT_MASK(_bit))
#define WN_BIT_CLEAR(_value, _bit) ((_value) &= ~WN_BIT_MASK(_bit))
#define WN_BIT_FLIP(_value, _bit) ((_value) ^= WN_BIT_MASK(_bit))
#define WN_BIT_TEST(_value, _bit) (((_value) & WN_BIT_MASK(_bit)) != 0)

#endif // __WN_CORE_MACROS_H__
