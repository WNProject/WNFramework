// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_INTERNAL_GENERIC_BASIC_TRAITS_H__
#define __WN_MATH_INTERNAL_GENERIC_BASIC_TRAITS_H__

#include "WNCore/inc/type_traits.h"
#include "WNCore/inc/types.h"
#include "WNCore/inc/utilities.h"

#include <cmath>

namespace wn {
namespace math {
namespace internal {

struct basic_traits_generic : core::non_constructable {
  template <typename type>
  static inline typename core::enable_if<(std::is_integral<type>::value &&
                                             core::is_signed<type>::value),
      type>::type
  abs(const type& _value) {
    const type negate = static_cast<type>(_value < 0);

    return ((_value ^ -negate) + negate);
  }

  template <typename type>
  static inline
      typename core::enable_if<(core::is_floating_point<type>::value &&
                                   std::is_fundamental<type>::value),
          type>::type
      abs(const type& _value) {
    return (::fabs(_value));
  }

  template <typename type>
  static inline
      typename core::enable_if<(core::is_floating_point<type>::value &&
                                   !std::is_fundamental<type>::value),
          type>::type
      abs(const type& _value) {
    return (static_cast<type>(abs(static_cast<float>(_value))));
  }

  template <typename type>
  static inline
      typename core::enable_if<(!std::is_integral<type>::value &&
                                   !core::is_floating_point<type>::value &&
                                   core::is_signed<type>::value),
          type>::type
      abs(const type& _value) {
    return (_value < static_cast<type>(0) ? -_value : _value);
  }

  template <typename type>
  static inline
      typename core::enable_if<std::is_integral<type>::value, type>::type
      mod(const type& _dividend, const type& _divisor) {
    return (_dividend % _divisor);
  }

  template <typename type>
  static inline
      typename core::enable_if<std::is_floating_point<type>::value, type>::type
      mod(const type& _dividend, const type& _divisor) {
    return (::fmod(_dividend, _divisor));
  }

  template <typename type>
  static inline
      typename core::enable_if<(core::is_floating_point<type>::value &&
                                   !std::is_floating_point<type>::value),
          type>::type
      mod(const type& _dividend, const type& _divisor) {
    return (static_cast<type>(
        mod(static_cast<float>(_dividend), static_cast<float>(_divisor))));
  }

  static inline float sqrt(const float& _value) {
    return (::sqrt(_value));
  }

  static inline double sqrt(const double& _value) {
    return (::sqrt(_value));
  }

  template <typename type>
  static inline
      typename core::enable_if<std::is_integral<type>::value, type>::type
      sqrt(const type& _value) {
    return (static_cast<type>(sqrt(static_cast<double>(_value))));
  }

  template <typename type>
  static inline
      typename core::enable_if<(core::is_floating_point<type>::value &&
                                   !std::is_floating_point<type>::value),
          type>::type
      sqrt(const type& _value) {
    return (static_cast<type>(sqrt(static_cast<float>(_value))));
  }

  template <typename type>
  static inline
      typename core::enable_if<(std::is_floating_point<type>::value &&
                                   !core::is_same<type, float>::value &&
                                   !core::is_same<type, double>::value),
          type>::type
      sqrt(const type& _value) {
    return (::sqrt(_value));
  }

  static inline float invsqrt(const float& _value) {
    return (1.0f / sqrt(_value));
  }

  static inline double invsqrt(const double& _value) {
    return (1.0 / sqrt(_value));
  }

  template <typename type>
  static inline
      typename core::enable_if<(core::is_floating_point<type>::value &&
                                   !std::is_floating_point<type>::value),
          type>::type
      invsqrt(const type& _value) {
    return (static_cast<type>(invsqrt(static_cast<float>(_value))));
  }

  template <typename type>
  static inline
      typename core::enable_if<(std::is_floating_point<type>::value &&
                                   !core::is_same<type, float>::value &&
                                   !core::is_same<type, double>::value),
          type>::type
      invsqrt(const type& _value) {
    return (static_cast<type>(1.0) / sqrt(_value));
  }

  template <typename type>
  static inline
      typename core::enable_if<std::is_integral<type>::value, type>::type
      min(const type& _value1, const type& _value2) {
    return (_value2 ^ ((_value1 ^ _value2) & -(_value1 < _value2)));
  }

  template <typename type>
  static inline
      typename core::enable_if<std::is_floating_point<type>::value, type>::type
      min(const type& _value1, const type& _value2) {
    return (::fmin(_value1, _value2));
  }

  template <typename type>
  static inline
      typename core::enable_if<(core::is_floating_point<type>::value &&
                                   !std::is_floating_point<type>::value),
          type>::type
      min(const type& _value1, const type& _value2) {
    return (static_cast<type>(
        min(static_cast<float>(_value1), static_cast<float>(_value2))));
  }

