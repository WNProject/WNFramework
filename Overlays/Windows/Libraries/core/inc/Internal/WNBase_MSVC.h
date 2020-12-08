// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CORE_INTERNAL_BASE_MSVC_H__
#define __WN_CORE_INTERNAL_BASE_MSVC_H__

#if _MSC_VER < 1800
#error                                                                         \
    "Compiler not supported: MSVC 12 (Visual Studio 2013) or higher must be used"
#endif

#define _WN_MSVC _MSC_FULL_VER
#define _WN_MSVC_MAJOR (_MSC_VER / 100)
#define _WN_MSVC_MINOR (_MSC_VER % 100)
#define _WN_MSVC_BUILD (_MSC_FULL_VER % 100000)

#if defined _WIN32 || defined _WIN64
#define NOMINMAX

// clang-format off

#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>

// clang-format on

#if !defined _WIN32_WINNT_VISTA || _WIN32_WINNT < _WIN32_WINNT_VISTA
#error "Windows API Version not supported: Windows Vista or higher must be used"
#endif

#define WN_OSCALL_BEGIN WINAPI
#define WN_OSCALL_END
#endif

#define WN_DEBUG_BREAK() __debugbreak()

#define PACK(decl) __pragma(pack(push, 1)) decl __pragma(pack(pop))

#endif  // __WN_CORE_INTERNAL_BASE_MSVC_H__
