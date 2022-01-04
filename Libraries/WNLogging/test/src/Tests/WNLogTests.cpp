// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNLogging/inc/WNConsoleLogger.h"
#include "WNLogging/inc/WNFileLogger.h"
#include "WNLogging/inc/WNLog.h"
#include "executable_test/inc/WNTestHarness.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
#include "WNMultiTasking/inc/thread.h"
#endif

// using namespace wn::logging;
// TODO(awoloszyn): This will probably have to be moved
//    under the runtime at some point.
TEST(WNLogInterface, SimpleTest) {
  wn::logging::console_logger<> logger;
  wn::logging::static_log<wn::logging::log_level::max> default_log(&logger);
  default_log.log()->log_critical("asdf", "asfd");
  default_log.log()->flush();
  default_log.log()->log_critical("asdf", "asfd");
  default_log.log()->flush();
}
