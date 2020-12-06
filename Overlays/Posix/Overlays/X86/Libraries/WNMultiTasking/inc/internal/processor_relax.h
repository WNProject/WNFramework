// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MULTI_TASKING_X86_INTERNAL_PROCESSOR_RELAX_H__
#define __WN_MULTI_TASKING_X86_INTERNAL_PROCESSOR_RELAX_H__

#include "core/inc/base.h"

namespace wn {
namespace multi_tasking {
namespace internal {

inline void processor_relax() {
  __asm__ __volatile__("rep; nop" ::: "memory");
}

}  // namespace internal
}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_X86_INTERNAL_PROCESSOR_RELAX_H__
