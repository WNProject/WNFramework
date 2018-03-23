// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATRIX_HELPERS_H__
#define __WN_MATRIX_HELPERS_H__

#include <cmath>
#include "WNMath/inc/WNMatrix.h"

namespace wn {
namespace math {

inline mat44f get_perspective_matrix(
    float _fov_in_radians, float _aspect, float _near_plane, float _far_plane) {
  const float half_fov = (_fov_in_radians * 0.5f);
  const float y = 1.0f / tan(half_fov);
  const float x = y / _aspect;

  const float nmf = _near_plane - _far_plane;
  const float fpz = _far_plane / nmf;

  return mat44f({
      // comments here for formatting
      x, 0.f, 0.f, 0.f,                     // col1
      0.f, y, 0.f, 0.f,                     // col2
      0.f, 0.f, fpz, -1.0f,                 // col3
      0.f, 0.f, 2 * _near_plane * fpz, 0.f  // col4
  });
}

inline mat44f get_rotation_y(float _amount_in_radians) {
  const float cos_t = cos(_amount_in_radians);
  const float sin_t = sin(_amount_in_radians);

  return mat44f(
      {cos_t, 0, -sin_t, 0, 0, 1, 0, 0, sin_t, 0, cos_t, 0, 0, 0, 0, 1});
};

inline mat44f get_rotation_x(float _amount_in_radians) {
  const float cos_t = cos(_amount_in_radians);
  const float sin_t = sin(_amount_in_radians);

  return mat44f(
      {1, 0, 0, 0, 0, cos_t, -sin_t, 0, 0, sin_t, cos_t, 0, 0, 0, 0, 1});
};

inline mat44f get_rotation_z(float _amount_in_radians) {
  const float cos_t = cos(_amount_in_radians);
  const float sin_t = sin(_amount_in_radians);

  return mat44f(
      {cos_t, 0, sin_t, 0, 0, 1, 0, 0, -sin_t, 0, cos_t, 0, 0, 0, 0, 1});
};

inline mat44f get_translation(float _x, float _y, float _z) {
  return mat44f({1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, _x, _y, _z, 1});
};

}  // namespace math
}  // namespace wn

#endif  // __WN_MATRIX_HELPERS_H__
