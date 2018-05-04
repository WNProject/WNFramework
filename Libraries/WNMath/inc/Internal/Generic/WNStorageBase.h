// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_INTERNAL_STORAGE_BASE_GENERIC_H__
#define __WN_MATH_INTERNAL_STORAGE_BASE_GENERIC_H__

#include "WNCore/inc/types.h"
#include "WNCore/inc/utilities.h"

namespace wn {
namespace math {
namespace internal {

template <typename T, size_t Dimension>
class storage_base_generic {
public:
  storage_base_generic() = default;

protected:
  T m_values[Dimension];
};

}  // namespace internal
}  // namespace math
}  // namespace wn

#endif  // __WN_MATH_INTERNAL_STORAGE_BASE_GENERIC_H__
