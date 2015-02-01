// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_SSE2_VECTOR_TRAITS_H__
#define __WN_MATH_INTERNAL_X86_SSE2_VECTOR_TRAITS_H__

#include "WNMath/inc/Internal/Extensions/x86/SSE/WNVectorTraits.h"

#include <emmintrin.h>

namespace wn {
    namespace internal {
        namespace math {
            struct vector_traits_sse2 : vector_traits_sse {
                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void normalize(element_array<type, dimension>& _element_array) {
                    base::normalize(_element_array);
                }

                template <>
                static WN_FORCE_INLINE wn_void normalize(element_array<wn_float32, 4>& _element_array) {
                    const __m128 squared = _mm_mul_ps(_element_array.m_xmm_values[0], _element_array.m_xmm_values[0]);
                    __m128 length_squared = _mm_add_ps(squared, sse2_shuffle_ps<_MM_SHUFFLE(1, 0, 3, 2)>(squared));

                    length_squared = _mm_add_ps(length_squared, sse2_shuffle_ps<_MM_SHUFFLE(0, 1, 0, 1)>(length_squared));

                    #ifdef __WN_MATH_APPROXIMATIONS_ENABLED
                        const __m128 inverse_length = _mm_rsqrt_ps(length_squared);

                        _element_array.m_xmm_values[0] = _mm_mul_ps(_element_array.m_xmm_values[0], inverse_length);
                    #else
                        const __m128 length = _mm_sqrt_ps(length_squared);

                        _element_array.m_xmm_values[0] = _mm_div_ps(_element_array.m_xmm_values[0], length);
                    #endif
                }

            protected:
                template <const wn_int32 mask>
                static WN_FORCE_INLINE __m128 sse2_shuffle_ps(__m128 _value) {
                    return(_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(_value), mask)));
                }

            private:
                typedef vector_traits_sse base;
            };
        }
    }
}

#endif // __WN_MATH_INTERNAL_X86_SSE2_VECTOR_TRAITS_H__