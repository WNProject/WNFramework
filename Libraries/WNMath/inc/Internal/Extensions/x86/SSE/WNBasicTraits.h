// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_SSE_BASIC_TRAITS_H__
#define __WN_MATH_INTERNAL_X86_SSE_BASIC_TRAITS_H__

#include "WNMath/inc/Internal/Generic/WNBasicTraits.h"

#include <xmmintrin.h>

namespace wn {
    namespace internal {
        namespace math {
            struct basic_traits_sse : basic_traits_generic {
                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<core::is_signed<type>::value, type>::type abs(const type& _value) {
                    return(base::abs(_value));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<core::is_same<float, type>::value, type>::type abs(const float& _value) {
                    return(_mm_cvtss_f32(_mm_andnot_ps(_mm_set_ss(-0.0f), _mm_set_ss(_value))));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<(std::is_integral<type>::value ||
                                                                 core::is_real<type>::value), type>::type sqrt(const type& _value) {
                    return(base::sqrt(_value));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<core::is_same<float, type>::value, type>::type sqrt(const float& _value) {
                    #ifdef __WN_MATH_APPROXIMATIONS_ENABLED
                        return(_mm_cvtss_f32(_mm_rcp_ss(_mm_rsqrt_ss(_mm_set_ss(_value)))));
                    #else
                        return(_mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(_value))));
                    #endif
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<core::is_real<type>::value, type>::type invsqrt(const type& _value) {
                    return(base::invsqrt(_value));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<core::is_same<float, type>::value, type>::type invsqrt(const float& _value) {
                    #ifdef __WN_MATH_APPROXIMATIONS_ENABLED
                        return(_mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(_value))));
                    #else
                        return(_mm_cvtss_f32(_mm_div_ss(_mm_set_ss(1.0f), _mm_sqrt_ss(_mm_set_ss(_value)))));
                    #endif
                }

                template <typename type>
                static WN_FORCE_INLINE type min(const type& _value1, const type& _value2) {
                    return(base::min(_value1, _value2));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<core::is_same<float, type>::value, type>::type min(const float& _value1, const float& _value2) {
                    return(_mm_cvtss_f32(_mm_min_ss(_mm_set_ss(_value1), _mm_set_ss(_value2))));
                }

                template <typename type>
                static WN_FORCE_INLINE type max(const type& _value1, const type& _value2) {
                    return(base::max(_value1, _value2));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<core::is_same<float, type>::value, type>::type max(const float& _value1, const float& _value2) {
                    return(_mm_cvtss_f32(_mm_max_ss(_mm_set_ss(_value1), _mm_set_ss(_value2))));
                }

                template <typename type>
                static WN_FORCE_INLINE type clamp(const type& _value, const type& _min, const type& _max) {
                    return(base::clamp(_value, _min, _max));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<core::is_same<float, type>::value, type>::type clamp(const float& _value, const float& _min,
                                                        const float& _max) {
                    return(_mm_cvtss_f32(_mm_min_ss(_mm_max_ss(_mm_set_ss(_value), _mm_set_ss(_min)), _mm_set_ss(_max))));
                }

            private:
                typedef basic_traits_generic base;
            };
         }
    }
}

#endif // __WN_MATH_INTERNAL_X86_SSE_BASIC_TRAITS_H__
