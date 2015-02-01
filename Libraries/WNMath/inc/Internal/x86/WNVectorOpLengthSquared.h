// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_VECTOR_OP_LENGTH_SQUARED_H__
#define __WN_MATH_INTERNAL_X86_VECTOR_OP_LENGTH_SQUARED_H__

#ifdef __WN_SSE4_1_AVAILABLE
    #include "WNMath/inc/Internal/x86/WNVectorOpLengthSquared_SSE4.1.h"
#elif defined __WN_SSE3_AVAILABLE
    #include "WNMath/inc/Internal/x86/WNVectorOpLengthSquared_SSE3.h"
#endif

#ifdef __WN_SSE_AVAILABLE
    #include "WNMath/inc/Internal/x86/WNVectorOpLengthSquared_SSE.h"
#endif

#ifdef __WN_SSE2_AVAILABLE
    #include "WNMath/inc/Internal/x86/WNVectorOpLengthSquared_SSE2.h"
#endif

#endif // __WN_MATH_INTERNAL_X86_VECTOR_OP_LENGTH_SQUARED_H__