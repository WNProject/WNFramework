// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_INTERNAL_ELEMENT_ARRAY_GENERIC_H__
#define __WN_MATH_INTERNAL_ELEMENT_ARRAY_GENERIC_H__

#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNUtility.h"

namespace wn {
namespace internal {
namespace math {
template <typename type, const size_t dimension>
struct element_array_generic : core::non_copyable {
  static_assert(dimension > 1, "Must have a dimension of at least 2");

  type m_values[dimension];
};
}
}
}

#endif  // __WN_MATH_INTERNAL_ELEMENT_ARRAY_GENERIC_H__