// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_VECTOR_H__
#define __WN_MATH_VECTOR_H__

#include "WNCore/inc/WNAssert.h"
#include "WNCore/inc/WNTypeTraits.h"
#include "WNMath/inc/Internal/WNElementArray.h"
#include "WNMath/inc/Internal/WNVectorTraits.h"
#include "WNMath/inc/WNCommon.h"

namespace wn {
namespace math {

template <typename T, const size_t Dimension>
class packed_vector;

template <typename T, const size_t Dimension,
    typename TraitsType = internal::vector_traits<T, Dimension>>
class basic_vector final {
  static_assert(Dimension > 0,
      "template integral parameter 'Dimension' must be greater than 0");
  static_assert(!core::is_const<T>::value,
      "template type parameter 'T' must not be const type");
  static_assert(!core::is_reference<T>::value,
      "template type paramter 'T' must not be a reference type");

public:
  typedef T value_type;
  typedef TraitsType traits_type;
  typedef core::add_lvalue_reference_t<T> reference;
  typedef core::add_lvalue_reference_t<core::add_const_t<T>> const_reference;
  typedef typename core::add_pointer<T>::type pointer;
  typedef typename core::add_pointer<core::add_const_t<T>>::type const_pointer;
  typedef T* iterator;
  typedef T const* const_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

  enum { dimension = Dimension };

  basic_vector() = default;

  WN_FORCE_INLINE basic_vector(basic_vector&& _vector) {
    TraitsType::assign(m_elements, core::move(_vector.m_elements));
  }

  WN_FORCE_INLINE basic_vector(const basic_vector& _vector) {
    TraitsType::assign(m_elements, _vector.m_elements);
  }

  template <const size_t OtherDimension, typename OtherTraitsType>
  WN_FORCE_INLINE basic_vector(
      basic_vector<T, OtherDimension, OtherTraitsType>&& _vector) {
    TraitsType::assign(m_elements, core::move(_vector.m_elements));
  }

  template <const size_t OtherDimension, typename OtherTraitsType>
  WN_FORCE_INLINE basic_vector(
      const basic_vector<T, OtherDimension, OtherTraitsType>& _vector) {
    TraitsType::assign(m_elements, _vector.m_elements);
  }

  basic_vector(const packed_vector<T, Dimension>& _vector);

  WN_FORCE_INLINE explicit basic_vector(const T& _value) {
    TraitsType::assign(m_elements, _value);
  }

  WN_FORCE_INLINE explicit basic_vector(const T* _values)
    : basic_vector(_values, Dimension) {}

  WN_FORCE_INLINE explicit basic_vector(const T* _values, const size_t _size) {
    TraitsType::assign(m_elements, _values, _size);
  }

  WN_FORCE_INLINE explicit basic_vector(const T (&_values)[Dimension])
    : basic_vector(&(_values[0])) {}

  WN_FORCE_INLINE basic_vector& operator=(basic_vector&& _vector) {
    assign(core::move(_vector));

    return *this;
  }

  WN_FORCE_INLINE basic_vector& operator=(const basic_vector& _vector) {
    assign(_vector);

    return *this;
  }

  template <const size_t OtherDimension, typename OtherTraitsType>
  WN_FORCE_INLINE basic_vector& operator=(
      basic_vector<T, OtherDimension, OtherTraitsType>&& _vector) {
    assign(core::move(_vector));

    return *this;
  }

  template <const size_t OtherDimension, typename OtherTraitsType>
  WN_FORCE_INLINE basic_vector& operator=(
      const basic_vector<T, OtherDimension, OtherTraitsType>& _vector) {
    assign(_vector);

    return *this;
  }

  WN_FORCE_INLINE basic_vector& operator=(
      const packed_vector<T, Dimension>& _vector) {
    assign(_vector);

    return *this;
  }

  WN_FORCE_INLINE basic_vector& operator=(const T& _value) {
    assign(_value);

    return *this;
  }

  WN_FORCE_INLINE basic_vector& operator=(const T (&_values)[Dimension]) {
    assign(_values);

    return *this;
  }

