// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_MATRIX_2_H__
#define __WN_MATH_MATRIX_2_H__

#include "WNMath/inc/Internal/WNMatrixBase.h"

namespace wn {
template <typename type>
class WNMatrix2 : public internal::__WNMatrixBase<WNMatrix2<type>, type, 2> {
public:
  WN_FORCE_INLINE WNMatrix2() {}
  WNMatrix2(const WNMatrix2& _matrix);
  explicit WNMatrix2(const type& _number);
  explicit WNMatrix2(const type* _numbers);

private:
  typedef class internal::__WNMatrixBase<WNMatrix2<type>, type, 2> Base;
};

template <typename type>
WNMatrix2<type> operator+(const type& _number, const WNMatrix2<type>& _matrix);

template <typename type>
WNMatrix2<type> operator-(const type& _number, const WNMatrix2<type>& _matrix);

template <typename type>
WNMatrix2<type> operator*(const type& _number, const WNMatrix2<type>& _matrix);

template <typename type>
WNMatrix2<type> operator/(const type& _number, const WNMatrix2<type>& _matrix);
}

#include "WNMath/inc/Internal/WNMatrix2.inl"

#endif  // __WN_MATH_MATRIX_2_H__