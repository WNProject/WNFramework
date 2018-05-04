// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_BASIC_H__
#define __WN_MATH_BASIC_H__

#include "WNCore/inc/type_traits.h"
#include "WNCore/inc/types.h"
#include "WNMath/inc/Internal/WNBasicTraits.h"
#include "WNMath/inc/WNConstants.h"

namespace wn {
namespace math {

template <typename _Type>
WN_FORCE_INLINE
    typename core::enable_if<core::is_signed<_Type>::value, _Type>::type
    abs(const _Type& _value) {
  return (math::internal::basic_traits::abs<_Type>(_value));
}

template <typename _Type>
WN_FORCE_INLINE
    typename core::enable_if<core::is_floating_point<_Type>::value, _Type>::type
    ceil(const _Type& _value) {
  return (math::internal::basic_traits::ceil(_value));
}

template <typename _Type>
WN_FORCE_INLINE
    typename core::enable_if<core::is_floating_point<_Type>::value, _Type>::type
    floor(const _Type& _value) {
  return (math::internal::basic_traits::floor(_value));
}

template <typename _Type>
WN_FORCE_INLINE
    typename core::enable_if<core::is_floating_point<_Type>::value, _Type>::type
    round(const _Type& _value) {
  return (math::internal::basic_traits::round(_value));
}

template <typename _Type>
WN_FORCE_INLINE
    typename core::enable_if<core::is_floating_point<_Type>::value, _Type>::type
    trunc(const _Type& _value) {
  return (math::internal::basic_traits::trunc(_value));
}

template <typename _Type>
WN_FORCE_INLINE
    typename core::enable_if<(core::is_floating_point<_Type>::value ||
                                 std::is_integral<_Type>::value),
        _Type>::type
    mod(const _Type& _dividend, const _Type& _divisor) {
  return (math::internal::basic_traits::mod(_dividend, _divisor));
}

template <typename _Type>
WN_FORCE_INLINE
    typename core::enable_if<(core::is_floating_point<_Type>::value ||
                                 std::is_integral<_Type>::value),
        _Type>::type
    sqrt(const _Type& _value) {
  return (math::internal::basic_traits::sqrt(_value));
}

template <typename _Type>
WN_FORCE_INLINE
    typename core::enable_if<core::is_floating_point<_Type>::value, _Type>::type
    invsqrt(const _Type& _value) {
  return (math::internal::basic_traits::invsqrt(_value));
}

template <typename _Type>
WN_FORCE_INLINE _Type pow(const _Type& _base, const _Type& _exponent) {
  return (math::internal::basic_traits::pow(_base, _exponent));
}

template <typename _Type>
WN_FORCE_INLINE _Type min(const _Type& _value1, const _Type& _value2) {
  return (math::internal::basic_traits::min(_value1, _value2));
}

template <typename _Type>
WN_FORCE_INLINE _Type max(const _Type& _value1, const _Type& _value2) {
  return (math::internal::basic_traits::max(_value1, _value2));
}

template <typename _Type>
WN_FORCE_INLINE _Type clamp(
    const _Type& _value, const _Type& _min, const _Type& _max) {
  return (math::internal::basic_traits::clamp(_value, _min, _max));
}

template <typename _Type>
WN_FORCE_INLINE _Type deg_to_rad(const _Type& _value) {
  return (static_cast<_Type>(static_cast<double>(_value) * (WN_PI / 180.0)));
}

template <typename _Type>
WN_FORCE_INLINE _Type rad_to_deg(const _Type& _value) {
  return (static_cast<_Type>(static_cast<double>(_value) * (180.0 / WN_PI)));
}

// From wikipedia: https://en.wikipedia.org/wiki/Hamming_weight
WN_FORCE_INLINE uint8_t popcount(uint64_t value) {
  const uint64_t m1 = 0x5555555555555555;
  const uint64_t m2 = 0x3333333333333333;
  const uint64_t m4 = 0x0f0f0f0f0f0f0f0f;
  const uint64_t h01 = 0x0101010101010101;
  value -= (value >> 1) & m1;
  value = (value & m2) + ((value >> 2) & m2);
  value = (value + (value >> 4)) & m4;
  return (value * h01) >> 56;
}

// Extrapolated 32-bit version from above
WN_FORCE_INLINE uint8_t popcount(uint32_t value) {
  const uint32_t m1 = 0x55555555;
  const uint32_t m2 = 0x33333333;
  const uint32_t m4 = 0x0f0f0f0f;
  const uint32_t h01 = 0x01010101;

  value -= (value >> 1) & m1;
  value = (value & m2) + ((value >> 2) & m2);
  value = (value + (value >> 4)) & m4;
  return (value * h01) >> 24;
}

template <typename T>
WN_FORCE_INLINE uint8_t popcount_sparse(T value) {
  uint8_t count = 0;
  while (value) {
    count++;
    value &= (value - 1);
  }
  return count;
}

WN_FORCE_INLINE uint8_t trailing_zeros(uint32_t value) {
  static const uint8_t debruijn_values[32] = {0, 1, 28, 2, 29, 14, 24, 3, 30,
      22, 20, 15, 25, 17, 4, 8, 31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6,
      11, 5, 10, 9};
  const uint8_t v =
      (uint32_t)(((int32_t)value & -(int32_t)value) * 0x077CB531) >> 27;
  return debruijn_values[v];
}

WN_FORCE_INLINE uint8_t trailing_zeros(uint64_t value) {
  // table from
  // http://chessprogramming.wikispaces.com/De+Bruijn+Sequence+Generator
  const uint8_t debruijn_values[64] = {
      0,
      1,
      2,
      53,
      3,
      7,
      54,
      27,
      4,
      38,
      41,
      8,
      34,
      55,
      48,
      28,
      62,
      5,
      39,
      46,
      44,
      42,
      22,
      9,
      24,
      35,
      59,
      56,
      49,
      18,
      29,
      11,
      63,
      52,
      6,
      26,
      37,
      40,
      33,
      47,
      61,
      45,
      43,
      21,
      23,
      58,
      17,
      10,
      51,
      25,
      36,
      32,
      60,
      20,
      57,
      16,
      50,
      31,
      19,
      15,
      30,
      14,
      13,
      12,
  };
  return debruijn_values[(uint64_t)(((int64_t)value & -(int64_t)value) *
                                    0x022fdd63cc95386d) >>
                         58];
}

}  // namespace math
}  // namespace wn

#endif  // __WN_MATH_BASIC_H__