  WN_FORCE_INLINE void assign(basic_vector&& _vector) {
    basic_vector(core::move(_vector)).swap(*this);
  }

  WN_FORCE_INLINE void assign(const basic_vector& _vector) {
    basic_vector(_vector).swap(*this);
  }

  template <const size_t OtherDimension, typename OtherTraitsType>
  WN_FORCE_INLINE void assign(
      basic_vector<T, OtherDimension, OtherTraitsType>&& _vector) {
    basic_vector(core::move(_vector)).swap(*this);
  }

  template <const size_t OtherDimension, typename OtherTraitsType>
  WN_FORCE_INLINE void assign(
      const basic_vector<T, OtherDimension, OtherTraitsType>& _vector) {
    basic_vector(_vector).swap(*this);
  }

  WN_FORCE_INLINE void assign(const packed_vector<T, Dimension>& _vector) {
    basic_vector(_vector).swap(*this);
  }

  WN_FORCE_INLINE void assign(const T& _value) {
    basic_vector(_value).swap(*this);
  }

  WN_FORCE_INLINE void assign(const T* _values) {
    basic_vector(_values).swap(*this);
  }

  WN_FORCE_INLINE void assign(const T* _values, const size_t _size) {
    basic_vector(_values, _size).swap(*this);
  }

  WN_FORCE_INLINE void assign(const T (&_values)[Dimension]) {
    basic_vector(_values).swap(*this);
  }

  // element access

  WN_FORCE_INLINE reference operator[](const size_t _index) {
    return at(_index);
  }

  WN_FORCE_INLINE const_reference operator[](const size_t _index) const {
    return at(_index);
  }

  WN_FORCE_INLINE reference at(const size_t _index) {
    return m_elements[_index];
  }

  WN_FORCE_INLINE const_reference at(const size_t _index) const {
    return m_elements[_index];
  }

  WN_FORCE_INLINE pointer data() {
    return m_elements.data();
  }

  WN_FORCE_INLINE const_pointer data() const {
    return m_elements.data();
  }

  template <typename U = T,
      typename = core::enable_if_t<core::conjunction<core::is_same<T, U>,
          core::bool_constant<(Dimension <= 4)>>::value>>
  WN_FORCE_INLINE U& x() {
    return at(0);
  }

  template <typename U = T,
      typename = core::enable_if_t<core::conjunction<core::is_same<T, U>,
          core::bool_constant<(Dimension <= 4)>>::value>>
  WN_FORCE_INLINE const U& x() const {
    return at(0);
  }

  template <typename U = T,
      typename = core::enable_if_t<core::conjunction<core::is_same<T, U>,
          core::bool_constant<(Dimension <= 4 && Dimension > 1)>>::value>>
  WN_FORCE_INLINE U& y() {
    return at(1);
  }

  template <typename U = T,
      typename = core::enable_if_t<core::conjunction<core::is_same<T, U>,
          core::bool_constant<(Dimension <= 4 && Dimension > 1)>>::value>>
  WN_FORCE_INLINE const U& y() const {
    return at(1);
  }

  template <typename U = T,
      typename = core::enable_if_t<core::conjunction<core::is_same<T, U>,
          core::bool_constant<(Dimension <= 4 && Dimension > 2)>>::value>>
  WN_FORCE_INLINE U& z() {
    return at(2);
  }

  template <typename U = T,
      typename = core::enable_if_t<core::conjunction<core::is_same<T, U>,
          core::bool_constant<(Dimension <= 4 && Dimension > 2)>>::value>>
  WN_FORCE_INLINE const U& z() const {
    return at(2);
  }

  template <typename U = T,
      typename = core::enable_if_t<core::conjunction<core::is_same<T, U>,
          core::bool_constant<(Dimension == 4)>>::value>>
  WN_FORCE_INLINE U& w() {
    return at(3);
  }

  template <typename U = T,
      typename = core::enable_if_t<core::conjunction<core::is_same<T, U>,
          core::bool_constant<(Dimension == 4)>>::value>>
  WN_FORCE_INLINE const U& w() const {
    return at(3);
  }

  // iterators

  WN_FORCE_INLINE iterator begin() {
    return m_elements.data();
  }

