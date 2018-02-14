// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MULTI_TASKING_SHARED_MUTEX_H__
#define __WN_MULTI_TASKING_SHARED_MUTEX_H__

#include "WNCore/inc/WNUtility.h"
#include "WNMultiTasking/inc/internal/shared_mutex_base.h"
#include "WNMultiTasking/inc/lock_guard.h"

namespace wn {
namespace multi_tasking {

class shared_mutex final : internal::shared_mutex_base {
public:
  shared_mutex() : base() {}

  ~shared_mutex() = default;

  void lock() {
    base::lock();
  }

  bool try_lock() {
    return base::try_lock();
  }

  void unlock() {
    base::unlock();
  }

  void lock_shared() {
    base::lock_shared();
  }

  bool try_lock_shared() {
    return base::try_lock_shared();
  }

  void unlock_shared() {
    base::unlock_shared();
  }

private:
  using base = internal::shared_mutex_base;
};

}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_SHARED_MUTEX_H__
