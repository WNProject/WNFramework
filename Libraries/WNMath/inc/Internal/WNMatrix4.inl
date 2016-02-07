// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_INTERNAL_MATRIX_4_INL__
#define __WN_MATH_INTERNAL_MATRIX_4_INL__

#ifndef __WN_MATH_MATRIX_4_H__
#error                                                                         \
    "Internal/WNMatrix4.inl should never be included directly. Please include WNMatrix4.h instead"
#endif

namespace wn {
template <typename type>
WN_FORCE_INLINE WNMatrix4<type>::WNMatrix4(const WNMatrix4& _matrix) {
  Base::Set(_matrix);
}

template <typename type>
WN_FORCE_INLINE WNMatrix4<type>::WNMatrix4(const type& _number) {
  Base::Set(_number);
}

template <typename type>
WN_FORCE_INLINE WNMatrix4<type>::WNMatrix4(const type* _numbers) {
  Base::Set(_numbers);
}
template <typename type>
WN_FORCE_INLINE void WNMatrix4<type>::MakeTranslation(
    const type& _translation) {
  this->MakeTranslation(_translation, _translation, _translation);
}

template <typename type>
WN_FORCE_INLINE void WNMatrix4<type>::MakeTranslation(
    const WNVector3<type>& _translation) {
  this->MakeTranslation(
      _translation.GetX(), _translation.GetY(), _translation.GetZ());
}

template <typename type>
WN_FORCE_INLINE void WNMatrix4<type>::MakeTranslation(
    const type& _x, const type& _y, const type& _z) {
  // First Column
  this->mElements.mValues[0] = static_cast<type>(1);
  this->mElements.mValues[1] = static_cast<type>(0);
  this->mElements.mValues[2] = static_cast<type>(0);
  this->mElements.mValues[3] = static_cast<type>(0);

  // Second Column
  this->mElements.mValues[4] = static_cast<type>(0);
  this->mElements.mValues[5] = static_cast<type>(1);
  this->mElements.mValues[6] = static_cast<type>(0);
  this->mElements.mValues[7] = static_cast<type>(0);

  // Third Column
  this->mElements.mValues[8] = static_cast<type>(0);
  this->mElements.mValues[9] = static_cast<type>(0);
  this->mElements.mValues[10] = static_cast<type>(1);
  this->mElements.mValues[11] = static_cast<type>(0);

  // Forth Column
  this->mElements.mValues[13] = _x;
  this->mElements.mValues[14] = _y;
  this->mElements.mValues[15] = _z;
  this->mElements.mValues[15] = static_cast<type>(1);
}

template <typename type>
WN_FORCE_INLINE void WNMatrix4<type>::MakeScale(const type& _scale) {
  this->MakeScale(_scale, _scale, _scale);
}

template <typename type>
WN_FORCE_INLINE void WNMatrix4<type>::MakeScale(const WNVector3<type>& _scale) {
  this->MakeScale(_scale.GetX(), _scale.GetY(), _scale.GetZ());
}

template <typename type>
WN_FORCE_INLINE void WNMatrix4<type>::MakeScale(
    const type& _x, const type& _y, const type& _z) {
  // First Column
  this->mElements.mValues[0] = _x;
  this->mElements.mValues[1] = static_cast<type>(0);
  this->mElements.mValues[2] = static_cast<type>(0);
  this->mElements.mValues[3] = static_cast<type>(0);

  // Second Column
  this->mElements.mValues[4] = static_cast<type>(0);
  this->mElements.mValues[5] = _y;
  this->mElements.mValues[6] = static_cast<type>(0);
  this->mElements.mValues[7] = static_cast<type>(0);

  // Third Column
  this->mElements.mValues[8] = static_cast<type>(0);
  this->mElements.mValues[9] = static_cast<type>(0);
  this->mElements.mValues[10] = _z;
  this->mElements.mValues[11] = static_cast<type>(0);

  // Forth Column
  this->mElements.mValues[13] = static_cast<type>(0);
  this->mElements.mValues[14] = static_cast<type>(0);
  this->mElements.mValues[15] = static_cast<type>(0);
  this->mElements.mValues[15] = static_cast<type>(1);
}

template <typename type>
WN_FORCE_INLINE WNMatrix4<type> WNMatrix4<type>::CreateTranslation(
    const type& _translation) {
  WNMatrix4<type> matrix;

  return (matrix.MakeTranslation(_translation), matrix);
}

template <typename type>
WN_FORCE_INLINE WNMatrix4<type> WNMatrix4<type>::CreateTranslation(
    const WNVector3<type>& _translation) {
  WNMatrix4<type> matrix;

  return (matrix.MakeTranslation(_translation), matrix);
}

template <typename type>
WN_FORCE_INLINE WNMatrix4<type> WNMatrix4<type>::CreateTranslation(
    const type& _x, const type& _y, const type& _z) {
  WNMatrix4<type> matrix;

  return (matrix.MakeTranslation(_x, _y, _z), matrix);
}

template <typename type>
WN_FORCE_INLINE WNMatrix4<type> WNMatrix4<type>::CreateScale(
    const type& _scale) {
  WNMatrix4<type> matrix;

  return (matrix.MakeScale(_scale), matrix);
}

template <typename type>
WN_FORCE_INLINE WNMatrix4<type> WNMatrix4<type>::CreateScale(
    const WNVector3<type>& _scale) {
  WNMatrix4<type> matrix;

  return (matrix.MakeScale(_scale), matrix);
}

template <typename type>
WN_FORCE_INLINE WNMatrix4<type> WNMatrix4<type>::CreateScale(
    const type& _x, const type& _y, const type& _z) {
  WNMatrix4<type> matrix;

  return (matrix.MakeScale(_x, _y, _z), matrix);
}

template <typename type>
WN_FORCE_INLINE WNMatrix4<type> operator+(
    const type& _number, const WNMatrix4<type>& _matrix) {
  WNMatrix4<type> matrix = _matrix;

  matrix += _number;

  return (matrix);
}

template <typename type>
WN_FORCE_INLINE WNMatrix4<type> operator-(
    const type& _number, const WNMatrix4<type>& _matrix) {
  WNMatrix4<type> matrix = WNMatrix4<type>(_number);

  matrix -= _matrix;

  return (matrix);
}

template <typename type>
WN_FORCE_INLINE WNMatrix4<type> operator*(
    const type& _number, const WNMatrix4<type>& _matrix) {
  WNMatrix4<type> matrix = _matrix;

  matrix *= _number;

  return (matrix);
}

template <typename type>
WN_FORCE_INLINE WNMatrix4<type> operator/(
    const type& _number, const WNMatrix4<type>& _matrix) {
  WNMatrix4<type> matrix = WNMatrix4<type>(_number);

  matrix /= _matrix;

  return (matrix);
}
}

#endif  // __WN_MATH_INTERNAL_MATRIX_4_INL__