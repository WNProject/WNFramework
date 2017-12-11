// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MULTI_TASKING_SPIN_LOCK_H__
#define __WN_MULTI_TASKING_SPIN_LOCK_H__

#include "WNMultiTasking/inc/internal/spin_lock_base.h"
#include "WNMultiTasking/inc/lock_guard.h"

namespace wn {
namespace multi_tasking {

class spin_lock final : internal::spin_lock_base {
public:
  spin_lock() : base() {}

  ~spin_lock() = default;

  void lock() {
    base::lock();
  }

  bool try_lock() {
    return base::try_lock();
  }

  void unlock() {
    base::unlock();
  }

private:
  using base = internal::spin_lock_base;
};

using spin_lock_guard = lock_guard<spin_lock>;

}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_SPIN_LOCK_H__
