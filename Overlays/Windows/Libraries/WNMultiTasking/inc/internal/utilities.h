// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MULTI_TASKING_WINDOWS_INTERNAL_UTILITIES_H__
#define __WN_MULTI_TASKING_WINDOWS_INTERNAL_UTILITIES_H__

#include "WNMultiTasking/inc/call_once.h"
#include "core/inc/base.h"

namespace wn {
namespace multi_tasking {
namespace internal {

inline void processor_relax() {
  ::YieldProcessor();
}

inline size_t processor_logical_core_count() {
  SYSTEM_INFO system_info;

  ::GetSystemInfo(&system_info);

  return system_info.dwNumberOfProcessors;
}

}  // namespace internal
}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_WINDOWS_INTERNAL_UTILITIES_H__
