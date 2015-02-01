// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_DIVIDE_NUMBER_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_DIVIDE_NUMBER_H__

#ifdef __WN_SSE_AVAILABLE
    #include "WNMath/inc/Internal/x86/WNGeneralOpDivideNumber_SSE.h"
#elif defined __WN_3DNOW_AVAILABLE
    #include "WNMath/inc/Internal/x86/WNGeneralOpDivideNumber_3DNow.h"
#endif

#ifdef __WN_SSE2_AVAILABLE
    #include "WNMath/inc/Internal/x86/WNGeneralOpDivideNumber_SSE2.h"
#endif

#ifdef __WN_AVX_AVAILABLE
    #include "WNMath/inc/Internal/x86/WNGeneralOpDivideNumber_AVX.h"
#endif

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_DIVIDE_NUMBER_H__