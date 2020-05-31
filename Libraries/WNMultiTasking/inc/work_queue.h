// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MULTITASKING_WORK_QUEUE_H__
#define __WN_MULTITASKING_WORK_QUEUE_H__

#include "WNFunctional/inc/WNFunction.h"
#include "WNMultiTasking/inc/condition_variable.h"
#include "WNMultiTasking/inc/mutex.h"

#include <chrono>

namespace wn {
namespace multi_tasking {

// Simple queue to do work on another thread.
class thread_exclusive_work_queue {
  struct job {
    job(job&& _other) {
      m_function = core::move(_other.m_function);
      m_done_semaphore = _other.m_done_semaphore;
      _other.m_done_semaphore = nullptr;
    }

    job(functional::function<void()> _fn, semaphore* _sem)
      : m_function(core::move(_fn)), m_done_semaphore(_sem) {}

    functional::function<void()> m_function;
    semaphore* m_done_semaphore;
  };

public:
  thread_exclusive_work_queue(memory::allocator* _allocator)
    : m_functions(_allocator), m_deferred_functions(_allocator) {}

  // Returns wether or not there may be additional work
  // in the queue.
  bool try_do_work(std::chrono::milliseconds _expiry) {
    std::unique_lock<mutex> lock(m_work_lock);
    if (m_functions.empty()) {
      m_cv.wait_for(lock, _expiry);
    }
    if (!m_functions.empty()) {
      auto job = core::move(m_functions[0]);
      m_functions.pop_front();
      lock.unlock();
      job.m_function();
      if (job.m_done_semaphore) {
        job.m_done_semaphore->notify();
      }
      return true;
    }
    m_functions.swap(m_deferred_functions);
    return !m_functions.empty();
  }

  void run_work_async(functional::function<void()> fn) {
    std::unique_lock<mutex> lock(m_work_lock);
    m_functions.push_back(job(core::move(fn), nullptr));
    lock.unlock();
    m_cv.notify_one();
  }

  void run_work_sync(functional::function<void()> fn) {
    std::unique_lock<mutex> lock(m_work_lock);
    semaphore work_semaphore;
    m_functions.push_back(job(core::move(fn), &work_semaphore));
    lock.unlock();
    m_cv.notify_one();
    work_semaphore.wait();
  }

  void run_defer_async(functional::function<void()> fn) {
    // These are jobs that only want to be run at most once
    // per period on the main thread.
    std::unique_lock<mutex> lock(m_work_lock);
    m_deferred_functions.push_back(job{core::move(fn), nullptr});
  }

private:
  condition_variable m_cv;
  mutex m_work_lock;
  containers::deque<job> m_functions;
  containers::deque<job> m_deferred_functions;
};

}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTITASKING_WORK_QUEUE_H__
