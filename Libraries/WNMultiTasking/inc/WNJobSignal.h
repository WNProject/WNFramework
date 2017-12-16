// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MULTI_TASKING_JOB_SIGNAL_H__
#define __WN_MULTI_TASKING_JOB_SIGNAL_H__

#include "WNCore/inc/WNUtility.h"

#include <atomic>

namespace wn {
namespace multi_tasking {
class job_pool;

class job_signal : core::non_copyable {
public:
  // If this is created from within a task, then
  // use this constructor, it will handle the
  // job pool for you.
  explicit job_signal(size_t _value);
  // If this is created with the default constructor,
  // it is unassigned to any job pool.
  job_signal();
  // If you are creating this from outside of a
  // job, use this job_signal.
  job_signal(job_pool* _pool, size_t _value) : pool(_pool), value(_value) {}

  void initialize_signal(job_pool* pool, size_t _value);
  void increment(size_t _value);
  void decrement(size_t _value);
  void set(size_t _value);
  void wait_until(size_t _value);
  size_t current_value();

  job_pool* get_pool() {
    return pool;
  }

private:
  job_pool* pool;
  std::atomic<size_t> value;

  friend class thread_job_pool;
  friend class job_pool;
};

}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_JOB_SIGNAL_H__
