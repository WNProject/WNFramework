// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MEMORY_INTERNAL_X86_CONFIG_H__
#define __WN_MEMORY_INTERNAL_X86_CONFIG_H__

#ifndef __WN_MEMORY_CONFIG_H__
    #error "Internal/WNConfig.h should never be included directly."
#endif

#if defined __WN_MMX_AVAILABLE || defined __WN_SSE_AVAILABLE || defined __WN_SSE2_AVAILABLE || defined __WN_AVX_AVAILABLE
    #define __WN_MEMORY_HAS_PLATFORM_SPECIALIZED_MEMSET
#endif

#endif // __WN_MEMORY_INTERNAL_X86_CONFIG_H__