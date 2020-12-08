// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MULTI_TASKING_X86_64_INTERNAL_PROCESSOR_RELAX_H__
#define __WN_MULTI_TASKING_X86_64_INTERNAL_PROCESSOR_RELAX_H__

#include "core/inc/base.h"

#include <xmmintrin.h>

namespace wn {
namespace multi_tasking {
namespace internal {

inline void processor_relax() {
  _mm_pause();
}

}  // namespace internal
}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_X86_64_INTERNAL_PROCESSOR_RELAX_H__
