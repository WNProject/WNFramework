// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_WINDOW_WINDOW_FACTORY_H__
#define __WN_WINDOW_WINDOW_FACTORY_H__

#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/unique_ptr.h"
#include "WNMultiTasking/inc/job_pool.h"
#include "WNWindow/inc/WNWindowEnums.h"

namespace wn {
namespace memory {
class allocator;
}  // namespace memory

namespace multi_tasking {
class job_pool;
}  // namespace multi_tasking

namespace runtime {
namespace application {
struct application_data;
}  // namespace application

namespace window {

class window;

class window_factory {
public:
  window_factory(memory::allocator* _allocator, logging::log* _log)
    : m_allocator(_allocator), m_log(_log) {}
  // This creates a new window of the given type.
  // Any message-pump or similar will run as a blocking job on the given
  // _job_pool.
  // The given signal will be notified when the window has either been
  // successfully created
  // or the creation has failed. Either way the value can be gathered from the
  // window*
  // with a call to is_valid().
  virtual memory::unique_ptr<window> create_window(window_type _type,
      multi_tasking::job_pool* _pool,
      multi_tasking::job_signal* _creation_complete_signal,
      const application::application_data* _app_data, uint32_t _x, uint32_t _y,
      uint32_t _width, uint32_t _height);

private:
  memory::allocator* m_allocator;
  logging::log* m_log;
};

}  // namespace window
}  // namespace runtime
}  // namespace wn

#endif  // __WN_WINDOW_WINDOW_FACTORY_H__
