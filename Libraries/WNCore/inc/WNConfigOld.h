// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CORE_CONFIG_OLD_H__
#define __WN_CORE_CONFIG_OLD_H__

//#define _WN_CORE_CONFIG_DISABLE_EXTENSIONS            // Disables all
//extension for the current configuration

// MSVC: x86 Specific
// //////////////////////////////////////////////////////////////////////////////////////////////////////////

//#define _WN_CORE_CONFIG_MSVC_X86_ENABLE_MMX           // Enables MMX, 3DNow!
//and lower functionality when using MSVC
//#define _WN_CORE_CONFIG_MSVC_X86_ENABLE_SSE           // Enables SSE and lower
//functionality when using MSVC
//#define _WN_CORE_CONFIG_MSVC_X86_ENABLE_SSE2          // Enables SSE2 and
//lower functionality when using MSVC
//#define _WN_CORE_CONFIG_MSVC_X86_ENABLE_SSE3          // Enables SSE3, SSSE3
//and lower functionality when using MSVC
//#define _WN_CORE_CONFIG_MSVC_X86_ENABLE_SSE4          // Enables SSE4.1,
//SSE4.2, SSE4a and lower functionality when using MSVC
//#define _WN_CORE_CONFIG_MSVC_X86_ENABLE_AVX           // Enables AVX and lower
//functionality when using MSVC
//#define _WN_CORE_CONFIG_MSVC_X86_ENABLE_AVX2          // Enables AVX2, FMA3/4,
//F16C and lower functionality when using MSVC

//#define _WN_CORE_CONFIG_MSVC_X86_USING_AMD            // Enables all
//non-standard AMD specific extension when using MSVC
//#define _WN_CORE_CONFIG_MSVC_X86_USING_VEX            // Makes assumptions
//based on vex prefix being available. Use this in
// conjunction with /arch:AVX for best results

// MSVC: ARM Specific
// //////////////////////////////////////////////////////////////////////////////////////////////////////////

//#define _WN_CORE_CONFIG_MSVC_ARM_FORCE_VFP3           // Forces VFP3
//functionality when using MSVC
//#define _WN_CORE_CONFIG_MSVC_ARM_FORCE_VFP4           // Forces VFP4
//functionality when using MSVC

#include "WNCore/inc/Internal/WNConfigOld.h"

#endif  // __WN_CORE_CONFIG_OLD_H__