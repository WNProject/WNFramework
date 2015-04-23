// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_SSE_VECTOR_TRAITS_H__
#define __WN_MATH_INTERNAL_X86_SSE_VECTOR_TRAITS_H__

#include "WNMath/inc/Internal/Generic/WNVectorTraits.h"

#include <xmmintrin.h>

namespace wn {
    namespace internal {
        namespace math {
            struct vector_traits_sse : vector_traits_generic {
                typedef vector_traits_generic base;

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE type length_squared(const element_array<type, dimension>& _element_array) {
                    return(base::template length_squared<type, dimension>(_element_array));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<core::is_same<type, wn_float32>::value>::type length_squared(const element_array<wn_float32, 2>& _element_array) {
                    const __m128 squared = _mm_mul_ps(_element_array.m_xmm_values[0], _element_array.m_xmm_values[0]);

                    return(_mm_cvtss_f32(_mm_add_ss(squared, _mm_shuffle_ps(squared, squared, _MM_SHUFFLE(1, 0, 2, 3)))));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<core::is_same<type, wn_float32>::value, wn_float32>::type length_squared(const element_array<wn_float32, 4>& _element_array) {
                    const __m128 squared = _mm_mul_ps(_element_array.m_xmm_values[0], _element_array.m_xmm_values[0]);
                    const __m128 partial = _mm_add_ps(squared, _mm_shuffle_ps(squared, squared, _MM_SHUFFLE(1, 0, 3, 2)));

                    return(_mm_cvtss_f32(_mm_add_ss(partial, _mm_shuffle_ps(partial, partial, _MM_SHUFFLE(0, 1, 0, 1)))));
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE wn_void normalize(element_array<type, dimension>& _element_array) {
                    base::normalize(_element_array);
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<core::is_same<type, wn_float64>::value>::type normalize(element_array<wn_float32, 4>& _element_array) {
                    const __m128 squared = _mm_mul_ps(_element_array.m_xmm_values[0], _element_array.m_xmm_values[0]);
                    __m128 shuffle = _mm_shuffle_ps(squared, squared, _MM_SHUFFLE(1, 0, 3, 2));
                    __m128 length_squared = _mm_add_ps(squared, shuffle);

                    shuffle = _mm_shuffle_ps(length_squared, length_squared, _MM_SHUFFLE(0, 1, 0, 1));
                    length_squared = _mm_add_ps(length_squared, shuffle);

                    #ifdef __WN_MATH_APPROXIMATIONS_ENABLED
                        const __m128 inverse_length = _mm_rsqrt_ps(length_squared);

                        _element_array.m_xmm_values[0] = _mm_mul_ps(_element_array.m_xmm_values[0], inverse_length);
                    #else
                        const __m128 length = _mm_sqrt_ps(length_squared);

                        _element_array.m_xmm_values[0] = _mm_div_ps(_element_array.m_xmm_values[0], length);
                    #endif
                }

                template <typename type, const wn_size_t dimension>
                static WN_FORCE_INLINE typename core::enable_if<core::is_real<type>::value>::type
                recipricol(element_array<type, dimension>& _element_array) {
                    base::recipricol(_element_array);
                }

                template <const wn_size_t dimension>
                static WN_FORCE_INLINE typename core::enable_if<(dimension > 1 && dimension < 5)>::type
                recipricol(element_array<wn_float32, dimension>& _element_array) {
                    #ifdef __WN_MATH_APPROXIMATIONS_ENABLED
                        _element_array.m_xmm_values[0] = _mm_rcp_ps(_element_array.m_xmm_values[0]);
                    #else
                        _element_array.m_xmm_values[0] = _mm_div_ps(_mm_set_ss(1.0f), _element_array.m_xmm_values[0]);
                    #endif
                }

                template <const wn_size_t dimension>
                static WN_FORCE_INLINE typename core::enable_if<(dimension > 4)>::type
                recipricol(element_array<wn_float32, dimension>& _element_array) {
                    #ifndef __WN_MATH_APPROXIMATIONS_ENABLED
                        const __m128 one = _mm_set_ss(1.0f);
                    #endif

                    for (wn_size_t i = 0; i < xmm_count_ps<dimension>::value; ++i) {
                        #ifdef __WN_MATH_APPROXIMATIONS_ENABLED
                            _element_array.m_xmm_values[i] = _mm_rcp_ps(_element_array.m_xmm_values[i]);
                        #else
                            _element_array.m_xmm_values[i] = _mm_div_ps(one, _element_array.m_xmm_values[i]);
                        #endif
                    }
                }
            };
        }
    }
}

#endif // __WN_MATH_INTERNAL_X86_SSE_GENERAL_TRAITS_H__
