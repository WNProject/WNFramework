// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_VECTOR_H__
#define __WN_MATH_VECTOR_H__

#include "WNCore/inc/type_traits.h"
#include "WNMath/inc/Internal/WNVectorBase.h"

namespace wn {
namespace math {

template <typename T, size_t Dimension, bool Precise = false>
class vector final
  : public internal::vector_base<wn::math::vector, T, Dimension, Precise> {
  static_assert(
      Dimension > 0, "template integral parameter 'Dimension' must not be 0");
  static_assert(!core::is_const<T>::value,
      "template type parameter 'T' must not be const type");
  static_assert(!core::is_reference<T>::value,
      "template type parameter 'T' must not be a reference type");

private:
  using base = internal::vector_base<wn::math::vector, T, Dimension, Precise>;

public:
  WN_FORCE_INLINE vector() : base() {}

  WN_FORCE_INLINE vector(vector&& _vector) : base(core::move(_vector)) {}

  WN_FORCE_INLINE vector(const vector& _vector) : base(_vector) {}

  WN_FORCE_INLINE explicit vector(const T& _value) : base(_value) {}

  WN_FORCE_INLINE explicit vector(const T (&_values)[Dimension])
    : base(_values) {}

  template <typename Itr>
  WN_FORCE_INLINE vector(Itr _begin, Itr _end) : base(_begin, _end) {}

  WN_FORCE_INLINE vector(std::initializer_list<T> _initializer_list)
    : vector(_initializer_list.begin(), _initializer_list.end()) {}

  WN_FORCE_INLINE vector& operator=(vector&& _vector) {
    assign(core::move(_vector));

    return *this;
  }

  WN_FORCE_INLINE vector& operator=(const vector& _vector) {
    assign(_vector);

    return *this;
  }

  WN_FORCE_INLINE vector& operator=(const T& _value) {
    base::assign(_value);

    return *this;
  }

  WN_FORCE_INLINE vector& operator=(const T (&_values)[Dimension]) {
    base::assign(_values);

    return *this;
  }

  WN_FORCE_INLINE vector& operator=(
      std::initializer_list<T> _initializer_list) {
    vector(_initializer_list).swap(*this);

    return *this;
  }

  using base::assign;

  WN_FORCE_INLINE void assign(vector&& _vector) {
    vector(core::move(_vector)).swap(*this);
  }

  WN_FORCE_INLINE void assign(const vector& _vector) {
    vector(_vector).swap(*this);
  }

  WN_FORCE_INLINE vector operator+() const {
    return *this;
  }

  WN_FORCE_INLINE vector& operator*=(const T& _value) {
    base::multiply_assign(_value);

    return *this;
  }

  WN_FORCE_INLINE vector operator*(const T& _value) const {
    return base::multiply(_value);
  }

  WN_FORCE_INLINE vector& operator/=(const T& _value) {
    base::divide_assign(_value);

    return *this;
  }

  WN_FORCE_INLINE vector operator/(const T& _value) const {
    return base::divide(_value);
  }

  WN_FORCE_INLINE void scale(const T& _value) {
    *this *= _value;
  }

  WN_FORCE_INLINE vector scaled(const T& _value) {
    return (*this * _value);
  }
};

template <typename T, size_t Dimension, bool Precise>
WN_FORCE_INLINE vector<T, Dimension, Precise> operator*(
    const T& _lhs, vector<T, Dimension, Precise>&& _rhs) {
  _rhs *= _lhs;

  return _rhs;
}

template <typename T, size_t Dimension, bool Precise>
WN_FORCE_INLINE vector<T, Dimension, Precise> operator*(
    const T& _lhs, const vector<T, Dimension, Precise>& _rhs) {
  return (_rhs * _lhs);
}

template <typename T, size_t Dimension, bool Precise>
WN_FORCE_INLINE vector<T, Dimension, Precise> operator/(
    const T& _lhs, const vector<T, Dimension, Precise>& _rhs) {
  return (vector<T, Dimension, Precise>(_lhs) /= _rhs);
}

template <typename T, size_t Dimension>
using vector_precise = vector<T, Dimension, true>;

// short forms

template <typename T>
using vector2 = vector<T, 2>;

template <typename T>
using vector3 = vector<T, 3>;

template <typename T>
using vector4 = vector<T, 4>;

template <typename T>
using vector2_precise = vector_precise<T, 2>;

template <typename T>
using vector3_precise = vector_precise<T, 3>;

template <typename T>
using vector4_precise = vector_precise<T, 4>;

// short, short forms

template <typename T>
using vec2 = vector2<T>;

template <typename T>
using vec3 = vector3<T>;

template <typename T>
using vec4 = vector4<T>;

template <typename T>
using vec2_p = vector2_precise<T>;

template <typename T>
using vec3_p = vector3_precise<T>;

template <typename T>
using vec4_p = vector4_precise<T>;

// short, short, short forms

using vec2i8 = vec2<int8_t>;
using vec2i16 = vec2<int16_t>;
using vec2i32 = vec2<int32_t>;
using vec2i64 = vec2<int64_t>;
using vec2u8 = vec2<uint8_t>;
using vec2u16 = vec2<uint16_t>;
using vec2u32 = vec2<uint32_t>;
using vec2u64 = vec2<uint64_t>;
using vec2f = vec2<float>;
using vec2d = vec2<double>;

using vec3i8 = vec3<int8_t>;
using vec3i16 = vec3<int16_t>;
using vec3i32 = vec3<int32_t>;
using vec3i64 = vec3<int64_t>;
using vec3u8 = vec3<uint8_t>;
using vec3u16 = vec3<uint16_t>;
using vec3u32 = vec3<uint32_t>;
using vec3u64 = vec3<uint64_t>;
using vec3f = vec3<float>;
using vec3d = vec3<double>;

using vec4i8 = vec4<int8_t>;
using vec4i16 = vec4<int16_t>;
using vec4i32 = vec4<int32_t>;
using vec4i64 = vec4<int64_t>;
using vec4u8 = vec4<uint8_t>;
using vec4u16 = vec4<uint16_t>;
using vec4u32 = vec4<uint32_t>;
using vec4u64 = vec4<uint64_t>;
using vec4f = vec4<float>;
using vec4d = vec4<double>;

using vec2f_p = vec2_p<float>;
using vec2d_p = vec2_p<double>;

using vec3f_p = vec3_p<float>;
using vec3d_p = vec3_p<double>;

using vec4f_p = vec4_p<float>;
using vec4d_p = vec4_p<double>;

}  // namespace math
}  // namespace wn

#endif  // __WN_MATH_VECTOR_H__
