// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNMultiTasking/inc/WNJobPool.h"

namespace wn {
namespace multi_tasking {

static WN_THREAD_LOCAL job_pool* tl_this_job_pool = 0;
job_pool* job_pool::this_job_pool() {
  return tl_this_job_pool;
}
void job_pool::set_this_job_pool(job_pool* _pool) {
  tl_this_job_pool = _pool;
}

static WN_THREAD_LOCAL thread_data* tl_this_thread;

thread_data* thread_job_pool::this_thread() {
  return tl_this_thread;
}

void thread_job_pool::set_this_thread(thread_data* _data) {
  tl_this_thread = _data;
}

}  // namespace multi_tasking
}  // namespace wn
