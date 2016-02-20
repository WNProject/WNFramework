// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CONTAINERS_ARRAY_H__
#define __WN_CONTAINERS_ARRAY_H__

#include <array>

namespace wn {
namespace containers {

template <typename T, std::size_t N>
using array = std::array<T, N>;

}  // namespace containers
}  // namespace wn
#endif  // __WN_CONTAINERS_ARRAY_H__