  WN_FORCE_INLINE const_iterator begin() const {
    return cbegin();
  }

  WN_FORCE_INLINE const_iterator cbegin() const {
    return m_elements.data();
  }

  WN_FORCE_INLINE iterator end() {
    return (m_elements.data() + Dimension);
  }

  WN_FORCE_INLINE const_iterator end() const {
    return cend();
  }

  WN_FORCE_INLINE const_iterator cend() const {
    return (m_elements.data() + Dimension);
  }

  WN_FORCE_INLINE reverse_iterator rbegin() {
    return reverse_iterator(end());
  }

  WN_FORCE_INLINE const_reverse_iterator crbegin() const {
    return const_reverse_iterator(cend());
  }

  WN_FORCE_INLINE reverse_iterator rend() {
    return reverse_iterator(begin());
  }

  WN_FORCE_INLINE const_reverse_iterator crend() const {
    return const_reverse_iterator(cbegin());
  }

  // properties

  WN_FORCE_INLINE T length() const {
    return TraitsType::length(m_elements);
  }

  template <typename U = T, typename = core::enable_if_t<core::conjunction<
                                core::is_same<T, U>, core::is_real<U>>::value>>
  WN_FORCE_INLINE U inverse_length() const {
    return TraitsType::inverse_length(m_elements);
  }

  WN_FORCE_INLINE T length_squared() const {
    return TraitsType::length_squared(m_elements);
  }

  WN_FORCE_INLINE T dot(const basic_vector& _vector) const {
    return TraitsType::dot(m_elements, _vector.m_elements);
  }

  template <typename U = basic_vector,
      typename =
          core::enable_if_t<core::conjunction<core::is_same<U, basic_vector>,
              core::bool_constant<(Dimension == 3)>>::value>>
  WN_FORCE_INLINE U cross(const basic_vector& _vector) const {
    basic_vector destination;

    TraitsType::cross(destination.m_elements, m_elements, _vector.m_elements);

    return core::move(destination);
  }

  // operators

  WN_FORCE_INLINE void make_zero() {
    TraitsType::make_zero(m_elements);
  }

  WN_FORCE_INLINE basic_vector operator+() const {
    basic_vector copy(*this);

    return core::move(copy);
  }

  WN_FORCE_INLINE
  typename core::enable_if<core::is_signed<T>::value, basic_vector>::type
  operator-() const {
    basic_vector copy(*this);

    TraitsType::negate(copy.m_elements);

    return core::move(copy);
  }

  WN_FORCE_INLINE basic_vector& operator+=(const basic_vector& _vector) {
    TraitsType::add(m_elements, _vector.m_elements);

    return *this;
  }

  WN_FORCE_INLINE basic_vector& operator-=(const basic_vector& _vector) {
    TraitsType::subtract(m_elements, _vector.m_elements);

    return *this;
  }

  WN_FORCE_INLINE basic_vector& operator*=(const basic_vector& _vector) {
    TraitsType::multiply(m_elements, _vector.m_elements);

    return *this;
  }

  WN_FORCE_INLINE basic_vector& operator/=(const basic_vector& _vector) {
    TraitsType::divide(m_elements, _vector.m_elements);

    return *this;
  }

  WN_FORCE_INLINE basic_vector& operator+=(const T& _value) {
    TraitsType::add(m_elements, _value);

    return *this;
  }

  WN_FORCE_INLINE basic_vector& operator-=(const T& _value) {
    TraitsType::subtract(m_elements, _value);

    return *this;
  }

  WN_FORCE_INLINE basic_vector& operator*=(const T& _value) {
    TraitsType::multiply(m_elements, _value);

    return *this;
  }

  WN_FORCE_INLINE basic_vector& operator/=(const T& _value) {
    TraitsType::divide(m_elements, _value);

    return *this;
  }

  WN_FORCE_INLINE basic_vector operator+(const basic_vector& _vector) const {
    basic_vector copy(*this);

    copy += _vector;

    return core::move(copy);
  }

  WN_FORCE_INLINE basic_vector operator-(const basic_vector& _vector) const {
    basic_vector copy(*this);

    copy -= _vector;

    return core::move(copy);
  }

