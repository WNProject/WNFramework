// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_RUNTIME_APPLICATION_APPLICATION_DATA_H__
#define __WN_RUNTIME_APPLICATION_APPLICATION_DATA_H__

#include "WNLogging/inc/WNLog.h"
#include "executable_data/inc/executable_data.h"

namespace wn {
namespace executable {

struct executable_data;

}  // namespace executable

namespace memory {

class allocator;

}  // namespace memory

namespace multi_tasking {

class job_pool;
class thread_exclusive_work_queue;

}  // namespace multi_tasking

namespace runtime {
namespace application {

struct application_data final {
  const executable::executable_data* executable_data;
  memory::allocator* system_allocator;
  logging::log* default_log;
  multi_tasking::job_pool* default_job_pool;
  multi_tasking::thread_exclusive_work_queue* main_thread_work_queue;
};

}  // namespace application
}  // namespace runtime
}  // namespace wn

#endif  // __WN_RUNTIME_APPLICATION_APPLICATION_DATA_H__
