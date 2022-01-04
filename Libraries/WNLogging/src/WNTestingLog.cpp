// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNLogging/inc/WNConsoleLogger.h"

namespace wn {
namespace logging {

log* GetTestingLog() {
  static wn::logging::console_logger<wn::logging::console_location::std_out>
      default_logger;
  static wn::logging::static_log<> log(&default_logger, log_level::max, true);
  return log.log();
}

}  // namespace logging
}  // namespace wn
