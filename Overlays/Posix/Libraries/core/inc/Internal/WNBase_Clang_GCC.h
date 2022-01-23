// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CORE_INTERNAL_BASE_CLANG_GCC_H__
#define __WN_CORE_INTERNAL_BASE_CLANG_GCC_H__

#ifdef _WN_LINUX
#include <stdlib.h>

#ifdef _WN_X86
#ifndef _WN_64_BIT
#define WN_OSCALL_BEGIN
#define WN_OSCALL_END __attribute__((cdecl))
#endif
#endif
#endif

#if defined _WN_LINUX
#include <signal.h>

#define WN_DEBUG_BREAK() raise(SIGTRAP)
#elif defined _WN_GCC || defined _WN_CLANG
#ifdef _WN_X86
#define WN_DEBUG_BREAK() __asm__ __volatile__("int $3")
#endif
#endif

#define PACK(decl) decl __attribute__((__packed__))
#endif  // __WN_CORE_INTERNAL_BASE_CLANG_GCC_H__
