// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MULTI_TASKING_INTERNAL_SPIN_LOCK_BASE_H__
#define __WN_MULTI_TASKING_INTERNAL_SPIN_LOCK_BASE_H__

#include "WNMultiTasking/inc/utilities.h"
#include "core/inc/utilities.h"

#include <atomic>

namespace wn {
namespace multi_tasking {
namespace internal {

class spin_lock_base : core::non_copyable {
protected:
  spin_lock_base() {
    m_flag.clear(std::memory_order_release);
  }

  void lock() {
    while (!try_lock()) {
      processor_relax();
    }
  }

  bool try_lock() {
    return !m_flag.test_and_set(std::memory_order_acquire);
  }

  void unlock() {
    m_flag.clear(std::memory_order_release);
  }

private:
  std::atomic_flag m_flag;
};

}  // namespace internal
}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_INTERNAL_SPIN_LOCK_BASE_H__
