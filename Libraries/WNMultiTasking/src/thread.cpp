// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNMultiTasking/inc/thread.h"

#include "WNMemory/inc/allocator.h"
#include "WNMemory/inc/intrusive_ptr.h"
#include "WNMultiTasking/inc/semaphore.h"
#include "core/inc/assert.h"
#include "core/inc/utilities.h"

namespace wn {
namespace multi_tasking {

const thread::attributes thread::default_attributes = {
    0,     // stack size
    false  // low priority
};

void thread::create(memory::allocator* _allocator,
    const attributes& _attributes, functional::function<void()>&& _f) {
  memory::intrusive_ptr<private_data> data(
      memory::make_intrusive<private_data>(_allocator, _allocator));

  if (data) {
    semaphore start_lock;
    private_execution_data* execution_data =
        _allocator->construct<private_execution_data>(
            _allocator, &start_lock, core::move(_f), data);

    if (execution_data) {
      const bool creation_success =
          base::create(_attributes, data.get(), execution_data);

      if (creation_success) {
        m_data = core::move(data);

        start_lock.wait();
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