  WN_FORCE_INLINE basic_vector operator*(const basic_vector& _vector) const {
    basic_vector copy(*this);

    copy *= _vector;

    return core::move(copy);
  }

  WN_FORCE_INLINE basic_vector operator/(const basic_vector& _vector) const {
    basic_vector copy(*this);

    copy /= _vector;

    return core::move(copy);
  }

  WN_FORCE_INLINE basic_vector operator+(const T& _value) const {
    basic_vector copy(*this);

    copy += _value;

    return core::move(copy);
  }

  WN_FORCE_INLINE basic_vector operator-(const T& _value) const {
    basic_vector copy(*this);

    copy -= _value;

    return core::move(copy);
  }

  WN_FORCE_INLINE basic_vector operator*(const T& _value) const {
    basic_vector copy(*this);

    copy *= _value;

    return core::move(copy);
  }

  WN_FORCE_INLINE basic_vector operator/(const T& _value) const {
    basic_vector copy(*this);

    copy /= _value;

    return core::move(copy);
  }

  WN_FORCE_INLINE bool operator==(const basic_vector& _vector) const {
    return TraitsType::equal(m_elements, _vector.m_elements);
  }

  WN_FORCE_INLINE bool operator!=(const basic_vector& _vector) const {
    return TraitsType::not_equal(m_elements, _vector.m_elements);
  }

  WN_FORCE_INLINE void multiply_add(
      const basic_vector& _vector1, const basic_vector& _vector2) {
    TraitsType::multiply_add(
        m_elements, _vector1.m_elements, _vector2.m_elements);
  }

  WN_FORCE_INLINE void multiply_add(const T& _number1, const T& _number2) {
    TraitsType::multiply_add(m_elements, _number1, _number2);
  }

  WN_FORCE_INLINE void multiply_subtract(
      const basic_vector& _vector1, const basic_vector& _vector2) {
    TraitsType::multiply_subtract(
        m_elements, _vector1.m_elements, _vector2.m_elements);
  }

  WN_FORCE_INLINE void multiply_subtract(const T& _number1, const T& _number2) {
    TraitsType::multiply_subtract(m_elements, _number1, _number2);
  }

  WN_FORCE_INLINE void translate(const basic_vector& _vector) {
    (*this) += _vector;
  }

  WN_FORCE_INLINE void translate(const T& _value) {
    (*this) += _value;
  }

  template <typename... Us,
      typename = core::enable_if_t<core::conjunction<core::are_same<T, Us...>,
          core::bool_constant<(sizeof...(Us) == Dimension)>,
          core::bool_constant<(Dimension > 1)>>::value>>
  WN_FORCE_INLINE void translate(const Us&... _values) {
    (*this) += basic_vector(_values...);
  }

  WN_FORCE_INLINE void scale(const basic_vector& _vector) {
    (*this) *= _vector;
  }

  WN_FORCE_INLINE void scale(const T& _value) {
    (*this) *= _value;
  }

  template <typename... Us,
      typename = core::enable_if_t<core::conjunction<core::are_same<T, Us...>,
          core::bool_constant<(sizeof...(Us) == Dimension)>,
          core::bool_constant<(Dimension > 1)>>::value>>
  WN_FORCE_INLINE void scale(const Us&... _values) {
    (*this) *= basic_vector(_values...);
  }

  WN_FORCE_INLINE void reverse() {
    TraitsType::reverse(m_elements);
  }

  WN_FORCE_INLINE void clamp(
      const basic_vector& _minimum, const basic_vector& _maximum) {
    TraitsType::clamp(m_elements, _minimum.m_elements, _maximum.m_elements);
  }

  WN_FORCE_INLINE void clamp(const T& _minimum, const T& _maximum) {
    TraitsType::clamp(m_elements, _minimum, _maximum);
  }

  WN_FORCE_INLINE void saturate() {
    TraitsType::saturate(m_elements);
  }

  WN_FORCE_INLINE void minimum(const basic_vector& _vector) {
    TraitsType::minimum(m_elements, _vector.m_elements);
  }

