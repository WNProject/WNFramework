// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MULTI_TASKING_UTILITIES_H__
#define __WN_MULTI_TASKING_UTILITIES_H__

#include "WNMultiTasking/inc/internal/utilities.h"

namespace wn {
namespace multi_tasking {

inline void processor_relax() {
  internal::processor_relax();
}

inline size_t processor_logical_core_count() {
  return internal::processor_logical_core_count();
}

}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_UTILITIES_H__
