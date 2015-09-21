// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_INTERNAL_VECTOR_TRAITS_H__
#define __WN_MATH_INTERNAL_VECTOR_TRAITS_H__

#include "WNMath/inc/WNConfig.h"

#ifdef __WN_MATH_EXTENSIONS_ENABLED
    #ifdef _WN_X86
        #include "WNMath/inc/Internal/Extensions/x86/WNVectorTraits.h"
    #endif
#else
    #include "WNMath/inc/Internal/Generic/WNVectorTraits.h"

    namespace wn {
        namespace internal {
            namespace math {
                typedef vector_traits_generic vector_traits;
            }
        }
    }
#endif

#endif // __WN_MATH_INTERNAL_GENERAL_TRAITS_H__