  template <typename type>
  static inline
      typename core::enable_if<(!std::is_integral<type>::value &&
                                   !core::is_floating_point<type>::value),
          type>::type
      min(const type& _value1, const type& _value2) {
    return (_value1 < _value2 ? _value1 : _value2);
  }

  template <typename type>
  static inline
      typename core::enable_if<std::is_integral<type>::value, type>::type
      max(const type& _value1, const type& _value2) {
    return (_value1 ^ ((_value1 ^ _value2) & -(_value1 < _value2)));
  }

  template <typename type>
  static inline
      typename core::enable_if<std::is_floating_point<type>::value, type>::type
      max(const type& _value1, const type& _value2) {
    return (::fmax(_value1, _value2));
  }

  template <typename type>
  static inline
      typename core::enable_if<(core::is_floating_point<type>::value &&
                                   !std::is_floating_point<type>::value),
          type>::type
      max(const type& _value1, const type& _value2) {
    return (static_cast<type>(
        max(static_cast<float>(_value1), static_cast<float>(_value2))));
  }

  template <typename type>
  static inline
      typename core::enable_if<(!std::is_integral<type>::value &&
                                   !core::is_floating_point<type>::value),
          type>::type
      max(const type& _value1, const type& _value2) {
    return (_value1 > _value2 ? _value1 : _value2);
  }

  template <typename type>
  static inline type clamp(
      const type& _value, const type& _min, const type& _max) {
    return (min(max(_value, _min), _max));
  }

  template <typename type>
  static inline
      typename core::enable_if<std::is_integral<type>::value, type>::type
      pow(const type& _base, const type& _exponent) {
    type base = _base;
    type exponent = _exponent;
    type result = 1;

    while (exponent) {
      if (exponent & 1) {
        result *= base;
      }

      exponent >>= 1;
      base *= base;
    }

    return (result);
  }

  template <typename type>
  static inline
      typename core::enable_if<(core::is_floating_point<type>::value &&
                                   !std::is_floating_point<type>::value),
          type>::type
      pow(const type& _base, const type& _exponent) {
    return (static_cast<type>(
        pow(static_cast<float>(_base), static_cast<float>(_exponent))));
  }

  template <typename type>
  static inline
      typename core::enable_if<std::is_floating_point<type>::value, type>::type
      pow(const type& _base, const type& _exponent) {
    return (::pow(_base, _exponent));
  }

  template <typename type>
  static inline
      typename core::enable_if<(core::is_floating_point<type>::value &&
                                   !std::is_floating_point<type>::value),
          type>::type
      ceil(const type& _value) {
    return (static_cast<type>(ceil(static_cast<float>(_value))));
  }

  template <typename type>
  static inline
      typename core::enable_if<std::is_floating_point<type>::value, type>::type
      ceil(const type& _value) {
    return (::ceil(_value));
  }

  template <typename type>
  static inline
      typename core::enable_if<(core::is_floating_point<type>::value &&
                                   !std::is_floating_point<type>::value),
          type>::type
      floor(const type& _value) {
    return (static_cast<type>(floor(static_cast<float>(_value))));
  }

  template <typename type>
  static inline
      typename core::enable_if<std::is_floating_point<type>::value, type>::type
      floor(const type& _value) {
    return (::floor(_value));
  }

  template <typename type>
  static inline
      typename core::enable_if<(core::is_floating_point<type>::value &&
                                   !std::is_floating_point<type>::value),
          type>::type
      round(const type& _value) {
    return (static_cast<type>(round(static_cast<float>(_value))));
  }

  template <typename type>
  static inline
      typename core::enable_if<std::is_floating_point<type>::value, type>::type
      round(const type& _value) {
    return (::round(_value));
  }

  template <typename type>
  static inline
      typename core::enable_if<(core::is_floating_point<type>::value &&
                                   !std::is_floating_point<type>::value),
          type>::type
      trunc(const type& _value) {
    return (static_cast<type>(trunc(static_cast<float>(_value))));
  }

  template <typename type>
  static inline
      typename core::enable_if<std::is_floating_point<type>::value, type>::type
      trunc(const type& _value) {
    return (::trunc(_value));
  }

private:
  template <typename type, const size_t exponent>
  struct pow2
    : core::integral_constant<type, (2 * pow2<type, (exponent - 1)>::value)> {};

  template <typename type>
  struct pow2<type, 0> : core::integral_constant<type, 1> {};
};

}  // namespace internal
}  // namespace math
}  // namespace wn

#endif  // __WN_MATH_INTERNAL_GENERIC_BASIC_TRAITS_H__
