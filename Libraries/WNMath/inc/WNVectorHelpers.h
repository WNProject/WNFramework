// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_VECTOR_HELPERS_H__
#define __WN_VECTOR_HELPERS_H__

#include "WNMath/inc/WNVector.h"

#include <cmath>

namespace wn {
namespace math {

inline vec3f cross(const vec3f& first, const vec3f& second) {
  return vec3f({first[1] * second[2] - first[2] * second[1],
      first[2] * second[0] - first[0] * second[2],
      first[0] * second[1] - first[1] * second[0]});
};

}  // namespace math
}  // namespace wn

#endif  // __WN_VECTOR_HELPERS_H__
