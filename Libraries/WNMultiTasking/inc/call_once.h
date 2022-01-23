// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MULTI_TASKING_CALL_ONCE_H__
#define __WN_MULTI_TASKING_CALL_ONCE_H__

#include "core/inc/types.h"

#include <mutex>

namespace wn {
namespace multi_tasking {

using std::call_once;
using std::once_flag;

}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_CALL_ONCE_H__
