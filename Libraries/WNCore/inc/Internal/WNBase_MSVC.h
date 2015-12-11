// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CORE_INTERNAL_BASE_MSVC_H__
#define __WN_CORE_INTERNAL_BASE_MSVC_H__

#if _MSC_VER < 1800
    #error "Compiler not supported: MSVC 12 (Visual Studio 2013) or higher must be used"
#endif

#define _WN_MSVC _MSC_FULL_VER
#define _WN_MSVC_MAJOR (_MSC_VER / 100)
#define _WN_MSVC_MINOR (_MSC_VER % 100)
#define _WN_MSVC_BUILD (_MSC_FULL_VER % 100000)

#define __WN_HAS_CUSTOM_NULLPTR

#define __WN_HAS_CPP11_NULLPTR
#define __WN_HAS_CPP11_STATIC_ASSERT
#define __WN_HAS_CPP11_RVALUE_REFERENCES
#define __WN_HAS_CPP11_TYPE_TRAITS
#define __WN_HAS_CPP11_DECLTYPE

// Before Visual Studio 2015 using alias declaration crashes the compiler in a number of instances
#if _WN_MSVC_MAJOR > 18
    #define __WN_HAS_CPP11_USING_ALIAS_DECLARATIONS
#endif

#define __WN_HAS_CPP11_CONSTRUCTOR_DELEGATION
#define __WN_HAS_CPP11_DEFAULT_AND_DELETED_FUNCTIONS
#define __WN_HAS_CPP11_DEFAULT_TEMPLATE_PARAMETERS_ON_FUNCTIONS
#define __WN_HAS_CPP11_VARIADIC_TEMPLATES

#define __WN_HAS_CPP11_STD_ROUND

#define __WN_HAS_CPP14_STD_MAKE_UNIQUE

#if defined _M_X64 || defined _M_AMD64
    #define _WN_X86
    #define _WN_64_BIT

    // For some reason the following don't get properly defined at editing time just at compile time.  Define them so
    // intellisense doesn't complain
    #ifndef _M_AMD64
        #define _M_AMD64

        #ifndef _AMD64_
            #define _AMD64_
        #endif
    #endif
#elif defined _M_IX86
    #define _WN_X86
    #define _WN_32_BIT
#elif defined _M_ARM
    #define _WN_ARM _M_ARM
    #define _WN_32_BIT
#endif

#if defined _WIN32 || defined _WIN64
    #define _WN_WINDOWS
    #define _WN_PLATFORM_NAME "Windows"

    #define NOMINMAX

    #include <winsock2.h>
    #include <Windows.h>

    #if !defined _WIN32_WINNT_VISTA || _WIN32_WINNT < _WIN32_WINNT_VISTA
        #error "Windows API Version not supported: Windows Vista or higher must be used"
    #endif

    #define WN_OSCALL_BEGIN WINAPI
    #define WN_OSCALL_END

    #define WN_UNUSED_ARGUMENT(_argument) UNREFERENCED_PARAMETER(_argument)
#endif

#define WN_DEBUG_BREAK() __debugbreak()

#endif // __WN_CORE_INTERNAL_BASE_MSVC_H__