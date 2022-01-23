// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNWindow/inc/WNWindowFactory.h"

#include "WNMemory/inc/unique_ptr.h"
#include "WNWindow/inc/Internal/WNConfig.h"
#include "WNWindow/inc/Internal/WNWindowInclude.h"

namespace wn {
namespace runtime {
namespace window {

memory::unique_ptr<window> window_factory::create_window(window_type _type,
    multi_tasking::job_pool* _pool,
    multi_tasking::signal_ptr _creation_complete_signal,
    const application::application_data* _app_data, uint32_t _x, uint32_t _y,
    uint32_t _width, uint32_t _height) {
  memory::unique_ptr<window> wnd;
  switch (_type) {
    case window_type::system:
      wnd = memory::make_unique<internal::default_window_type>(m_allocator,
          m_allocator, m_log, _pool, core::move(_creation_complete_signal),
          _app_data, _x, _y, _width, _height);
      break;
    case window_type::null:
      // TODO(awoloszyn): Create a null window type that does "nothing"
      wnd = nullptr;
      break;
  }
  if (!wnd) {
    return nullptr;
  }
  if (wnd->initialize() != window_error::ok) {
    return nullptr;
  }
  return wnd;
}

}  // namespace window
}  // namespace runtime
}  // namespace wn
