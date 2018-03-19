// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATRIX_HELPERS_H__
#define __WN_MATRIX_HELPERS_H__

#include <cmath>
#include "WNMath/inc/WNMatrix.h"
#include "WNMath/inc/WNVector.h"
#include "WNMath/inc/WNVectorHelpers.h"

namespace wn {
namespace math {

inline mat44f get_perspective_matrix(
    float _fov_in_radians, float _aspect, float _near_plane, float _far_plane) {
  const float half_fov = (_fov_in_radians * 0.5f);
  const float y = 1.0f / tan(half_fov);
  const float x = y / _aspect;

  const float nmf = _near_plane - _far_plane;
  const float fdz = _far_plane / nmf;

  return mat44f({
      // comments here for formatting
      x, 0.f, 0.f, 0.f,                     // col1
      0.f, y, 0.f, 0.f,                     // col2
      0.f, 0.f, fdz, -1.0f,                 // col3
      0.f, 0.f, 2 * _near_plane * fdz, 0.f  // col4
  });
}

inline mat44f get_scale_matrix(const vec3f& scale) {
  return mat44f({
      scale[0], 0, 0, 0,     // col1
      0, scale[1], 0, 0,     // col2
      0, 0, scale[2], 0.0f,  // col3
      0, 0, 0, 1.0f          // col4
  });
}

inline mat44f get_orthographic_matrix(float _left, float _right, float _top,
    float _bottom, float _near, float _far) {
  float nrml = 1.0f / (_right - _left);
  float ntmb = 1.0f / (_top - _bottom);
  float nfmn = 1.0f / (_near - _far);
  return mat44f({
      2.0f * nrml, 0, 0, 0,     // col1
      0, 2.0f * ntmb, 0, 0,     // col2
      0, 0, 2.0f * nfmn, 0.0f,  // col3
      -(_right + _left) * nrml, -(_top + _bottom) * ntmb, (_far + _near) * nfmn,
      1.0f  // col4
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

// TODO: figure out if we should just pass these by value.
inline mat44f get_lookat(const vec3f& eye, const vec3f& at, const vec3f& up) {
  vec3f lookat =
      vec3f({eye[0] - at[0], eye[1] - at[1], eye[2] - at[2]}).normalized();

  vec3f right = cross(up, lookat).normalized();
  vec3f new_up = cross(lookat, right);
  vec3f offset = {-right.dot(eye), -new_up.dot(eye), -lookat.dot(eye)};

  return mat44f({right[0], new_up[0], lookat[0], 0, right[1], new_up[1],
      lookat[1], 0, right[2], new_up[2], lookat[2], 0, offset[0], offset[1],
      offset[2], 1});
}

}  // namespace math
}  // namespace wn

#endif  // __WN_MATRIX_HELPERS_H__
