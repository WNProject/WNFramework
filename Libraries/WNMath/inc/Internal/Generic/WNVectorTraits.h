// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_INTERNAL_GENERIC_VECTOR_TRAITS_H__
#define __WN_MATH_INTERNAL_GENERIC_VECTOR_TRAITS_H__

#include "WNMath/inc/Internal/WNGeneralTraits.h"
#include "WNMath/inc/WNBasic.h"
#include "WNMath/inc/WNCommon.h"

namespace wn {
namespace math {
namespace internal {

// all types for all dimensions

template <typename T, const size_t Dimension, const bool Precise = false>
class vector_traits_generic_base
    : public general_traits<T, Dimension, Precise> {
  static_assert(Dimension > 0, "dimension must be at greater than 0");

private:
  using base = general_traits<T, Dimension, Precise>;

public:
  static WN_FORCE_INLINE T dot(const element_array<T, Dimension>& _first,
      const element_array<T, Dimension>& _second) {
    T result(0);

    for (size_t i = 0; i < Dimension; ++i) {
      result += (_first[i] * _second[i]);
    }

    return core::move(result);
  }

  static WN_FORCE_INLINE T length_squared(
      const element_array<T, Dimension>& _elements) {
    return dot(_elements, _elements);
  }

  static WN_FORCE_INLINE T length(
      const element_array<T, Dimension>& _elements) {
    return sqrt(length_squared(_elements));
  }

  static WN_FORCE_INLINE void truncate(
      element_array<T, Dimension>& _elements, const T& _length) {
    static const T zero(0);

    if (_length > zero) {
      const T result(length_squared(_elements));

      if (result > (_length * _length)) {
        const T adjusted_result(_length / sqrt(result));

        base::multiply(_elements, adjusted_result);
      }
    } else {
      base::assign(_elements, zero);
    }
  }

  static WN_FORCE_INLINE void minimum(element_array<T, Dimension>& _elements1,
      const element_array<T, Dimension>& _elements2) {
    for (size_t i = 0; i < Dimension; ++i) {
      _elements1[i] = min(_elements1[i], _elements2[i]);
    }
  }

  static WN_FORCE_INLINE void maximum(element_array<T, Dimension>& _elements1,
      const element_array<T, Dimension>& _elements2) {
    for (size_t i = 0; i < Dimension; ++i) {
      _elements1[i] = max(_elements1[i], _elements2[i]);
    }
  }

  static WN_FORCE_INLINE void average(element_array<T, Dimension>& _elements1,
      const element_array<T, Dimension>& _elements2) {
    static const T two(2);

    for (size_t i = 0; i < Dimension; ++i) {
      _elements1[i] = _elements1[i] + ((_elements2[i] - _elements1[i]) / two);
    }
  }
};

// all real types for all dimensions

template <typename T, const size_t Dimension, const bool Precise = false,
    typename = core::enable_if_t<true>>
class vector_traits_generic_real
    : public vector_traits_generic_base<T, Dimension, Precise> {};

template <typename T, const size_t Dimension, const bool Precise>
class vector_traits_generic_real<T, Dimension, Precise,
    core::enable_if_t<core::is_real<T>::value>>
    : public vector_traits_generic_base<T, Dimension, Precise> {
private:
  using base = vector_traits_generic_base<T, Dimension, Precise>;

public:
  static WN_FORCE_INLINE T inverse_length(
      const element_array<T, Dimension>& _elements) {
    return invsqrt(base::length_squared(_elements));
  }

  static WN_FORCE_INLINE void recipricol(
      element_array<T, Dimension>& _elements) {
    static const T one(1);

    for (size_t i = 0; i < Dimension; ++i) {
      _elements[i] = one / _elements[i];
    }
  }
};

// all real types with precision differences for all dimensions

template <typename T, const size_t Dimension, const bool Precise = false,
    typename = core::enable_if_t<true>>
class vector_traits_generic_real_precision
    : public vector_traits_generic_real<T, Dimension, Precise> {
private:
  using base = vector_traits_generic_real_precision<T, Dimension, true>;

public:
  static WN_FORCE_INLINE void normalize(
      element_array<T, Dimension>& _elements) {
    base::multiply(_elements, base::inverse_length(_elements));
  }
};

template <typename T, const size_t Dimension>
class vector_traits_generic_real_precision<T, Dimension, true,
    core::enable_if_t<core::is_real<T>::value>>
    : public vector_traits_generic_real<T, Dimension, true> {
private:
  using base = vector_traits_generic_real_precision<T, Dimension, true>;

public:
  static WN_FORCE_INLINE void normalize(
      element_array<T, Dimension>& _elements) {
    base::divide(_elements, base::length(_elements));
  }
};

// all types for 3 dimensions

template <typename T, const size_t Dimension, const bool Precise = false>
class vector_traits_generic_3d
    : public vector_traits_generic_real_precision<T, Dimension, Precise> {};

template <typename T, const bool Precise>
class vector_traits_generic_3d<T, 3, Precise>
    : public vector_traits_generic_real_precision<T, 3, Precise> {
public:
  static WN_FORCE_INLINE void cross(element_array<T, 3>& _destination,
      const element_array<T, 3>& _elements1,
      const element_array<T, 3>& _elements2) {
    _destination[0] =
        (_elements1[1] * _elements2[2]) - (_elements1[2] * _elements2[1]);
    _destination[1] =
        -((_elements1[0] * _elements2[2]) - (_elements1[2] * _elements2[0]));
    _destination[2] =
        (_elements1[0] * _elements2[1]) - (_elements1[1] * _elements2[0]);
  }
};

// full traits stack

template <typename T, const size_t Dimension, const bool Precise = false>
using vector_traits_generic = vector_traits_generic_3d<T, Dimension, Precise>;

}  // namespace internal
}  // namespace math
}  // namespace wn

#endif  // __WN_MATH_INTERNAL_GENERIC_VECTOR_TRAITS_H__
