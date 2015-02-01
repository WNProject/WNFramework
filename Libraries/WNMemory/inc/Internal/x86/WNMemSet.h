// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MEMORY_INTERNAL_X86_MEMSET_H__
#define __WN_MEMORY_INTERNAL_X86_MEMSET_H__

#ifdef __WN_AVX_AVAILABLE
    #include "WNMemory/inc/Internal/x86/WNMemSet_AVX.h"
#elif defined __WN_SSE2_AVAILABLE
    #include "WNMemory/inc/Internal/x86/WNMemSet_SSE2.h"
#elif defined __WN_SSE_AVAILABLE
    #include "WNMemory/inc/Internal/x86/WNMemSet_SSE.h"
#elif defined __WN_MMX_AVAILABLE
    #include "WNMemory/inc/Internal/x86/WNMemSet_MMX.h"
#endif

#endif // __WN_MEMORY_INTERNAL_X86_MEMSET_H__