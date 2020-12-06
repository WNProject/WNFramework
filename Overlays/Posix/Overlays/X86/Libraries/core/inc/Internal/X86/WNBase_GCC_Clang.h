// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CORE_INTERNAL_X86_BASE_GCC_H__
#define __WN_CORE_INTERNAL_X86_BASE_GCC_H__

#ifndef _WN_64_BIT
#ifdef __MMX__
#define __WN_MMX_AVAILABLE
#endif

#ifdef __3dNOW__
#define _WN_X86_3DNOW
#endif
#endif

#ifdef __SSE__
#define _WN_X86_SSE
#endif

#ifdef __SSE2__
#define __WN_SSE2_AVAILABLE
#endif

#ifdef __SSE3__
#define __WN_SSE3_AVAILABLE
#endif

#ifdef __SSSE3__
#define __WN_SSSE3_AVAILABLE
#endif

#ifdef __SSE4_1__
#define __WN_SSE4_1_AVAILABLE
#endif

#ifdef __SSE4_2__
#define __WN_SSE4_2_AVAILABLE
#endif

#ifdef __SSE4A__
#define __WN_SSE4A_AVAILABLE
#endif

#ifdef __AES__
#define __WN_AES_AVAILABLE
#endif

#ifdef __AVX__
#define __WN_VEX_AVAILABLE
#define __WN_AVX_AVAILABLE
#endif

#ifdef __FMA4__
#define __WN_FMA4_AVAILABLE
#endif

#ifdef __XOP__
#define __WN_XOP_AVAILABLE
#endif

#ifdef __FMA__
#define __WN_FMA3_AVAILABLE
#endif

#ifdef __F16C__
#define __WN_F16C_AVAILABLE
#endif

#ifdef __AVX2__
#define __WN_AVX2_AVAILABLE
#endif

#ifdef _WN_ANDROID
#ifndef __WN_MMX_AVAILABLE
#define __WN_MMX_AVAILABLE
#endif

#ifndef _WN_X86_SSE
#define _WN_X86_SSE
#endif

#ifndef __WN_SSE2_AVAILABLE
#define __WN_SSE2_AVAILABLE
#endif

#ifndef __WN_SSE3_AVAILABLE
#define __WN_SSE3_AVAILABLE
#endif
#endif

#endif  // __WN_CORE_INTERNAL_X86_BASE_GCC_H__
