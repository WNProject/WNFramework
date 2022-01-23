// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNMemory/inc/unique_ptr.h"
#include "WNMultiTasking/inc/job_pool.h"
#include "executable_test/inc/WNTestHarness.h"

#include <thread>

TEST(job_pool, basic) {
  wn::testing::allocator allocator;

  {
    wn::multi_tasking::job_pool pool(
        &allocator, wn::logging::get_null_logger(), 0, 4 * 1024 * 1024, true);
    auto sig = pool.get_signal();
    pool.add_job(JOB_NAME,
        wn::functional::function<void()>(&allocator,
            [signal = sig, &allocator, &pool]() {
              pool.call_blocking_function(JOB_NAME,
                  wn::functional::function<void()>(&allocator, []() {}));
              for (size_t i = 0; i < 10; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                signal.increment_by(1);
              }
            }),
        nullptr, wn::multi_tasking::job_priority::high);
    for (size_t j = 0; j < 10000; ++j) {
      pool.add_job(JOB_NAME, wn::functional::function<void()>(
                                 &allocator, [signal = sig, elem = j]() {
                                   for (size_t i = 0; i < 10; ++i) {
                                     signal.wait_until(i);
                                   }
                                 }));
    }

    pool.add_job_on_main(JOB_NAME,
        wn::functional::function<void()>(&allocator, [signal = sig, &pool]() {
          signal.wait_until(10);
          pool.exit_this_thread();
        }));

    pool.attach_main_thread();
  }
}
