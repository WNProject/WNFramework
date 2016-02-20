// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MULTI_TASKING_THREAD_TASK_H__
#define __WN_MULTI_TASKING_THREAD_TASK_H__

#include "WNMemory/inc/WNIntrusivePtr.h"
#include "WNMultiTasking/inc/WNSemaphore.h"

#include <atomic>

namespace wn {
namespace multi_tasking {

class thread_pool;

class thread_task : public memory::intrusive_ptr_base {
public:
  virtual ~thread_task() = default;

  WN_FORCE_INLINE bool join() {
    if (!run_task()) {
      m_completion_semaphore.wait();
      m_completion_semaphore.notify();
    }

    return true;
  }

protected:
  WN_FORCE_INLINE thread_task() : memory::intrusive_ptr_base() {
    m_executed.clear(std::memory_order_release);
  }

  virtual void run() = 0;

private:
  friend class thread_pool;

  WN_FORCE_INLINE bool run_task() {
    if (!m_executed.test_and_set(std::memory_order_acquire)) {
      run();

      m_completion_semaphore.notify();

      return true;
    }

    return false;
  }

  std::atomic_flag m_executed;
  semaphore m_completion_semaphore;
};

typedef memory::intrusive_ptr<thread_task> thread_task_ptr;

}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_THREAD_TASK_H__
