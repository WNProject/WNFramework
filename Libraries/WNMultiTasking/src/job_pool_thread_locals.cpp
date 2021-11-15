// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNMultiTasking/inc/job_pool.h"

namespace wn {
namespace multi_tasking {

namespace {
static thread_local job_pool* tl_this_job_pool = nullptr;
}

job_pool::thread_data*& job_pool::this_thread_data() {
  static thread_local job_pool::thread_data* td = nullptr;
  return td;
}

job_pool* job_pool::this_job_pool() {
  return tl_this_job_pool;
}

void job_pool::set_this_job_pool(job_pool* _pool) {
  tl_this_job_pool = _pool;
}

}  // namespace multi_tasking
}  // namespace wn
