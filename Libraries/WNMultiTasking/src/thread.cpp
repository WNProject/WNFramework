// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNMultiTasking/inc/thread.h"

namespace wn {
namespace multi_tasking {

void thread::create(
    memory::allocator* _allocator, functional::function<void()>&& _f) {
  memory::intrusive_ptr<private_data> data(
      memory::make_intrusive<private_data>(_allocator, _allocator));

  if (data) {
    private_execution_data* execution_data =
        _allocator->construct<private_execution_data>(
            _allocator, core::move(_f), data);

    if (execution_data) {
      const bool creation_success = base::create(data.get(), execution_data);

      if (creation_success) {
        m_data = core::move(data);
      } else {
        _allocator->destroy(execution_data);

        WN_RELEASE_ASSERT(creation_success, "failed to create thread");
      }
    } else {
      WN_RELEASE_ASSERT(execution_data,
          "failed to allocate needed execution data for thread");
    }
  } else {
    WN_RELEASE_ASSERT(data, "failed to allocate needed data for thread");
  }
}

}  // namespace multi_tasking
}  // namespace wn
