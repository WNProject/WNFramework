// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_RUNTIME_WINDOW_WINDOW_H__
#define __WN_RUNTIME_WINDOW_WINDOW_H__

#include "WNLogging/inc/WNLog.h"
#include "WNWindow/inc/WNWindowErrors.h"

namespace wn {
namespace memory {
class allocator;
}  // namespace memory

namespace multi_tasking {
class job_pool;
}  // namespace multi_tasking

namespace runtime {
namespace window {

class window {
public:
  virtual window_error initialize() = 0;
  virtual ~window() {}
  virtual bool is_valid() = 0;

protected:
  window() {}
};

}  // namespace window
}  // namespace runtime
}  // namespace wn

#endif  // __WN_RUNTIME_WINDOW_WINDOW_H__
