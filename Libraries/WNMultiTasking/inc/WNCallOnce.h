// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MULTI_TASKING_CALL_ONCE_H__
#define __WN_MULTI_TASKING_CALL_ONCE_H__

#include "WNCore/inc/WNTypes.h"

#include <mutex>

namespace wn {
namespace multi_tasking {

using std::once_flag;
using std::call_once;

}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_CALL_ONCE_H__
