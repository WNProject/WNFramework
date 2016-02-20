// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_SSE2_BASIC_TRAITS_H__
#define __WN_MATH_INTERNAL_X86_SSE2_BASIC_TRAITS_H__

#include "WNMath/inc/Internal/Extensions/x86/SSE/WNBasicTraits.h"

#include <emmintrin.h>

namespace wn {
namespace internal {
namespace math {
struct basic_traits_sse2 : basic_traits_sse {
  template <typename type>
  static WN_FORCE_INLINE
      typename core::enable_if<core::is_signed<type>::value, type>::type
      abs(const type& _value) {
    return (base::abs(_value));
  }

  template <typename type>
  static WN_FORCE_INLINE
      typename core::enable_if<core::is_same<double, type>::value, type>::type
      abs(const double& _value) {
    return (_mm_cvtsd_f64(_mm_andnot_pd(_mm_set_sd(-0.0), _mm_set_sd(_value))));
  }

  template <typename type>
  static WN_FORCE_INLINE typename core::enable_if<
      (std::is_integral<type>::value || core::is_real<type>::value), type>::type
  sqrt(const type& _value) {
    return (base::sqrt(_value));
  }

  template <typename type>
  static WN_FORCE_INLINE
      typename core::enable_if<core::is_same<double, type>::value, type>::type
      sqrt(const double& _value) {
    return (_mm_cvtsd_f64(_mm_sqrt_sd(_mm_setzero_pd(), _mm_set_sd(_value))));
  }

  template <typename type>
  static WN_FORCE_INLINE
      typename core::enable_if<core::is_real<type>::value, type>::type
      invsqrt(const type& _value) {
    return (base::invsqrt(_value));
  }

  template <typename type>
  static WN_FORCE_INLINE
      typename core::enable_if<core::is_same<double, type>::value, type>::type
      invsqrt(const double& _value) {
    return (_mm_cvtsd_f64(_mm_div_sd(
        _mm_set_sd(1.0), _mm_sqrt_sd(_mm_setzero_pd(), _mm_set_sd(_value)))));
  }

  template <typename type>
  static WN_FORCE_INLINE type min(const type& _value1, const type& _value2) {
    return (base::min(_value1, _value2));
  }

  template <typename type>
  static WN_FORCE_INLINE
      typename core::enable_if<core::is_same<double, type>::value, type>::type
      min(const double& _value1, const double& _value2) {
    return (
        _mm_cvtsd_f64(_mm_min_sd(_mm_set_sd(_value1), _mm_set_sd(_value2))));
  }

  template <typename type>
  static WN_FORCE_INLINE type max(const type& _value1, const type& _value2) {
    return (base::max(_value1, _value2));
  }

  template <typename type>
  static WN_FORCE_INLINE
      typename core::enable_if<core::is_same<double, type>::value, type>::type
      max(const double& _value1, const double& _value2) {
    return (
        _mm_cvtsd_f64(_mm_max_sd(_mm_set_sd(_value1), _mm_set_sd(_value2))));
  }

  template <typename type>
  static WN_FORCE_INLINE type clamp(
      const type& _value, const type& _min, const type& _max) {
    return (base::clamp(_value, _min, _max));
  }

  template <typename type>
  static WN_FORCE_INLINE
      typename core::enable_if<core::is_same<double, type>::value, type>::type
      clamp(const double& _value, const double& _min, const double& _max) {
    return (_mm_cvtsd_f64(_mm_min_sd(
        _mm_max_sd(_mm_set_sd(_value), _mm_set_sd(_min)), _mm_set_sd(_max))));
  }

private:
  typedef basic_traits_sse base;
};
}
}
}

#endif  // __WN_MATH_INTERNAL_X86_SSE2_BASIC_TRAITS_H__
