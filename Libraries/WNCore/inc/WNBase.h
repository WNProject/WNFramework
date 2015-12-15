// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

// Library Version -------------------------------------------------------------------------------------------------------------
//      WN_LIBRARIES
//      WN_LIBRARIES_MAJOR
//      WN_LIBRARIES_MINOR
//      WN_LIBRARIES_BUILD

// Calling Conventions ---------------------------------------------------------------------------------------------------------
//      WN_CDECL_BEGIN / WN_CDECL_END
//      WN_STDCALL_BEGIN / WN_STDCALL_END
//      WN_FASTCALL_BEGIN / WN_FASTCALL_END

// Keyword ---------------------------------------------------------------------------------------------------------------------
//      WN_INLINE
//      WN_FORCE_INLINE
//      WN_NO_INLINE
//      WN_RESTRICT

// Properties ------------------------------------------------------------------------------------------------------------------
//      WN_THREAD_LOCAL
//      WN_ALIGN

// Compiler (Only 1 of the following) ------------------------------------------------------------------------------------------
//      _WN_MSVC    - Microsoft Visual C++ compiler
//      _WN_GCC     - GNU C++ compiler
//      _WN_CLANG   - Clang compiler

// Platform (Only 1 of the following) ------------------------------------------------------------------------------------------
//      _WN_WINDOWS - Microsoft Windows
//      _WN_LINUX   - Linux
//      _WN_ANDROID - Android

// Architecture (Only 1 of the following) --------------------------------------------------------------------------------------
//      _WN_X86     - Defined for all x86 based architectures (i.e. x86, x86-64, etc.)
//      _WN_ARM     - Defined for all ARM based architectures. The value will be the ARM version (i.e. 4, 5, 6, 7, etc.)
//      _WN_MIPS    - Defined for all MIPS based architectures

// Processor Bit Width (Only 1 of the following) -------------------------------------------------------------------------------
//      _WN_32_BIT  - Defined if architecture is 32-bit
//      _WN_64_BIT  - Defined if architecture is 64-bit

// Endianess (Only 1 of the following) -----------------------------------------------------------------------------------------
//      _WN_ENDIAN_LITTLE
//      _WN_ENDIAN_BIG
//      _WN_ENDIAN_UNKNOWN - Requires runtime check due to Bi-endian architecture

// Character Set (Only 1 of the following) -------------------------------------------------------------------------------------
//      _WN_UNICODE
//      _WN_ANSI

// Configuration (Only 1 of the following) -------------------------------------------------------------------------------------
//      _WN_DEBUG
//      _WN_RELEASE
//      _WN_RETAIL

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CORE_BASE_H__
#define __WN_CORE_BASE_H__

#include "WNCore/inc/Internal/WNConfig.h"

#ifndef __cplusplus
    #error "C++ required. Please use with a C++ compiler and/or compile for C++."
#else
    #define __WN_CPP __cplusplus
#endif

#ifdef _HAS_EXCEPTIONS
    #undef _HAS_EXCEPTIONS
#endif

#define _HAS_EXCEPTIONS 0

#if ((defined DEBUG || defined _DEBUG) && !defined NDEBUG) || (defined __ANDROID__ && !defined NDEBUG)
    #define _WN_DEBUG
    #define _WN_CONFIG_NAME "Debug"
#else
    #if (defined RETAIL || defined _RETAIL || defined _WN_RETAIL) && !defined NRETAIL
        #ifdef _WN_RETAIL
            #undef _WN_RETAIL
        #endif

        #define _WN_RETAIL
        #define _WN_CONFIG_NAME "Retail"
    #else
        #define _WN_RELEASE
        #define _WN_CONFIG_NAME "Release"
    #endif
#endif

#if defined UNICODE || defined _UNICODE
    #define _WN_UNICODE
#else
    #define _WN_ANSI
#endif

#ifdef _MSC_VER // Microsoft Visual C++
    #include "WNCore/inc/Internal/WNBase_MSVC.h"
#elif defined __clang__ // Clang
    #include "WNCore/inc/Internal/WNBase_Clang.h"
#elif defined __GNUC__ // GNU C++
    #include "WNCore/inc/Internal/WNBase_GCC.h"
#else
    #error "Compiler not supported"
#endif

#include "WNCore/inc/WNConfigOld.h"

#ifdef _WN_X86
    #include "WNCore/inc/Internal/x86/WNBase.h"
#elif defined _WN_ARM
    #include "WNCore/inc/Internal/ARM/WNBase.h"
#endif

#ifndef WN_OSCALL_BEGIN
    #define WN_OSCALL_BEGIN
#endif

#ifndef WN_OSCALL_END
    #define WN_OSCALL_END
#endif

#ifndef _WN_PLATFORM_NAME
    #define _WN_PLATFORM_NAME "Unknown Platform"
#endif

#ifndef _WN_ARCH_NAME
    #define _WN_ARCH_NAME "Unknown Architecture"
#endif

#ifndef _WN_CONFIG_NAME
    #define _WN_CONFIG_NAME "Unknown Configuration"
#endif

#if !defined _WN_ENDIAN_LITTLE && !defined _WN_ENDIAN_BIG
    #ifndef _WN_ENDIAN_UNKNOWN
        #define _WN_ENDIAN_UNKNOWN
    #endif
#endif

#define WN_STRINGERIZE(_value) #_value

#ifndef WN_UNUSED_ARGUMENT
    #define WN_UNUSED_ARGUMENT(_argument) (_argument)
#endif

#ifndef WN_DEBUG_BREAK
    #define WN_DEBUG_BREAK()
#endif

#define WN_LIBRARIES 23000 // 0.23.0
#define WN_LIBRARIES_MAJOR (WN_LIBRARIES / 100000)
#define WN_LIBRARIES_MINOR (WN_LIBRARIES / 100 % 1000)
#define WN_LIBRARIES_BUILD (WN_LIBRARIES % 100)

#endif // __WN_CORE_BASE_H__
