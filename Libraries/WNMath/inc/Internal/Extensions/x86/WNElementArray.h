// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_ELEMENT_ARRAY_H__
#define __WN_MATH_INTERNAL_X86_ELEMENT_ARRAY_H__

#ifdef __WN_SSE2_AVAILABLE
    #include "WNMath/inc/Internal/Extensions/x86/SSE2/WNElementArray.h"
#elif defined __WN_SSE_AVAILABLE
    #include "WNMath/inc/Internal/Extensions/x86/SSE/WNElementArray.h"
#endif

namespace wn {
    namespace internal {
        namespace math {
            #ifdef __WN_SSE2_AVAILABLE
                #ifdef __WN_HAS_CPP11_USING_ALIAS_DECLARATIONS
                    template <typename type, const wn_size_t dimension>
                    using element_array = element_array_sse2<type, dimension>;
                #else
                    template <typename type, const wn_size_t dimension>
                    struct element_array : element_array_sse2<type, dimension> {};
                #endif
            #elif defined __WN_SSE_AVAILABLE
                #ifdef __WN_HAS_CPP11_USING_ALIAS_DECLARATIONS
                    template <typename type, const wn_size_t dimension>
                    using element_array = element_array_sse<type, dimension>;
                #else
                    template <typename type, const wn_size_t dimension>
                    struct element_array : element_array_sse<type, dimension> {};
                #endif
            #endif
        }
    }
}

#endif // __WN_MATH_INTERNAL_X86_ELEMENT_ARRAY_H__