  WN_FORCE_INLINE void maximum(const basic_vector& _vector) {
    TraitsType::maximum(m_elements, _vector.m_elements);
  }

  WN_FORCE_INLINE void average(const basic_vector& _vector) {
    TraitsType::average(m_elements, _vector.m_elements);
  }

  WN_FORCE_INLINE void truncate(const T& _length) {
    TraitsType::truncate(m_elements, _length);
  }

  template <typename U = T, typename = core::enable_if_t<core::conjunction<
                                core::is_same<T, U>, core::is_real<U>>::value>>
  WN_FORCE_INLINE void normalize() {
    TraitsType::normalize(m_elements);
  }

  template <typename U = T, typename = core::enable_if_t<core::conjunction<
                                core::is_same<T, U>, core::is_real<U>>::value>>
  WN_FORCE_INLINE void snap(
      const snap_direction _direction = snap_direction::nearest) {
    TraitsType::snap(m_elements, _direction);
  }

  template <typename U = T, typename = core::enable_if_t<core::conjunction<
                                core::is_same<T, U>, core::is_real<U>>::value>>
  WN_FORCE_INLINE void recipricol() {
    TraitsType::recipricol(m_elements);
  }

  WN_FORCE_INLINE void swap(basic_vector& _other) {
    TraitsType::swap(m_elements, _other.m_elements);
  }

private:
  template <typename U, const size_t OtherDimension, typename OtherTraitsType>
  friend class basic_vector;

  internal::element_array<T, Dimension> m_elements;
};

template <typename T, const size_t Dimension, typename TraitsType>
WN_FORCE_INLINE basic_vector<T, Dimension, TraitsType> operator+(
    const T& _lhs, basic_vector<T, Dimension, TraitsType>&& _rhs) {
  _rhs += _lhs;

  return core::move(_rhs);
}

template <typename T, const size_t Dimension, typename TraitsType>
WN_FORCE_INLINE basic_vector<T, Dimension, TraitsType> operator+(
    const T& _lhs, const basic_vector<T, Dimension, TraitsType>& _rhs) {
  return (_rhs + _lhs);
}

template <typename T, const size_t Dimension, typename TraitsType>
WN_FORCE_INLINE basic_vector<T, Dimension, TraitsType> operator-(
    const T& _lhs, const basic_vector<T, Dimension, TraitsType>& _rhs) {
  return (basic_vector<T, Dimension, TraitsType>(_lhs) -= _rhs);
}

template <typename T, const size_t Dimension, typename TraitsType>
WN_FORCE_INLINE basic_vector<T, Dimension, TraitsType> operator*(
    const T& _lhs, basic_vector<T, Dimension, TraitsType>&& _rhs) {
  _rhs *= _lhs;

  return core::move(_rhs);
}

template <typename T, const size_t Dimension, typename TraitsType>
WN_FORCE_INLINE basic_vector<T, Dimension, TraitsType> operator*(
    const T& _lhs, const basic_vector<T, Dimension, TraitsType>& _rhs) {
  return (_rhs * _lhs);
}

template <typename T, const size_t Dimension, typename TraitsType>
WN_FORCE_INLINE basic_vector<T, Dimension, TraitsType> operator/(
    const T& _lhs, const basic_vector<T, Dimension, TraitsType>& _rhs) {
  return (basic_vector<T, Dimension, TraitsType>(_lhs) /= _rhs);
}

template <typename T, const size_t Dimension>
class packed_vector final {
public:
  typedef T value_type;
  typedef core::add_lvalue_reference_t<T> reference;
  typedef core::add_lvalue_reference_t<core::add_const_t<T>> const_reference;
  typedef typename core::add_pointer<T>::type pointer;
  typedef typename core::add_pointer<core::add_const_t<T>>::type const_pointer;
  typedef T* iterator;
  typedef T const* const_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

  packed_vector() = default;

  WN_FORCE_INLINE packed_vector(packed_vector&& _vector) {
    internal::general_traits_generic<T, Dimension>::assign(
        m_elements, core::move(_vector.m_elements));
  }

