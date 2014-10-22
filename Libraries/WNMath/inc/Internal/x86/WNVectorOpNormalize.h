// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_VECTOR_OP_NORMALIZE_H__
#define __WN_MATH_INTERNAL_X86_VECTOR_OP_NORMALIZE_H__

#ifdef __WN_SSE4_1_AVAILABLE
    #include "WNMath/inc/Internal/x86/WNVectorOpNormalize_SSE4.1.h"
#elif defined __WN_SSE3_AVAILABLE
    #include "WNMath/inc/Internal/x86/WNVectorOpNormalize_SSE3.h"
#elif defined __WN_SSE_AVAILABLE
    #include "WNMath/inc/Internal/x86/WNVectorOpNormalize_SSE.h"
#endif

#endif // __WN_MATH_INTERNAL_X86_VECTOR_OP_NORMALIZE_H__