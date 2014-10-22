// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MEMORY_INTERNAL_X86_UTILITIES_YMM_H__
#define __WN_MEMORY_INTERNAL_X86_UTILITIES_YMM_H__

#ifndef __WN_YMM_ZEROUPPER
    #ifndef __WN_VEX_AVAILABLE
        #define __WN_YMM_ZEROUPPER() _mm256_zeroupper()
    #else
        #define __WN_YMM_ZEROUPPER()
    #endif
#endif

#endif // __WN_MEMORY_INTERNAL_X86_UTILITIES_YMM_H__