  WN_FORCE_INLINE packed_vector(const packed_vector& _vector) {
    internal::general_traits_generic<T, Dimension>::assign(
        m_elements, _vector.m_elements);
  }

  template <typename Traits>
  WN_FORCE_INLINE packed_vector(
      const basic_vector<T, Dimension, Traits>& _vector) {
    internal::general_traits_generic<T, Dimension>::assign(
        m_elements, _vector.data(), Dimension);
  }

  WN_FORCE_INLINE packed_vector& operator=(packed_vector&& _vector) {
    assign(std::move(_vector));

    return *this;
  }

  WN_FORCE_INLINE packed_vector& operator=(const packed_vector& _vector) {
    assign(_vector);

    return *this;
  }

  template <typename Traits>
  WN_FORCE_INLINE packed_vector& operator=(
      const basic_vector<T, Dimension, Traits>& _vector) {
    assign(_vector);

    return *this;
  }

  WN_FORCE_INLINE void assign(packed_vector&& _vector) {
    packed_vector(std::move(_vector)).swap(*this);
  }

  WN_FORCE_INLINE void assign(const packed_vector& _vector) {
    packed_vector(_vector).swap(*this);
  }

  template <typename Traits>
  WN_FORCE_INLINE void assign(
      const basic_vector<T, Dimension, Traits>& _vector) {
    packed_vector(_vector).swap(*this);
  }

  WN_FORCE_INLINE reference operator[](const size_t _index) {
    return at(_index);
  }

  WN_FORCE_INLINE const_reference operator[](const size_t _index) const {
    return at(_index);
  }

  WN_FORCE_INLINE reference at(const size_t _index) {
    WN_DEBUG_ASSERT_DESC(
        _index < Dimension, "attempting to get element outside of bounds");

    return m_elements[_index];
  }

  WN_FORCE_INLINE const_reference at(const size_t _index) const {
    WN_DEBUG_ASSERT_DESC(
        _index < Dimension, "attempting to get element outside of bounds");

    return m_elements[_index];
  }

  WN_FORCE_INLINE pointer data() {
    return m_elements.data();
  }

  WN_FORCE_INLINE const_pointer data() const {
    return m_elements.data();
  }

  // iterators

  WN_FORCE_INLINE iterator begin() {
    return m_elements.data();
  }

  WN_FORCE_INLINE const_iterator begin() const {
    return cbegin();
  }

  WN_FORCE_INLINE const_iterator cbegin() const {
    return m_elements.data();
  }

  WN_FORCE_INLINE iterator end() {
    return (m_elements.data() + Dimension);
  }

  WN_FORCE_INLINE const_iterator end() const {
    return cend();
  }

  WN_FORCE_INLINE const_iterator cend() const {
    return (m_elements.data() + Dimension);
  }

  WN_FORCE_INLINE reverse_iterator rbegin() {
    return reverse_iterator(end());
  }

  WN_FORCE_INLINE const_reverse_iterator crbegin() const {
    return const_reverse_iterator(cend());
  }

  WN_FORCE_INLINE reverse_iterator rend() {
    return reverse_iterator(begin());
  }

  WN_FORCE_INLINE const_reverse_iterator crend() const {
    return const_reverse_iterator(cbegin());
  }

  WN_FORCE_INLINE void swap(packed_vector& _other) {
    internal::general_traits_generic<T, Dimension>::swap(
        m_elements, _other.m_elements);
  }

private:
  internal::element_array_generic<T, Dimension> m_elements;
};

template <typename T, const size_t Dimension, typename TraitsType>
WN_FORCE_INLINE basic_vector<T, Dimension, TraitsType>::basic_vector(
    const packed_vector<T, Dimension>& _vector)
  : basic_vector(_vector.data(), Dimension) {}

// short forms

template <typename T, const size_t Dimension>
using vector = basic_vector<T, Dimension>;

template <typename T>
using vector2 = vector<T, 2>;

template <typename T>
using vector3 = vector<T, 3>;

template <typename T>
using vector4 = vector<T, 4>;

template <typename T, const size_t Dimension>
using vector_precise =
    basic_vector<T, Dimension, internal::vector_traits<T, Dimension, true>>;

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
