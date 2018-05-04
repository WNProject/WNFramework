// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MULTI_TASKING_CONDITION_VARIABLE_H__
#define __WN_MULTI_TASKING_CONDITION_VARIABLE_H__

#include "WNCore/inc/types.h"

#include <condition_variable>

namespace wn {
namespace multi_tasking {

using condition_variable = std::condition_variable_any;

}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_CONDITION_VARIABLE_H__
