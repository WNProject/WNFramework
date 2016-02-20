// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_MATRIX_4_H__
#define __WN_MATH_MATRIX_4_H__

#include "WNMath/inc/Internal/WNMatrix4Base.h"

namespace wn {
template <typename type>
class WNMatrix4 : public internal::__WNMatrix4Base<type> {
public:
  WN_FORCE_INLINE WNMatrix4() {}
  WNMatrix4(const WNMatrix4& _matrix);
  explicit WNMatrix4(const type& _number);
  explicit WNMatrix4(const type* _numbers);

  void MakeTranslation(const type& _translation);
  void MakeTranslation(const vector3<type>& _translation);
  void MakeTranslation(const type& _x, const type& _y, const type& _z);
  void MakeScale(const type& _scale);
  void MakeScale(const vector3<type>& _scale);
  void MakeScale(const type& _x, const type& _y, const type& _z);

public:
  static WNMatrix4 CreateTranslation(const type& _translation);
  static WNMatrix4 CreateTranslation(const vector3<type>& _translation);
  static WNMatrix4 CreateTranslation(
      const type& _x, const type& _y, const type& _z);
  static WNMatrix4 CreateScale(const type& _scale);
  static WNMatrix4 CreateScale(const vector3<type>& _scale);
  static WNMatrix4 CreateScale(const type& _x, const type& _y, const type& _z);

private:
  typedef class internal::__WNMatrixBase<WNMatrix4<type>, type, 4> Base;
};

template <typename type>
WNMatrix4<type> operator+(const type& _number, const WNMatrix4<type>& _matrix);

template <typename type>
WNMatrix4<type> operator-(const type& _number, const WNMatrix4<type>& _matrix);

template <typename type>
WNMatrix4<type> operator*(const type& _number, const WNMatrix4<type>& _matrix);

template <typename type>
WNMatrix4<type> operator/(const type& _number, const WNMatrix4<type>& _matrix);
}

#include "WNMath/inc/Internal/WNMatrix4.inl"

#endif  // __WN_MATH_MATRIX_4_H__