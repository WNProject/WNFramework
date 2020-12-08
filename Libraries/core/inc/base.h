// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Library Version
// -------------------------------------------------------------------------------------------------------------
//      WN_LIBRARIES
//      WN_LIBRARIES_MAJOR
//      WN_LIBRARIES_MINOR
//      WN_LIBRARIES_BUILD

// Compiler (Only 1 of the following)
// ------------------------------------------------------------------------------------------
//      _WN_MSVC    - Microsoft Visual C++ compiler
//      _WN_GCC     - GNU C++ compiler
//      _WN_CLANG   - Clang compiler

// Configuration (Only 1 of the following)
// -------------------------------------------------------------------------------------
//      _WN_DEBUG
//      _WN_RELEASE
//      _WN_RETAIL

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CORE_BASE_H__
#define __WN_CORE_BASE_H__

#include "core/inc/internal/config.h"

#ifdef _HAS_EXCEPTIONS
#undef _HAS_EXCEPTIONS
#endif

#define _HAS_EXCEPTIONS 0

#if ((defined DEBUG || defined _DEBUG) && !defined NDEBUG) ||                  \
    (defined __ANDROID__ && !defined NDEBUG)
#define _WN_DEBUG
#define _WN_CONFIG_NAME "Debug"
#else
#if (defined RETAIL || defined _RETAIL || defined _WN_RETAIL) &&               \
    !defined NRETAIL
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

#ifdef _MSC_VER  // Microsoft Visual C++
#include "core/inc/Internal/WNBase_MSVC.h"
#elif defined __clang__  // Clang
#include "core/inc/Internal/WNBase_Clang.h"
#elif defined __GNUC__  // GNU C++
#include "core/inc/Internal/WNBase_GCC.h"
#else
#error "Compiler not supported"
#endif

#ifdef _WN_X86
#include "core/inc/Internal/X86/WNBase.h"
#elif defined _WN_ARM
#include "core/inc/Internal/ARM/WNBase.h"
#endif

#ifndef WN_OSCALL_BEGIN
#define WN_OSCALL_BEGIN
#endif

#ifndef WN_OSCALL_END
#define WN_OSCALL_END
#endif

#ifndef _WN_CONFIG_NAME
#define _WN_CONFIG_NAME "Unknown Configuration"
#endif

#ifndef WN_DEBUG_BREAK
#define WN_DEBUG_BREAK()
#endif

#define WN_LIBRARIES 23000  // 0.23.0
#define WN_LIBRARIES_MAJOR (WN_LIBRARIES / 100000)
#define WN_LIBRARIES_MINOR (WN_LIBRARIES / 100 % 1000)
#define WN_LIBRARIES_BUILD (WN_LIBRARIES % 100)

#endif  // __WN_CORE_BASE_H__
