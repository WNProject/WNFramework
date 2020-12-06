// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNApplicationData/inc/WNApplicationData.h"
#include "WNLogging/inc/WNConsoleLogger.h"
#include "WNMemory/inc/basic_allocator.h"
#include "WNMultiTasking/inc/job_pool.h"
#include "WNMultiTasking/inc/job_signal.h"
#include "WNMultiTasking/inc/work_queue.h"
#include "executable_entry/inc/executable_entry.h"

namespace wn {
namespace runtime {
namespace application {
namespace internal {

void test_dummy() {}

}  // namespace internal
}  // namespace application
}  // namespace runtime
}  // namespace wn

extern int32_t wn_application_main(
    const wn::runtime::application::application_data* _application_data);

struct app_params {
  uint32_t ret_val;
  wn::runtime::application::application_data data;
};

struct main_application {
  void main_application_job(app_params* params) {
    params->ret_val = wn_application_main(&params->data);
  }
};

int32_t wn_main(const wn::executable::executable_data* _data) {
  wn::executable::wn_dummy();
  wn::memory::basic_allocator root_allocator;
  wn::logging::console_logger<wn::logging::console_location::std_out>
      default_logger;
  wn::logging::static_log<> log(&default_logger);

  app_params params{0,
      {
          _data,            // executable_data
          &root_allocator,  // system_allocator
          log.log(),        // system_logger
          nullptr           // default job pool (set below)
      }};

  {
    main_application app;
    wn::multi_tasking::thread_job_pool job_pool(&root_allocator, 1);
    wn::multi_tasking::thread_exclusive_work_queue work_queue(&root_allocator);
    wn::multi_tasking::job_signal main_done_signal(&job_pool, 0);
    params.data.default_job_pool = &job_pool;
    params.data.main_thread_work_queue = &work_queue;
    // TODO(awoloszyn): Finish fiber job pool, start using that instead
    // of the super slow thread job pool.
    // TODO(awoloszyn): Replace the hard-coded 2 with the number
    // of cores in the system.

    job_pool.add_job(&main_done_signal, &main_application::main_application_job,
        &app, &params);

    // Wait until EITHER the main application thread is done *and*
    // we have no more jobs to run on the actaul main thread.
    bool ran_work = false;
    do {
      ran_work = work_queue.try_do_work(std::chrono::milliseconds(10));
    } while (ran_work || !main_done_signal.current_value());
  }

  log.log()->flush();

  return params.ret_val;
}
