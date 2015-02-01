// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_IS_ZERO_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_IS_ZERO_H__

#ifdef __WN_SSE_AVAILABLE
    #include "WNMath/inc/Internal/x86/WNGeneralOpIsZero_SSE.h"
#endif

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_IS_ZERO_H__