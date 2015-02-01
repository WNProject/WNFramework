// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_ELEMENT_ARRAY_H__
#define __WN_MATH_INTERNAL_ELEMENT_ARRAY_H__

#include "WNMath/inc/WNConfig.h"

#ifdef __WN_MATH_EXTENSIONS_ENABLED
    #ifdef _WN_X86
        #include "WNMath/inc/Internal/Extensions/x86/WNElementArray.h"
    #endif
#else
    #include "WNMath/inc/Internal/Generic/WNElementArray.h"

    namespace wn {
        namespace internal {
            namespace math {
                #ifdef __WN_HAS_CPP11_USING_ALIAS_DECLARATIONS
                    template <typename type, const wn_size_t dimension>
                    using element_array = element_array_generic<type, dimension>;
                #else
                    template <typename type, const wn_size_t dimension>
                    struct element_array : element_array_generic<type, dimension> {};
                #endif
            }
        }
    }
#endif

#endif // __WN_MATH_INTERNAL_ELEMENT_ARRAY_H__