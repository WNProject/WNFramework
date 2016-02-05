// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_SSE4_1_VECTOR_TRAITS_H__
#define __WN_MATH_INTERNAL_X86_SSE4_1_VECTOR_TRAITS_H__

#include "WNMath/inc/Internal/Extensions/x86/SSE2/WNVectorTraits.h"

#include <smmintrin.h>

namespace wn {
    namespace internal {
        namespace math {
            struct vector_traits_sse4_1 : vector_traits_sse2 {
                template <typename type, const size_t dimension>
                static WN_FORCE_INLINE type length(const element_array<type, dimension>& _element_array) {
                    return(base::length(_element_array));
                }

                template <>
                static WN_FORCE_INLINE float length(const element_array<float, 2>& _element_array) {
                    const __m128 squared = _mm_mul_ps(_element_array.m_xmm_values[0], _element_array.m_xmm_values[0]);
                    const __m128 length_squared = _mm_dp_ps(squared, squared, 0x31);

                    #ifdef __WN_MATH_APPROXIMATIONS_ENABLED
                        const __m128 length = _mm_rsqrt_ss(length_squared);
                    #else
                        const __m128 length = _mm_sqrt_ss(length_squared);
                    #endif

                    return(_mm_cvtss_f32(length));
                }

                template <>
                static WN_FORCE_INLINE float length(const element_array<float, 3>& _element_array) {
                    const __m128 squared = _mm_mul_ps(_element_array.m_xmm_values[0], _element_array.m_xmm_values[0]);
                    const __m128 length_squared = _mm_dp_ps(squared, squared, 0x71);

                    #ifdef __WN_MATH_APPROXIMATIONS_ENABLED
                        const __m128 length = _mm_rsqrt_ss(length_squared);
                    #else
                        const __m128 length = _mm_sqrt_ss(length_squared);
                    #endif

                    return(_mm_cvtss_f32(length));
                }

                template <>
                static WN_FORCE_INLINE float length(const element_array<float, 4>& _element_array) {
                    const __m128 squared = _mm_mul_ps(_element_array.m_xmm_values[0], _element_array.m_xmm_values[0]);
                    const __m128 length_squared = _mm_dp_ps(squared, squared, 0xF1);

                    #ifdef __WN_MATH_APPROXIMATIONS_ENABLED
                        const __m128 length = _mm_rsqrt_ss(length_squared);
                    #else
                        const __m128 length = _mm_sqrt_ss(length_squared);
                    #endif

                    return(_mm_cvtss_f32(length));
                }

                template <typename type, const size_t dimension>
                static WN_FORCE_INLINE type length_squared(const element_array<type, dimension>& _element_array) {
                    return(base::length_squared(_element_array));
                }

                template <>
                static WN_FORCE_INLINE float length_squared(const element_array<float, 2>& _element_array) {
                    const __m128 squared = _mm_mul_ps(_element_array.m_xmm_values[0], _element_array.m_xmm_values[0]);

                    return(_mm_cvtss_f32(_mm_dp_ps(squared, squared, 0x31)));
                }

                template <>
                static WN_FORCE_INLINE float length_squared(const element_array<float, 3>& _element_array) {
                    const __m128 squared = _mm_mul_ps(_element_array.m_xmm_values[0], _element_array.m_xmm_values[0]);

                    return(_mm_cvtss_f32(_mm_dp_ps(squared, squared, 0x71)));
                }

                template <>
                static WN_FORCE_INLINE float length_squared(const element_array<float, 4>& _element_array) {
                    const __m128 squared = _mm_mul_ps(_element_array.m_xmm_values[0], _element_array.m_xmm_values[0]);

                    return(_mm_cvtss_f32(_mm_dp_ps(squared, squared, 0xF1)));
                }

                template <typename type, const size_t dimension>
                static WN_FORCE_INLINE void normalize(element_array<type, dimension>& _element_array) {
                    base::normalize(_element_array);
                }

                template <>
                static WN_FORCE_INLINE void normalize(element_array<float, 4>& _element_array) {
                    const __m128 squared = _mm_mul_ps(_element_array.m_xmm_values[0], _element_array.m_xmm_values[0]);
                    const __m128 length_squared = _mm_dp_ps(squared, squared, 0xFF);

                    #ifdef __WN_MATH_APPROXIMATIONS_ENABLED
                        const __m128 inverse_length = _mm_rsqrt_ps(length_squared);

                        _element_array.m_xmm_values[0] = _mm_mul_ps(_element_array.m_xmm_values[0], inverse_length);
                    #else
                        const __m128 length = _mm_sqrt_ps(length_squared);

                        _element_array.m_xmm_values[0] = _mm_div_ps(_element_array.m_xmm_values[0], length);
                    #endif
                }

            private:
                typedef vector_traits_sse2 base;
            };
        }
    }
}

#endif // __WN_MATH_INTERNAL_X86_SSE4_1_VECTOR_TRAITS_H__