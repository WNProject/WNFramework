// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_SSE4_1_BASIC_TRAITS_H__
#define __WN_MATH_INTERNAL_X86_SSE4_1_BASIC_TRAITS_H__

#include "WNMath/inc/Internal/Extensions/x86/SSE2/WNBasicTraits.h"

#include <smmintrin.h>

namespace wn {
    namespace internal {
        namespace math {
            struct basic_traits_sse4_1 : basic_traits_sse2 {
                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<(is_real<type>::value || std::is_integral<type>::value), type>::type
                mod(const type& _dividend, const type& _divisor) {
                    base::mod(_dividend, _divisor);
                }

                template <>
                static WN_FORCE_INLINE wn_float32 mod(const wn_float32& _dividend, const wn_float32& _divisor) {
                    const __m128 divisor = _mm_load_ss(&_divisor);

                    #ifdef __WN_MATH_APPROXIMATIONS_ENABLED
                        const __m128 result = _mm_mul_ss(_mm_load_ss(&_dividend), _mm_rcp_ss(divisor));
                    #else
                        const __m128 result = _mm_div_ss(_mm_load_ss(&_dividend), divisor);
                    #endif

                    const __m128 interger_portion = _mm_round_ss(_mm_setzero_ps(), result, _MM_FROUND_TRUNC);

                    return(_mm_cvtss_f32(_mm_mul_ss(_mm_sub_ss(result, interger_portion), divisor)));
                }

                template <>
                static WN_FORCE_INLINE wn_float64 mod(const wn_float64& _dividend, const wn_float64& _divisor) {
                    const __m128d divisor = _mm_load_sd(&_divisor);
                    const __m128d result = _mm_div_sd(_mm_load_sd(&_dividend), divisor);
                    const __m128d interger_portion = _mm_round_sd(_mm_setzero_pd(), result, _MM_FROUND_TRUNC);

                    return(_mm_cvtsd_f64(_mm_mul_sd(_mm_sub_sd(result, interger_portion), divisor)));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<is_real<type>::value, type>::type round(const type& _value) {
                    return(base::round(_value));
                }

                template <>
                static WN_FORCE_INLINE wn_float32 round(const wn_float32& _value) {
                    return(_mm_cvtss_f32(_mm_round_ss(_mm_setzero_ps(), _mm_load_ss(&_value), _MM_FROUND_NINT)));
                }

                template <>
                static WN_FORCE_INLINE wn_float64 round(const wn_float64& _value) {
                    return(_mm_cvtsd_f64(_mm_round_sd(_mm_setzero_pd(), _mm_load_sd(&_value), _MM_FROUND_NINT)));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<is_real<type>::value, type>::type ceil(const type& _value) {
                    return(base::ceil(_value));
                }

                template <>
                static WN_FORCE_INLINE wn_float32 ceil(const wn_float32& _value) {
                    return(_mm_cvtss_f32(_mm_ceil_ss(_mm_setzero_ps(), _mm_load_ss(&_value))));
                }

                template <>
                static WN_FORCE_INLINE wn_float64 ceil(const wn_float64& _value) {
                    return(_mm_cvtsd_f64(_mm_ceil_sd(_mm_setzero_pd(), _mm_load_sd(&_value))));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<is_real<type>::value, type>::type floor(const type& _value) {
                    return(base::floor(_value));
                }

                template <>
                static WN_FORCE_INLINE wn_float32 floor(const wn_float32& _value) {
                    return(_mm_cvtss_f32(_mm_floor_ss(_mm_setzero_ps(), _mm_load_ss(&_value))));
                }

                template <>
                static WN_FORCE_INLINE wn_float64 floor(const wn_float64& _value) {
                    return(_mm_cvtsd_f64(_mm_floor_sd(_mm_setzero_pd(), _mm_load_sd(&_value))));
                }

                template <typename type>
                static WN_FORCE_INLINE typename core::enable_if<is_real<type>::value, type>::type trunc(const type& _value) {
                    return(base::trunc(_value));
                }

                template <>
                static WN_FORCE_INLINE wn_float32 trunc(const wn_float32& _value) {
                    return(_mm_cvtss_f32(_mm_round_ss(_mm_setzero_ps(), _mm_load_ss(&_value), _MM_FROUND_TRUNC)));
                }

                template <>
                static WN_FORCE_INLINE wn_float64 trunc(const wn_float64& _value) {
                    return(_mm_cvtsd_f64(_mm_round_sd(_mm_setzero_pd(), _mm_load_sd(&_value), _MM_FROUND_TRUNC)));
                }

            private:
                typedef basic_traits_sse2 base;
            };
        }
    }
}

#endif // __WN_MATH_INTERNAL_X86_SSE4_1_BASIC_TRAITS_H__