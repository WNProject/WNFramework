// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNApplication/inc/WNApplicationData.h"
#include "WNExecutable/inc/WNEntry.h"
#include "WNLogging/inc/WNConsoleLogger.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNMultiTasking/inc/WNJobPool.h"

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

int32_t wn_main(
    const wn::entry::system_data* _data, int32_t _argc, char* _argv[]) {
  wn_dummy();
  wn::memory::basic_allocator root_allocator;
  wn::logging::console_logger<wn::logging::console_location::std_out>
      default_logger;
  wn::logging::static_log<> log(&default_logger);

  app_params params{0,
      {
          _data,            // system_data
          &root_allocator,  // system_allocator
          log.log(),        // system_logger
          nullptr,          // default job pool (set below)
          _argc,            // argument count
          _argv             // arguments
      }};

  {
    main_application app;
    wn::multi_tasking::thread_job_pool job_pool(&root_allocator, 1);

    params.data.default_job_pool = &job_pool;
    // TODO(awoloszyn): Finish fiber job pool, start using that instead
    // of the super slow thread job pool.
    // TODO(awoloszyn): Replace the hard-coded 2 with the number
    // of cores in the system.

    job_pool.add_job(
        nullptr, &main_application::main_application_job, &app, &params);
  }

  log.log()->flush();

  return params.ret_val;
}
