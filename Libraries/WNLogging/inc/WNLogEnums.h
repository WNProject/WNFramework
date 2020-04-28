// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_LOGGING_LOG_ENUMS_H__
#define __WN_LOGGING_LOG_ENUMS_H__

#include "WNCore/inc/base.h"
#include "WNLogging/inc/WNLog.h"

#include <vector>

namespace wn {
namespace logging {

enum class log_level {  // starting at 1 so that we can turn off with 0
  none = 0,
  critical,
  error,
  warning,
  issue,
  info,
  debug,
  max
};

}  // namespace logging
}  // namespace wn

#endif  // __WN_LOGGING_LOG_ENUMS_H__
