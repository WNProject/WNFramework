// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_CONFIG_H__
#define __WN_MATH_INTERNAL_CONFIG_H__

#ifndef __WN_MATH_CONFIG_H__
    #error "Internal/WNConfig.h should never be included directly."
#endif

#ifndef _WN_MATH_CONFIG_DISABLE_APPROXIMATIONS
    #define __WN_MATH_APPROXIMATIONS_ENABLED
#endif

#ifndef _WN_MATH_CONFIG_DISABLE_EXTENSIONS
    #include "WNCore/inc/WNBase.h"

    #define __WN_MATH_EXTENSIONS_ENABLED

    #ifdef _WN_X86
        #include "WNMath/inc/Internal/x86/WNConfig.h"
    #elif defined _WN_ARM
        #include "WNMath/inc/Internal/ARM/WNConfig.h"
    #endif
#endif

#endif // __WN_MATH_INTERNAL_CONFIG_H__