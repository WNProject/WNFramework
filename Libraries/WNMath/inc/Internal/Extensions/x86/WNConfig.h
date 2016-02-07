// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_CONFIG_H__
#define __WN_MATH_INTERNAL_X86_CONFIG_H__

#ifdef __WN_MMX_AVAILABLE
#include <mmintrin.h>

#ifndef __WN_MATH_MM_AVAILABLE
#define __WN_MATH_MM_AVAILABLE
#endif
#endif

#ifdef _WN_X86_3DNOW
#include <mm3dnow.h>

#ifndef __WN_MATH_MM_AVAILABLE
#define __WN_MATH_MM_AVAILABLE
#endif
#endif

#ifdef _WN_X86_SSE
#include <xmmintrin.h>

#ifndef __WN_MATH_XMM_AVAILABLE
#define __WN_MATH_XMM_AVAILABLE
#endif
#endif

#ifdef __WN_SSE2_AVAILABLE
#include <emmintrin.h>

// This is to fix a bug in the SSE2 header for MSVC
#ifdef _WN_MSVC
#ifdef _WN_64_BIT
#include <intrin.h>

#pragma intrinsic(_mm_set1_epi64x)
#elif defined _WN_32_BIT && _WN_MSVC_MAJOR < 19
WN_FORCE_INLINE __m128i _mm_set1_epi64x(__int64 i) {
  __m128i n;

  n.m128i_i64[0] = i;
  n.m128i_i64[1] = i;

  return (n);
}
#endif
#endif

#ifndef __WN_MATH_XMM_AVAILABLE
#define __WN_MATH_XMM_AVAILABLE
#endif
#endif

#ifdef __WN_SSE3_AVAILABLE
#include <pmmintrin.h>

#ifndef __WN_MATH_XMM_AVAILABLE
#define __WN_MATH_XMM_AVAILABLE
#endif
#endif

#ifdef __WN_SSE4_1_AVAILABLE
#include <smmintrin.h>

#ifndef __WN_MATH_XMM_AVAILABLE
#define __WN_MATH_XMM_AVAILABLE
#endif
#endif

#ifdef __WN_SSE4_2_AVAILABLE
#include <tmmintrin.h>

#ifndef __WN_MATH_XMM_AVAILABLE
#define __WN_MATH_XMM_AVAILABLE
#endif
#endif

#if defined __WN_AVX_AVAILABLE || defined __WN_AVX2_AVAILABLE
#include <immintrin.h>

#ifdef _WN_MSVC
#pragma warning(disable : 4752)
#endif

#ifndef __WN_MATH_YMM_AVAILABLE
#define __WN_MATH_YMM_AVAILABLE
#endif
#endif

#if !defined __WN_MATH_MM_AVAILABLE && !defined __WN_MATH_XMM_AVAILABLE &&     \
    !defined __WN_MATH_YMM_AVAILABLE
#undef __WN_MATH_EXTENSIONS_ENABLED
#endif

#endif  // __WN_MATH_INTERNAL_X86_CONFIG_H__