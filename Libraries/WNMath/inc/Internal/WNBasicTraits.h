// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_BASIC_TRAITS_H__
#define __WN_MATH_INTERNAL_BASIC_TRAITS_H__

#include "WNMath/inc/WNConfig.h"

#ifdef __WN_MATH_EXTENSIONS_ENABLED
    #ifdef _WN_X86
        #include "WNMath/inc/Internal/Extensions/x86/WNBasicTraits.h"
    #endif
#else
    #include "WNMath/inc/Internal/Generic/WNBasicTraits.h"

    namespace wn {
        namespace internal {
            namespace math {
                typedef basic_traits_generic basic_traits;
            }
        }
    }
#endif

#endif // __WN_MATH_INTERNAL_BASIC_TRAITS_H__