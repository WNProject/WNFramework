// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_GENERIC_VECTOR_BASE_H__
#define __WN_MATH_INTERNAL_GENERIC_VECTOR_BASE_H__

#include "WNMath/inc/Internal/WNCommonBase.h"
#include "WNMath/inc/WNBasic.h"
#include "core/inc/type_traits.h"

namespace wn {
namespace math {
namespace internal {

template <template <typename, size_t, bool> class VectorType, typename T,
    size_t Dimension, bool Precise>
class vector_base_generic_common : public common_base<T, Dimension> {
private:
  using base = common_base<T, Dimension>;

public:
  using base::assign;

  inline vector_base_generic_common() : base() {}

  inline vector_base_generic_common(vector_base_generic_common&& _vector)
    : base() {
    for (size_t i = 0; i < Dimension; ++i) {
      base::at(i) = core::move(_vector.at(i));
    }
  }

  inline vector_base_generic_common(const vector_base_generic_common& _vector)
    : base(_vector.m_values) {}

  inline explicit vector_base_generic_common(const T& _value) : base(_value) {}

  inline explicit vector_base_generic_common(const T (&_values)[Dimension])
    : base(_values) {}

  template <typename Itr>
  inline vector_base_generic_common(Itr _begin, Itr _end)
    : base(_begin, _end) {}

  inline T dot(const vector_base_generic_common& _vector) const {
    T result(0);

    for (size_t i = 0; i < Dimension; ++i) {
      result += base::at(i) * _vector.at(i);
    }

    return result;
  }

  inline T length_squared() const {
    return dot(*this);
  }

  inline T length() const {
    return sqrt(length_squared());
  }

  inline void truncate(const T& _length) {
    static const T zero(0);

    if (_length > zero) {
      const T result(length_squared());

      if (result > (_length * _length)) {
        const T adjusted_result(_length / sqrt(result));

        multiply_assign(adjusted_result);
      }
    } else {
      assign(zero);
    }
  }

  inline void reverse() {
    const size_t adjusted_size = Dimension - 1;

    for (size_t i = 0; i < (Dimension / 2); ++i) {
      core::swap(base::at(i), base::at(adjusted_size - i));
    }
  }

protected:
  inline VectorType<T, Dimension, Precise> multiply(const T& _value) const {
    VectorType<T, Dimension, Precise> new_vector;

    for (size_t i = 0; i < Dimension; ++i) {
      new_vector.at(i) = base::at(i) * _value;
    }

    return new_vector;
  }

  inline VectorType<T, Dimension, Precise> divide(const T& _value) const {
    VectorType<T, Dimension, Precise> new_vector;

    for (size_t i = 0; i < Dimension; ++i) {
      new_vector.at(i) = base::at(i) / _value;
    }

    return new_vector;
  }

  inline void multiply_assign(const T& _value) {
    for (size_t i = 0; i < Dimension; ++i) {
      base::at(i) *= _value;
    }
  }

  inline void divide_assign(const T& _value) {
    for (size_t i = 0; i < Dimension; ++i) {
      base::at(i) /= _value;
    }
  }
};

template <template <typename, size_t, bool> class VectorType, typename T,
    size_t Dimension, bool Precise>
class vector_base_generic_signed_only
  : public vector_base_generic_common<VectorType, T, Dimension, Precise> {
private:
  using base = vector_base_generic_common<VectorType, T, Dimension, Precise>;

public:
  inline vector_base_generic_signed_only() : base() {}

  inline vector_base_generic_signed_only(
      vector_base_generic_signed_only&& _vector)
    : base(core::move(_vector)) {}

  inline vector_base_generic_signed_only(
      const vector_base_generic_signed_only& _vector)
    : base(_vector) {}

  inline explicit vector_base_generic_signed_only(const T& _value)
    : base(_value) {}

  inline explicit vector_base_generic_signed_only(const T (&_values)[Dimension])
    : base(_values) {}

  template <typename Itr>
  inline vector_base_generic_signed_only(Itr _begin, Itr _end)
    : base(_begin, _end) {}

  inline VectorType<T, Dimension, Precise> operator-() const {
    VectorType<T, Dimension, Precise> new_vector;

    for (size_t i = 0; i < Dimension; ++i) {
      new_vector.at(i) = -base::at(i);
    }

    return new_vector;
  }

  inline void negate() {
    for (size_t i = 0; i < Dimension; ++i) {
      base::at(i) = -base::at(i);
    }
  }
};

template <template <typename, size_t, bool> class VectorType, typename T,
    size_t Dimension, bool Precise>
class vector_base_generic_floating_point_only
  : public vector_base_generic_signed_only<VectorType, T, Dimension, Precise> {
private:
  using base =
      vector_base_generic_signed_only<VectorType, T, Dimension, Precise>;

public:
  inline vector_base_generic_floating_point_only() : base() {}

  inline vector_base_generic_floating_point_only(
      vector_base_generic_floating_point_only&& _vector)
    : base(core::move(_vector)) {}

  inline vector_base_generic_floating_point_only(
      const vector_base_generic_floating_point_only& _vector)
    : base(_vector) {}

  inline explicit vector_base_generic_floating_point_only(const T& _value)
    : base(_value) {}

  inline explicit vector_base_generic_floating_point_only(
      const T (&_values)[Dimension])
    : base(_values) {}

  template <typename Itr>
  inline vector_base_generic_floating_point_only(Itr _begin, Itr _end)
    : base(_begin, _end) {}

  inline T inverse_length() const {
    return invsqrt(base::length_squared());
  }

  inline void normalize() {
    base::multiply_assign(inverse_length());
  }

  inline VectorType<T, Dimension, Precise> normalized() const {
    VectorType<T, Dimension, Precise> v;
    for (size_t i = 0; i < Dimension; ++i) {
      v.at(i) = base::at(i);
    }
    v.normalize();
    return v;
  }
};

template <template <typename, size_t, bool> class VectorType, typename T,
    size_t Dimension>
class vector_base_generic_floating_point_only<VectorType, T, Dimension, true>
  : public vector_base_generic_floating_point_only<VectorType, T, Dimension,
        false> {
private:
  using base =
      vector_base_generic_floating_point_only<VectorType, T, Dimension, false>;

public:
  inline vector_base_generic_floating_point_only() : base() {}

  inline vector_base_generic_floating_point_only(
      vector_base_generic_floating_point_only&& _vector)
    : base(core::move(_vector)) {}

  inline vector_base_generic_floating_point_only(
      const vector_base_generic_floating_point_only& _vector)
    : base(_vector) {}

  inline explicit vector_base_generic_floating_point_only(const T& _value)
    : base(_value) {}

  inline explicit vector_base_generic_floating_point_only(
      const T (&_values)[Dimension])
    : base(_values) {}

  template <typename Itr>
  inline vector_base_generic_floating_point_only(Itr _begin, Itr _end)
    : base(_begin, _end) {}

  inline void normalize() {
    base::divide_assign(base::length());
  }
};

template <template <typename, size_t, bool> class VectorType, typename T,
    size_t Dimension, bool Precise>
using vector_base_generic = core::conditional_t<
    core::is_floating_point<T>::value,
    vector_base_generic_floating_point_only<VectorType, T, Dimension, Precise>,
    core::conditional_t<core::is_signed<T>::value,
        vector_base_generic_signed_only<VectorType, T, Dimension, Precise>,
        vector_base_generic_common<VectorType, T, Dimension, Precise>>>;

}  // namespace internal
}  // namespace math
}  // namespace wn

#endif  // __WN_MATH_INTERNAL_GENERIC_VECTOR_BASE_H__
