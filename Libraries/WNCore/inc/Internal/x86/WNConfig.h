// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CORE_INTERNAL_X86_CONFIG_H__
#define __WN_CORE_INTERNAL_X86_CONFIG_H__

#ifndef __WN_CORE_CONFIG_OLD_H__
#error "Internal/x86/WNConfig.h should never be included directly."
#elif !defined _WN_X86
#error                                                                         \
    "Internal/x86/WNConfig.h has been included on a non x86 platform. Please rectify this."
#elif !defined __WN_CORE_EXTENSIONS_ENABLED
#error                                                                         \
    "Internal/x86/WNConfig.h has been included when extensions are disabled. Please rectify this."
#endif

#ifdef _WN_MSVC
#if defined _WN_CORE_CONFIG_MSVC_X86_ENABLE_AVX2 || defined __AVX2__
#define _WN_CORE_CONFIG_MSVC_X86_ENABLE_AVX
#endif

#if defined _WN_CORE_CONFIG_MSVC_X86_ENABLE_AVX || defined __AVX__
#define _WN_CORE_CONFIG_MSVC_X86_ENABLE_SSE4
#endif

#ifdef _WN_CORE_CONFIG_MSVC_X86_ENABLE_SSE4
#define _WN_CORE_CONFIG_MSVC_X86_ENABLE_SSE3
#endif

#ifdef _WN_CORE_CONFIG_MSVC_X86_ENABLE_SSE3
#define _WN_CORE_CONFIG_MSVC_X86_ENABLE_SSE2
#endif

#ifdef _WN_CORE_CONFIG_MSVC_X86_ENABLE_SSE2
#define _WN_CORE_CONFIG_MSVC_X86_ENABLE_SSE
#endif

#ifdef _WN_CORE_CONFIG_MSVC_X86_ENABLE_SSE
#define _WN_CORE_CONFIG_MSVC_X86_ENABLE_MMX
#endif

#if defined __AVX__ || defined __AVX2__
#define _WN_CORE_CONFIG_MSVC_X86_USING_VEX
#endif
#endif

#endif  // __WN_CORE_INTERNAL_X86_CONFIG_H__