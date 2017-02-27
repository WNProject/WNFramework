// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_RUNTIME_APPLICATION_APPLICATION_DATA_H__
#define __WN_RUNTIME_APPLICATION_APPLICATION_DATA_H__

#include "WNExecutable/inc/WNEntry.h"
#include "WNLogging/inc/WNLog.h"

namespace wn {
namespace entry {
struct system_data;
}  // namespace entry
namespace memory {

class allocator;

}  // namespace memory

namespace multi_tasking {

class job_pool;

}  // namespace multi_tasking

namespace runtime {
namespace application {

struct application_data WN_FINAL {
  const entry::system_data* system_data;
  memory::allocator* system_allocator;
  logging::log* default_log;
  multi_tasking::job_pool* default_job_pool;
  int32_t argc;
  char** argv;
  // TODO: Add parsed arguments here
};

}  // namespace application
}  // namespace runtime
}  // namespace wn

#endif  // __WN_RUNTIME_APPLICATION_APPLICATION_DATA_H__