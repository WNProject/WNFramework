// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CORE_TUPLE_H__
#define __WN_CORE_TUPLE_H__

#include <tuple>

namespace wn {
namespace core {

using std::tuple;
using std::tuple_element;
using std::tuple_element_t;
using std::tuple_size;

using std::ignore;

using std::forward_as_tuple;
using std::get;
using std::make_tuple;
using std::tie;
using std::tuple_cat;

}  // namespace core
}  // namespace wn

#endif  // __WN_CORE_TUPLE_H__
