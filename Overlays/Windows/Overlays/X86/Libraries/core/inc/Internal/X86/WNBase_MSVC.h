// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CORE_INTERNAL_X86_BASE_MSVC_H__
#define __WN_CORE_INTERNAL_X86_BASE_MSVC_H__

#ifndef _WN_64_BIT
#ifdef _WN_CORE_CONFIG_MSVC_X86_ENABLE_MMX
#define __WN_MMX_AVAILABLE

#ifdef _WN_CORE_CONFIG_MSVC_X86_USING_AMD
#define _WN_X86_3DNOW
#endif
#endif
#endif

#ifdef _WN_CORE_CONFIG_MSVC_X86_ENABLE_SSE
#define _WN_X86_SSE
#endif

#ifdef _WN_CORE_CONFIG_MSVC_X86_ENABLE_SSE2
#define __WN_SSE2_AVAILABLE
#endif

#ifdef _WN_CORE_CONFIG_MSVC_X86_ENABLE_SSE3
#define __WN_SSE3_AVAILABLE
#define __WN_SSSE3_AVAILABLE
#endif

#ifdef _WN_CORE_CONFIG_MSVC_X86_ENABLE_SSE4
#define __WN_SSE4_1_AVAILABLE
#define __WN_SSE4_2_AVAILABLE

#ifdef _WN_CORE_CONFIG_MSVC_X86_USING_AMD
#define __WN_SSE4A_AVAILABLE
#endif
#endif

#ifdef _WN_CORE_CONFIG_MSVC_X86_ENABLE_AVX
#define __WN_AES_AVAILABLE
#define __WN_FMA3_AVAILABLE
#define __WN_F16C_AVAILABLE
#define __WN_AVX_AVAILABLE

#ifdef _WN_CORE_CONFIG_MSVC_X86_USING_VEX
#define __WN_VEX_AVAILABLE
#endif

#ifdef _WN_CORE_CONFIG_MSVC_X86_USING_AMD
#define __WN_FMA4_AVAILABLE
#endif

#define __WN_XOP_AVAILABLE
#endif

#ifdef _WN_CORE_CONFIG_MSVC_X86_FORCE_AVX2
#define __WN_AVX2_AVAILABLE
#define __WN_FMA3_AVAILABLE
#define __WN_F16C_AVAILABLE

#ifdef _WN_CORE_CONFIG_MSVC_X86_USING_AMD
#define __WN_FMA4_AVAILABLE
#endif
#endif

#ifdef _WN_32_BIT
#ifdef _M_IX86_FP
#if _M_IX86_FP > 0
#if _M_IX86_FP >= 1
#ifndef __WN_MMX_AVAILABLE
#define __WN_MMX_AVAILABLE
#endif

#ifdef _WN_CORE_CONFIG_MSVC_X86_USING_AMD
#ifndef _WN_X86_3DNOW
#define _WN_X86_3DNOW
#endif
#endif

#ifndef _WN_X86_SSE
#define _WN_X86_SSE
#endif

#if _M_IX86_FP >= 2
#ifndef __WN_SSE2_AVAILABLE
#define __WN_SSE2_AVAILABLE
#endif
#endif
#endif
#endif
#endif
#endif

#endif  // __WN_CORE_INTERNAL_X86_BASE_MSVC_H__
