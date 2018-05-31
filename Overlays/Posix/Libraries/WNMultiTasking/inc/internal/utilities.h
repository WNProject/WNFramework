// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MULTI_TASKING_POSIX_INTERNAL_UTILITIES_H__
#define __WN_MULTI_TASKING_POSIX_INTERNAL_UTILITIES_H__

#include "WNMultiTasking/inc/internal/processor_relax.h"

#include <unistd.h>

namespace wn {
namespace multi_tasking {
namespace internal {

inline size_t processor_logical_core_count() {
  return ::sysconf(_SC_NPROCESSORS_ONLN);
}

}  // namespace internal
}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_POSIX_INTERNAL_UTILITIES_H__
