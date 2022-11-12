// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNApplicationData/inc/WNApplicationData.h"
#include "WNFunctional/inc/WNFunction.h"
#include "WNLogging/inc/WNConsoleLogger.h"
#include "WNLogging/inc/WNMultiLogger.h"
#include "WNLogging/inc/WNProfilerLogger.h"
#include "WNMemory/inc/basic_allocator.h"
#include "WNMultiTasking/inc/job_pool.h"
#include "WNMultiTasking/inc/work_queue.h"
#include "executable_entry/inc/executable_entry.h"
#include "profiling/inc/profiling.h"

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

  wn::logging::multi_logger<
      wn::logging::console_logger<wn::logging::console_location::std_out>,
      wn::logging::profiler_logger>
      multi_logger;
  // If profiling is enabled we need 2 things
  // 1) We need to log to BOTH console and the profiler.
  // 2) We need to actually flush the log after every message.
  wn::logging::static_log<> log(
      wn::profiling::is_profiling_enabled() ? &multi_logger : multi_logger.l1(),
      wn::logging::log_level::error, wn::profiling::is_profiling_enabled());

  app_params params{0,
      {
          _data,            // executable_data
          &root_allocator,  // system_allocator
          log.log(),        // system_logger
          nullptr           // default job pool (set below)
      }};

  {
    main_application app;
    wn::multi_tasking::job_pool job_pool(
        &root_allocator, log.log(), 2, 1024 * 1024 * 2, true);
    wn::multi_tasking::signal_ptr main_done_signal = job_pool.get_signal();
    params.data.default_job_pool = &job_pool;
    // TODO(awoloszyn): Finish fiber job pool, start using that instead
    // of the super slow thread job pool.
    // TODO(awoloszyn): Replace the hard-coded 2 with the number
    // of cores in the system.

    job_pool.add_job(JOB_NAME,
        wn::functional::function<void()>(&root_allocator,
            [&app, &params]() { app.main_application_job(&params); }),
        main_done_signal);

    job_pool.add_job_on_main(
        JOB_NAME, wn::functional::function<void()>(
                      &root_allocator, [&main_done_signal, &job_pool]() {
                        main_done_signal.wait_until(1);
                        job_pool.exit_this_thread();
                      }));

    job_pool.attach_main_thread();
  }

  log.log()->flush();

  return params.ret_val;
}
