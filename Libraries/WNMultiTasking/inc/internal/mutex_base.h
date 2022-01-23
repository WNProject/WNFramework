// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MULTI_TASKING_INTERNAL_MUTEX_BASE_H__
#define __WN_MULTI_TASKING_INTERNAL_MUTEX_BASE_H__

#include "core/inc/WNUtility.h"

#include <mutex>

namespace wn {
namespace multi_tasking {
namespace internal {

class mutex_base : core::non_copyable {
protected:
  mutex_base() = default;

  ~mutex_base() = default;

  void lock() {
    m_mutex.lock();
  }

  bool try_lock() {
    return m_mutex.try_lock();
  }

  void unlock() {
    m_mutex.unlock();
  }

private:
  std::mutex m_mutex;
};

}  // namespace internal
}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_INTERNAL_MUTEX_BASE_H__
