// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CORE_INTERNAL_X86_BASE_H__
#define __WN_CORE_INTERNAL_X86_BASE_H__

#ifdef _WN_MSVC
#include "WNCore/inc/Internal/X86/WNBase_MSVC.h"
#else
#include "WNCore/inc/Internal/X86/WNBase_GCC_Clang.h"
#endif

#ifndef __WN_FPU_AVAILABLE
#define __WN_FPU_AVAILABLE
#endif

#ifdef _WN_64_BIT
#ifndef _WN_X86_SSE
#define _WN_X86_SSE
#endif

#ifndef __WN_SSE2_AVAILABLE
#define __WN_SSE2_AVAILABLE
#endif
#endif

#endif  // __WN_CORE_INTERNAL_X86_BASE_H__
