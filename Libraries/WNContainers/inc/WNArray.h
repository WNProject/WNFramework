// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CONTAINERS_ARRAY_H__
#define __WN_CONTAINERS_ARRAY_H__

#include <array>

namespace wn {
namespace containers {

template <typename T, const size_t Dimension>
using array = std::array<T, Dimension>;

}  // namespace containers
}  // namespace wn

#endif  // __WN_CONTAINERS_ARRAY_H__
