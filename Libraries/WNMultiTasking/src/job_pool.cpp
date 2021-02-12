// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNMultiTasking/inc/job_pool.h"

namespace wn {
namespace multi_tasking {
namespace {

static thread_local job_pool* tl_this_job_pool = nullptr;
static thread_local thread_data* tl_this_thread = nullptr;

}  // namespace

job_pool* job_pool::this_job_pool() {
  return tl_this_job_pool;
}

void job_pool::set_this_job_pool(job_pool* _pool) {
  tl_this_job_pool = _pool;
}

thread_data* thread_job_pool::this_thread() {
  return tl_this_thread;
}

void thread_job_pool::set_this_thread(thread_data* _data) {
  tl_this_thread = _data;
}

}  // namespace multi_tasking
}  // namespace wn
