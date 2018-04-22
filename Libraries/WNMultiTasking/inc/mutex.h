// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MULTI_TASKING_MUTEX_H__
#define __WN_MULTI_TASKING_MUTEX_H__

#include "WNMultiTasking/inc/internal/mutex_base.h"
#include "WNMultiTasking/inc/lock_guard.h"

#include <atomic>

namespace wn {
namespace multi_tasking {

class mutex final : internal::mutex_base {
public:
  enum : uint32_t { default_spin_count = 4000u };

  mutex() : mutex(default_spin_count) {}

  mutex(uint32_t _spin_count) : base(), m_spin_count(_spin_count) {}

  void lock() {
    for (uint32_t i = 0u; i < m_spin_count; ++i) {
      if (try_lock()) {
        return;
      }
    }

    base::lock();
  }

  bool try_lock() {
    return base::try_lock();
  }

  void unlock() {
    base::unlock();
  }

  uint32_t spin_count() const {
    return m_spin_count;
  }

  void set_spin_count(uint32_t _spin_count) {
    m_spin_count.store(_spin_count, std::memory_order_acquire);
  }

private:
  using base = internal::mutex_base;

  std::atomic<uint32_t> m_spin_count;
};

using mutex_guard = lock_guard<mutex>;

}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_MUTEX_H__
